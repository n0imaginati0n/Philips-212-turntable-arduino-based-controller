#ifndef _STATUS_H
#define _STATUS_H

enum class S_MODE : uint8_t {
  OFF = 0,
  RPM33,
  RPM45
};

enum class DBG_LEVEL : uint8_t {
  NONE = 0,
  BUTTONS = 1,
  AUTOSTOP = 2,
  SPEED = 4
};

typedef struct {
  volatile S_MODE mode;
  volatile unsigned long pulse_period_us;
} S_STATUS;

typedef struct {
  int16_t autostop_min_value;   // minimal value, at which autostop should let the motor run
  int16_t autostop_threshold;   // minimal difference between to avg values to stop the motor
  unsigned long period_33rpm_min_us;  // minimal tacho period in us for 33 RPM
  unsigned long period_45rpm_min_us;  // minimal tacho period in us for 45 RPM
  uint8_t debounce_ms;          // button debounce delay in ms
  uint8_t debug_mask;           // mask for debugging.
                                // 0 - no debug
                                // 1 - buttons
                                // 2 - autostop
                                // 4 - speed
  float Kp;                     // motor PID Kp coefficient. Proportional
  float Ki;                     // motor PID Ki coefficient. Integral
  float Kd;                     // motor PID Kd coefficient. Differential
} S_CONFIG;

void status_init();

const S_CONFIG* get_config();

bool log_autostop();
bool log_buttons();
bool log_speed();

void set_autostop_min(int16_t value);
void set_autostop_diff(int16_t value);
void set_debounce(uint8_t value);
void set_period_33rpm_min(unsigned long us);
void set_period_45rpm_min(unsigned long us);
void set_debug_level(uint8_t mask);
void set_pid_p(float p);
void set_pid_i(float i);
void set_pid_d(float d);

extern S_STATUS g_status;

#endif // _STATUS_H