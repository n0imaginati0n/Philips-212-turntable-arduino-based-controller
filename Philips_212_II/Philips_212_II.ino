#include <stdint.h>

#define AIN_PHOTO 14
#define AIN_33TRIM 15
#define AIN_45TRIM 16

#define DIN_SEN45 4
#define DIN_SENOFF 6
#define DIN_SEN33 8
#define DIN_TACHO 10

#define DOUT_LED45 5
#define DOUT_LEDOFF 7
#define DOUT_LED33 9

#define PWM_MOTOR 11

#define ADC_RESOLUTION 10

void setup() {
  Serial.begin(115200);
  Serial.println("Philips GA212 firmware. April 2026");

  analogReference(DEFAULT);

  pinMode(DIN_SEN45, INPUT);
  pinMode(DIN_SENOFF, INPUT);
  pinMode(DIN_SEN33, INPUT);
  pinMode(DIN_TACHO, INPUT);  // ??
  Serial.println("digital inputs configured");

  // initialize before set as output removes blinking on the start
  digitalWrite(DOUT_LED45, HIGH);
  digitalWrite(DOUT_LEDOFF, LOW);
  digitalWrite(DOUT_LED33, HIGH);
  Serial.println("digital outputs initialized");

  pinMode(DOUT_LED45, OUTPUT);
  pinMode(DOUT_LEDOFF, OUTPUT);
  pinMode(DOUT_LED33, OUTPUT);
  Serial.println("digital outputs configured");
}

void loop() {
  Serial.print("33.3 RPM voltage is: ");
  Serial.println(analogRead(AIN_33TRIM));

  Serial.print("45 RPM voltage is: ");
  Serial.println(analogRead(AIN_45TRIM));

  delay(1000);
}
