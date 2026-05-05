#include <Arduino.h>

#include "hardware.h"
#include "appstate.h"

#include "task_leds.h"

void task_leds_init() {
  digitalWrite(DOUT_LED45, HIGH);
  digitalWrite(DOUT_LEDOFF, HIGH);
  digitalWrite(DOUT_LED33, HIGH);

  pinMode(DOUT_LED45, OUTPUT);
  pinMode(DOUT_LEDOFF, OUTPUT);
  pinMode(DOUT_LED33, OUTPUT);
}

void task_leds_loop() {
  switch(g_status.mode) {
    case S_MODE_OFF:
      digitalWrite(DOUT_LEDOFF, LOW);
      digitalWrite(DOUT_LED45, HIGH);
      digitalWrite(DOUT_LED33, HIGH);
      break;
    case S_MODE_33RPM:
      digitalWrite(DOUT_LEDOFF, HIGH);
      digitalWrite(DOUT_LED45, HIGH);
      digitalWrite(DOUT_LED33, LOW);
      break;
    case S_MODE_45RPM:
      digitalWrite(DOUT_LEDOFF, HIGH);
      digitalWrite(DOUT_LED45, LOW);
      digitalWrite(DOUT_LED33, HIGH);
      break;
    default:
      digitalWrite(DOUT_LEDOFF, HIGH);
      digitalWrite(DOUT_LED45, HIGH);
      digitalWrite(DOUT_LED33, HIGH);
  }
}