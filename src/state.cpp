#include <Arduino.h>
#include <state.h>

State::State(){
    time_ms = 0;
    current_function = "";
    position = 0;
    left_low_reflectance = false;
    right_low_reflectance = false;
    error = 0;
    controller_output = 0;
    left_speed = 0;
    right_speed = 0;
}

void State::reset(){
    time_ms = 0;
    current_function = "";
    position = 0;
    left_low_reflectance = false;
    right_low_reflectance = false;
    error = 0;
    controller_output = 0;
    left_speed = 0;
    right_speed = 0;
}

String State::log(){
    return String(time_ms) + "," + current_function + "," + String(position) + "," + String(left_low_reflectance) + "," + String(right_low_reflectance) + "," + String(error) + "," + String(controller_output) + "," + String(left_speed) + "," + String(right_speed) + "\n";
}

String State::log_header(){
    return header;
}