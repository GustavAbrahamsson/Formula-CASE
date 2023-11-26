#pragma once

#include <Arduino.h>


// Encapsulate the different states of a digital input
template <typename Func>
struct Input_Peripheral{
  Func assigned_function;
  bool current_val;
  bool raw_val;
  bool last_val;
  uint16_t input_streak;
  bool debounce_state;
};

extern void paddle_R_function(bool state);

extern void paddle_L_function(bool state);

extern Input_Peripheral<decltype(&paddle_R_function)> Paddle_R;
extern Input_Peripheral<decltype(&paddle_L_function)> Paddle_L;


