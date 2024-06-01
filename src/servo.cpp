#include <Arduino.h>
#include <constants.h>
#include <servo.h>

Servo::Servo(int pwm_pin, int angle_pin)
{
    _pwm_pin = pwm_pin;
    _angle_pin = angle_pin;

}

void Servo::begin()
{
    // Setup Potentiometers
    pinMode(_pwm_pin, OUTPUT);
    pinMode(_angle_pin, INPUT);
}

void Servo::set_angle(int angle)
{
    analogWrite(_pwm_pin, angle);
}

float Servo::measure_angle()
{
    int angle = analogRead(_angle_pin);
    return map(angle, 0, 1023, 0, 180);
}