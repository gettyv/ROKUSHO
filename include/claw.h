#include <Servo.h>

class Claw {
    Servo fourBarServo;
    Servo grabbingServo;

public:
    Claw(int FourBarPWMPin, int GrabbingPWMPin, int FourBarPotentiometerPin, int GrabbingPotentiometerPin);
    void begin();
    void readAngles();
    void setFourBarAngle(int angle);
    void setGrabbingAngle(int angle);
    int getFourBarAngle();
    int getGrabbingAngle();

    void grabDisc(int disc);
    void releaseDisc();
    void fourBarSweep();
    void grabberSweep();
};