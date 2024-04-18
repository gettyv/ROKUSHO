#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller {
public:
    Controller(float Kp, float Kd);
    float update(float error);
private:
    float _Kp;
    float _Kd;
    float _prev_error;

};

#endif