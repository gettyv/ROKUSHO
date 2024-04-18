#include <Arduino.h>
#include <constants.h>
#include <motor.h>

Motor::Motor(int fwd_pin, int rev_pin)
{
    _fwd_pin = fwd_pin;
    _rev_pin = rev_pin;
    pinMode(_fwd_pin, OUTPUT);
    pinMode(_rev_pin, OUTPUT);
}

float Motor::set_speed(float speed)
{
    float pwm_command = map(abs(speed), 0, max_speed, 0, 255);
    if (speed > 0){
        analogWrite(_fwd_pin, pwm_command);
        analogWrite(_rev_pin, 0);
    }
    else if (speed < 0){
        analogWrite(_fwd_pin, 0);
        analogWrite(_rev_pin, pwm_command);
    }
    else{
        analogWrite(_fwd_pin, 0);
        analogWrite(_rev_pin, 0);
    }
}