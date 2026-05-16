#include "HardwareSerial.h"
#include <stdint.h>
#include <Arduino.h>

#include "../hardware.h"
#include "../appstate.h"
#include "../functional.h"

#include "../task_autostop.h"

/*
  as higher the input value, as closer we to stop position. i.e. there is 
  a minimal value, where the motor should not start

  every 200ms a new measurement session starts. during this session would 
  be done sequence of the measurements and an average value will be calculated.

  this value will be compared to the one from the previous measurement. if
  new one less than old-one for more than a threshold, stop the motor.
  i.e should be a thresold value in configuration
 */

#define PHOTO_MEASUREMET_SESSION_PERIOD_MS 200

uint16_t photo_values_sum     = 0;
uint16_t photo_values_counter = 0;
uint16_t photo_values_last_average = 1023;
unsigned long photo_values_time = 0;

void task_autostop_init() {}

void task_autostop_loop()
{
  if (g_status.mode == S_MODE::OFF) {
    return;
  }

  int photo_value = analogRead(AIN_PHOTO);
  if (photo_value > get_config()->autostop_min_value) {
    g_status.mode = S_MODE::OFF;
    if (log_autostop()) {
      Serial.println(F("Autostop value too low. stop"));
    }
    return;
  }

  if (PHOTO_MEASUREMET_SESSION_PERIOD_MS < duration(photo_values_time, millis())) {
    // if duration since last measurement is bigger, than that period, then
    // session is in progress
    photo_values_sum += photo_value;
    ++photo_values_counter;

    if (photo_values_counter >= 16) {
      // collected 16 values, time to make average
      photo_values_sum >>= 4; // divide by 16
      if (log_autostop()) {
        Serial.print(F("Autostop new average "));
        Serial.print(photo_values_sum);
      }
  
      // moving to the center direction
      if (get_config()->autostop_threshold + photo_values_last_average < photo_values_sum) {
        // increase so fast, that old value even with a threshold is less than a new value
        // i.e. their distance longer, than threshold.
        //
        // __________|last_avg === thrsh ===|____|avg__________
        //
        // moving there, when it goes dark (to the center) --->

        // stop the motor
        g_status.mode = S_MODE::OFF;
        
        if (log_autostop()) {
          Serial.print(F(" previous average "));
          Serial.print(photo_values_last_average);
          Serial.print(F(", autostop executed"));
        }
      }

      if (log_autostop()) {
        Serial.println();
      }
      // measurement session is over, prepare a new one
      photo_values_last_average = photo_values_sum;
      photo_values_counter = 0;
      photo_values_sum = 0;
      photo_values_time = millis();
    }
  }
}
