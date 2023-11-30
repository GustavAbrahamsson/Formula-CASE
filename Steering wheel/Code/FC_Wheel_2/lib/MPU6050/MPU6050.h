#pragma once

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <vector>

struct xyz_vector{
  float x;
  float y;
  float z;
};

class IMU{
  private:
  Adafruit_MPU6050 imu;
  sensors_event_t a, g, temp;
  int address;

  public:
  IMU(int addr);
  int begin(mpu6050_accel_range_t acc_range, mpu6050_gyro_range_t gyr_range, mpu6050_bandwidth_t bandw, mpu6050_highpass_t hp_filter);
  void read();
  xyz_vector raw_acc();
  xyz_vector raw_gyr();
  int get_temp();

};