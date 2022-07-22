
#ifndef __MPU6050_HPP__
#define __MPU6050_HPP__

#define MPU6050_ADDRESS_DEFAULT       0x68
#define MPU6050_DEVICE_ID             0x68
#define MPU6050_SELF_TEST_X           0x0D
#define MPU6050_SELF_TEST_Y           0x0E
#define MPU6050_SELF_TEST_Z           0x0F
#define MPU6050_SELF_TEST_A           0x10
#define MPU6050_SMPLRT_DIV            0x19
#define MPU6050_CONFIG                0x1A
#define MPU6050_GYRO_CONFIG           0x1B
#define MPU6050_ACCEL_CONFIG          0x1C
#define MPU6050_INT_PIN_CONFIG        0x37
#define MPU6050_WHO_AM_I              0x75
#define MPU6050_SIGNAL_PATH_RESET     0x68
#define MPU6050_USER_CTRL             0x6A
#define MPU6050_PWR_MGMT_1            0x6B
#define MPU6050_PWR_MGMT_2            0x6C
#define MPU6050_TEMP_H                0x41
#define MPU6050_TEMP_L                0x42
#define MPU6050_ACCEL_OUT             0x3B
#define MPU6050_GYRO_OUT              0x43

#define MPU6050_POWER_DELAY           20
#define MPU6050_POLL_PERIOD           10


#include <Wire.h>

class MPU6050{
public:
  MPU6050(TwoWire *wire = NULL, int addr = MPU6050_ADDRESS_DEFAULT);
  bool begin();
  void end();
  bool read(float *gyroX, float *gyroY, float *gyroZ, float *accelX, float *accelY, float *accelZ);

private:
  TwoWire *wire;
  int addr;
};

#endif // __MPU6050_HPP__
