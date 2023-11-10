#ifndef TLC_LED_Array_H
#define TLC_LED_Array_H

#include "TLC59116.h"

class TLC_LED_Array{
  private:
    int address;
    uint8_t global_brightness = 127;
    float blue_green_scaling = 0.8;
    TLC59116 tlc;


  public:
    TLC_LED_Array(int addr);

    int begin();

    void set_LED(uint8_t i_led, uint8_t brightness);


};

#endif