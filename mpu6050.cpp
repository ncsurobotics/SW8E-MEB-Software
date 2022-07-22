
#include "mpu6050.hpp"
#include <Arduino.h>


MPU6050::MPU6050(TwoWire *wire, int addr) : wire(wire), addr(addr){
  if(this->wire == NULL){
    this->wire = &Wire;
  }
}

bool MPU6050::begin(){
  delay(MPU6050_POWER_DELAY);

  // I2C master mode
  // wire->begin();

  // Make sure device exists (detect device at address)
  wire->beginTransmission(addr);
  if(wire->endTransmission(true) != 0){
    wire->end();
    return false;
  }

  // Reset MPU6050
  wire->beginTransmission(addr);
  wire->write(MPU6050_PWR_MGMT_1);
  wire->endTransmission(true);
  wire->requestFrom(addr, 1);
  uint8_t pm1val = wire->read();
  pm1val |= 0x80;        // Set bit 7
  wire->beginTransmission(addr);
  wire->write(MPU6050_PWR_MGMT_1);
  wire->write(pm1val);
  wire->endTransmission(true);

  // Wait for reset to finish
  while(true){
    wire->beginTransmission(addr);
    wire->write(MPU6050_PWR_MGMT_1);
    wire->endTransmission(true);
    wire->requestFrom(addr, 1);
    pm1val = wire->read();

    // Wait for bit 7 to become 0
    if((pm1val & 0x80) == 0)
      break;
    delay(MPU6050_POLL_PERIOD);
  }
  delay(100);

  // Reset signal path to defaults
  wire->beginTransmission(addr);
  wire->write(MPU6050_SIGNAL_PATH_RESET);
  wire->write(0x07);
  wire->endTransmission(true);

  // Sample rate divider = 0
  wire->beginTransmission(addr);
  wire->write(MPU6050_SMPLRT_DIV);
  wire->write(0x00);
  wire->endTransmission(true);

  // Disable low pass filter
  wire->beginTransmission(addr);
  wire->write(MPU6050_CONFIG);
  wire->endTransmission(true);
  wire->requestFrom(addr, 1);
  uint8_t cfgval = wire->read();
  cfgval &= ~(0x7);   // Zero lowest three bits to disable lpf
  wire->beginTransmission(addr);
  wire->write(MPU6050_CONFIG);
  wire->write(cfgval);
  wire->endTransmission(true);

  // Set clk = pll w/ gyro x ref
  wire->beginTransmission(addr);
  wire->write(MPU6050_PWR_MGMT_1);
  wire->write(0x01);
  wire->endTransmission(true);

  // Set gyro range
  // 0x00 = 250DPS
  // 0x01 = 500DPS
  // 0x02 = 1000DPS
  // 0x03 = 2000DPS
  wire->beginTransmission(addr);
  wire->write(MPU6050_GYRO_CONFIG);
  wire->endTransmission(true);
  wire->requestFrom(addr, 1);
  uint8_t gyrocfgval = wire->read();
  gyrocfgval &= ~(0x18);      // Zero bits 4 and 3
  gyrocfgval |= (0x01) << 3;  // Set range in correct bit pos
  wire->beginTransmission(addr);
  wire->write(MPU6050_GYRO_CONFIG);
  wire->write(gyrocfgval);
  wire->endTransmission(true);

  // Set accel range
  // 0x00 = +/- 2G
  // 0x01 = +/- 4G
  // 0x02 = +/- 8G
  // 0x03 = +/- 16G
  wire->beginTransmission(addr);
  wire->write(MPU6050_ACCEL_CONFIG);
  wire->endTransmission(true);
  wire->requestFrom(addr, 1);
  uint8_t accelcfgval = wire->read();
  accelcfgval &= ~(0x18);      // Zero bits 4 and 3
  accelcfgval |= (0x00) << 3;  // Set range in correct bit pos
  wire->beginTransmission(addr);
  wire->write(MPU6050_ACCEL_CONFIG);
  wire->write(accelcfgval);
  wire->endTransmission(true);

  return true;
}

bool MPU6050::read(float *gyroX, float *gyroY, float *gyroZ, float *accelX, float *accelY, float *accelZ){
  uint8_t rawData[6];
  
  // Read gyro data
  wire->beginTransmission(addr);
  wire->write(MPU6050_GYRO_OUT);
  wire->endTransmission(true);
  uint8_t len = wire->requestFrom(addr, 6);
  for(uint8_t i =0; i < len; ++i){
    rawData[i] = wire->read();
  }
  if(len != 6)
    return false;

  // Conversion to deg / sec (depends on configured range)
  // 131.0f = 250DPS
  // 65.5f  = 500DPS
  // 32.8f  = 1000DPS
  // 16.4f  = 2000DPS
  *gyroX = (int16_t)(rawData[1] | (rawData[0] << 8)) / 65.5f;
  *gyroY = (int16_t)(rawData[3] | (rawData[2] << 8)) / 65.5f;
  *gyroZ = (int16_t)(rawData[5] | (rawData[4] << 8)) / 65.5f;

  // Read accel data
  wire->beginTransmission(addr);
  wire->write(MPU6050_ACCEL_OUT);
  wire->endTransmission(true);
  len = wire->requestFrom(addr, 6);
  for(uint8_t i =0; i < len; ++i){
    rawData[i] = wire->read();
  }
  if(len != 6)
    return false;

  // Scaling of values depends both on mode and range
  // Sensor is in high resolution mode so:
  // 16384.0f = +/- 2G
  // 8192.0f  = +/- 4G
  // 4096.0f  = +/- 8G
  // 2048.0f  = +/- 16G
  *accelX = (int16_t)((rawData[1] | (rawData[0] << 8))) / 16384.0f;
  *accelY = (int16_t)((rawData[3] | (rawData[2] << 8))) / 16384.0f;
  *accelZ = (int16_t)((rawData[5] | (rawData[4] << 8))) / 16384.0f;

  // Convert from g's to m / s^2
  *accelX *= 9.80665f;
  *accelY *= 9.80665f;
  *accelZ *= 9.80665f;
  
  return true;
}
