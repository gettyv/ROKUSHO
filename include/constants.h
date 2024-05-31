#ifndef CONSTANTS_H
#define CONSTANTS_H
constexpr int num_motors_ps = 2;
constexpr int m_pin[][2] = {{4,5}, {6,7}, {12,13}, {3,2}};

constexpr int calibration_LED_pin = 13;

constexpr const int num_line_sensors = 13;
constexpr unsigned char line_follower_pins[num_line_sensors] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12};
constexpr const int line_center_position = 1000 * (num_line_sensors - 1) / 2;

constexpr float hw_max_speed = 10;
constexpr float clamp_max_speed = 10;
constexpr float base_speed = 6;
constexpr float turn_speed = 10;
constexpr float Kp = 0.01;
constexpr float Kd = 0.0025;

constexpr int calibration_iterations = 100;
#endif