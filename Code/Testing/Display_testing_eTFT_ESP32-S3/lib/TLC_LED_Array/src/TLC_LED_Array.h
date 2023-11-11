// Class definition for the 15-LED RPM gauge using
// a TLC59116 16CH I2C LED Driver 

#ifndef TLC_LED_Array_H
#define TLC_LED_Array_H

#include "TLC59116.h"

#define TLC_NUM_LEDS 15

#define TLC_NUM_RED_LEDS 5
#define TLC_NUM_GREEN_LEDS 5
#define TLC_NUM_BLUE_LEDS 5

#define TLC_RED_LEDS_START 0
#define TLC_GREEN_LEDS_START 5
#define TLC_BLUE_LEDS_START 10

#define TLC_BLUE_GREEN_SCALING 0.8

class TLC_LED_Array{
  private:
    int address;
    uint8_t reset_pin;
    uint8_t global_brightness = 127;
    TLC59116 tlc;
    int8_t current_ramp_LED = -1;

  public:
    TLC_LED_Array(int addr, uint8_t reset);
    void begin();
    int set_LED(uint8_t i_led, uint8_t val);
    int set_all_LEDs(uint8_t val);
    int reset_LEDs();
    int set_LEDs_by_color(String color, uint8_t val);
    int ramp_set(uint8_t level, bool blue_fixed);
    int ramp_set(uint8_t level);
};

#endif