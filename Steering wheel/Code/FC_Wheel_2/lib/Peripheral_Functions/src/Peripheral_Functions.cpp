#include "Peripheral_Functions.h"
#include "Global_Variables.h"

void paddle_R_function(bool state){
  if(state && (current_gear < GEAR_INDEX_MAX)){
    current_gear++;
    disp.change_gear(current_gear);
  }
}

void paddle_L_function(bool state){
  if(state && (current_gear > -1)){
    current_gear--;
    disp.change_gear(current_gear);
  }
}

Peripheral_Digital<decltype(&paddle_R_function)> Paddle_R = {
  &paddle_R_function,  // assigned_function
  0,          // current_val
  0,          // raw_val
  0,          // last_val
  1,          // input_streak
  0           // debounce_state
};

Peripheral_Digital<decltype(&paddle_L_function)> Paddle_L = {
  &paddle_L_function,  // assigned_function
  0,          // current_val
  0,          // raw_val
  0,          // last_val
  1,          // input_streak
  0           // debounce_state
};

Peripheral_Analog throttle_trigger = {
  0,
  0,
  1500,       // min_val <--- TODO: Needs fixing
  2000,       // max_val <--- TODO: Needs fixing
  0,
};

Peripheral_Analog brake_trigger = {
  0,
  0,
  1500,       // min_val <--- TODO: Needs fixing
  2000,       // max_val <--- TODO: Needs fixing
  0,
};