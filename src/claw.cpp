#include <Arduino.h>
#include <claw.h>
#include <Servo.h>

Claw::Claw(int FourBarPWMPin, int GrabbingPWMPin, int FourBarPotentiometerPin, int GrabbingPotentiometerPin): 
    fourBarServo(FourBarPWMPin, FourBarPotentiometerPin),
    grabbingServo(GrabbingPWMPin, GrabbingPotentiometerPin)
    {}

void Claw::begin(){
    //Start Servos
    fourBarServo.begin();
    grabbingServo.begin();
}

void Claw::readAngles(){
    Serial.print("Four Bar Angle: ");
    Serial.print(fourBarServo.measure_angle());
    Serial.print(", Grabbing Angle: ");
    Serial.println(grabbingServo.measure_angle());
}