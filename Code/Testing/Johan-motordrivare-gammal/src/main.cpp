#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"
#include <Servo.h>




typedef struct struct_message {
  // axis data
  int16_t axisX;
  int16_t axisY;
  int16_t axisRX;
  int16_t axisRY;
} struct_message;

// Motor driver pins
int buttonPin0 = 0;
int buttonPin1 = 1;

int fwrPin1 = 4;
int revPin1 = 5;

int fwrPin2 = 6;
int revPin2 = 7;

// Function to map values
int mapSpeed(int x) {
    // Input is -512 to 512
    // Output is 0 to 255
    int y = map(x, -512, 512, -30, 30);
    return y;
}

struct_message recivedData;
long lastReciveTime = 2000;

Servo myservo = Servo();
int servoPin = buttonPin0;


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&recivedData, incomingData, sizeof(recivedData));

    // Serial.printf(
    //     "axisX: %d, axisY: %d, axisRX: %d, axisRY: %d",
    //     recivedData.axisX,
    //     recivedData.axisY,
    //     recivedData.axisRX,
    //     recivedData.axisRY
    // );
    lastReciveTime = millis();

    // Set motor speed
    int speed1 = mapSpeed(recivedData.axisY);
    int speed2 = mapSpeed(recivedData.axisRY);
    // Serial.printf("\tspeed1: %d, speed2: %d, lastReciveTime: %d\n", speed1, speed2, lastReciveTime);

    if (speed1 < 0) {
        analogWrite(fwrPin1, 0);
        analogWrite(revPin1, abs(speed1));
    }
    else {
        analogWrite(revPin1, 0);
        analogWrite(fwrPin1, speed1);
    }
    
    if(speed2 < 0) {
        analogWrite(fwrPin2, 0);
        // analogWrite(revPin2, abs(speed2));
        analogWrite(revPin2, abs(speed1));
    }
    else {
        analogWrite(revPin2, 0);
        // analogWrite(fwrPin2, speed2);
        analogWrite(fwrPin2, speed1);
    }

    // Set servo angle
    // -200 to 200 to 30 to 150 is good
    // -200 = 
    int angle = map(recivedData.axisRX, -512, 512, 114, 66);
    // myservo.write(servoPin, angle);
    myservo.write(servoPin, angle, 30, 1);

    // Serial.printf("angle: %d\n", angle);


}









void setup() {
    Serial.begin(115200);
    // while (!Serial);
    // Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);
    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    // Once ESPNow is successfully Init, we will register for recv CB to
    // get recv packer info
    esp_now_register_recv_cb(OnDataRecv);

    // Print mac address
    Serial.print("ESP32 MAC Address: ");
    Serial.println(WiFi.macAddress());

    // Motor
    // pinMode(buttonPin0, INPUT_PULLUP);
    pinMode(buttonPin1, INPUT_PULLUP);

    pinMode(fwrPin1, OUTPUT);
    pinMode(revPin1, OUTPUT);
    pinMode(fwrPin2, OUTPUT);
    pinMode(revPin2, OUTPUT);
    analogWriteFrequency(20000);





}

void loop() {

    // // Control motors based on recived data
    // int speed1 = mapSpeed(recivedData.axisY);
    // int speed2 = mapSpeed(recivedData.axisRY);
    // Serial.printf("speed1: %d, speed2: %d, lastReciveTime: %d\n", speed1, speed2, lastReciveTime);


    // // Forward
    // if (speed1 > 0) {
    //     analogWrite(fwrPin1, speed1);
    //     analogWrite(revPin1, 0);
    // }
    // else {
    //     analogWrite(revPin1, abs(speed1));
    //     analogWrite(fwrPin1, 0);
    // }


    delay(5);
}


