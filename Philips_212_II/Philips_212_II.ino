#include <Arduino.h>

#include "appstate.h"

#include "task_leds.h"
#include "task_autostop.h"
#include "task_shell.h"
#include "task_speed.h"
#include "task_buttons.h"
#include "task_tacho.h"

void setup() {
  analogReference(DEFAULT);

  status_init();

  task_leds_init();
  task_autostop_init();
  task_speed_init();
  task_buttons_init();
  task_shell_init();
  task_tacho_init();
}

void loop() {
  task_buttons_loop();
  task_autostop_loop(); // autostop can overcome the buttons
  task_leds_loop();     // leds should display actual state

  task_tacho_loop();
  task_speed_loop();    // motor works as it defined earlier

  // asynchronous, not related to the order as it offers no 
  // possibility to modify the state or measured values
  task_shell_loop();    
}
