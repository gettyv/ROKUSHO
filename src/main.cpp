#include <Arduino.h>

#include <motor.h>
#include <constants.h>
#include <fns.h>
#include <rpi.h>
#include <state.h>

#include <QTRSensors.h>
#include <controller.h>
#include <claw.h>


enum JunctionType {
  NONE,
  LEFT,
  RIGHT,
  T,
};

State state;
RPI rpi(19200);

Claw grabber(four_bar_pwm_pin, claw_pwm_pin, four_bar_potentiometer_pin, claw_potentiometer_pin);
QTRSensors lf;
Controller base_controller(Kp, Kd);
Motor motors[] = {Motor(m_pin[0][0], m_pin[0][1]), 
Motor(m_pin[1][0], m_pin[1][1]), 
Motor(m_pin[2][0], m_pin[2][1]), 
Motor(m_pin[3][0], m_pin[3][1])};

int dropoff_location = 6;
bool pickup_location[3][2] = {{1, 0}, 
                              {1, 0}, 
                              {1, 0}};
int double_grab = 0;


int return_junction(bool left_readings[readings], bool right_readings[readings]) {
  bool right_is_black = false;
  bool left_is_black = false;

  int right_tally = 0;
  int left_tally = 0;

  for (int i = 0; i < readings; i++){
    if (right_readings[i]) right_tally++;
    if (left_readings[i]) left_tally++;
  }

  if (right_tally >= 1 && left_tally >=1) return T;
  else if (right_tally >= readings) return RIGHT;
  else if (left_tally >= readings) return LEFT;
  else return NONE;
}


