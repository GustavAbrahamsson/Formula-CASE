#include <Arduino.h>
#include "esp_now.h"
#include "WiFi.h"
//#include "Motor_controller.h"
#include <ESP32Servo.h>

// Wheel 2.0 MAC: 34:85:18:5C:86:A0
uint8_t wheel_address[] = {0x34, 0x85, 0x18, 0x5C, 0x86, 0xA0};

// Car MAC: 30:30:F9:6F:DF:20
uint8_t car_address[] = {0x30, 0x30, 0xF9, 0x6F, 0xDF, 0x20};

typedef struct wheel_to_car_msg {
    uint8_t throttle;
    uint8_t brake;
    float angle;
} wheel_to_car_msg;

typedef struct car_to_wheel_msg {
    uint8_t throttle;
    uint8_t brake;
    float angle;
} car_to_wheel_msg;

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

// // Front right forward direction works, with M1_ASS_PWM_CHL = 2
//   digitalWrite(M1_FWR_PIN, 0);
//   ledcSetup(M1_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
//   ledcAttachPin(M1_FWR_PIN, M1_ASS_PWM_CHL);

//   // // Rear right forward direction works, with M2_ASS_PWM_CHL = 2
//   digitalWrite(M2_REV_PIN, 0);
//   ledcSetup(M2_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
//   ledcAttachPin(M2_FWR_PIN, M2_ASS_PWM_CHL);
  
//   // Front left forward direction works, with M3_ASS_PWM_CHL = 2
//   digitalWrite(M3_REV_PIN, 0);
//   ledcSetup(M3_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
//   ledcAttachPin(M3_FWR_PIN, M3_ASS_PWM_CHL);

//   // Rear left forward direction works, with M4_ASS_PWM_CHL = 6
//   digitalWrite(M4_REV_PIN, 0);
//   ledcSetup(M4_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
//   ledcAttachPin(M4_FWR_PIN, M4_ASS_PWM_CHL);



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

wheel_to_car_msg incoming_message;

esp_now_peer_info_t peerInfo;
String success;
// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

uint8_t wheel_throttle;
uint8_t wheel_brake;
float wheel_angle;


Servo servo;

// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&incoming_message, incomingData, sizeof(incoming_message));
  Serial.print("Bytes received: ");
  Serial.println(len);
  wheel_throttle = incoming_message.throttle;
  wheel_brake = incoming_message.brake;
  wheel_angle = incoming_message.angle;

  servo.write(int(90 - constrain(wheel_angle * 50.0/90.0, -50.0, 50.0)));
  
  if(wheel_throttle > 20){
    ledcWrite(M2_ASS_PWM_CHL, wheel_throttle);
    ledcWrite(M4_ASS_PWM_CHL, wheel_throttle);
  }
  else{
    ledcWrite(M2_ASS_PWM_CHL, 0);
    ledcWrite(M4_ASS_PWM_CHL, 0);
  }

  if(wheel_brake > 20){
    ledcWrite(M1_ASS_PWM_CHL, wheel_brake);
    ledcWrite(M3_ASS_PWM_CHL, wheel_brake);
  }
  else{
    ledcWrite(M1_ASS_PWM_CHL, 0);
    ledcWrite(M3_ASS_PWM_CHL, 0);
  }
}
  

void init_esp_now(){
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, wheel_address, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("ESP_NOW initialized");
}


// Motor_controller motor_controller = Motor_controller();

uint8_t servo_channel;

void setup() {
  Serial.begin(115200);
  servo.setPeriodHertz(50);
	servo_channel = servo.attach(SERVO_PIN, 925, 1925);
  servo.write(90);


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


  delay(3000);

  init_esp_now();

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
  
  //ledcDetachPin(SERVO_PIN);

  delay(500);

  // Front right forward direction works, with M1_ASS_PWM_CHL = 2
  digitalWrite(M1_FWR_PIN, 0);
  ledcSetup(M1_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M1_REV_PIN, M1_ASS_PWM_CHL);

  // // Rear right forward direction works, with M2_ASS_PWM_CHL = 2
  digitalWrite(M2_REV_PIN, 0);
  ledcSetup(M2_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M2_FWR_PIN, M2_ASS_PWM_CHL);
  
  // Front left forward direction works, with M3_ASS_PWM_CHL = 2
  digitalWrite(M3_REV_PIN, 0);
  ledcSetup(M3_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M3_REV_PIN, M3_ASS_PWM_CHL);

  // Rear left forward direction works, with M4_ASS_PWM_CHL = 6
  digitalWrite(M4_REV_PIN, 0);
  ledcSetup(M4_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M4_FWR_PIN, M4_ASS_PWM_CHL);

  ledcWrite(M1_FWR_PIN, 0);
  ledcWrite(M2_FWR_PIN, 0);
  ledcWrite(M3_FWR_PIN, 0);
  ledcWrite(M4_FWR_PIN, 0);

}

void loop() {
  // int channel1 = M1_ASS_PWM_CHL;
  // int channel2 = M2_ASS_PWM_CHL;
  // int channel3 = M3_ASS_PWM_CHL;
  // int channel4 = M4_ASS_PWM_CHL;
  
  delay(10000);
  
  // Serial.println("T > 0");
  // ledcWrite(channel1, 50);
  // ledcWrite(channel2, 50);
  // delay(2000);
  // Serial.println("T = 0\n");
  // ledcWrite(channel1, 0);
  // ledcWrite(channel2, 0);
  // delay(2000);

  // Serial.println(String(wheel_throttle)+" "+String(wheel_brake)+" "+String(wheel_angle)+"\n");
  // Serial.println(90 + constrain(wheel_angle * 50.0/90.0, -50.0, 50.0));
  // Serial.flush();


  // while(1){
  //   //Serial.println("T > 0");
  //   ledcWrite(channel1, 100);
  //   delay(10);
  //   ledcWrite(channel2, 100);
  //   delay(10);
  //   ledcWrite(channel3, 100);
  //   delay(10);
  //   ledcWrite(channel4, 100);
  //   delay(2000);
  //   //Serial.println("T = 0\n");
  //   ledcWrite(channel1, 0);
  //   delay(10);
  //   ledcWrite(channel2, 0);
  //   delay(10);
  //   ledcWrite(channel3, 0);
  //   delay(10);
  //   ledcWrite(channel4, 0);
  //   delay(4000);

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
  

