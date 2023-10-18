#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    while (!Serial || millis() < 2000) {
        delay(1);
    }
    Serial.print("ESP32 Board MAC Address: ");
    Serial.println(WiFi.macAddress());
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello World");
  digitalWrite(GPIO_NUM_15, HIGH);
  delay(500);
  digitalWrite(GPIO_NUM_15, LOW);
  delay(500);

}

