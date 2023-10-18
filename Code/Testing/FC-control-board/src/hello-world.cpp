#include <Arduino.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(GPIO_NUM_15, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World");
  digitalWrite(GPIO_NUM_15, HIGH);
  delay(500);
  digitalWrite(GPIO_NUM_15, LOW);
  delay(500);

}

