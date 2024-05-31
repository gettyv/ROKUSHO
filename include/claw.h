#ifndef CLAW_H
#define CLAW_H
#include <Servo.h>

class Claw {
    Servo fourBarServo;
    Servo grabbingServo;
    String incomingByte;
    String command;

    int fourBarAngle;
    int grabbingAngle;

public:
    Claw(int FourBarPWMPin, int GrabbingPWMPin, int FourBarPotentiometerPin, int GrabbingPotentiometerPin);
    void begin();
    void readAngles();
    void setFourBarAngle(int angle);
    void setGrabbingAngle(int angle);
    int getFourBarAngle();
    int getGrabbingAngle();

    void sendManual();
    void grabDisc(int disc);
    void releaseDisc();
    void fourBarSweep();
    void grabberSweep();

};

#endif