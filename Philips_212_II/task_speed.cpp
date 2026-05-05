#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>

#include <Arduino.h>

#include <stdint.h>

#include "hardware.h"
#include "appstate.h"

#include "functional.h"

#include "task_speed.h"

volatile unsigned long pulse_length;
volatile unsigned long pulse_on;
volatile bool changed;

void tacho_pulse() {
  unsigned long current = micros();
  // pulse_length = duration(pulse_on, current);
  pulse_on = current;
  changed = true;
}

void task_speed_init() {
  pulse_length = 0;
  changed = false;
  pulse_on = micros();

  pinMode(DIN_TACHO, INPUT);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(DIN_TACHO), tacho_pulse, RISING);
}


ANALOG WRITE RANGE is 0...255 !!!

void task_speed_loop() {
  if (g_status.mode == S_MODE_OFF) {


      analogWrite(PWM_MOTOR, 0);
      changed = false;
  }
  if ((g_status.mode == S_MODE_33RPM)) {
      int adc_val = analogRead(AIN_33TRIM);
      uint16_t value = map(adc_val, 0, 1023, get_config()->start_33rpm_range, get_config()->start_33rpm_range + 50);
      if (get_config()->monitor_sensors) {
        Serial.print(F("33 RPM PWM value is "));
        Serial.print(value);
        Serial.print(F(", tacho period is "));
        Serial.println(pulse_length);
      }
      analogWrite(PWM_MOTOR, value);
      changed = false;
  }
  else if ((g_status.mode == S_MODE_45RPM) && changed) {
      int adc_val = analogRead(AIN_45TRIM);
      uint16_t value = map(adc_val, 0, 1023, get_config()->start_45rpm_range, get_config()->start_45rpm_range + 50);
      if (get_config()->monitor_sensors) {
        Serial.print(F("45 RPM PWM value is "));
        Serial.print(value);
        Serial.print(F(", tacho period is "));
        Serial.println(pulse_length);
      }
      analogWrite(PWM_MOTOR, value);
      changed = false;
  }
  else {
      analogWrite(PWM_MOTOR, 0);
      changed = false;
  }
}

  
