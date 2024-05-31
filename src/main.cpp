#include <Arduino.h>

#include <motor.h>
#include <constants.h>
#include <fns.h>
#include <rpi.h>
#include <state.h>

#include <QTRSensors.h>
#include <controller.h>

State state;
RPI rpi(19200);
QTRSensors lf;
Controller base_controller(Kp, Kd);
Motor motors[] = {Motor(m_pin[0][0], m_pin[0][1]), 
Motor(m_pin[1][0], m_pin[1][1]), 
Motor(m_pin[2][0], m_pin[2][1]), 
Motor(m_pin[3][0], m_pin[3][1])};

int dropoff_location = 6;
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

void turn_left() {
  while(true){
  uint16_t sensors[num_line_sensors];
  int16_t previous_position = lf.readLineBlack(sensors);
  int solid_sensor_readings = 0;
  for (int i = 0; i < num_line_sensors;i++) {
    if (sensors[i] > 900) solid_sensor_readings++;
  }
  if (solid_sensor_readings >= 3) break;
  }
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
  delay(10e3);
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

  int right_turn_cutoff_index = 3;
  int left_turn_cutoff_index = 9;

  for (int i = num_line_sensors-1; i > right_turn_cutoff_index; i--) {
    if (sensors[i] < 700) {
      state.left_low_reflectance = false;
    }
  }

  for (int i = 0; i < left_turn_cutoff_index; i++) {
    if (sensors[i] < 700) {
      state.right_low_reflectance = false;
    }
  }

  if (state.slow_cycles > 0) {
    state.slow_cycles--;
  }

  else {
    switch (state.current_function) {
      case 0: // Normal Line Following
        if (state.left_low_reflectance && !state.right_low_reflectance) {
          state.current_function = 1;
          state.counted_left_junctions++;
          state.slow_cycles = 10;
        }

        // Reached 90 degree right turn
        else if (state.right_low_reflectance && !state.left_low_reflectance) {
          state.slow_cycles = 10;
          state.counted_right_junctions++;
          if (state.counted_right_junctions == dropoff_location){
            state.current_function = 2;
          }
        }

        // Reached T junction
        else if (state.left_low_reflectance && state.right_low_reflectance) {
          // state.current_function = 3;
          state.counted_T_junctions++;
          state.slow_cycles = 10;
        }
        break;

      case 1: // 90 degree left turn
        int left_solid_sensor_readings = 0;
        for (int i = num_line_sensors-1; i > right_turn_cutoff_index;i--) {
          if (sensors[i] > 800) left_solid_sensor_readings++;
        }
        if (left_solid_sensor_readings >= 3) state.current_function = 0;
        break;

      case 2: // 90 degree right turn
        int right_solid_sensor_readings = 0;
        for (int i = 0; i < left_turn_cutoff_index;i++) {
          if (sensors[i] > 800) right_solid_sensor_readings++;
        }
        if (right_solid_sensor_readings >= 3) state.current_function = 0;
        break;

      default:
        state.current_function = -1;
        // delay(1e6);
        break;
    }
  }

  switch (state.current_function) {
    case 0: // Normal Line Following
        state.error = state.position - line_center_position;
        state.controller_output = base_controller.update(state.error);
        state.left_speed = clamp(base_speed + state.controller_output, -clamp_max_speed, clamp_max_speed);
        state.right_speed = clamp(base_speed - state.controller_output, -clamp_max_speed, clamp_max_speed);
      break;
    case 1: // 90 degree left turn

      state.left_speed = clamp(turn_speed, -clamp_max_speed, clamp_max_speed);
      state.right_speed = clamp(-turn_speed, -clamp_max_speed, clamp_max_speed);

      // Code for 90 degree left turn mode
      break;
    case 2: // 90 degree right turn
      state.left_speed = -turn_speed;
      state.right_speed = turn_speed;
      break;
    default:
      state.current_function = -11;
      // delay(1e6);
      break;
  }

  motors[0].set_speed(state.left_speed);
  motors[1].set_speed(state.left_speed);
  motors[2].set_speed(state.right_speed);
  motors[3].set_speed(state.right_speed);

  Serial.print(state.log());
}


