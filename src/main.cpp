#include <Arduino.h>

#include <motor.h>
#include <constants.h>
#include <rpi.h>
#include <state.h>

#include <QTRSensors.h>
#include <controller.h>

State state;
RPI rpi(9600);
QTRSensors lf;
Controller base_controller(Kp, Kd);
Motor motors[] = {Motor(m_pin[0][0], m_pin[0][1]), 
Motor(m_pin[1][0], m_pin[1][1]), 
Motor(m_pin[2][0], m_pin[2][1]), 
Motor(m_pin[3][0], m_pin[3][1])};

bool picked_up = false;
int turning_cycles = 0;
int avoid_cycles = 0;
int dropoff_location = 0;
int dropoff_target = 2;

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

void go_forward_for_time(int duration) {
  unsigned long start_time = millis();
  while (millis() - start_time < duration) {
    motors[0].set_speed(base_speed);
    motors[1].set_speed(base_speed);
    motors[2].set_speed(-base_speed);
    motors[3].set_speed(-base_speed);
  }
  motors[0].set_speed(0);
  motors[1].set_speed(0);
  motors[2].set_speed(0);
  motors[3].set_speed(0);
}

void turn_right() {
  uint16_t sensors[num_line_sensors];
  int16_t previous_position = lf.readLineBlack(sensors);
  
  motors[0].set_speed(turn_speed);
  motors[1].set_speed(turn_speed);
  motors[2].set_speed(-turn_speed);
  motors[3].set_speed(-turn_speed);

  bool is_decreasing = true;
  while (is_decreasing) {
    Serial.println("Decreasing position");
    int16_t position = lf.readLineBlack(sensors);

    // Check if the readlineblack value is decreasing
    if (position >= previous_position) {
      is_decreasing = false;
    }
    previous_position = position;
  }

  // Now the average should be around the center, then snap to the right when it hits the line
  while (true) {
    Serial.println("Can't find position");
    int16_t position = lf.readLineBlack(sensors);
    if (position > line_center_position + 3000) {
      break;
    }
  }

  // Now just need to control to be onto the line
  while (true) {
    Serial.println("Final Turn Stretch");
    int16_t position = lf.readLineBlack(sensors);
    if (abs(position - line_center_position) < 500) {
      break;
    }
  }
  motors[0].set_speed(0);
  motors[1].set_speed(0);
  motors[2].set_speed(0);
  motors[3].set_speed(0);
}


void setup() {
  rpi.begin();
  if (rpi.wait_rpi_ready(1e6) == 1) {
    // Timeout, hold forever
    while (true) {delay(10000000);}
  }
  lf.setTypeAnalog();
  lf.setSensorPins(line_follower_pins, num_line_sensors);


  digitalWrite(calibration_LED_pin, HIGH);
  for (uint8_t i = 0; i < calibration_iterations; i++)
  {
    lf.calibrate();
    delay(20);
    Serial.println(i);
  }
  digitalWrite(calibration_LED_pin, LOW);
  rpi.sendMessage("ARDUINO_READY");
  rpi.sendMessage(state.log_header());
}




void loop() {
  state.time_ms = millis();
  uint16_t sensors[num_line_sensors];
  state.position = lf.readLineBlack(sensors);
  
  state.left_low_reflectance = true;
  state.right_low_reflectance = true;

  for (int i = 0; i < num_line_sensors; i++) {
    if (sensors[i] < 100) {
      
      if (i < num_line_sensors / 2) {
        state.right_low_reflectance = false;
      } else {
        state.left_low_reflectance = false;
      }
    }
  }

  state.error = state.position - line_center_position;
  state.controller_output = base_controller.update(state.error);

  float fwd_speed = base_speed;
  if (state.left_low_reflectance && !state.right_low_reflectance) {
    turning_cycles = 50;
  }
  if (turning_cycles > 0) {
    fwd_speed = 4;
    turning_cycles--;
  }

  // if (allRightLowReflectance && !allLeftLowReflectance) {
  //   dropoff_location++;
  //   if (dropoff_location == dropoff_target) {
  //     controller_output = 30;
  //     int left_speed = clamp(fwd_speed + controller_output, -max_speed, max_speed);
  //     int right_speed = clamp(fwd_speed - controller_output, -max_speed, max_speed);

  //     motors[0].set_speed(-left_speed);
  //     motors[1].set_speed(-left_speed);
  //     motors[2].set_speed(-right_speed);
  //     motors[3].set_speed(right_speed);
  //     delay(1000);
  //     motors[0].set_speed(0);
  //     motors[1].set_speed(0);
  //     motors[2].set_speed(0);
  //     motors[3].set_speed(0);
  //     delay(1e1000);
  //   }
  //   else {
  //     avoid_cycles = 30;
  //   }
  // }

  // if (avoid_cycles > 0) {
  //   controller_output = -3;
  //   avoid_cycles--;
  // }

  state.left_speed = clamp(fwd_speed + state.controller_output, -max_speed, max_speed);
  state.right_speed = clamp(fwd_speed - state.controller_output, -max_speed, max_speed);

  motors[0].set_speed(-state.left_speed);
  motors[1].set_speed(-state.left_speed);
  motors[2].set_speed(-state.right_speed);
  motors[3].set_speed(state.right_speed);

  Serial.print(state.log());
}


