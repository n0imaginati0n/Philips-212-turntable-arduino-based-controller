#ifndef _STATUS_H
#define _STATUS_H

typedef enum {
  S_MODE_OFF = 0,
  S_MODE_33RPM,
  S_MODE_45RPM
} S_MODE;

typedef struct {
  volatile S_MODE mode;
} S_STATUS;

typedef struct {
  int16_t autostop_event_value; // this is value, at which autostop should execute
  int16_t start_33rpm_range;    // minimal of speed range for 33RPM tacho
  int16_t start_45rpm_range;    // minimal of speed range for 45RPM tacho
  uint8_t debounce_ms;          // button debounce delay in ms
  bool    monitor_sensors;      // true, if sensors and buttons should to be monitored
} S_CONFIG;

void status_init();

const S_CONFIG* get_config();
void set_autostop(int16_t value);
void set_debounce(int8_t value);
void set_start_33rpm_range(int16_t value);
void set_start_45rpm_range(int16_t value);
void set_moniotor_sensors(bool value);

extern S_STATUS g_status;

#endif // _STATUS_H