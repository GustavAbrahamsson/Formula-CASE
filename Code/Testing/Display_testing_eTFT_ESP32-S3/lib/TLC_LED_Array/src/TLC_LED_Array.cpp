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

/* Set the brightness of a specific LED
@param i_led Index of the LED
@param val Brightness
*/
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

// Set all LEDs to a specific brightness
// @param val Brightness
int TLC_LED_Array::set_all_LEDs(uint8_t val){
  if (val > 255){
    return -1;
  }

  for(int i = 0; i < TLC_NUM_LEDS; i++){
    TLC_LED_Array::tlc.analogWrite(i, val);
  }

  return 0;
}

// Set all LEDs to 0
int TLC_LED_Array::reset_LEDs(){
  return TLC_LED_Array::set_all_LEDs(0);
}

/* Set LEDs by color. E.g. "set all blue LEDs to 255".
  @param color: "red", "green" or "blue"
  @param val Brightness
*/
int TLC_LED_Array::set_LEDs_by_color(String color, uint8_t val){
  uint8_t start_index;
  
  if(color == "red"){
    start_index = TLC_RED_LEDS_START;
  }
  else if(color == "green"){
    start_index = TLC_GREEN_LEDS_START;
  }
  else if(color == "blue"){
    start_index = TLC_BLUE_LEDS_START;
  }
  else{
    // Incorrect string input
    return -1;
  }

  // Assume equal number of LEDs per color
  for(int i = 0; i < TLC_NUM_LEDS / 3; i++){
    TLC_LED_Array::set_LED(start_index + i, val);
  }

  return 0;
}

/* Set LEDs to light up until specified percentage
  @param level Percentage in range [0, 255]
  @param blue_fixed True: the blue LEDs flash in unison if reached.
  False: blue LEDs act the same as red/green
*/
int TLC_LED_Array::ramp_set(uint8_t level, bool blue_fixed){
  int8_t target_led = ((float)level/255.0) * (TLC_NUM_LEDS);

  // If target is current level, nothing needs to be done
  if(target_led == TLC_LED_Array::current_ramp_LED){
    return 0;
  }
  // If ramp has not been initialized
  else if(TLC_LED_Array::current_ramp_LED == -1){
    // Enable up to 'target_led'
    for(int i = 0; i < target_led; i++){
      TLC_LED_Array::set_LED(i, global_brightness);
    }
    TLC_LED_Array::current_ramp_LED = target_led;

    // Disable the rest
    for(int i = target_led + 1; i < TLC_NUM_LEDS; i++){
      TLC_LED_Array::set_LED(i, 0);
    }

    return 0;
  }
  // If ramp has already been initialized and needs to be changed
  else if(target_led >= 0 && target_led < TLC_NUM_LEDS && target_led != TLC_LED_Array::current_ramp_LED){
    int8_t diff = target_led - TLC_LED_Array::current_ramp_LED;
    int8_t sign = (0 < diff) - (diff < 0); // sign(x)
    if (sign == 0) return -1;
    Serial.println("diff: " + String(diff));
    Serial.println("sign: " + String(sign));

    uint8_t replacemenet_val = global_brightness;
    if(sign < 0) replacemenet_val = 0;

    // Begin at current level and step in the direction of the target until reached
    for(int i = TLC_LED_Array::current_ramp_LED; i += sign; i < TLC_NUM_LEDS && i > 0){
      if(i < 0 || i > TLC_NUM_LEDS) break;

      TLC_LED_Array::current_ramp_LED = i;

      // Turn on all blue LEDs when one is lit (if requested)
      if(blue_fixed && i >= TLC_BLUE_LEDS_START){
        TLC_LED_Array::set_LEDs_by_color("blue", replacemenet_val);
      }
      // Otherwise, set LEDs as usuual
      else{
        TLC_LED_Array::set_LED(i, replacemenet_val);
      }
      
      Serial.println("target_led: " + String(target_led));
      Serial.println("current_led: " + String(TLC_LED_Array::current_ramp_LED));
      

      // If target_led is reached, break
      if(i == target_led){
        return 0;
      }
    }
    return -1;
  }

  
}

/* Set LEDs to light up until specified percentage
  @param level Percentage in range [0, 255]
*/
int TLC_LED_Array::ramp_set(uint8_t level){
  return TLC_LED_Array::ramp_set(level, false);
}




