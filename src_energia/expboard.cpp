
#include "expboard.hpp"


ExpansionBoard::ExpansionBoard(uint8_t addr, TwoWire *wire) : addr(addr), wire(wire){
  if(this->wire == NULL){
    this->wire = &Wire;
  }
}

int ExpansionBoard::transfer(uint8_t *writeBuf, unsigned int writeCount, uint8_t *readBuf, unsigned int readCount){
  
  if(writeCount > 0){
    wire->beginTransmission(addr);
    wire->write(writeBuf, writeCount);
    unsigned int res = wire->endTransmission(readCount == 0);
    if(res != 0)
      return -res; // Failed to write
  }

  if(readCount > 0){
    if(wire->requestFrom((uint8_t)addr, (uint8_t)readCount, (uint8_t)true) != readCount){
      // Not enough data read. Disacard data in buffer and return error code
      while(wire->available()) wire->read();
      return -6;
    }
    // Move read data into buffer
    for(unsigned int i = 0; i < readCount; ++i){
      readBuf[i] = wire->read();
    }
  }

  // Success
  return 0;
}
