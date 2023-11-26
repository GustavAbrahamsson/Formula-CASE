#include <Arduino.h>
#include "Global_Variables.h"
#include "TFT_eSPI.h"
#include "TLC_LED_Array.h"
#include "FC_Display.h"


// 15-LED array object
// All 15 LEDs at max brightness: 87 mA @ 4.03 V
TLC_LED_Array tlc(LED_DRIVER_ADDR, LED_RESET);

// Instance of the display class
TFT_eSPI tft_disp = TFT_eSPI();

// Wheel display sending the address
FC_Display disp(&tft_disp);

const float battery_voltage_coeff = 4.05 / 3800.0;

uint16_t throttle_R;
uint16_t pitch;
bool btn1_state;

uint8_t current_gear = 0;
bool paddle_R = 0;
bool paddle_L = 0;
bool new_paddle_R = 0;
bool new_paddle_L = 0;
uint32_t last_paddle_L_time = 0;
uint32_t last_paddle_R_time = 0;