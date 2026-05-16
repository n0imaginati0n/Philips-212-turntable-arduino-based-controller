#include <QuickPID.h>

#include <Arduino.h>

#include <stdint.h>

#include "hardware.h"
#include "appstate.h"
#include "functional.h"

#include "task_speed.h"

S_MODE previous = S_MODE::OFF;

/* PID controller will take tacho period as input and 
  return PWM value as output. this is reverse direction, as 
  increasing PWM will increase a frequency and decrease 
  tacho period.

  target point for 33 and for 45 RPM will be taken from 
  configuration. potentiometer will add ~ +/- 10% tune
 */

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=
 * MATH:
 * D = 155mm - diameter of the big disk
 * d = 13mm  - diameter of the motor
 * N - RPM of the big disk
 * n - RPM of the small disk
 *
 * l = pi * diameter - length of the circle
 *
 * big disk per minute should rotate length of the belt:
 * 
 * L = N * pi * D
 *
 * the same length should rotate the motor, but 'n' will be higher, as
 * 'd' is smaller
 *
 * N * pi * D = n * pi * d => n = N * D / d
 *
 * frequency is a rotations per second:
 *
 * f = n / 60 = N * D / (d * 60.)
 *
 * period of the motor:
 *
 * t = 1 / f = d * 60. / N * D = 0.15098284021950584 sec
 * T = 60. / 33.33 = 1.8001800 sec
 *
 * or just simple:
 * T = 60.0 / N
 * t = T * (d/D)
 * 
 * tacho gives 6 periods per rotation.
 */
constexpr float PLATE_DISK_DIAMETER_MM = 155.0f;
constexpr float MOTOR_AXIS_DIAMETER_MM = 13.0f;
constexpr float MOTOR_PERIODS_PER_ROTATION = 6.0f;

constexpr float calc_tacho_frequency(const float plate_rpm) {
  return 1.0                                            // sec -> 10*N ms. just better fopr PID 
    * MOTOR_PERIODS_PER_ROTATION                          // motor tacho has 6 evolitions per round
    * (plate_rpm / 60.0f)                                 // RPM -> rotation per second = freq Hz
    * (PLATE_DISK_DIAMETER_MM / MOTOR_AXIS_DIAMETER_MM);  // plate loops -> motor loops
}
constexpr float freq33 = calc_tacho_frequency(100.f/3.f); // 100.0/3. = 33.33(33)
constexpr float freq45 = calc_tacho_frequency(45.f);      // 45
constexpr unsigned long initial_period_us = 150000;       // after reaching this period, PID will come into the game

inline float calc_period_frequency(uint16_t period) {
  return 1.0f                                           // sec -> 10*N ms. just better fopr PID 
    * (1000000.0f / period);                              // period us -> freq Hz
}

// #define PERIOD_TO_FREQ(PERIOD_US) (1000000.0f / PERIOD_US)

// these values are for input-output of the PID
float pidSetpoint = 0.f, pidInput = 0.f, pidOutput = 0.f;
QuickPID motorPid(&pidInput, &pidOutput, &pidSetpoint);

template<uint8_t percent>
constexpr float get_range_min() { return 1.0f - 1.0f * percent / 200.f;}

template<uint8_t percent>
constexpr float get_range_len() { return 1.f * percent / 100.f; }

template<uint8_t percent>                                                         
constexpr float get_pid_setpoint_(int pot_value, float freq_base) {
  return freq_base * (get_range_min<percent>() +
    get_range_len<percent>() * pot_value / 1023.0f);
}
constexpr float get_pid_setpoint(int pot_value, float freq_base) {
    return get_pid_setpoint_<20>(pot_value, freq_base);
}

// -=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=-=*^*=

void task_speed_init() {
  // Set up PID speed control
  motorPid.SetTunings(get_config()->Kp, get_config()->Ki, get_config()->Kd);
  motorPid.SetOutputLimits(0, 255);
  motorPid.SetControllerDirection(motorPid.Action::reverse);
  // motorPid.SetAntiWindupMode(motorPid.iAwMode::iAwOff);
  motorPid.SetSampleTimeUs(2000);
  motorPid.SetMode(motorPid.Control::automatic);

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
        pidSetpoint = freq33;
      }
      else if (g_status.mode == S_MODE::RPM45) {
        pidSetpoint = freq45;
      }
      initial_start = true;
      pidInput = pidSetpoint * 1.5;
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
    // start before everything will be controlle dby PID
    if (millis() - play_mode_switch_ms < 1000) {
      analogWrite(PWM_MOTOR, 250);
    }
    else {
      initial_start = false;
      motorPid.SetOutputLimits(10.f, 100.f);
    }
  }
  else {
    // set PID setpoint
    if (g_status.mode == S_MODE::RPM33) {
      pidSetpoint = get_pid_setpoint(analogRead(AIN_33TRIM), freq33);
    }
    else if (g_status.mode == S_MODE::RPM45) {
      pidSetpoint = get_pid_setpoint(analogRead(AIN_45TRIM), freq45);
    }

    pidInput = calc_period_frequency(g_status.pulse_period_us);

    bool changed = motorPid.Compute();
    if (!changed) {
      if (log_speed()) {
        Serial.println(F("PWM stays intact"));
      }
    }
    else {
      if (log_speed()) {
        Serial.print(F("Set: ")); Serial.print(pidSetpoint, 2);
        Serial.print(F(" Hz, In: ")); Serial.print(pidInput, 2);
        Serial.print(F(" Hz, PWM: ")); Serial.println(pidOutput, 2);
      }
      analogWrite(PWM_MOTOR, static_cast<int>(pidOutput));
    }
  }
}

  
