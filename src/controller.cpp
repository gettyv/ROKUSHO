#include <Arduino.h>
#include <constants.h>
#include <controller.h>

Controller::Controller(float Kp, float Kd) {
    _Kp = Kp;
    _Kd = Kd;
    _prev_error = 0;
}

float Controller::update(float error) {
    float derivative = error - _prev_error;
    _prev_error = error;
    return _Kp * error + _Kd * derivative;
}