#include <Arduino.h>

// Pinmappings
// Motor control pins
// #define M1_FWR_PIN 35
// #define M1_REV_PIN 36
// #define M2_FWR_PIN 37
// #define M2_REV_PIN 38
// #define M3_FWR_PIN 39
// #define M3_REV_PIN 40
// #define M4_FWR_PIN 41
// #define M4_REV_PIN 42

#define M1_FWR_PIN 37
#define M1_REV_PIN 38
#define M2_FWR_PIN 35
#define M2_REV_PIN 36
#define M3_FWR_PIN 39
#define M3_REV_PIN 40
#define M4_FWR_PIN 41
#define M4_REV_PIN 42

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
#define SERVO1_PIN 15

void setup() {
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

  analogWriteFrequency(500);

  delay(3000);
}

void loop() {
  
  analogWrite(M1_FWR_PIN, 255);
  digitalWrite(M1_REV_PIN, 0);

  digitalWrite(M2_FWR_PIN, 0);
  analogWrite(M2_REV_PIN, 255);

  analogWrite(M3_FWR_PIN, 255);
  digitalWrite(M3_REV_PIN, 0);

  digitalWrite(M4_FWR_PIN, 0);
  analogWrite(M4_REV_PIN, 255);

  delay(3000);
  
  analogWrite(M1_FWR_PIN, 0);
  digitalWrite(M1_REV_PIN, 0);

  digitalWrite(M2_FWR_PIN, 0);
  analogWrite(M2_REV_PIN, 0);

  analogWrite(M3_FWR_PIN, 0);
  digitalWrite(M3_REV_PIN, 0);

  digitalWrite(M4_FWR_PIN, 0);
  analogWrite(M4_REV_PIN, 0);

  delay(8000);
}
