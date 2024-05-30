#include <Arduino.h>

#include <motor.h>
#include <constants.h>
#include <fns.h>
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

int dropoff_location = 0;
int dropoff_target = 2;

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

  memcpy(state.ssr, sensors, sizeof(sensors));
  
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

  // Decide which operating mode to be in

  // Reached 90 degree left turn
  if ((state.left_low_reflectance && !state.right_low_reflectance) && state.current_function == 0) {
    state.current_function = 1;
    state.counted_left_junctions++;
    state.slow_cycles = 50;
  }

  // Reached 90 degree right turn
  if ((state.right_low_reflectance && !state.left_low_reflectance) && state.current_function == 0) {
    state.current_function = 2;
    state.counted_right_junctions++;
    state.straight_cycles = 0;
  }

  // Reached T junction
  if (state.left_low_reflectance && state.right_low_reflectance) {
    state.current_function = 3;
    state.counted_T_junctions++;
  }

  // Normal Line Following
  else {
    state.current_function = 0;
  }

  float fwd_speed = base_speed;
  
  if (state.slow_cycles > 0) {
    fwd_speed = 4;
    state.slow_cycles--;
  }

  state.left_speed = clamp(fwd_speed + state.controller_output, -clamp_max_speed, clamp_max_speed);
  state.right_speed = clamp(fwd_speed - state.controller_output, -clamp_max_speed, clamp_max_speed);

  if (state.straight_cycles > 0) {
    fwd_speed = 4;
    state.left_speed = fwd_speed;
    state.right_speed = fwd_speed;
    state.straight_cycles--;
  }

  motors[0].set_speed(state.left_speed);
  motors[1].set_speed(state.left_speed);
  motors[2].set_speed(state.right_speed);
  motors[3].set_speed(state.right_speed);

  Serial.print(state.log());
}


