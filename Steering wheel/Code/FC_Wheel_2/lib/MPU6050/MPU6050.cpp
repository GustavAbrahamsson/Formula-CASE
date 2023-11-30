#include "MPU6050.h"

IMU::IMU(int addr){
  address = addr;
};

//Initialize the IMU
int IMU::begin(mpu6050_accel_range_t acc_range, mpu6050_gyro_range_t gyr_range, mpu6050_bandwidth_t bandw, mpu6050_highpass_t hp_filter){
  if (!imu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  imu.setAccelerometerRange(acc_range);
  imu.setGyroRange(gyr_range);
  imu.setFilterBandwidth(bandw);
  imu.setHighPassFilter(hp_filter);

  return 0;
}

// Receive the accelerometer values in an 'xyz_vector'
xyz_vector IMU::raw_acc(){
  xyz_vector out;
  out.x = a.acceleration.x;
  out.y = a.acceleration.y;
  out.z = a.acceleration.z;
  return out;
};

// Receive the gyroscope values in an 'xyz_vector'
xyz_vector IMU::raw_gyr(){
  xyz_vector out;
  out.x = g.gyro.x;
  out.y = g.gyro.y;
  out.z = g.gyro.z;
  return out;
};

// Receive the temperature
int IMU::get_temp(){
  return temp.temperature;
};

void IMU::read(){
  IMU::imu.getEvent(&a, &g, &temp);
}