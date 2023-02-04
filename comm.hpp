
#ifndef __COMM_HPP__
#define __COMM_HPP__

#include <Arduino.h>

#define serial_t HardwareSerial
#define MAX_MSG_LEN   64


class Communication{
public:
  /**
   * Start communication with the PC via UART
   * @param serial Reference to serial port to use
   * @Param baud Baud rate
   */
  void begin(serial_t &serial, unsigned int baud);

  /**
   * Close serial communication
   */
  void end();

  /**
   * Send a message to the PC
   * @param msg Message data to send
   * @param len Length of msg
   */
  void sendMessage(uint8_t *msg, unsigned int len);

  /**
   * Read a complete message from the PC
   * Must call periodically (often enough to prevent uart buffer from filling)
   * @param msg Buffer to store the message in (must be size MAX_MSG_LEN)
   * @return Number of bytes written into buffer
   */
  unsigned int readMessage(uint8_t *msg);

private:
  serial_t serial;
  uint8_t read_buf[MAX_MSG_LEN];
  unsigned int read_buf_count;
  bool parse_started, parse_escaped;
};


#endif // __COMM_HPP__
