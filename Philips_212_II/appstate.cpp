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
void set_autostop(int16_t value) {
  g_config.autostop_event_value = value;
  save_config();
}
void set_debounce(int8_t value) {
  g_config.debounce_ms = value;
  save_config();
}
void set_start_33rpm_range(int16_t value) {
  g_config.start_33rpm_range = value;
  save_config();
}
void set_start_45rpm_range(int16_t value) {
  g_config.start_45rpm_range = value;
  save_config();
}
void set_moniotor_sensors(bool value) {
  g_config.monitor_sensors = value;
  save_config();
}


/* =-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^=-_-=^ */

void status_init() {
  g_status.mode = S_MODE_OFF;

  load_config();
}