#include <stdint.h>
#include <Arduino.h>
#include <EEPROM.h>

#include "appstate.h"

#define EEPROM_CONFIG_ADDR  0

S_STATUS g_status;
S_CONFIG g_config;


/* =-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^ */

void save_config() {
  EEPROM.put(EEPROM_CONFIG_ADDR, g_config);
}

void load_config() {
  EEPROM.get(EEPROM_CONFIG_ADDR, g_config);
}

/* =-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^ */

const S_CONFIG* get_config() {
  return &g_config;
}

bool log_autostop() {
  return get_config()->debug_mask & static_cast<uint8_t>(DBG_LEVEL::AUTOSTOP);
}
bool log_buttons() {
  return get_config()->debug_mask & static_cast<uint8_t>(DBG_LEVEL::BUTTONS);
}
bool log_speed() {
  return get_config()->debug_mask & static_cast<uint8_t>(DBG_LEVEL::SPEED);
}

void set_autostop_min(int16_t value) {
  g_config.autostop_min_value = value;
  save_config();
}
void set_autostop_diff(int16_t value) {
  g_config.autostop_threshold = value;
  save_config();
}
void set_debounce(uint8_t value) {
  g_config.debounce_ms = value;
  save_config();
}

void set_period_33rpm_min(unsigned long us) {
  g_config.period_33rpm_min_us = us;
  save_config();
}
void set_period_45rpm_min(unsigned long us) {
  g_config.period_45rpm_min_us = us;
  save_config();
}

void set_debug_level(uint8_t mask) {
  g_config.debug_mask = mask;
  save_config();
}

void set_pid_p(float p) {
  g_config.Kp = p;
  save_config();
}
void set_pid_i(float i) {
  g_config.Ki = i;
  save_config();
}
void set_pid_d(float d) {
  g_config.Kd = d;
  save_config();
}


/* =-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^ */

void status_init() {
  g_status.mode = S_MODE::OFF;
  g_status.pulse_period_us = 0;

  load_config();
}