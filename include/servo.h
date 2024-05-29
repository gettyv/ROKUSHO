#ifndef SERVO_H
#define SERVO_H

class Servo {
    public:
        Servo(int pwm_pin, int angle_pin);
        void begin();
        void set_angle(int angle);
        float measure_angle();

    private:
        int _angle_pin;
        int _pwm_pin;
};
#endif