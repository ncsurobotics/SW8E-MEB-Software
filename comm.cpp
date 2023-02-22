
#include "comm.hpp"
#include "conversions.hpp"

#define START_BYTE    253
#define END_BYTE      254
#define ESCAPE_BYTE   255


#define crc16_ccitt_false(data, len)      crc16_ccitt_false_partial((data), (len), 0xFFFF)  

/**
 * Calculate 16-bit CRC (CCITT-FALSE) of the given data
 * @param data Data to calculate crc of
 * @param len Length of data
 * @param initial Initial value to use in calc. Useful for fragmented data sets. 65535 for "default"
 * @return uint16_t Calculated crc
 */
static uint16_t crc16_ccitt_false_partial(uint8_t *data, unsigned int len, uint16_t initial){
    uint16_t crc = initial;
    int pos = 0;
    while(pos < len){
        uint8_t b = data[pos];
        for(int i = 0; i < 8; ++i){
            uint8_t bit = ((b >> (7 - i) & 1) == 1);
            uint8_t c15 = ((crc >> 15 & 1) == 1);
            crc <<= 1;
            if(c15 ^ bit){
                crc ^= 0x1021;
            }
        }
        pos++;
    }
    return crc;
}

Communication::Communication() : serial(NULL), curr_msg_id(0){

}

void Communication::begin(HardwareSerial &serial, unsigned int baud){
  this->serial = &serial;
  if(this->serial == NULL)
    return;
  read_buf_count = 0;
  parse_started = false;
  parse_escaped = false;
  this->serial->begin(baud);
}

void Communication::end(){
  if(this->serial != NULL)
    this->serial->end();
}

void Communication::sendMessage(uint8_t *msg, unsigned int len){
  // Write start byte
  serial->write(START_BYTE);
  
  // Write message id (big endian). Escape it as needed.
  uint8_t id_buf[2];
  Conversions::convertInt16ToData(curr_msg_id++, id_buf, false);
  if(id_buf[0] == START_BYTE || id_buf[0] == END_BYTE || id_buf[0] == ESCAPE_BYTE)
      serial->write(ESCAPE_BYTE);
  serial->write(id_buf[0]);
  if(id_buf[1] == START_BYTE || id_buf[1] == END_BYTE || id_buf[1] == ESCAPE_BYTE)
      serial->write(ESCAPE_BYTE);
  serial->write(id_buf[1]);

  // Write each byte of msg (escaping it if necessary)
  for(unsigned int i = 0; i < len; ++i){
      if(msg[i] == START_BYTE || msg[i] == END_BYTE || msg[i] == ESCAPE_BYTE)
          serial->write(ESCAPE_BYTE);
      serial->write(msg[i]);
  }

  // Calculate CRC and write it. CRC INCLUDES MESSAGE ID BYTES!!!
  // Each byte of the CRC must also be escaped if it matches a special byte.
  uint16_t crc = crc16_ccitt_false(id_buf, 2);
  crc = crc16_ccitt_false_partial(msg, len, crc);
  uint8_t high_byte = (crc >> 8) & 0xFF;
  uint8_t low_byte = crc & 0xFF;
  if(high_byte == START_BYTE || high_byte == END_BYTE || high_byte == ESCAPE_BYTE)
      serial->write(ESCAPE_BYTE);
  serial->write(high_byte);
  if(low_byte == START_BYTE || low_byte == END_BYTE || low_byte == ESCAPE_BYTE)
      serial->write(ESCAPE_BYTE);
  serial->write(low_byte);

  // Write end byte
  serial->write(END_BYTE);

  // Write the message now
  serial->flush();
}

bool Communication::readMessage(uint8_t *msg, unsigned int &msg_len, uint16_t &msg_id){
  // Read and handle a finite number of bytes
  unsigned int count = serial->available();
  uint8_t b;
  for(unsigned int i = 0; i < count; ++i){
    // Read next available character
    b = serial->read();

    // If message queue is full, discard bytes
    // Allow normal code to run for start byte because it will zero len
    if(b != START_BYTE && read_buf_count == MAX_MSG_LEN)
        continue;

    // Parse the meaning of this byte
    if(parse_escaped){
        // Currently escaped (previous byte was ESCAPE_BYTE)
        // Handle **valid** escape sequences (only special bytes can be escaped)
        // Ignore invalid escape sequences
        if(b == START_BYTE || b == END_BYTE || b == ESCAPE_BYTE)
            read_buf[read_buf_count++] = b;
        
        // Handled the byte after an escape byte. No longer escaped
        parse_escaped = false;
    }else if(parse_started){
        // If a start byte was previously received, handle this byte
        switch(b){
        case START_BYTE:
            // Handle start byte (special meaning when not escaped)
            // Discard old data when a start byte received
            read_buf_count = 0;
            break;
        case END_BYTE:
            // Handle end byte (special meaning when not escaped)
            // End byte means the buffer now holds the entire message

            if(read_buf_count < 4){
                // Too short to contain message id and crc bits. Invalid message.
                parse_started = false;
            }else{
                // Calculate CRC of read data. Exclude last two bytes.
                // Last two bytes are the CRC (big endian) appended to the original data
                // First two bytes are message ID. These are INCLUDED in CRC calc.
                uint16_t read_crc = crc16_ccitt_false(read_buf, read_buf_count - 2);
                uint16_t calc_crc = Conversions::convertDataToInt16(&read_buf[read_buf_count - 2], false);

                if(read_crc == calc_crc){
                    // This is a complete, valid message
                    
                    // First two bytes are msg_id (big endian)
                    msg_id = Conversions::convertDataToInt16(read_buf, false);

                    // Last two bytes omitted from copied data because they are crc
                    for(unsigned int i = 0; i < read_buf_count - 4; ++i){
                      msg[i] = read_buf[i + 2];
                    }
                    msg_len = read_buf_count - 4;

                    return true;
                }else{
                    // Got a complete message, but it is invalid. Ignore it.
                    parse_started = false;
                }
            }
            break;
        case ESCAPE_BYTE:
            // Handle escape byte (special meaning when not escaped)
            parse_escaped = true;
            break;
        default:
            // Handle normal bytes (these are just data)
            read_buf[read_buf_count++] = b;
            break;
        }
    }else if(b == START_BYTE){
        // Received a start byte. Start parsing. Discard old data.
        parse_started = true;
        read_buf_count = 0;
    }
  }
  return false;
}

void Communication::acknowledge(uint16_t msg_id, uint8_t error_code, uint8_t *result, unsigned int result_len){
  static uint8_t ack_buf[MAX_MSG_LEN];
  ack_buf[0] = 'A';
  ack_buf[1] = 'C';
  ack_buf[2] = 'K';
  Conversions::convertInt16ToData(msg_id, &ack_buf[3], false);
  ack_buf[5] = error_code;
  if(result_len > (MAX_MSG_LEN - 6))
    result_len = MAX_MSG_LEN - 6;
  for(unsigned int i = 0; i < result_len; ++i)
      ack_buf[6 + i] = result[i];
  sendMessage(ack_buf, 6 + result_len);
}
