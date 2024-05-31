#include <Arduino.h>
#include <claw.h>
#include <servo.h>
#include <constants.h>

Claw::Claw(int FourBarPWMPin, int GrabbingPWMPin, int FourBarPotentiometerPin, int GrabbingPotentiometerPin): 
    fourBarServo(FourBarPWMPin, FourBarPotentiometerPin),
    grabbingServo(GrabbingPWMPin, GrabbingPotentiometerPin)
    {
      incomingByte = "";
      command = "";
      fourBarAngle = disc1;
      grabbingAngle = grab_close;
    }

void Claw::begin(){
    //Start Servos
    fourBarServo.begin();
    grabbingServo.begin();

    // Set initial angles
    // Serial.println("Setting initial angles");
    // fourBarServo.set_angle(disc1);
    // delay(ACTION_DELAY);
    // grabbingServo.set_angle(grab_open);
    // delay(ACTION_DELAY);
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

void Claw::sendManual(){
  
  if(Serial.available()>0){
    incomingByte = Serial.readString();
    if (incomingByte == "Y") {
      Serial.print("sending ");
      Serial.println(command.toInt());
      fourBarServo.set_angle(command.toInt());
      command = "";
    } else {
      command.concat(incomingByte);
      Serial.println(command);
    }
  }
  
}
void Claw::grabDisc(int disc){

  Serial.println("Opening Grabbers");
  for (int angle = grabbingAngle; angle <= grab_open; angle++) {
    int clampedAngle = clamp(angle, grab_min, grab_max);
    grabbingAngle = clampedAngle;
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle      
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(ACTION_DELAY);

  // bring bar to platform
  Serial.println("Moving bar to platform");
  for (int angle = fourBarAngle; angle >= platform; angle--) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    fourBarAngle = clampedAngle;
    setFourBarAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(ACTION_DELAY);
 
  // grab disc
  Serial.println("Closing Grabbers");
  for (int angle = grabbingAngle; angle >= grab_close; angle--) {
    int clampedAngle = clamp(angle, grab_min, grab_max);
    grabbingAngle = clampedAngle;
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle      
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(ACTION_DELAY);

  // bring disc back to cup
  Serial.println("Moving disc to cup");
  for (int angle = fourBarAngle; angle <= disc; angle++) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    fourBarAngle = clampedAngle;
    setFourBarAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }
 
}

void Claw::releaseDisc(){

  //open grabber
  Serial.println("Opening Grabbers");
  for (int angle = grabbingAngle; angle <= grab_open; angle++) {
    int clampedAngle = clamp(angle, grab_min, grab_max);
    grabbingAngle = clampedAngle;
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle      
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(ACTION_DELAY);

  // bring disc back to cup
  Serial.println("Moving disc to cup");
  for (int angle = fourBarAngle; angle <= disc1; angle++) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    fourBarAngle = clampedAngle;
    setFourBarAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(ACTION_DELAY);

  //close grabber
  Serial.println("Closing Grabbers");
  for (int angle = grabbingAngle; angle >= grab_close; angle--) {
    int clampedAngle = clamp(angle, grab_min, grab_max);
    grabbingAngle = clampedAngle;
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle      
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(ACTION_DELAY);

  // bring bar to platform
  Serial.println("Moving bar to platform");
  for (int angle = fourBarAngle; angle >= platform; angle--) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    fourBarAngle = clampedAngle;
    setFourBarAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(ACTION_DELAY);

  //open grabber
  Serial.println("Opening Grabbers");
  for (int angle = grabbingAngle; angle <= grab_open; angle++) {
    int clampedAngle = clamp(angle, grab_min, grab_max);
    grabbingAngle = clampedAngle;
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle      
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(ACTION_DELAY);

  // bring bar back to cup
  Serial.println("Moving disc to cup");
  for (int angle = fourBarAngle; angle <= disc1; angle++) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    fourBarAngle = clampedAngle;
    setFourBarAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
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

  delay(ACTION_DELAY);

  //bring bar back
  for (int PWM = bar_max; PWM >= bar_min; PWM--) {
    setFourBarAngle(PWM);   // Set servo angle
    Serial.print(PWM);
    Serial.print(",");
    Serial.println(getFourBarAngle());
    delay(SWEEP_DELAY);     // Wait before changing angle
  }

  delay(ACTION_DELAY);
}

void Claw::grabberSweep(){

    // Sweep the four bar servo back and forth
   for (int PWM = grab_min; PWM <= grab_max; PWM++) {
    grabbingServo.set_angle(PWM);   // Set servo angle
    Serial.print(PWM);
    Serial.print(",");
    Serial.println(getGrabbingAngle());
    delay(SWEEP_DELAY);     // Wait before changing angle
  }

  delay(ACTION_DELAY);

  //bring bar back
  for (int PWM = grab_max; PWM >= grab_min; PWM--) {
    grabbingServo.set_angle(PWM);   // Set servo angle
    Serial.print(PWM);
    Serial.print(",");
    Serial.println(getGrabbingAngle());
    delay(SWEEP_DELAY);     // Wait before changing angle
  }

  delay(ACTION_DELAY);

}

