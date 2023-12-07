
#include "Arduino.h"
#include "Motor_controller.h"

Motor_controller::Motor_controller(){
  
}

/* Apply a positive torque (forward)
@param motor Index of motor
@param value Torque value [0, 255]
*/
void Motor_controller::torque_pos(int8_t motor, uint8_t value){
  motors[motor]->torque_pos(value);
}

/* Apply a negative torque (reverse)
@param motor Index of motor
@param value Torque value [0, 255]
*/
void Motor_controller::torque_neg(int8_t motor, uint8_t value){
  motors[motor]->torque_neg(value);
}

int Motor_controller::read_speed(uint8_t motor){
  return 0;
}