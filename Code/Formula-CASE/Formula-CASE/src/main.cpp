#include <Arduino.h>
#include "esp_now.h"
#include "WiFi.h"
//#include "Motor_controller.h"
#include <ESP32Servo.h>

#include <Wire.h>
#include <LSM6.h>

// Global control variable for testing
#define MOTORS_ALLOWED false

LSM6 imu;

// Wheel 2.0 MAC: 34:85:18:5C:86:A0
uint8_t wheel_address[] = {0x34, 0x85, 0x18, 0x5C, 0x86, 0xA0};

// Car MAC: 0x34, 0x85, 0x18, 0x89, 0xE0, 0xA8
uint8_t car_address[] = {0x34, 0x85, 0x18, 0x89, 0xE0, 0xA8};

typedef struct wheel_to_car_msg {
    uint8_t throttle;
    uint8_t brake;
    float angle;
} wheel_to_car_msg;

typedef struct car_to_wheel_msg {
    float w_ICE; // RPM
    float phi; // rad
} car_to_wheel_msg;

// ADC pins
#define HALL_EFF_0_PIN 4  // 0
#define HALL_EFF_1_PIN 5
#define HALL_EFF_2_PIN 6
#define HALL_EFF_3_PIN 7

// Buzzer
#define BUZZER_PIN 2

// Battery measurement
#define BAT_SENSE_PIN 21

// Motor direction pins
#define M0_DIR_PIN 10
#define M1_DIR_PIN 11
#define M2_DIR_PIN 12
#define M3_DIR_PIN 13

// Servo pins
#define SERVO_PIN 1

#define SERVO_DEFLECTION 50 // deg

#define M0_FWR_PIN 14 // 0
#define M0_REV_PIN 36 // 0
#define M1_FWR_PIN 37
#define M1_REV_PIN 38
#define M2_FWR_PIN 39
#define M2_REV_PIN 40
#define M3_FWR_PIN 41
#define M3_REV_PIN 42

// Assigned PWM channels
#define SERVO_CHL 0
#define M0_ASS_PWM_CHL 5
#define M1_ASS_PWM_CHL 2
#define M2_ASS_PWM_CHL 3
#define M3_ASS_PWM_CHL 4
#define BUZZER_CHNL 7

// Motor sense pins ADC
#define M0_SENSE_PIN 15 // 0
#define M1_SENSE_PIN 16
#define M2_SENSE_PIN 17
#define M3_SENSE_PIN 18

#define MOTOR_PWM_BIT_RES 8
#define MOTOR_PWM_FREQ 25000

// Tasks
#define MOTOR_TASK_FREQ 50 // Hz
#define MOTOR_TASK_CORE 1

#define PERIPH_TASK_FREQ 50 // Hz
#define PERIPH_TASK_CORE 1

#define SPEED_TASK_FREQ 750 // Hz
#define SPEED_TASK_CORE 1

#define RAD_PER_S_TO_RPM 9.54957
#define RPM_TO_RAD_PER_S 0.10471

struct motor_measurement {
  int16_t w_m;      // Wheel speed, RPM
  int16_t w_last;   // Old speed, RPM
  float phi;      // Wheel angle, rad
  float phi_last; // Old angle, rad
  int16_t hall_raw; // Raw measurement from analogRead()
  int16_t max;
  int16_t min;
  int16_t mid;
  uint8_t pin;
};

motor_measurement m_meas[4];


car_to_wheel_msg outgoing_message;

wheel_to_car_msg incoming_message;

esp_now_peer_info_t peerInfo;
String success;
// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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
  //Serial.print("Bytes received: ");
  //Serial.println(len);
}
  
