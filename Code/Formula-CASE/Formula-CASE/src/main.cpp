#include <Arduino.h>
#include "Motor_controller.h"
#include <ESP32Servo.h>

// ADC pins
#define HALL_EFF_1_PIN 5
#define HALL_EFF_2_PIN 6
#define HALL_EFF_3_PIN 7
#define HALL_EFF_4_PIN 8
#define BAT_SENSE_PIN 9

// Motor sense pins ADC
#define M4_SENSE_PIN 4
#define M3_SENSE_PIN 3
#define M2_SENSE_PIN 2
#define M1_SENSE_PIN 1

// Servo pins
#define SERVO_PIN 15

#define SERVO_DEFLECTION 50 // deg


Motor_controller motor_controller = Motor_controller();

Servo servo;

void setup() {
  Serial.begin(115200);
  servo.setPeriodHertz(50);
	servo.attach(SERVO_PIN, 925, 1925);
  servo.write(90);

  delay(2000);
  Serial.println("Program started");
}

void loop() {

  motor_controller.torque_pos(0,75);
  motor_controller.torque_pos(1,75);
  motor_controller.torque_pos(2,75);
  motor_controller.torque_pos(3,75);
  
  delay(4000);

  motor_controller.torque_pos(0,0);
  motor_controller.torque_pos(1,0);
  motor_controller.torque_pos(2,0);
  motor_controller.torque_pos(3,0);


  delay(4000);
  
}
