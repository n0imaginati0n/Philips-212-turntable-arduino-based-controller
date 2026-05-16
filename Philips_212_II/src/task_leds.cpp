#include <Arduino.h>

#include "../hardware.h"
#include "../appstate.h"

#include "../task_leds.h"

void task_leds_init() {
  digitalWrite(DOUT_LED45, HIGH);
  digitalWrite(DOUT_LEDOFF, HIGH);
  digitalWrite(DOUT_LED33, HIGH);

  pinMode(DOUT_LED45, OUTPUT);
  pinMode(DOUT_LEDOFF, OUTPUT);
  pinMode(DOUT_LED33, OUTPUT);
}

void task_leds_loop() {
  digitalWrite(DOUT_LEDOFF, g_status.mode == S_MODE::OFF ? LOW : HIGH);
  digitalWrite(DOUT_LED45, g_status.mode == S_MODE::RPM45 ? LOW : HIGH);
  digitalWrite(DOUT_LED33, g_status.mode == S_MODE::RPM33 ? LOW : HIGH);
}