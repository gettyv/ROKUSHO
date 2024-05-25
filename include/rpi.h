#ifndef RPI_H
#define RPI_H

#include <Arduino.h>

class RPI {
public:
    // Constructor
    RPI(long baudRate);

    // Initialize serial communication
    void begin();

    void wait_rpi_ready(unsigned long timeoutDuration);

    // Send a message to the Raspberry Pi
    void sendMessage(const String &message);

    // Check if a message is available from the Raspberry Pi
    bool messageAvailable();

    // Read the incoming message from the Raspberry Pi
    String readMessage();

private:
    long baudRate;
};

#endif // RPI_H
