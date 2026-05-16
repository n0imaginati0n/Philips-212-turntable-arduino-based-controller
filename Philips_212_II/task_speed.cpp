#include <QuickPID.h>

#include <Arduino.h>

#include <stdint.h>

#include "hardware.h"
#include "appstate.h"
#include "functional.h"

#include "task_speed.h"

S_MODE previous = S_MODE::OFF;

// Factor that turns velocity into a number that looks like RPMs. Set by trying values while measuring turntable with a separate tachometer.
// The higher the value, the slower the motor spins
constexpr float VEL_FACTOR = 0.846798789f;
// Motor velocity target values for 33RPM and 45RPM
constexpr float TARGET_VEL_33 = 100.f * (100.0f / 3.f);
constexpr float TARGET_VEL_45 = 100.f * 45.f;

class VelocitySource {
public:
  float get(const float period) const {
    return m_multiplier * m_velocityFactor / period;
  }

private:
  const float m_multiplier { 1e8 };
  const float m_velocityFactor { VEL_FACTOR };
};

class ExpFilteredVelocitySource {
public:
  ExpFilteredVelocitySource(const float alpha) : m_alpha(alpha)
  {}

  float get(const float period) {
    float tmp = m_source.get(period);
    if (!m_first) {
      m_filteredValue = m_alpha * tmp + (1 - m_alpha) * m_filteredValue;
    }
    else {
      m_filteredValue = tmp;
      m_first = false;
    }
    return m_filteredValue;
  }
  void reset() {
    m_filteredValue = 0.f;
    m_first = true;
  }
private:
  bool m_first = false;
  float m_filteredValue = 0.f;
  const float m_alpha;

  VelocitySource m_source;
};

ExpFilteredVelocitySource currentVelocity(0.025);


// these values are for input-output of the PID
float pidSetpoint = 0.f, pidInput = 0.f, pidOutput = 0.f;
QuickPID motorPid(&pidInput, &pidOutput, &pidSetpoint);

float trim_amount(int pot_value, float freq_base) {
    return freq_base * map(pot_value, 1023, 0, -512, 512) / 512;
}

// -=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=

void task_speed_init() {
  // Set up PID speed control
  motorPid.SetTunings(get_config()->Kp, get_config()->Ki, get_config()->Kd);
  motorPid.SetOutputLimits(10, 250);
  motorPid.SetAntiWindupMode(motorPid.iAwMode::iAwClamp);
  motorPid.SetSampleTimeUs(2000);
  motorPid.SetMode(motorPid.Control::automatic);
  motorPid.SetControllerDirection(motorPid.Action::direct);

  // 25 KHz for PWM silence. PIN11, Timer2, Prescaler = 1
  TCCR2B = (TCCR2B & 0b11111000) | 0x01;
}


void task_speed_loop() {
  static bool initial_start = false;
  static long play_mode_switch_ms = 0; // us after play mode was switched on

  if (previous != g_status.mode) {
    // new mode was switched on
    if (previous == S_MODE::OFF) {  // if it was stopped. any mode can be ON
      // i can't avoid the PID - it shoudl control all the speed changes starting from 0
      // but i can set some dummy input and wait for a while to let the motor get the 
      // initial speed. PID will calculate the changes speed based on it's internal timer
      if (g_status.mode == S_MODE::RPM33) {
        pidSetpoint = TARGET_VEL_33;
      }
      else if (g_status.mode == S_MODE::RPM45) {
        pidSetpoint = TARGET_VEL_45;
      }

      initial_start = true;
      currentVelocity.reset();
      play_mode_switch_ms = millis();
    }
    else {                          // don't let to switch between the 33/44 without stop
      g_status.mode = S_MODE::OFF;
    }
    previous = g_status.mode;
  }

  if (g_status.mode == S_MODE::OFF) {
    // if current mode is to STOP
    analogWrite(PWM_MOTOR, 0);
  }
  else if (initial_start) {
    // start before everything will be controlled by PID
    if (millis() - play_mode_switch_ms < 800) {
      analogWrite(PWM_MOTOR, 200);
    }
    else {
      initial_start = false;
    }
  }
  else {
    // set PID setpoint
    if (g_status.mode == S_MODE::RPM33) {
      pidSetpoint = TARGET_VEL_33 + trim_amount(analogRead(AIN_33TRIM), TARGET_VEL_33);
    }
    else if (g_status.mode == S_MODE::RPM45) {
      pidSetpoint = TARGET_VEL_45 + trim_amount(analogRead(AIN_45TRIM), TARGET_VEL_45);
    }

    pidInput = currentVelocity.get(g_status.pulse_period_us);

    if (motorPid.Compute()) {
      if (log_speed()) {
        Serial.print(pidSetpoint, 2);
        Serial.print(F(" , "));
        Serial.print(pidInput, 2);
        Serial.print(F(" , "));
        Serial.println(pidOutput, 0);
      }
      analogWrite(PWM_MOTOR, static_cast<int>(pidOutput));
    }
  }
}
