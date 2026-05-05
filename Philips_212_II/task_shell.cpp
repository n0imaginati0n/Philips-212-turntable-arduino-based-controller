#include <avr/pgmspace.h>
#include <stdio.h>
#include <Arduino.h>

#include <stdint.h>
#include <limits.h>

#include "hardware.h"
#include "appstate.h"

#include "task_shell.h"

String command;

const char help_message[] PROGMEM = "Philips GA212 firmware interactive shell. April 2026\n"
  "Commands:\n"
  "\th        \t: help\n"
  "\td        \t: dump current configuration\n"
  "\tm <uint> \t: monitor sensors. on, if number !=0\n"
  "\tb <uint> \t: debounse periond, ms\n"
  "\ta <uint> \t: set autostop value\n"
  "\t3 <uint> \t: minimal tacho range side for 33RPM\n"
  "\t4 <uint> \t: minimal tacho range side for 45RPM\n->";

template<typename T, unsigned size>
constexpr unsigned arr_size(T(&)[size]) noexcept {
  return size;
}

char txt_buffer[82];

void print_prog_string(const char* str, unsigned size) {
  unsigned pos = 0;
  while (pos < size - arr_size(txt_buffer)) {
    strlcpy_P(txt_buffer, str + pos, arr_size(txt_buffer));
    pos += arr_size(txt_buffer) - 1;
    Serial.print(txt_buffer);
  }
  if (pos < size) {
    strlcpy_P(txt_buffer, str + pos, size - pos);
    Serial.print(txt_buffer);
  }
}

void print_help() {
  Serial.println();
  print_prog_string(help_message, arr_size(help_message));
  Serial.println();
}

void print_configuration() {
  snprintf_P(
    txt_buffer, sizeof(txt_buffer)/sizeof(txt_buffer[0]), 
    PSTR("autostop: %u, debounce: %u, 33_min: %u, 45_min: %u, monitor: %u"),
    get_config()->autostop_event_value,
    get_config()->debounce_ms,
    get_config()->start_33rpm_range,
    get_config()->start_45rpm_range,
    (get_config()->monitor_sensors ? 1U : 0)
    );
  Serial.println(txt_buffer);
}

void task_shell_init() {
  Serial.begin(115200);
  print_help();
}

void task_shell_loop() {
  if (Serial.available() != 0) {
    command = Serial.readString();
    command.trim();
    command.toLowerCase();

    if (command.length() > 0) {
      char const* cmd = command.c_str();

      if (command.length() == 1) {
        switch(cmd[0]) {
        case 'd':
          print_configuration();
          break;
        case 'h':
        default:
          print_help();
        }
      }
      else {
        unsigned long v  = strtoul(cmd + 1, NULL, 10);
        switch(cmd[0]) {
          case 'b':
            set_debounce(min(UINT8_MAX, v));
            break;
          case 'a':
            set_autostop(min(UINT16_MAX, v));
            break;
          case '3':
            set_start_33rpm_range(min(UINT16_MAX, v));
            break;
          case '4':
            set_start_45rpm_range(min(UINT16_MAX, v));
            break;
          case 'm':
            set_moniotor_sensors(v != 0);
            break;
          default:
            print_help();
        }
        print_configuration();
      }
    }
  }
}