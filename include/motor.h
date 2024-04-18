#ifndef MOTOR_H
#define MOTOR_H
#include <constants.h>

class Motor
{
public:
    Motor(int fwd_pin, int rev_pin);
    float set_speed(float speed);
private:
    int current_speed;
    int _fwd_pin;
    int _rev_pin;
};
#endif
