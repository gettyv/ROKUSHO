#include <Arduino.h>


#include <motor.h>
#include <servo.h>
#include <constants.h>


#include <QTRSensors.h>
#include <controller.h>


QTRSensors lf;
Controller controller(Kp, Kd);
Motor motors[] = {Motor(m_pin[0][0], m_pin[0][1]),
Motor(m_pin[1][0], m_pin[1][1]),
Motor(m_pin[2][0], m_pin[2][1]),
Motor(m_pin[3][0], m_pin[3][1])};


Servo bar_servo = Servo(s_pin[0][0], s_pin[0][1]);
Servo claw_servo = Servo(s_pin[1][0], s_pin[1][1]);


bool picked_up = false;


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


int SWEEP_DELAY = 30;

//four-bar angles
int disc_1 = 180;
int disc_2 = 170;
int disc_3 = 160;
int platform = 130;

int bar_servoPos = 0;
int bar_min = 180;
int bar_max = 80;

//claw angles
int claw_open = 40;
int claw_close = 70;

int claw_servoPos = 0;
int claw_min = 80;
int claw_max = 180;

void grabDisc();
void dropoffDisc();


int minAngle = 80;  // Minimum angle for the servo
int maxAngle = 130;  // Maximum angle for the servo
int startAngle_bar = disc_1;  // Current angle of the servo
int startAngle_claw = claw_open;  // Current angle of the servo


bool switchPressed = false;
bool switchPressed_L = false; // Flag to track switch state change
bool switchPressed_R = false;


void setup() {
  Serial.begin(9600);
  bar_servo.set_angle(startAngle_bar);
  claw_servo.set_angle(startAngle_claw);
  pinMode(LIMIT_SWITCH_PIN_L, INPUT_PULLUP); // Configure the limit switch pin as input with internal pull-up resistor
  pinMode(LIMIT_SWITCH_PIN_R, INPUT_PULLUP);
}


void loop() {
  int true_angle = bar_servo.measure_angle();  // Read the sensor value from analog pin A0 (example pin)
  int limitSwitchState_L = digitalRead(LIMIT_SWITCH_PIN_L);
  int limitSwitchState_R = digitalRead(LIMIT_SWITCH_PIN_R);

  if (limitSwitchState_L == LOW && !switchPressed_L) {
    switchPressed_L = true; // Set the flag
  }

  if (limitSwitchState_R == LOW && !switchPressed_R) {
    switchPressed_R = true; // Set the flag
  }

  if(switchPressed_L && switchPressed_R){
    switchPressed = true;
  }

  if (switchPressed) {
    grabDisc(disc_1);
    switchPressed_L = false;
    switchPressed_R = false;
    switchPressed = false;
  }
}



void grabDisc(int disc){
   
  // bring bar to platform
  for (int angle = disc; angle >= platform; angle--) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    bar_servo.set_angle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);
 
  // grab disc
  for (int angle = claw_open; angle >= claw_close; angle--) {
    int clampedAngle = clamp(angle, claw_min, claw_max);
    claw_servo.set_angle(clampedAngle);  // Move the servo to the current angle      
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);

  // bring disc back to cup
  for (int angle = platform; angle <= disc; angle++) {
    int clampedAngle = clamp(angle, bar_min, bar_max);
    bar_servo.set_angle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);

  // drop disc
  for (int angle = claw_close; angle <= claw_open; angle++) {
    int clampedAngle = clamp(angle, claw_min, claw_max);
    claw_servo.set_angle(clampedAngle);  // Move the servo to the current angle
    delay(SWEEP_DELAY);  // Delay for smoother movement
  }

  delay(1000);

}


void dropoffDisc(){

    //bring bar towards patty
    for (int angle = bar_traj[1]; angle <= bar_traj[2]; angle++) {
      bar_servo.set_angle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }
 
    delay(500);

    //grab patty
    for (int angle = claw_open; angle <= claw_close; angle++) {
      claw_servo.set_angle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }

    delay(500);

    //bring bar back
    for (int angle = bar_traj[2]; angle >= bar_traj[1]; angle--) {
      bar_servo.set_angle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }
 
    delay(500);

    //release patty
    for (int angle = claw_close; angle >= claw_open; angle--) {
      bar_servo.set_angle(angle);   // Set servo angle
      delay(SWEEP_DELAY);     // Wait before changing angle
    }
}











