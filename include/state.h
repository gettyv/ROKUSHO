#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
class State{
    public:
        State();
        String log();
        String log_header();
        void reset();

        // State Variables
        unsigned long time_ms;
        int current_function;
        int position;
        bool left_low_reflectance;
        bool right_low_reflectance;
        float error;
        float controller_output;
        float left_speed;
        float right_speed;

    private:
        String header = "time_ms,current_function,position,left_low_reflectance,right_low_reflectance,error,controller_output,left_speed,right_speed\n";
};

#endif