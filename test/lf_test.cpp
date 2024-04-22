#include <Arduino.h>
#include <constants.h>
#include <QTRSensors.h>

QTRSensors lf;

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
Serial.begin(9600);
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
    // for (int i = 0; i < num_line_sensors; i++) {
    //     Serial.print(sensors[i]);
    //     Serial.print(" ");
    // }
    // Serial.println();
    // int16_t position = lf.readLineBlack(sensors);
    Serial.println(position);
}


