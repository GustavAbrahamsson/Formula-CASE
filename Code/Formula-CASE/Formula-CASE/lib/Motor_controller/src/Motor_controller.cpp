
#include "Arduino.h"
#include "Motor_controller.h"

Motor_controller::Motor_controller(){
  
}

void Motor_controller::begin(){
  for (int i = 0; i < num_motors; i++){
    motors[i]->begin();
  }
}

/* Apply a positive torque (forward)
@param motor Index of motor
@param value Torque value [0, 255]
*/
void Motor_controller::forward(int8_t motor, uint8_t value){
  motors[motor]->forward(value);
}

/* Apply a negative torque (reverse)
@param motor Index of motor
@param value Torque value [0, 255]
*/
void Motor_controller::reverse(int8_t motor, uint8_t value){
  motors[motor]->reverse(value);
}

int Motor_controller::read_speed(uint8_t motor){
  return 0;
}