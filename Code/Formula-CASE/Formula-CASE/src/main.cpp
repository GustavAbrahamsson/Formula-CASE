#include <Arduino.h>
//#include "Motor_controller.h"
//#include <ESP32Servo.h>

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

#define M1_FWR_PIN 37
#define M1_REV_PIN 38
#define M2_FWR_PIN 35
#define M2_REV_PIN 36
#define M3_FWR_PIN 39
#define M3_REV_PIN 40
#define M4_FWR_PIN 41
#define M4_REV_PIN 42

// Assigned PWM channels
#define SERVO_CHL 0
#define M1_ASS_PWM_CHL 2
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


// Motor_controller motor_controller = Motor_controller();

// Servo servo;
uint8_t servo_channel;

void setup() {
  //Serial.begin(115200);
  // servo.setPeriodHertz(50);
	// servo_channel = servo.attach(SERVO_PIN, 925, 1925);
  // servo.write(90);

  delay(2000);

  pinMode(M1_FWR_PIN, OUTPUT);
  pinMode(M1_REV_PIN, OUTPUT);
  pinMode(M2_FWR_PIN, OUTPUT);
  pinMode(M2_REV_PIN, OUTPUT);
  pinMode(M3_FWR_PIN, OUTPUT);
  pinMode(M3_REV_PIN, OUTPUT);
  pinMode(M4_FWR_PIN, OUTPUT);
  pinMode(M4_REV_PIN, OUTPUT);

  digitalWrite(M1_FWR_PIN, 0);
  digitalWrite(M1_REV_PIN, 0);
  digitalWrite(M2_FWR_PIN, 0);
  digitalWrite(M2_REV_PIN, 0);
  digitalWrite(M3_FWR_PIN, 0);
  digitalWrite(M3_REV_PIN, 0);
  digitalWrite(M4_FWR_PIN, 0);
  digitalWrite(M4_REV_PIN, 0);


  //Serial.println("Program started");
  // Serial.println("Servo channel: "+String(servo_channel));

  //motor_controller.begin();

  ledcDetachPin(M1_FWR_PIN);
  ledcDetachPin(M1_REV_PIN);
  ledcDetachPin(M2_FWR_PIN);
  ledcDetachPin(M2_REV_PIN);
  ledcDetachPin(M3_FWR_PIN);
  ledcDetachPin(M3_REV_PIN);
  ledcDetachPin(M4_FWR_PIN);
  ledcDetachPin(M4_REV_PIN);
  
  ledcDetachPin(SERVO_PIN);

  delay(500);

  // Front right forward direction works, with M1_ASS_PWM_CHL = 2
  digitalWrite(M1_FWR_PIN, 0);
  ledcSetup(M1_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M1_FWR_PIN, M1_ASS_PWM_CHL);

  // // Rear right forward direction works, with M2_ASS_PWM_CHL = 2
  digitalWrite(M2_REV_PIN, 0);
  ledcSetup(M2_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M2_FWR_PIN, M2_ASS_PWM_CHL);
  
  // Front left forward direction works, with M3_ASS_PWM_CHL = 2
  digitalWrite(M3_REV_PIN, 0);
  ledcSetup(M3_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M3_FWR_PIN, M3_ASS_PWM_CHL);

  // Rear left forward direction works, with M4_ASS_PWM_CHL = 6
  digitalWrite(M4_REV_PIN, 0);
  ledcSetup(M4_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M4_FWR_PIN, M4_ASS_PWM_CHL);

}

