#include "TLC_LED_Array.h"

TLC_LED_Array::TLC_LED_Array(int addr, uint8_t reset){
  TLC_LED_Array::address = addr;
  TLC_LED_Array::reset_pin = reset;
}

void TLC_LED_Array::begin(){
  // Set the GPIO pin to output and keep it HIGH
  // to make sure that the IC is always enabled
  pinMode(TLC_LED_Array::reset_pin, OUTPUT);
  digitalWrite(TLC_LED_Array::reset_pin, HIGH);

  // Init the actual TLC59116 driver over I2C
  TLC_LED_Array::tlc.begin();
}

// Set the brightness of a specific LED
int TLC_LED_Array::set_LED(uint8_t i_led, uint8_t val){

  if (i_led > 15 || val > 255){
    return -1;
  }

  // Remap to make the perceived brightness more "linear"
  uint8_t output = (float)(val * val) / 255.0;

  // Scale down the green/blue LEDs to match the reds
  if(i_led >= TLC_GREEN_LEDS_START){
    output *= TLC_BLUE_GREEN_SCALING;
  }

  // Set the brightness on the TLC59116
  TLC_LED_Array::tlc.analogWrite(i_led, output);

  return 0;
}

int TLC_LED_Array::set_all_LEDs(uint8_t val){
  if (val > 255){
    return -1;
  }

  
}
