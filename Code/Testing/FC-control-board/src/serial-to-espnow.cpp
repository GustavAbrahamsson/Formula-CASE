#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"
#include <Servo.h>

#include "config.h"

struct_message transmitData;


uint8_t control[4];

float send_values[4] = {3.14, 2.71, -3, 10};

// Setup
void setup() {
    Serial.begin(115200);
    delay(50);
}


// Main loop
void loop() {
    // Write send_values to serial
    Serial.println("Sending values");
    Serial.write((uint8_t *) &send_values, sizeof(send_values));


    delay(1000);
}