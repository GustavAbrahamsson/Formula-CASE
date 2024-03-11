#pragma once

#include <Arduino.h>
#include <LSM6.h>


struct xyz{
  float x;
  float y;
  float z;
  float x_raw;
  float y_raw;
  float z_raw;
};

class LSM6D_IMU{
  private:
  LSM6 imu;
  
  static const uint8_t num_motors = 2 ;

  public:
  LSM6D_IMU();

  void begin();
  void update();

  void set_resolution();

  xyz a; // Accelerometer
  xyz g; // Gyroscope

};