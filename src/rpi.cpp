#include "RPI.h"

// Constructor to initialize baud rate
RPI::RPI(long baudRate) {
    this->baudRate = baudRate;
}

// Initialize serial communication with the given baud rate
void RPI::begin() {
    Serial.begin(baudRate);
    while (!Serial) {
        ; // Wait for serial port to connect. Needed for native USB
    }
}

int RPI::wait_rpi_ready(unsigned long timeoutDuration /*= 0*/) {
    unsigned long startTime = millis();
    while (true) {
        if (RPI::readMessage() == "RPI_READY") {
            return 0;
        }
        if (timeoutDuration != 0 && millis() - startTime >= timeoutDuration) {
            // Timeout occurred, handle the timeout here
            return 1;
        }
    }
}

// Send a message to the Raspberry Pi
void RPI::sendMessage(const String &message) {
    Serial.println(message);
}

// Check if a message is available from the Raspberry Pi
bool RPI::messageAvailable() {
    return Serial.available() > 0;
}

// Read the incoming message from the Raspberry Pi
String RPI::readMessage() {
    String message = "";
    if (Serial.available() > 0) {
        message = Serial.readStringUntil('\n');
    }
    return message;
}