void loop() {
  int channel1 = M1_ASS_PWM_CHL;
  int channel2 = M2_ASS_PWM_CHL;
  int channel3 = M3_ASS_PWM_CHL;
  int channel4 = M4_ASS_PWM_CHL;

  delay(2000);
  
  // Serial.println("T > 0");
  // ledcWrite(channel1, 50);
  // ledcWrite(channel2, 50);
  // delay(2000);
  // Serial.println("T = 0\n");
  // ledcWrite(channel1, 0);
  // ledcWrite(channel2, 0);
  // delay(2000);

  while(1){
    //Serial.println("T > 0");
    ledcWrite(channel1, 100);
    delay(10);
    ledcWrite(channel2, 100);
    delay(10);
    ledcWrite(channel3, 100);
    delay(10);
    ledcWrite(channel4, 100);
    delay(2000);
    //Serial.println("T = 0\n");
    ledcWrite(channel1, 0);
    delay(10);
    ledcWrite(channel2, 0);
    delay(10);
    ledcWrite(channel3, 0);
    delay(10);
    ledcWrite(channel4, 0);
    delay(4000);

  }
  










  // ledcWrite(M1_ASS_PWM_CHL, 10);
  // delay(10);
  // ledcWrite(M2_ASS_PWM_CHL, 10);
  // delay(10);
  // // ledcWrite(M3_ASS_PWM_CHL, 10);
  // delay(10);
  // ledcWrite(M4_ASS_PWM_CHL, 10);

  // delay(500);

  // // ledcWrite(M1_ASS_PWM_CHL, 0);
  // delay(10);
  // ledcWrite(M2_ASS_PWM_CHL, 0);
  // delay(10);
  // // ledcWrite(M3_ASS_PWM_CHL, 0);
  // delay(10);
  // ledcWrite(M4_ASS_PWM_CHL, 0);
  // delay(3000);

  // // ledcDetachPin(M1_FWR_PIN);
  // // delay(10);
  // // digitalWrite(M1_FWR_PIN, 0);
  // // delay(10);
  // // ledcSetup(M1_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  // // delay(10);
  // // ledcAttachPin(M1_REV_PIN, M1_ASS_PWM_CHL);

  // delay(10);
  // ledcDetachPin(M2_FWR_PIN);
  // // ledcDetachPin(M2_REV_PIN);
  // delay(10);
  // pinMode(M2_FWR_PIN, OUTPUT);
  // digitalWrite(M2_FWR_PIN, 0);
  // delay(10);
  // // ledcSetup(M2_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  // delay(10);
  // ledcAttachPin(M2_REV_PIN, M2_ASS_PWM_CHL);
  
  // // delay(10);
  // // ledcDetachPin(M3_FWR_PIN);
  // // delay(10);
  // // digitalWrite(M3_FWR_PIN, 0);
  // // delay(10);
  // // // ledcSetup(M3_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  // // delay(10);
  // // ledcAttachPin(M3_REV_PIN, M3_ASS_PWM_CHL);

  // delay(10);
  // ledcDetachPin(M4_FWR_PIN);
  // // ledcDetachPin(M4_REV_PIN);
  // delay(10);
  // digitalWrite(M4_FWR_PIN, 0);
  // delay(10);
  // // ledcSetup(M4_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  // delay(10);
  // ledcAttachPin(M4_REV_PIN, M4_ASS_PWM_CHL);
  // delay(10);
  

  // // ledcWrite(M1_ASS_PWM_CHL, 10);
  // delay(10);
  // ledcWrite(M2_ASS_PWM_CHL, 10);
  // delay(10);
  // // ledcWrite(M3_ASS_PWM_CHL, 10);
  // delay(10);
  // ledcWrite(M4_ASS_PWM_CHL, 10);
  // delay(500);
  // // ledcWrite(M1_ASS_PWM_CHL, 0);
  // ledcWrite(M2_ASS_PWM_CHL, 0);
  // // ledcWrite(M3_ASS_PWM_CHL, 0);
  // ledcWrite(M4_ASS_PWM_CHL, 0);
  // delay(4000);

  // // ledcDetachPin(M1_REV_PIN);
  // // delay(10);
  // // digitalWrite(M1_REV_PIN, 0);
  // // delay(10);
  // // // ledcSetup(M1_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  // // delay(10);
  // // ledcAttachPin(M1_FWR_PIN, M1_ASS_PWM_CHL);
  // // delay(10);

  // ledcDetachPin(M2_REV_PIN);
  // // ledcDetachPin(M2_FWR_PIN);
  // delay(10);
  // digitalWrite(M2_REV_PIN, 0);
  // delay(10);
  // // ledcSetup(M2_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  // delay(10);
  // ledcAttachPin(M2_FWR_PIN, M2_ASS_PWM_CHL);
  // delay(10);
  
  // // ledcDetachPin(M3_REV_PIN);
  // // delay(10); 
  // // digitalWrite(M3_REV_PIN, 0);
  // // delay(10);
  // // // ledcSetup(M3_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  // // delay(10);
  // // ledcAttachPin(M3_FWR_PIN, M3_ASS_PWM_CHL);
  // // delay(10);

  // ledcDetachPin(M4_REV_PIN);
  // // ledcDetachPin(M4_FWR_PIN);
  // delay(10);
  // digitalWrite(M4_REV_PIN, 0);
  // delay(10);
  // // ledcSetup(M4_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  // delay(10);
  // ledcAttachPin(M4_FWR_PIN, M4_ASS_PWM_CHL);

  // delay(10);
  // motor_controller.forward(0,1);
  // motor_controller.forward(1,1);
  // motor_controller.forward(2,1);
  // motor_controller.forward(3,1);
  
  // delay(4000);

  // motor_controller.forward(0,0);
  // motor_controller.forward(1,0);
  // motor_controller.forward(2,0);
  // motor_controller.forward(3,0);


  // delay(4000);

  
  // motor_controller.reverse(0,1);
  // motor_controller.reverse(1,1);
  // motor_controller.reverse(2,1);
  // motor_controller.reverse(3,1);
  
  // delay(4000);

  // motor_controller.reverse(0,0);
  // motor_controller.reverse(1,0);
  // motor_controller.reverse(2,0);
  // motor_controller.reverse(3,0);


  // delay(4000);
  
}
