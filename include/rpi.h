#ifndef RPI_H
#define RPI_H

#include <Arduino.h>
#include <string>

class RPI {
public:
    RaspberryPi(const std::string& serialPort);
    ~RaspberryPi();

    bool connect();
    void disconnect();

    bool sendData(const std::string& data);
    std::string receiveData();

private:
    std::string serialPort_;
    int serialPortHandle_;
};

#endif // RPI_H