#include <Arduino.h>

#include <motor.h>
#include <constants.h>

#include <QTRSensors.h>
#include <controller.h>

QTRSensors lf;
Controller controller(Kp, Kd);
Motor motors[] = {Motor(m_pin[0][0], m_pin[0][1]), 
Motor(m_pin[1][0], m_pin[1][1]), 
Motor(m_pin[2][0], m_pin[2][1]), 
Motor(m_pin[3][0], m_pin[3][1])};

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

void setup() {
  lf.setTypeAnalog();
  lf.setSensorPins(line_follower_pins, num_line_sensors);


  digitalWrite(calibration_LED_pin, HIGH);
  for (uint8_t i = 0; i < calibration_iterations; i++)
  {
    lf.calibrate();
    delay(20);
  }
  digitalWrite(calibration_LED_pin, LOW);
}



void loop() {
  uint16_t sensors[num_line_sensors];
  // Get calibrated sensor values returned in the sensors array, along with the
  // line position, which will range from 0 to 2000, with 1000 corresponding to
  // a position under the middle sensor.
  int16_t position = lf.readLineBlack(sensors);
  
  // If all sensors see very low reflectance, take some appropriate action
  // for this situation.
  bool allSensorsLowReflectance = true;
  for (int i = 0; i < num_line_sensors; i++) {
    if (sensors[i] > 750) {
      allSensorsLowReflectance = false;
      break;
    }
  }
  
  if (allSensorsLowReflectance) {
    if (picked_up) {
      // This is when we are at a junction, and we have already picked up the patties
      // Put code here for storing the patties
      return;
    }
    else {
      // This is when we are at a junction, and we have not picked up the patties
      // Put code here for picking up the patties
      picked_up = true;
      return;
    }
    return;
  }

  int error = position - line_center_position;
  float controller_output = controller.update(error);

  int left_speed = clamp(base_speed + controller_output, -max_speed, max_speed);
  int right_speed = clamp(base_speed - controller_output, -max_speed, max_speed);

  motors[0].set_speed(left_speed);
  motors[1].set_speed(left_speed);
  motors[2].set_speed(right_speed);
  motors[3].set_speed(right_speed);

  Serial.print(left_speed);
  Serial.print(" ");
  Serial.println(right_speed);
}


