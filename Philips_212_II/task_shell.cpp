#include <avr/pgmspace.h>
#include <stdio.h>
#include <Arduino.h>

#include <stdint.h>
#include <limits.h>

#include "hardware.h"
#include "functional.h"
#include "appstate.h"

#include "task_shell.h"

String command;

const char help_message[] PROGMEM = "Philips GA212 firmware interactive shell. May 2026\n"
  "Commands:\n"
  "\th        \t: help\n"
  "\tc        \t: dump current configuration\n"
  "\tl <uint> \t: debug log mask. 0 - none, +1 - buttons, +2 - autostop, +4 - speed\n"
  "\tb <uint> \t: debounce periond, ms\n"
  "\ta <uint> \t: set autostop min value. minimal light to start motor\n"
  "\tt <uint> \t: set autostop threshold value\n"
  "\t3 <uint> \t: minimal tacho period for 33RPM, us\n"
  "\t4 <uint> \t: minimal tacho period for 45RPM, us\n"
  "\tp <float>\t: motor PID P(roportional) coefficient\n"
  "\ti <float>\t: motor PID I(ntegral) coefficient\n"
  "\td <float>\t: motor PID D(ifferential) coefficient\n";

void print_help() {
  uint16_t i { 0 };
  for (char c = pgm_read_byte_near(help_message + i++);
    c != '\0';
    c = pgm_read_byte_near(help_message + i++))
  {
    Serial.print(c);
  }
  Serial.println();
}

void print_configuration() {
  Serial.print(F("autostop: "));
  Serial.print(get_config()->autostop_min_value);
  Serial.print(F("/"));
  Serial.print(get_config()->autostop_threshold);
  Serial.print(F(", debounce: "));
  Serial.print(get_config()->debounce_ms);
  Serial.print(F(", 33_min: "));
  Serial.print(get_config()->period_33rpm_min_us);
  Serial.print(F(", 45_min: "));
  Serial.print(get_config()->period_45rpm_min_us);
  Serial.print(F(", log: "));
  Serial.print(get_config()->debug_mask);
  Serial.print(F(", PID P="));
  Serial.print(get_config()->Kp);
  Serial.print(F(" I="));
  Serial.print(get_config()->Ki);
  Serial.print(F(" D="));
  Serial.print(get_config()->Kd);
  Serial.println();
}

void task_shell_init() {
  Serial.begin(115200);
  print_help();
}


const char simple_cmd[] PROGMEM = "ch";
const char int_cmd[] PROGMEM = "bat34l";
const char float_cmd[] PROGMEM = "pid";


void task_shell_loop() {
  if (Serial.available() != 0) {
    command = Serial.readString();
    command.trim();
    command.toLowerCase();

    if (command.length() > 0) {
      if ((command.length() == 1) && strchr_P(simple_cmd, command[0])) {
        switch(command[0]) {
        case 'c':
          print_configuration();
          break;
        case 'h':
          print_help();
          break;
        }
      }
      else if (strchr_P(int_cmd, command[0])) {
        long v  = max(0L, command.substring(1).toInt());
        switch(command[0]) {
          case 'b':
            set_debounce(min(UINT8_MAX, v));
            break;
          case 'a':
            set_autostop_min(min(UINT16_MAX, v));
            break;
          case 't':
            set_autostop_diff(min(UINT16_MAX, v));
            break;
          case '3':
            set_period_33rpm_min(min(UINT16_MAX, v));
            break;
          case '4':
            set_period_45rpm_min(min(UINT16_MAX, v));
            break;
          case 'l':
            set_debug_level(v);
            break;
        }
        print_configuration();
      }
      else if (strchr_P(float_cmd, command[0])) {
        float val = command.substring(1).toFloat();
        switch(command[0]) {
          case 'p':
            set_pid_p(val);
            break;
          case 'i':
            set_pid_i(val);
            break;
          case 'd':
            set_pid_d(val);
            break;
        }
        print_configuration();
      }
    }
  }
}