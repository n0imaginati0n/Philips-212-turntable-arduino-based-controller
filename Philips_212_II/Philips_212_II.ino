#include <Arduino.h>

#include "appstate.h"

#include "task_leds.h"
#include "task_autostop.h"
#include "task_shell.h"
#include "task_speed.h"
#include "task_buttons.h"

void setup() {
  analogReference(DEFAULT);

  status_init();

  task_leds_init();
  task_autostop_init();
  task_speed_init();
  task_buttons_init();
  task_shell_init();
}

void loop() {
  task_leds_loop();
  task_autostop_loop();
  task_speed_loop();
  task_buttons_loop();
  task_shell_loop();
}
