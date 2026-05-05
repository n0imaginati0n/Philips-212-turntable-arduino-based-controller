#ifndef _HARDWARE_H
#define _HARDWARE_H

// autostop photosensor input
#define AIN_PHOTO   14
// 33RPM analog potentiometer input
#define AIN_33TRIM  15
// 45RPM analog potentiometer input
#define AIN_45TRIM  16

// 45RPM sensor digital input
#define DIN_SEN45   4
// STOP sensor digital input
#define DIN_SENOFF  6
// 33RPM sensor digital input
#define DIN_SEN33   8
// tacho digital input
#define DIN_TACHO   10

// 45RPM led
#define DOUT_LED45  5
// STOP led
#define DOUT_LEDOFF 7
// 33RPM led
#define DOUT_LED33  9

// motor speed PWM output
#define PWM_MOTOR   11

#endif // _HARDWARE_H