#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include <constants.h>

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
        uint16_t ssr[num_line_sensors];
        int counted_left_junctions;
        int counted_right_junctions;
        int counted_T_junctions;
        int slow_cycles;
        int straight_cycles;
        int left_limit_switch;
        int right_limit_switch;
        int disk_num;

    private:
    String header = "time_ms,current_function,position,left_low_reflectance,right_low_reflectance,error,controller_output,left_speed,right_speed,counted_left_junctions,counted_right_junctions,counted_T_junctions,slow_cycles,straight_cycles,left_limit_switch,right_limit_switch,ssr1,ssr2,ssr3,ssr4,ssr5,ssr6,ssr7,ssr8,ssr9,ssr10,ssr11,ssr12,ssr13\n";
            // String header = "time_ms,current_function,position,left_low_reflectance,right_low_reflectance,error,controller_output,left_speed,right_speed,ssr1,ssr2,ssr3,ssr4,ssr5,ssr6,ssr7,ssr8,ssr9,ssr10,ssr11,ssr12,ssr13\n";
        // String header = "time_ms,current_function,position,left_low_reflectance,right_low_reflectance,error,controller_output,left_speed,right_speed\n";
};

#endif