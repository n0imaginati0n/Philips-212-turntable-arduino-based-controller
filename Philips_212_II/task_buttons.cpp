#include <Arduino.h>

#include "hardware.h"
#include "appstate.h"
#include "functional.h"

#include "task_buttons.h"

#define PRESSED_STOP        1
#define PRESSED_33RPM       (1 << 1)
#define PRESSED_45RPM       (1 << 2)

typedef enum {
  BTN_NOTPRTESSED = 0,
  BTN_PRESSED_WAIT_DEBOUNCE,
  BTN_WAS_PROCESSED
} BTN_STATE;

BTN_STATE button_step;
unsigned long last_pressed_time;
uint8_t pressed_buttons;

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

inline void buttons_pressed(uint8_t buttons) {
  if (buttons & PRESSED_STOP) {
    g_status.mode = S_MODE_OFF;
    if (get_config()->monitor_sensors) {
      Serial.println(F("Pressed 'OFF' button"));
    }
  }
  else if (buttons & PRESSED_33RPM) {
    g_status.mode = S_MODE_33RPM;
    if (get_config()->monitor_sensors) {
      Serial.println(F("Pressed '33 Start' button"));
    }
  }
  else if (buttons & PRESSED_45RPM) {
    g_status.mode = S_MODE_45RPM;
    if (get_config()->monitor_sensors) {
      Serial.println(F("Pressed '45 Start' button"));
    }
  }
  else {
    g_status.mode = S_MODE_OFF;
    if (get_config()->monitor_sensors) {
      Serial.println(F("Pressed unknown button. set OFF"));
    }
  }
}

void task_buttons_init() {
  button_step = BTN_NOTPRTESSED;

  pinMode(DIN_SEN45, INPUT);
  pinMode(DIN_SENOFF, INPUT);
  pinMode(DIN_SEN33, INPUT);

  
}

/* to avoid multiple sequential button events just because finger presses button and 
   don't release it, I need to implement latch. otherwise pressed button will conflict 
   with autostop, for example
 */
void task_buttons_loop() {
  
  uint8_t buttons_now = get_buttons();

  switch(button_step) {
    case BTN_NOTPRTESSED:
      // if buttons where not pressed before
      if (buttons_now != 0) {
        // rememeber state
        pressed_buttons = buttons_now;
        // remember time
        last_pressed_time = millis();
        // switch to debounce mode
        button_step = BTN_PRESSED_WAIT_DEBOUNCE;
      }
      break;

    case BTN_PRESSED_WAIT_DEBOUNCE:
      // wait some time to get check value
      if (get_config()->debounce_ms > duration(last_pressed_time, millis())) {
        if (pressed_buttons != buttons_now) {
          // the value is different, need to be re-captured
          button_step = BTN_NOTPRTESSED;
        }
        else {
          // values are the same. i.e. buttons are really pressed
          // do the action
          buttons_pressed(pressed_buttons);
          // switch to wait until buttons release
          button_step = BTN_WAS_PROCESSED;
        }
      }
      break;

    case BTN_WAS_PROCESSED:
      // buttons are released. ready for a new wait
      if (buttons_now == 0) {
        button_step = BTN_NOTPRTESSED;
      }
      break;

    default:
      // wrong step value is a reason to make it safely correct
      button_step = BTN_NOTPRTESSED;
  }
}