void setup() {
  rpi.begin();
  if (rpi.wait_rpi_ready(1e6) == 1) {
    // Timeout, hold forever
    while (true) {delay(10000000);}
  }
  lf.setTypeAnalog();
  lf.setSensorPins(line_follower_pins, num_line_sensors);

  pinMode(limit_switch_left, INPUT_PULLUP);
  pinMode(limit_switch_right, INPUT_PULLUP);

  grabber.begin();

  

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

bool switchPressed_L = false;
bool switchPressed_R = false;
bool switchPressed = false;


int reflectance_counter = 0;

bool left_reflectance_readings[3] = {false, false, false};
bool right_reflectance_readings[3] = {false, false, false};

void loop() {
  state.time_ms = millis();

  state.left_limit_switch = digitalRead(limit_switch_left);   // 1 if not pressed 0 if pressed
  state.right_limit_switch = digitalRead(limit_switch_right); // 1 if not pressed 0 if pressed

  uint16_t sensors[num_line_sensors];
  state.position = lf.readLineBlack(sensors);

  memcpy(state.ssr, sensors, sizeof(sensors));
  
  state.left_low_reflectance = true;
  state.right_low_reflectance = true;

  int right_turn_cutoff_index = 4;
  int left_turn_cutoff_index = 8;

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

left_reflectance_readings[reflectance_counter] = state.left_low_reflectance;
right_reflectance_readings[reflectance_counter] = state.right_low_reflectance;
reflectance_counter++;
if (reflectance_counter >= readings) reflectance_counter = 0;


  if (state.slow_cycles > 0) {
    state.slow_cycles--;
  }

  else {
    switch (state.current_function) {
      case 0: // Normal Line Following
        switch (return_junction(left_reflectance_readings, right_reflectance_readings)) {
          case LEFT: // 90 degree left turn
            state.current_function = 1;
            state.counted_left_junctions++;
            state.slow_cycles = 10;
            break;
          case RIGHT: // 90 degree right turn for manuvering
            state.slow_cycles = 10;
            state.counted_right_junctions++;
            if (state.counted_right_junctions == dropoff_location){
              state.current_function = 22;
            }
            break;
          case T: // T junction
            // state.current_function = 3;
            motors[0].set_speed(4);
            motors[1].set_speed(4);
            motors[2].set_speed(4);
            motors[3].set_speed(4);
            delay(0.09e3);
            state.counted_T_junctions++;
            state.slow_cycles = 15;

            //Decide which direction to go based on input
            if (pickup_location[state.counted_T_junctions-1][0] && pickup_location[state.counted_T_junctions-1][1]) {
              // Mark that grabbing both
              double_grab = 1;
              // Pick up left side first
              state.current_function = 11;
            }
            else if (pickup_location[state.counted_T_junctions-1][0]) {
              // Pick up left side
              state.current_function = 11;
            }
            else if (pickup_location[state.counted_T_junctions-1][1]) {
              // Pick up right side
              state.current_function = 22;
            }
            break;
          case NONE:
            break;
          }
        break;
      case 1: // 90 degree left turn for manuv
        {
        int left_solid_sensor_readings = 0;
        for (int i = 0 ; i < num_line_sensors; i++) {
          if (sensors[i] > 800) left_solid_sensor_readings++;
        }
        if (left_solid_sensor_readings >= 3) state.current_function = 0;
        break;
        }
      case 11: // 90 degree left turn for objective
        {
        int left_solid_sensor_readings = 0;
        for (int i = 0; i < num_line_sensors; i++) {
          if (sensors[i] > 800) left_solid_sensor_readings++;
        }
        if (left_solid_sensor_readings >= 3) state.current_function = 10;
        }
        break;
      case 10:
        if (!state.left_limit_switch || !state.right_limit_switch) {
          motors[0].set_speed(0);
          motors[1].set_speed(0);
          motors[2].set_speed(0);
          motors[3].set_speed(0);
          
          switch (state.disk_num) {
            case 0:
              grabber.grabDisc(disc_positions[state.disk_num]);
              state.disk_num++;
              break;
            case 1:
              grabber.grabDisc(disc_positions[state.disk_num]);
              state.disk_num++;
              break;
            case 2:
              grabber.grabDisc(disc_positions[state.disk_num]);
              state.disk_num++;
              state.straight_cycles = 8e3 / 50;
              break;
            case 3:
              grabber.releaseDisc();
          }

          state.current_function = 13;
        }
        break;

      case 13: // Reversing from obj
      {
        if (state.left_low_reflectance && state.right_low_reflectance) {
          motors[0].set_speed(7);
          motors[1].set_speed(7);
          motors[2].set_speed(7);
          motors[3].set_speed(7);
          delay(200);
          state.slow_cycles = 15;
          state.current_function = 2;
        }
        break;
      }

      case 2: // 90 degree right turn for manuv
        {
        int right_solid_sensor_readings = 0;
        for (int i = 0; i < num_line_sensors; i++) {
          if (sensors[i] > 800) right_solid_sensor_readings++;
        }
        if (right_solid_sensor_readings >= 3) {
          // Handle case if grabbing both discs
          if (double_grab) {
            state.slow_cycles = 15;
            state.current_function = 22;
            double_grab--;
          }
          else {
            state.current_function = 0;
          }
          }
        }
        break;

      case 22: // 90 degree right turn for objective
        {
        int right_solid_sensor_readings = 0;
        for (int i = 0; i < num_line_sensors; i++) {
          if (sensors[i] > 800) right_solid_sensor_readings++;
        }
        if (right_solid_sensor_readings >= 3) state.current_function = 20;
        }
        break;

      case 20: // Line follow until wall then grab or release
      {
        if (!state.left_limit_switch || !state.right_limit_switch) {
          motors[0].set_speed(0);
          motors[1].set_speed(0);
          motors[2].set_speed(0);
          motors[3].set_speed(0);
          
          switch (state.disk_num) {
            case 0:
              grabber.grabDisc(disc_positions[state.disk_num]);
              state.disk_num++;
              break;
            case 1:
              grabber.grabDisc(disc_positions[state.disk_num]);
              state.disk_num++;
              break;
            case 2:
              grabber.grabDisc(disc_positions[state.disk_num]);
              state.disk_num++;
              state.straight_cycles = 8e3 / 50;
              break;
            case 3:
              grabber.releaseDisc();
          }

          state.current_function = 23;
        }
        break;
      }
      case 23: // Reversing from obj
      {
        if (state.left_low_reflectance && state.right_low_reflectance) {
          // Fudge factor turn to make turning easier
          motors[0].set_speed(7);
          motors[1].set_speed(7);
          motors[2].set_speed(7);
          motors[3].set_speed(7);
          delay(200);
          state.slow_cycles = 15;
          state.current_function = 1;
        }
        break;
      }
      case 3: // blind backwards
        break;
      default:
        state.current_function = -1;
        // delay(1e6);
        break;
    }
  }

  switch (state.current_function) {
    case 0: // Normal Line Following
    case 20:
    case 10:
      state.error = state.position - line_center_position;
      state.controller_output = base_controller.update(state.error);

      // Go faster for straight cycles
      if (state.straight_cycles > 0) {
        state.straight_cycles--;
        state.base_speed = 10;
        }
      else state.base_speed = base_speed;
      state.left_speed = clamp(state.base_speed + state.controller_output, -clamp_max_speed, clamp_max_speed);
      state.right_speed = clamp(state.base_speed - state.controller_output, -clamp_max_speed, clamp_max_speed);
      
      // Stop for dynamic obstacle
      if (!state.left_limit_switch || !state.right_limit_switch) {
        state.left_speed = 0;
        state.right_speed = 0;
      }
      break;
    case 23: // Reverse Line Follow
    case 13:
      state.error = state.position - line_center_position;
      state.controller_output = base_controller.update(state.error);

      // Update controller but still just go straight back
      state.left_speed = -3;
      state.right_speed = -3;
      break;
    case 1: // 90 degree left turn
    case 11:

      state.left_speed = clamp(turn_speed, -clamp_max_speed, clamp_max_speed);
      state.right_speed = clamp(-turn_speed, -clamp_max_speed, clamp_max_speed);
      break;
    case 2: // 90 degree right turn
    case 22:
      state.left_speed = -turn_speed;
      state.right_speed = turn_speed;
      break;
    case 3: //placing the object
      state.left_speed = 0;
      state.right_speed = 0;
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


