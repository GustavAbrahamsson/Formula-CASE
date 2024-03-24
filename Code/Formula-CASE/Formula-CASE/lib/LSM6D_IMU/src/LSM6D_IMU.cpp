
#include "Arduino.h"
#include "LSM6D_IMU.h"

LSM6D_IMU::LSM6D_IMU(){
  
}

void LSM6D_IMU::begin(){
  while(!imu.init(LSM6::device_DS33, LSM6::sa0_low)){
    Serial.println("Failed to initialize IMU!");
    delay(1000);
  }
  Serial.println("IMU initialized");
  set_resolution();
  
}

void LSM6D_IMU::update(){
  imu.read();
  a.x_raw = imu.a.x;
  a.y_raw = imu.a.y;
  a.z_raw = imu.a.z;
  
  g.x_raw = imu.g.x;
  g.y_raw = imu.g.y;
  g.z_raw = imu.g.z;
}

void LSM6D_IMU::set_resolution()
{
  // Accelerometer

  // 0x80 = 0b10000000
  // ODR = 1000 (1.66 kHz (high performance)); FS_XL = 00 (+/-2 g full scale)
  imu.writeReg(LSM6::CTRL1_XL, 0x80);

  // Gyro

  // 0x80 = 0b010000000
  // ODR = 1000 (1.66 kHz (high performance)); FS_G = 00 (245 dps for DS33, 250 dps for DSO)
  // From datasheet: CTRL2_G register:
  // ODR_G3, ODR_G2, ODR_G1, ODR_G0, FS1_G, FS0_G, FS_125, 0   (8 bits)

  // FS[1:0]_G   -   00: 250 deg/s
  //             -   01: 500 deg/s
  //             -   10: 1000 deg/s
  //             -   11: 2000 deg/s

  imu.writeReg(LSM6::CTRL2_G, 0b10001000);

  // Common

  // 0x04 = 0b00000100
  // IF_INC = 1 (automatically increment register address)
  imu.writeReg(LSM6::CTRL3_C, 0x04);
  
}
