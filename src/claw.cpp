#include <Arduino.h>
#include <claw.h>
#include <Servo.h>
#include <constants.h>

Claw::Claw(int FourBarPWMPin, int GrabbingPWMPin, int FourBarPotentiometerPin, int GrabbingPotentiometerPin): 
    fourBarServo(FourBarPWMPin, FourBarPotentiometerPin),
    grabbingServo(GrabbingPWMPin, GrabbingPotentiometerPin)
    {
      incomingByte = "";
      command = "";
    }

void Claw::begin(){
    //Start Servos
    fourBarServo.begin();
    grabbingServo.begin();

    // Set initial angles
    // Serial.println("Setting initial angles");
    // fourBarServo.set_angle(disc1);
    // delay(1000);
    // grabbingServo.set_angle(grab_open);
    // delay(1000);
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

  // bring bar to platform
  Serial.println("Moving bar to platform");
  for (int angle = disc; angle >= platform; angle--) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    setFourBarAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);
 
  // grab disc
  Serial.println("Closing Grabbers");
  for (int angle = grab_open; angle >= grab_close; angle--) {
    int clampedAngle = clamp(angle, grab_min, grab_max);
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle      
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);

  // bring disc back to cup
  Serial.println("Moving disc to cup");
  for (int angle = platform; angle <= disc; angle++) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    setFourBarAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);

  // drop disc
  Serial.println("Opening Grabbers");
  for (int angle = grab_close; angle <= grab_open; angle++) {
    int clampedAngle = clamp(angle, grab_min, grab_max);
    setGrabbingAngle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);
 
}

void Claw::releaseDisc(){
  
    //bring bar towards patty
    Serial.println("Moving bar to disc holder");
    setFourBarAngle(disc1);
    /*
    for (int angle = disc1; angle <= bar_max; angle++) {
      setFourBarAngle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }*/
 
    delay(1000);

    //grab patty
    Serial.println("Closing Grabbers");
    for (int angle = grab_open; angle >= grab_close; angle--) {
      setGrabbingAngle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }

    delay(1000);

    //bring bar to platform
    Serial.println("Moving bar to platform");
    for (int angle = disc1; angle >= platform; angle--) {
      setFourBarAngle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }
 
    delay(1000);

    //release patty
    Serial.println("Opening Grabbers");
    for (int angle = grab_close; angle <= grab_open; angle++) {
      setGrabbingAngle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }
    
    delay(1000);

    //bring bar back to disc holder
    Serial.println("Moving bar to disc holder");
    for (int angle = platform; angle <= disc1; angle++) {
      setFourBarAngle(angle);   // Set servo angle
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
    grabbingServo.set_angle(PWM);   // Set servo angle
    Serial.print(PWM);
    Serial.print(",");
    Serial.println(getGrabbingAngle());
    delay(SWEEP_DELAY);     // Wait before changing angle
  }

  delay(1000);

  //bring bar back
  for (int PWM = grab_max; PWM >= grab_min; PWM--) {
    grabbingServo.set_angle(PWM);   // Set servo angle
    Serial.print(PWM);
    Serial.print(",");
    Serial.println(getGrabbingAngle());
    delay(SWEEP_DELAY);     // Wait before changing angle
  }

  delay(1000);

}

