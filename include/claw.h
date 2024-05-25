#include <Servo.h>

class Claw {
    Servo fourBarServo;
    Servo grabbingServo;
    int fourBarPotentiometerPin;
    int grabbingPotentiometerPin;
    int fourBarPWMPin;
    int grabbingPWMPin;

public:
    Claw(int FourBarPWMPin, int GrabbingPWMPin, int FourBarPotentiometerPin, int GrabbingPotentiometerPin);
    void begin();
    void readAngles();
};