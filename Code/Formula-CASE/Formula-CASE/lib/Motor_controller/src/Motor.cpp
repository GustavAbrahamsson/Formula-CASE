
#include "Arduino.h"
#include "Motor_controller.h"
Motor::Motor(uint8_t fwr_pin, uint8_t rev_pin, uint8_t pwm_chl, uint16_t pwm_freq, uint8_t pwm_res, bool switch_dir){
  pwm_channel = pwm_chl;
  pwm_frequency = pwm_freq;
  pwm_resolution = pwm_res;

  // If forward pin in hardware is not "forward"
  if(switch_dir){
    reverse_pin = fwr_pin;
    forward_pin = rev_pin;
  }
  else{
    forward_pin = fwr_pin;
    reverse_pin = rev_pin;
  }
}

void Motor::begin(){
  ledcDetachPin(forward_pin);
  ledcDetachPin(reverse_pin);
  pinMode(reverse_pin, OUTPUT);
  digitalWrite(reverse_pin, 0);
  ledcAttachPin(forward_pin, pwm_channel);
  ledcSetup(pwm_channel, pwm_frequency, pwm_resolution);
  last_torque_dir_positive = false;
  forward(0);
}

/* Apply a positive torque (forward)
@param value Torque value [0, 255]
*/
void Motor::forward(uint8_t value){
  // Change pin of channel if last direction was negative
  if(!last_torque_dir_positive){
    ledcDetachPin(reverse_pin);
    ledcAttachPin(forward_pin, pwm_channel);
    pinMode(reverse_pin, OUTPUT);
    digitalWrite(reverse_pin, 0);
    ledcSetup(pwm_channel, pwm_frequency, pwm_resolution);
    last_torque_dir_positive = true;
  }
  ledcWrite(pwm_channel, value);
}

/* Apply a negative torque (reverse)
@param value Torque value [0, 255]
*/
void Motor::reverse(uint8_t value){
  // Change pin of channel if last direction was positive
  if(last_torque_dir_positive){
    ledcDetachPin(forward_pin);
    ledcAttachPin(reverse_pin, pwm_channel);
    digitalWrite(forward_pin, 0);
    last_torque_dir_positive = false;
    ledcSetup(pwm_channel, pwm_frequency, pwm_resolution);
    pinMode(forward_pin, OUTPUT);
  }
  ledcWrite(pwm_channel, value);
}
