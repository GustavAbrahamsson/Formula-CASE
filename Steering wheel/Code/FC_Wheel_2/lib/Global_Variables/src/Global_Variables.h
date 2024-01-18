#include <Arduino.h>
#include "TFT_eSPI.h"
#include "TLC_LED_Array.h"
#include "FC_Display.h"
#include <MPU6050_tockn.h>

#pragma once

// Pins
#define BAT_LEVEL_PIN GPIO_NUM_1
#define VOLUME_PIN GPIO_NUM_2

#define BTN1_PIN GPIO_NUM_4
#define BTN2_PIN GPIO_NUM_5
#define BTN3_PIN GPIO_NUM_6
#define BTN4_PIN GPIO_NUM_7

#define I2C_SDA GPIO_NUM_8
#define I2C_SCL GPIO_NUM_9

#define L_TRIGGER_PIN GPIO_NUM_17
#define R_TRIGGER_PIN GPIO_NUM_18

#define BUZZ_2_PIN GPIO_NUM_41
#define BUZZ_1_PIN GPIO_NUM_40
#define BZR_1_CHANNEL 0
#define BZR_2_CHANNEL 5

#define L_PADDLE_PIN GPIO_NUM_39
#define R_PADDLE_PIN GPIO_NUM_38

#define ENC_B_PIN GPIO_NUM_48
#define ENC_A_PIN GPIO_NUM_47
#define ENC_SW_PIN GPIO_NUM_21

#define LED_RESET GPIO_NUM_42
#define LED_DRIVER_ADDR 0x60

#define IMU_ADDR 0x68
#define IMU_ACC_RANGE MPU6050_RANGE_2_G
#define IMU_GYR_RANGE MPU6050_RANGE_250_DEG
#define IMU_BANDW MPU6050_BAND_5_HZ
#define IMU_HP_FILTER MPU6050_HIGHPASS_5_HZ

// Task frequencies
#define BUTTON_TASK_FREQ 50 // Hz
#define BUTTON_TASK_CORE 0 // 0 or 1

#define TRIGGER_TASK_FREQ 50 // Hz
#define TRIGGER_TASK_CORE 0

#define IMU_TASK_FREQ 100 // Hz
#define IMU_TASK_CORE 1

// General constants
#define PADDLE_DEBOUNCE_STREAK 5

extern const float battery_voltage_coeff;

#define GEAR_INDEX_MAX 8

#define TRIG_LP_ALPHA 0.4 // LP-filter alpha for the throttle and brake

// 15-LED array object
// All 15 LEDs at max brightness: 87 mA @ 4.03 V
extern TLC_LED_Array tlc;

// Instance of the display class
extern TFT_eSPI tft_disp;

// Wheel display sending the address
extern FC_Display disp;

// IMU
extern MPU6050 imu;
extern float wheel_angle; // deg

// From calibration!
extern const float gyr_x_offset;
extern const float gyr_y_offset;
extern const float gyr_z_offset;

extern uint16_t throttle_R;
extern uint16_t pitch;
extern bool btn1_state;
extern uint8_t current_gear;
extern bool paddle_R;
extern bool paddle_L;
extern bool new_paddle_R;
extern bool new_paddle_L;
extern uint32_t last_paddle_L_time;
extern uint32_t last_paddle_R_time;