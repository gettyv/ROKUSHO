#include <Arduino.h>
// #include <constants.h>
#include <motor.h>

Motor mtr(7, 8);

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("Speeding up");
  mtr.set_speed(10);
  delay(2000);
  mtr.set_speed(8);
  delay(2000);
  mtr.set_speed(5);
  delay(2000);
  mtr.set_speed(2);
  delay(2000);
}