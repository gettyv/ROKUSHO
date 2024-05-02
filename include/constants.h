#ifndef CONSTANTS_H
#define CONSTANTS_H
constexpr int num_motors_ps = 2;
constexpr int m_pin[][2] = {{2,3}, {4,5}, {6,7}, {8,9}};

constexpr int calibration_LED_pin = 13;

constexpr const int num_line_sensors = 13;
constexpr unsigned char line_follower_pins[num_line_sensors] = {A12, A11, A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, A0};
constexpr const int line_center_position = 1000 * (num_line_sensors - 1) / 2;

constexpr float max_speed = 5;
constexpr float base_speed = 3;
constexpr float turn_speed = 3;
constexpr float Kp = 0.0012;
constexpr float Kd = 0.0001;

constexpr int calibration_iterations = 100;
#endif