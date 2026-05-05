#include "HardwareSerial.h"
#include <stdint.h>
#include <Arduino.h>

#include "hardware.h"
#include "appstate.h"
#include "functional.h"

#include "task_autostop.h"

unsigned long last_monitor_print;

void task_autostop_init() {
  last_monitor_print = millis();
}

void task_autostop_loop() {
  if (g_status.mode != S_MODE_OFF) {
    // don't do long analogRead if it is not necessary
    if (analogRead(AIN_PHOTO) > get_config()->autostop_event_value) {
      // g_status.mode = S_MODE_OFF;
    }
  }

  if (get_config()->monitor_sensors && duration(last_monitor_print, millis()) > 5000)  {
    Serial.print(F("Autostop current value: "));
    Serial.print(analogRead(AIN_PHOTO));
    Serial.print(F(", State is "));
    Serial.println(g_status.mode == S_MODE_OFF ? F("OFF") : F("RUN"));
    last_monitor_print = millis();
  }
}
