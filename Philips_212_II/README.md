---
title: Philips GA212 firmware
---

# Implementation description

This firmware implemented as a list of tasks. Each task has 'init' and 'loop' function.
They are called from 'init' and 'loop' functions of the sketch appropriately. It is not RTOS,
it has no guaranteed response time to events, but it still allows to execute many 
simultaneous functions. The only rules: don't block the execution and be fast. better 
split long operation over more than one calls of 'loop' task function. Tasks related to 
interrupts has interrupt handlers in their own compilation units.

## LEDs task

light LEDs according to the current mode.

## Autostop task

if motor is on, track the value of the photosensor and switch teh mode to OFF as fast as 
photosensor value will show end of plate. the sensing value stored in configuration and 
can be set from shell

## Buttons task

this task implements simple de-bouncing for buttons and changes the state accordingly to
pressed buttons. De-bouncing algorithm is pretty common: it waits until the bouncing stop, 
waits for some time, prove, that button strate kept pressed and sign about event the main 
loop. only pins D2 and D3 can generate interrupt in this MCU. all the state tracking done
manually using digitalRead()

## Speed task

this task loads appropriate potentiometer value, calculate necessary PWM value and set it 
to the motor acceleration. as potentiometers aren't really precise, they left to function 
in a specific range. i.e. their value after appropriate scaling will be added to the 
configuration stored value to get result acceleration. to make it working, the configuration 
value can be set from shell. software will monitor not a PWM, but a tacho value, which is 
measured in microseconds. 33 RPM has period approximately 26700 us, 45 RPM has period 
approximately 19250 us in my case. as PWM value going to be not absolute, but relative - 
increase to shortend measured period and decrease to extend it, it is important to evaluate 
new PWM value only once per period measurement. otherwise intertia will be introduced - the PWM 
value will be corrected multiple times per period measurement trying to correct period and 
overcomensate in result

## Shell task

this task is responsible for primitive shell available on serial programming port. the shell 
can print current configuration, set new configuration values, switch on debug monitoring. shell 
is asynchronous, that is why it can react with a small delay

# Implementation Status

- [x] LEDs task
  - [x] Implemntation
  - [x] Monitoring
- [x] Shell task
  - [x] Implementation
  - [x] Monitoring
- [x] Autostop task
  - [x] Implemntation
  - [x] Monitoring
- [x] Buttons task
  - [x] Implementation
  - [x] Monitoring
- [x] Speed task
  - [ ] Implementation
  - [x] Monitoring
- [x] Configuration
  - [x] EEPROM storage
  - [x] use by getter
  - [x] update by setters

