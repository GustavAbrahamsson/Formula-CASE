

// #include <MPU6050_tockn.h>
// #include <Wire.h>

// MPU6050 mpu6050(Wire);

// long timer = 0;

// const float gyr_x_offset = -0.95;
// const float gyr_y_offset = 1.33;
// const float gyr_z_offset = 0.07;

// void setup() {
//   Serial.begin(115200);
//   Wire.begin();
//   mpu6050.begin();
//   //mpu6050.calcGyroOffsets(true);
//   mpu6050.setGyroOffsets(gyr_x_offset, gyr_y_offset, gyr_z_offset);
// }

// void loop() {
//   mpu6050.update();

//   if(millis() - timer > 10){
    
//     // Serial.println("=======================================================");
//     // Serial.print("temp : ");Serial.println(mpu6050.getTemp());
//     // Serial.print(mpu6050.getAccX()); Serial.print("\t");
//     // Serial.print(mpu6050.getAccY()); Serial.print("\t");
//     // Serial.print(mpu6050.getAccZ()); Serial.print("\t");
  
//     // Serial.print(mpu6050.getGyroX()); Serial.print("\t");
//     // Serial.print(mpu6050.getGyroY()); Serial.print("\t");
//     // Serial.print(mpu6050.getGyroZ()); Serial.print("\t");
  
//     // Serial.print(mpu6050.getGyroAngleZ()); Serial.print("\t");

//     // Serial.print(mpu6050.getAngleX()); Serial.print("\t");
//     // Serial.print(mpu6050.getAngleY()); Serial.print("\t");
//     Serial.print(mpu6050.getAngleZ()); Serial.println("\t");
//     //Serial.println("=======================================================\n");
//     timer = millis();
    
//   }

// }



// // #include <Arduino.h>
// // #include <Wire.h>
// // float acc_x_raw, acc_y_raw, acc_z_raw;
// // float gyr_x_raw, gyr_y_raw, gyr_z_raw;

// // float acc_x, acc_y, acc_z;
// // float gyr_x, gyr_y, gyr_z;
// // float temp;

// // const float gyr_x_offset = 0.0;
// // const float gyr_y_offset = 0.0;
// // const float gyr_z_offset = 0.0;

// // void gyro_signals(void) {
// //   Wire.beginTransmission(0x68);
// //   Wire.write(0x1A);
// //   Wire.write(0x05);
// //   Wire.endTransmission(); 
// //   Wire.beginTransmission(0x68);
// //   Wire.write(0x1B); 
// //   Wire.write(0x8); 
// //   Wire.endTransmission(); 
// //   Wire.beginTransmission(0x68);
// //   Wire.write(0x43);
// //   Wire.endTransmission();
// //   Wire.requestFrom(0x68, 14);

// //   acc_x_raw = Wire.read() << 8 | Wire.read();
// //   acc_y_raw = Wire.read() << 8 | Wire.read();
// //   acc_z_raw = Wire.read() << 8 | Wire.read();
// //   temp =      Wire.read() << 8 | Wire.read();
// //   gyr_x_raw = Wire.read() << 8 | Wire.read();
// //   gyr_y_raw = Wire.read() << 8 | Wire.read();
// //   gyr_z_raw = Wire.read() << 8 | Wire.read();

// //   acc_x = ((float) acc_x_raw) / 16384.0;
// //   acc_y = ((float) acc_y_raw) / 16384.0;
// //   acc_z = ((float) acc_z_raw) / 16384.0;

// //   gyr_x = ((float) gyr_x_raw) / 65.5;
// //   gyr_y = ((float) gyr_y_raw) / 65.5;
// //   gyr_z = ((float) gyr_z_raw) / 65.5;

// //   gyr_x -= gyr_x_offset;
// //   gyr_y -= gyr_y_offset;
// //   gyr_z -= gyr_z_offset;

// //   // angleGyroX += gyroX * interval;
// //   // angleGyroY += gyroY * interval;
// //   // angleGyroZ += gyroZ * interval;

// //   // angleX = (gyroCoef * (angleX + gyroX * interval)) + (accCoef * angleAccX);
// //   // angleY = (gyroCoef * (angleY + gyroY * interval)) + (accCoef * angleAccY);
// //   // angleZ = angleGyroZ;
// // }
// // void setup() {
// //   Serial.begin(115200);
// //   pinMode(13, OUTPUT);
// //   digitalWrite(13, HIGH);
// //   Wire.setClock(400000);
// //   Wire.begin();
// //   delay(250);
// //   Wire.beginTransmission(0x68); 
// //   Wire.write(0x6B);
// //   Wire.write(0x00);
// //   Wire.endTransmission();
// // }
// // void loop() {
// //   gyro_signals();
  
// //   Serial.print(gyr_x); Serial.print("\t");
// //   Serial.print(gyr_y); Serial.print("\t");
// //   Serial.print(gyr_z); Serial.print("\t");
  
// //   Serial.print(acc_x); Serial.print("\t");
// //   Serial.print(acc_y); Serial.print("\t");
// //   Serial.print(acc_z); Serial.println("\t");
  
// //   delay(50);
// // }
#include <Wire.h>

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

void setup(){
  Serial.begin(115200);
  Serial.println();
  delay(2000);
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}