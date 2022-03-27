
#include "aht10.hpp"
#include <Arduino.h>


AHT10::AHT10(TwoWire *wire, int addr) : wire(wire), addr(addr){
  if(this->wire == NULL){
    this->wire = &Wire;
  }
}

bool AHT10::begin(){
  delay(AHT10_POWER_DELAY);

  // I2C master mode
  wire->begin();

  // Make sure device exists (detect device at address)
  wire->beginTransmission(addr);
  if(wire->endTransmission() != 0){
    wire->end();
    return false;
  }

  // Reset
  wire->beginTransmission(addr);
  wire->write(AHT10_CMD_RESET);
  wire->endTransmission();

  // Calibrate
  wire->beginTransmission(addr);
  wire->write(AHT10_CMD_CALIBRATE);
  wire->write(0x08);
  wire->write(0x00);
  wire->endTransmission();

  // Wait for no longer busy and calibration complete
  while(readStatus() & AHT10_STATUS_BUSY) delay(AHT10_POLL_PERIOD);

  // Make sure calibration was successful
  if(!(readStatus() & AHT10_STATUS_CAL))
    return false;

  return true;
}

void AHT10::end() {
  if(this->wire != NULL)
    this->wire->end();
}

bool AHT10::read(float *temp, float *humidity){
  // Trigger read
  wire->beginTransmission(addr);
  wire->write(AHT10_CMD_TRIGGER);
  wire->write(0x33);
  wire->write(0x00);
  if(wire->endTransmission() != 0){
    return false;
  }

  // Wait until read done
  while(readStatus() & AHT10_STATUS_BUSY) delay(AHT10_POLL_PERIOD);

  // Read raw data
  uint8_t rawData[6];
  if(wire->requestFrom(addr, 6) != 6){
    return false;
  }
  for(uint8_t i = 0; i < 6; ++i){
    rawData[i] = wire->read();
  }

  // Calculate humidity as needed
  if(humidity != NULL){
    uint32_t h = rawData[1];
    h <<= 8;
    h |= rawData[2];
    h <<= 4;
    h |= rawData[3] >> 4;
    *humidity = ((float)h * 100) / 0x100000;
  }

  // Calculate temp as needed
  if(temp != NULL){
    uint32_t t = rawData[3] & 0x0F;
    t <<= 8;
    t |= rawData[4];
    t <<= 8;
    t |= rawData[5];
    *temp = ((float)t * 200 / 0x100000) - 50;
  }

  // Read successful
  return true;
}

uint8_t AHT10::readStatus() {
  wire->requestFrom(addr, 1);
  return wire->read();
}
