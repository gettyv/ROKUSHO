#include <Arduino.h>
#include <state.h>

State::State(){
    time_ms = 0;
    current_function = -1;
    position = 0;
    left_low_reflectance = false;
    right_low_reflectance = false;
    error = 0;
    controller_output = 0;
    left_speed = 0;
    right_speed = 0;
    for (int i = 0; i < num_line_sensors; i++) {
        ssr[i] = 0;
    }
    counted_left_junctions = 0;
    counted_right_junctions = 0;
    counted_T_junctions = 0;
    slow_cycles = 0;
    straight_cycles = 0;
}

void State::reset(){
    time_ms = 0;
    current_function = -1;
    position = 0;
    left_low_reflectance = false;
    right_low_reflectance = false;
    error = 0;
    controller_output = 0;
    left_speed = 0;
    right_speed = 0;
    for (int i = 0; i < num_line_sensors; i++) {
        ssr[i] = 0;
    }
    counted_left_junctions = 0;
    counted_right_junctions = 0;
    counted_T_junctions = 0;
    slow_cycles = 0;
    straight_cycles = 0;
}

String State::log(){
    String logString = String(time_ms) + "," + current_function + "," + String(position) + "," + String(left_low_reflectance) + "," + String(right_low_reflectance) + "," + String(error) + "," + String(controller_output) + "," + String(left_speed) + "," + String(right_speed) + "," + String(counted_left_junctions) + "," + String(counted_right_junctions) + "," + String(counted_T_junctions) + "," + String(slow_cycles) + "," + String(straight_cycles);
    for (int i = 0; i < num_line_sensors; i++) {
        logString += "," + String(ssr[i]);
    }
    logString += "\n";
    return logString;
}

String State::log_header(){
    return header;
}