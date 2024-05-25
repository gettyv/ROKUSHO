#include <Arduino.h>
// #include <constants.h>
#include <rpi.h>

RPI rpi(9600);

void setup() {
    rpi.begin();
    rpi.wait_rpi_ready(0);
    rpi.sendMessage("ARDUINO_READY");
}

void loop() {
  if (rpi.messageAvailable()) {
    String message = rpi.readMessage();
    rpi.sendMessage(message);
  }
}