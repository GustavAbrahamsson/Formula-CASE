#include <Arduino.h>

#define AIN0 A0

// Vars
long current_time = 0;
long offset_time = 0;
float voltage = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(AIN0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  // If read "t" from serial, set offset_time to t
  if (offset_time == 0 && current_time > 10000) {
    if (Serial.available()) {
      if (Serial.read() == 't') {
        offset_time = current_time;
      }
    }
  }
  current_time = micros();
  voltage = analogRead(AIN0) * (5.0 / 1023.0);
  Serial.print(current_time);
  Serial.print(", ");
  Serial.println(voltage, 5);
}