// Play a certain frequency via buzzer
void buzz_hz(uint16_t freq){
  ledcWriteTone(BUZZER_CHNL, freq);
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

// The task that finally applies new motor voltages and servo position
void motor_task(void *pvParameter){

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(1000.0 / MOTOR_TASK_FREQ);
  BaseType_t xWasDelayed;
  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  while(1){
    // Wait for the next cycle.
    xWasDelayed = xTaskDelayUntil( &xLastWakeTime, xFrequency );
    // xWasDelayed value can be used to determine whether a deadline was missed
    // if the code here took too long.
    
    wheel_throttle = incoming_message.throttle;
    wheel_brake = incoming_message.brake;
    wheel_angle = incoming_message.angle;

    servo.write(int(90 - constrain(wheel_angle * 50.0/90.0, -50.0, 50.0)));
  
    if(wheel_throttle > 10){
      // Temp

      ledcWrite(M2_ASS_PWM_CHL, wheel_throttle*0.05);
      ledcWrite(M3_ASS_PWM_CHL, wheel_throttle*0.05);
    }
    else{
      ledcWrite(M2_ASS_PWM_CHL, 0);
      ledcWrite(M3_ASS_PWM_CHL, 0);
    }

    if(wheel_brake > 10){
      // Temp
      ledcWrite(M1_ASS_PWM_CHL, wheel_brake*0.05);
      ledcWrite(M0_ASS_PWM_CHL, wheel_brake*0.05);
    }
    else{
      ledcWrite(M1_ASS_PWM_CHL, 0);
      ledcWrite(M0_ASS_PWM_CHL, 0);
    }
  }

}



// A peripheral task
void peripheral_task(void *pvParameter){

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(1000.0 / PERIPH_TASK_FREQ);
  BaseType_t xWasDelayed;
  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  while(1){
    // Wait for the next cycle.
    xWasDelayed = xTaskDelayUntil( &xLastWakeTime, xFrequency );
    // xWasDelayed value can be used to determine whether a deadline was missed
    // if the code here took too long.
    
    //buzz_hz(12 * m_meas[0].w_m + 1000);
    
    imu.read();
    
    Serial.print(imu.a.x); Serial.print("\t");
    Serial.print(imu.a.y); Serial.print("\t");
    Serial.print(imu.a.z); Serial.print("\t");
    
    Serial.print(imu.g.x); Serial.print("\t");
    Serial.print(imu.g.y); Serial.print("\t");
    Serial.print(imu.g.z); Serial.print("\t");
    
    Serial.println();

    outgoing_message.w_ICE = m_meas[0].w_m;
    outgoing_message.phi = m_meas[0].phi;
    esp_err_t result = esp_now_send(wheel_address, (uint8_t *) &outgoing_message, sizeof(outgoing_message));
    
  }
}


// The task that measures wheel speed
void speed_measurement_task(void *pvParameter){

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(1000.0 / SPEED_TASK_FREQ);
  BaseType_t xWasDelayed;
  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  float angle;

  float alpha = 0.005;

  while(1){
    // Wait for the next cycle.
    xWasDelayed = xTaskDelayUntil( &xLastWakeTime, xFrequency );
    // xWasDelayed value can be used to determine whether a deadline was missed
    // if the code here took too long.

    m_meas[0].hall_raw = analogRead(HALL_EFF_0_PIN);

    // Update if new max/min has been measured
    if (m_meas[0].hall_raw > m_meas[0].max){
      m_meas[0].max = m_meas[0].hall_raw;
      m_meas[0].mid = (m_meas[0].max + m_meas[0].min) / 2;
    }
    else if(m_meas[0].hall_raw < m_meas[0].min){
      m_meas[0].min = m_meas[0].hall_raw;
      m_meas[0].mid = (m_meas[0].max + m_meas[0].min) / 2;
    }

    m_meas[0].phi_last = m_meas[0].phi;
    // Convert to sine wave with amplitude 1, and extract angle from arcsin()
    m_meas[0].phi = asin((float)(m_meas[0].hall_raw - m_meas[0].min) / ((float)m_meas[0].max) - 1);

    m_meas[0].w_last = m_meas[0].w_m;
    // (Angle - old_angle) / T
    m_meas[0].w_m = RAD_PER_S_TO_RPM * (float)(m_meas[0].phi - m_meas[0].phi_last) / (float)(1.0/SPEED_TASK_FREQ);

    m_meas[0].w_m = abs(m_meas[0].w_m);

    m_meas[0].w_m = alpha * m_meas[0].w_m + (1.0-alpha) * m_meas[0].w_last;
  }
}


// Motor_controller motor_controller = Motor_controller();

void init_i2c(){
  Wire.setPins(SDA, SCL);
  Wire.begin();
}

void init_imu(){
  while(!imu.init(LSM6::device_DS33, LSM6::sa0_low)){
    Serial.println("Failed to initialize IMU!");
    delay(1000);
  }
  Serial.println("IMU initialized");
  imu.enableDefault(); // TODO: Investigate how to get other resolutions
}

void init_tasks(){
  xTaskCreatePinnedToCore(motor_task, "motor_task", 2048, NULL, 1, NULL, MOTOR_TASK_CORE);
  xTaskCreatePinnedToCore(peripheral_task, "peripheral_task", 2048, NULL, 1, NULL, PERIPH_TASK_CORE);
  xTaskCreatePinnedToCore(speed_measurement_task, "speed_measurement_task", 2048, NULL, 1, NULL, SPEED_TASK_CORE);
}

void init_pins(){
  // Motors
  pinMode(M0_FWR_PIN, OUTPUT);
  pinMode(M0_REV_PIN, OUTPUT);
  pinMode(M1_FWR_PIN, OUTPUT);
  pinMode(M1_REV_PIN, OUTPUT);
  pinMode(M2_FWR_PIN, OUTPUT);
  pinMode(M2_REV_PIN, OUTPUT);
  pinMode(M3_FWR_PIN, OUTPUT);
  pinMode(M3_REV_PIN, OUTPUT);

  digitalWrite(M0_FWR_PIN, 0);
  digitalWrite(M0_REV_PIN, 0);
  digitalWrite(M1_FWR_PIN, 0);
  digitalWrite(M1_REV_PIN, 0);
  digitalWrite(M2_FWR_PIN, 0);
  digitalWrite(M2_REV_PIN, 0);
  digitalWrite(M3_FWR_PIN, 0);
  digitalWrite(M3_REV_PIN, 0);

  // Motor directions
  pinMode(M0_DIR_PIN, OUTPUT);
  pinMode(M1_DIR_PIN, OUTPUT);
  pinMode(M2_DIR_PIN, OUTPUT);
  pinMode(M3_DIR_PIN, OUTPUT);

  digitalWrite(M0_DIR_PIN, 1);
  digitalWrite(M1_DIR_PIN, 0);
  digitalWrite(M2_DIR_PIN, 0);
  digitalWrite(M3_DIR_PIN, 0);

  // Buzzer
  ledcAttachPin(BUZZER_PIN, BUZZER_CHNL);
}

void init_motors(){
  ledcDetachPin(M1_FWR_PIN);
  ledcDetachPin(M1_REV_PIN);
  ledcDetachPin(M2_FWR_PIN);
  ledcDetachPin(M2_REV_PIN);
  ledcDetachPin(M3_FWR_PIN);
  ledcDetachPin(M3_REV_PIN);
  ledcDetachPin(M0_FWR_PIN);
  ledcDetachPin(M0_REV_PIN);

  delay(500);

  digitalWrite(M0_REV_PIN, 0);
  ledcSetup(M0_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M0_FWR_PIN, M0_ASS_PWM_CHL);
  
  digitalWrite(M1_FWR_PIN, 0);
  ledcSetup(M1_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M1_REV_PIN, M1_ASS_PWM_CHL);

  digitalWrite(M2_REV_PIN, 0);
  ledcSetup(M2_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M2_FWR_PIN, M2_ASS_PWM_CHL);
  
  digitalWrite(M3_FWR_PIN, 0);
  ledcSetup(M3_ASS_PWM_CHL, MOTOR_PWM_FREQ, MOTOR_PWM_BIT_RES);
  ledcAttachPin(M3_REV_PIN, M3_ASS_PWM_CHL);


  ledcWrite(M0_FWR_PIN, 0);
  ledcWrite(M1_FWR_PIN, 0);
  ledcWrite(M2_FWR_PIN, 0);
  ledcWrite(M3_FWR_PIN, 0);

  m_meas[0].pin = HALL_EFF_0_PIN;
  m_meas[1].pin = HALL_EFF_1_PIN;
  m_meas[2].pin = HALL_EFF_2_PIN;
  m_meas[3].pin = HALL_EFF_3_PIN;

  for (int i = 0; i < 4; i++){
    m_meas[i].max = 1500;
    m_meas[i].min = 150;
    m_meas[i].mid = (m_meas[i].max + m_meas[i].min) / 2;
  }

  delay(250);
}

uint8_t servo_channel;

void setup() {
  Serial.begin(115200);
  Serial.println("Car started");

  servo.setPeriodHertz(50);
	servo_channel = servo.attach(SERVO_PIN, 925, 1925);
  servo.write(90);

  init_pins();
  init_i2c();
  init_imu();

  delay(1000);

  init_esp_now();

  //Serial.println("Program started");
  // Serial.println("Servo channel: "+String(servo_channel));

  //motor_controller.begin();

  init_motors();

  // buzz_hz(0);
  // ledcChangeFrequency(BUZZER_CHNL, 0, 8);
  // ledcWrite(BUZZER_CHNL, 50);
  // while(1){
  //   for(int i = 0; i < 10000; i += 10){
  //     ledcChangeFrequency(BUZZER_CHNL, i, 8);
  //     delay(7);
  //   }

  // }


  // Start the tasks
  init_tasks();

}

void loop() {

  while(1){
    vTaskDelay(1000000 / portTICK_RATE_MS);
  }



  // int channel1 = M1_ASS_PWM_CHL;
  // int channel2 = M2_ASS_PWM_CHL;
  // int channel3 = M3_ASS_PWM_CHL;
  // int channel4 = M0_ASS_PWM_CHL;
  
  
  
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
  

// ------------------------------- MAC ADDRESS ---------------------------

// #include <Wire.h>

// #ifdef ESP32
//   #include <WiFi.h>
// #else
//   #include <ESP8266WiFi.h>
// #endif

// void setup(){
//   Serial.begin(115200);
//   Serial.println();
//   delay(2000);
//   Serial.print("ESP Board MAC Address:  ");
//   Serial.println(WiFi.macAddress());
// }
 
// void loop(){

// }


// // I2C address scanner program
// #include <Wire.h>
// #include <Arduino.h>

// void setup()
// {
//   Wire.setPins(8,9);
//   Wire.begin();
//   Serial.begin(115200);
//   Serial.println("I2C Scanner");

// }

// void loop()
// {
//   byte error, address;
//   int nDevices;

//   Serial.println("Scanning...");

//   nDevices = 0;
//   for(address = 1; address < 127; address++ )
//   {
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();

//     if (error == 0)
//     {
//       Serial.print("I2C device found at address 0x");
//       if (address < 16)
//         Serial.print("0");

//       Serial.print(address,HEX);
//       Serial.println("  !");

//       nDevices++;
//     }
//     else if (error==4)
//     {
//       Serial.print("Unknown error at address 0x");
//       if (address < 16)
//         Serial.print("0");

//       Serial.println(address,HEX);
//     }
//   }

//   if (nDevices == 0)
//     Serial.println("No I2C devices found");
//   else
//     Serial.println("done");

//   delay(5000); // wait 5 seconds for next scan
// }