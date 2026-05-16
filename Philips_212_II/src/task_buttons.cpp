#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>

#include <Arduino.h>

#include "../hardware.h"
#include "../appstate.h"
#include "../functional.h"

#include "../task_buttons.h"

#define PRESSED_STOP        1
#define PRESSED_33RPM       (1 << 1)
#define PRESSED_45RPM       (1 << 2)

unsigned long last_pressed_time;
uint8_t pressed_buttons;
bool check_time;

uint8_t get_buttons() {
  uint8_t res = 0;
  if (LOW == digitalRead(DIN_SENOFF)) {
    res |= PRESSED_STOP;
  }
  if (LOW == digitalRead(DIN_SEN33)) {
    res |= PRESSED_33RPM;
  }
  if (LOW == digitalRead(DIN_SEN45)) {
    res |= PRESSED_45RPM;
  }

  return res;
}

void buttons_pressed() {
  last_pressed_time = millis();
  pressed_buttons = get_buttons();
  check_time = true;
}

inline void buttons_pressed(uint8_t buttons) {
  if (buttons & PRESSED_STOP) {
    g_status.mode = S_MODE::OFF;
  }
  else if (buttons & PRESSED_33RPM) {
    g_status.mode = S_MODE::RPM33;
  }
  else if (buttons & PRESSED_45RPM) {
    g_status.mode = S_MODE::RPM45;
  }
  else {
    g_status.mode = S_MODE::OFF;
  }
}

void task_buttons_init() {
  pinMode(DIN_SEN45, INPUT);
  pinMode(DIN_SENOFF, INPUT);
  pinMode(DIN_SEN33, INPUT);

  // button are pressed, when the input goes LOW
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(DIN_SEN45), buttons_pressed, FALLING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(DIN_SENOFF), buttons_pressed, FALLING);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(DIN_SEN33), buttons_pressed, FALLING);
}

void task_buttons_loop() {
  if (check_time) {
    if ((get_config()->debounce_ms <= duration(last_pressed_time, millis()))
      && (get_buttons() == pressed_buttons))
    {
      buttons_pressed(pressed_buttons);
      check_time = false;

      if (log_buttons()) {
        Serial.print(F("Pressed "));
        if (pressed_buttons & PRESSED_STOP) {
          Serial.print(F("'STOP'"));
        }
        else if (pressed_buttons & PRESSED_33RPM) {
          Serial.print(F("'33'"));
        }
        else if (pressed_buttons & PRESSED_45RPM) {
          Serial.print(F("'45'"));
        }
        else {
          Serial.print(F("unknown"));
        }
        Serial.println(F(" button"));
      }
    }
  }
}