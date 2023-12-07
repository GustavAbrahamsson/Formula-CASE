#pragma once

#include <Arduino.h>

// Motor pins
#define M1_FWR_PIN 37
#define M1_REV_PIN 38
#define M2_FWR_PIN 35
#define M2_REV_PIN 36
#define M3_FWR_PIN 39
#define M3_REV_PIN 40
#define M4_FWR_PIN 41
#define M4_REV_PIN 42

// Assigned PWM channels
#define M1_ASS_PWM_CHL 7
#define M2_ASS_PWM_CHL 3
#define M3_ASS_PWM_CHL 4
#define M4_ASS_PWM_CHL 5

// Motor sense pins ADC
#define M1_SENSE_PIN 1
#define M2_SENSE_PIN 2
#define M3_SENSE_PIN 3
#define M4_SENSE_PIN 4

#define MOTOR_PWM_BIT_RES 8
#define MOTOR_PWM_FREQ 25000

class Motor{
  private:
  uint8_t pwm_channel;
  int16_t pwm_frequency;
  uint8_t pwm_resolution;
  uint8_t forward_pin;
  uint8_t reverse_pin;
  int16_t speed;
  int16_t current;
  bool last_torque_dir_positive;
  float R_a = 1; // From spec, update this
  float L_a = 0.008; // Update this
  float Psi_m = 0.5; // Update this

  public:
  Motor(uint8_t fwr_pin, uint8_t rev_pin, uint8_t pwm_chl, uint16_t pwm_freq, uint8_t pwm_res, bool switch_dir);

  void torque_pos(uint8_t value);
  void torque_neg(uint8_t value);
  void coast();
  void write_Nm(float torque);

  void change_pwm_chnl(uint16_t channel);
  void update_motor_params(float resistance, float inductance, float flux_linkage);

  int read_speed();


};


class Motor_controller{
  private:
  Motor motor_1 = Motor(M1_FWR_PIN, M1_REV_PIN, M1_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES, 0);
  Motor motor_2 = Motor(M2_FWR_PIN, M2_REV_PIN, M2_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES, 1);
  Motor motor_3 = Motor(M3_FWR_PIN, M3_REV_PIN, M3_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES, 0);
  Motor motor_4 = Motor(M4_FWR_PIN, M4_REV_PIN, M4_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES, 1);
  
  Motor* motors[4] = {&motor_1, &motor_2, &motor_3, &motor_4};

  public:
  Motor_controller();

  void torque_pos(int8_t motor, uint8_t value);
  void torque_neg(int8_t motor, uint8_t value);

  int read_speed(uint8_t motor);
};