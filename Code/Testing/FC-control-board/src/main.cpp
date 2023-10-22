#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"
#include <Servo.h>

// Pinmappings
// Motor control pins
#define M1_FWR_PIN 35
#define M1_REV_PIN 36
#define M2_FWR_PIN 37
#define M2_REV_PIN 38
#define M3_FWR_PIN 39
#define M3_REV_PIN 40
#define M4_FWR_PIN 41
#define M4_REV_PIN 42

// ADC pins
#define ADC1_4_PIN 5
#define ADC1_5_PIN 6
#define ADC1_6_PIN 7
#define ADC1_7_PIN 8
#define BAT_SENSE_PIN 9

// Motor sense pins ADC
#define M4_SENSE_PIN 4
#define M3_SENSE_PIN 3
#define M2_SENSE_PIN 2
#define M1_SENSE_PIN 1

// Servo pins
#define SERVO1_PIN 15

typedef struct struct_message {
  // axis data
  int16_t axisX;
  int16_t axisY;
  int16_t axisRX;
  int16_t axisRY;
} struct_message;

// Function to map values
int mapSpeed(int x) {
    // Input is -512 to 512
    // Output is 0 to 255
    int y = map(x, -512, 512, -30, 30);
    return y;
}

struct_message recivedData;
long lastReciveTime = 2000;
long lastPrintTime = 0;

Servo myservo = Servo();


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&recivedData, incomingData, sizeof(recivedData));

    Serial.printf(
        "axisX: %d, axisY: %d, axisRX: %d, axisRY: %d",
        recivedData.axisX,
        recivedData.axisY,
        recivedData.axisRX,
        recivedData.axisRY
    );
    lastReciveTime = millis();

    // Set motor speed
    int speed1 = mapSpeed(recivedData.axisY);
    int speed2 = mapSpeed(recivedData.axisRY);
    // Serial.printf("\tspeed1: %d, speed2: %d, lastReciveTime: %d\n", speed1, speed2, lastReciveTime);

    // if (speed1 < 0) {
    //     analogWrite(fwrPin1, 0);
    //     analogWrite(revPin1, abs(speed1));
    // }
    // else {
    //     analogWrite(revPin1, 0);
    //     analogWrite(fwrPin1, speed1);
    // }
    
    // if(speed2 < 0) {
    //     analogWrite(fwrPin2, 0);
    //     // analogWrite(revPin2, abs(speed2));
    //     analogWrite(revPin2, abs(speed1));
    // }
    // else {
    //     analogWrite(revPin2, 0);
    //     // analogWrite(fwrPin2, speed2);
    //     analogWrite(fwrPin2, speed1);
    // }

    // Set servo angle
    // -200 to 200 to 30 to 150 is good
    // -200 = 
    int angle = map(recivedData.axisRX, -512, 512, 114, 66);
    // myservo.write(servoPin, angle);
    // myservo.write(servoPin, angle, 30, 1);

    Serial.printf("angle: %d\n", angle);


}




void setup() {
    Serial.begin(115200);
    delay(50);
    // while (!Serial);
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);

    // Print mac address
    Serial.print("ESP32 MAC Address: ");
    Serial.println(WiFi.macAddress());

    // pinMode(fwrPin1, OUTPUT);
    // pinMode(revPin1, OUTPUT);
    // pinMode(fwrPin2, OUTPUT);
    // pinMode(revPin2, OUTPUT);
    analogWriteFrequency(20000);

}

void loop() {
    // If it has been more than 2 seconds since we have recived data
    if (millis() - lastReciveTime > 2000 && millis() - lastPrintTime > 2000) {
        Serial.println("No data recived");
        lastPrintTime = millis();
    }

    delay(5);
}


