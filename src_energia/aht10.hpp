
#ifndef __AHT10_HPP__
#define __AHT10_HPP__

#define AHT10_ADDRESS_DEFAULT       0x38
#define AHT10_CMD_CALIBRATE         0xE1
#define AHT10_CMD_TRIGGER           0xAC
#define AHT10_CMD_RESET             0xBA
#define AHT10_STATUS_BUSY           0x80
#define AHT10_STATUS_CAL            0x08

#define AHT10_POWER_DELAY           20
#define AHT10_POLL_PERIOD           10

#include <stdint.h>
#include <Wire.h>

class AHT10 {
public:
  AHT10(TwoWire *wire = NULL, int addr = AHT10_ADDRESS_DEFAULT);
  bool begin();
  void end();
  bool read(float *temp, float *humidity);

private:
  uint8_t readStatus();

  TwoWire *wire;
  int addr;
};

#endif // __AHT10_HPP__
