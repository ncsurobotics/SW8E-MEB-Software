#pragma once

#include <Arduino.h>
#include <Wire.h>


// Addresses of expansion boards
#define MSB_ADDR      0x20

// Note: Not implemented using virtual members and inheritance
//       because G2553 has only 512 bytes RAM
class ExpansionBoard{
public:
  ExpansionBoard(uint8_t addr, TwoWire *wire = NULL);

  /**
   * Transfer data between MEB and another board via I2C
   * There is no STOP between the write and read!
   * Either write or read counts may be zero
   * Returns an error code or number of bytes written
   *   Error codes are negative values returned by endTransmission
   *    -1 = data too long to fit in tx buffer
   *    -2 = received NACK on transmit of address
   *    -3 = received NACK on transmit of data
   *    -4 = other write error
   *    -5 = write timeout
   *    -6 = read failure (not enough bytes read)
   *   Zero means no error (write success and readCount bytes were read)
   */
  int transfer(uint8_t *writeBuf, unsigned int writeCount, uint8_t *readBuf, unsigned int readCount);
private:
  TwoWire *wire = NULL;
  uint8_t addr;
};
