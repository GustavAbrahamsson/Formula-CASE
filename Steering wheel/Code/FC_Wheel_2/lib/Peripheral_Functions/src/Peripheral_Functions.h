#pragma once

#include <Arduino.h>


// Encapsulate the different states of a digital input
template <typename Func>
struct Peripheral_Digital{
  Func assigned_function;
  bool current_val;
  bool raw_val;
  bool last_val;
  uint16_t input_streak;
  bool debounce_state;
};

// Encapsulate the different states of a digital input
struct Peripheral_Analog{
  uint16_t current_val;
  uint16_t old_val;
  uint16_t min_val;
  uint16_t max_val;
  uint16_t measurement;
};

// Gear shifting paddles (digital)
extern void paddle_R_function(bool state);
extern void paddle_L_function(bool state);

extern Peripheral_Digital<decltype(&paddle_R_function)> Paddle_R;
extern Peripheral_Digital<decltype(&paddle_L_function)> Paddle_L;

// Throttle and braking (analog)
extern Peripheral_Analog throttle_trigger;
extern Peripheral_Analog brake_trigger;