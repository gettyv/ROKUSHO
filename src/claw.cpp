#include <Arduino.h>
#include <claw.h>
#include <Servo.h>
#include <constants.h>

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

int Claw::getFourBarAngle(){
    return fourBarServo.measure_angle();
}

int Claw::getGrabbingAngle(){
    return grabbingServo.measure_angle();
}

void Claw::setFourBarAngle(int angle){
    fourBarServo.set_angle(angle);
}

void Claw::setGrabbingAngle(int angle){
    grabbingServo.set_angle(angle);
}

// Simple clamping function
template <typename T>
T clamp(T value, T minVal, T maxVal) {
  if (value < minVal) {
    return minVal;
  } else if (value > maxVal) {
    return maxVal;
  } else {
    return value;
  }
}

void Claw::grabDisc(int disc){

  // bring bar to platform
  for (int angle = disc; angle >= platform; angle--) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);
 
  // grab disc
  for (int angle = grab_open; angle >= grab_close; angle--) {
    int clampedAngle = clamp(angle, grab_min, grab_max);
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle      
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);

  // bring disc back to cup
  for (int angle = platform; angle <= disc; angle++) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    setFourBarAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);

  // drop disc
  for (int angle = grab_close; angle <= grab_open; angle++) {
    int clampedAngle = clamp(angle, grab_min, grab_max);
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);
 
}

void Claw::releaseDisc(){
  
    //bring bar towards patty
    for (int angle = bar_min; angle <= bar_max; angle++) {
      setFourBarAngle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }
 
    delay(500);

    //grab patty
    for (int angle = grab_open; angle <= grab_close; angle++) {
      setGrabbingAngle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }

    delay(500);

    //bring bar back
    for (int angle = bar_max; angle >= bar_min; angle--) {
      setFourBarAngle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }
 
    delay(500);

    //release patty
    for (int angle = grab_close; angle >= grab_open; angle--) {
      setGrabbingAngle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }
    
}


void Claw::fourBarSweep(){

  // Sweep the four bar servo back and forth
   for (int PWM = bar_min; PWM <= bar_max; PWM++) {
    setFourBarAngle(PWM);   // Set servo angle
    Serial.print(PWM);
    Serial.print(",");
    Serial.println(getFourBarAngle());
    delay(SWEEP_DELAY);     // Wait before changing angle
  }

  delay(1000);

  //bring bar back
  for (int PWM = bar_max; PWM >= bar_min; PWM--) {
    setFourBarAngle(PWM);   // Set servo angle
    Serial.print(PWM);
    Serial.print(",");
    Serial.println(getFourBarAngle());
    delay(SWEEP_DELAY);     // Wait before changing angle
  }

  delay(1000);
}

void Claw::grabberSweep(){

    // Sweep the four bar servo back and forth
   for (int PWM = grab_min; PWM <= grab_max; PWM++) {
    setGrabbingAngle(PWM);   // Set servo angle
    Serial.print(PWM);
    Serial.print(",");
    Serial.println(getGrabbingAngle());
    delay(SWEEP_DELAY);     // Wait before changing angle
  }

  delay(1000);

  //bring bar back
  for (int PWM = grab_max; PWM >= grab_min; PWM--) {
    setGrabbingAngle(PWM);   // Set servo angle
    Serial.print(PWM);
    Serial.print(",");
    Serial.println(getGrabbingAngle());
    delay(SWEEP_DELAY);     // Wait before changing angle
  }

  delay(1000);

}