#include <Servo.h>

class Claw {
    Servo fourBarServo;
    Servo grabbingServo;

public:
    Claw(int FourBarPWMPin, int GrabbingPWMPin, int FourBarPotentiometerPin, int GrabbingPotentiometerPin);
    void begin();
    void readAngles();
};