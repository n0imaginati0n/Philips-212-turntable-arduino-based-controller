#include <Arduino.h>

#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>

#include "hardware.h"
#include "appstate.h"
#include "functional.h"

#include "task_tacho.h"

volatile unsigned long pulse_last_interrupt_us = 0;

void tacho_pulse() {
  unsigned long current = micros();
  g_status.pulse_period_us = duration(pulse_last_interrupt_us, current);
  pulse_last_interrupt_us = current;
}

void task_tacho_init() {
  pinMode(DIN_TACHO, INPUT);
  attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(DIN_TACHO), tacho_pulse, RISING);
}

void task_tacho_loop() {

}
