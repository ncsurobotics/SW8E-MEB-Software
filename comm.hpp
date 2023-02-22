/**
 * Implements the message protocol used by AUVControlBoard
 */


#ifndef __COMM_HPP__
#define __COMM_HPP__

#include <Arduino.h>

#define MAX_MSG_LEN   64

// Acknowledge message error codes
#define ACK_ERR_NONE                    0   // No error
#define ACK_ERR_UNKNOWN_MSG             1   // The message is not recognized
#define ACK_ERR_INVALID_ARGS            2   // Arguments are invalid (too many, too few, etc)
#define ACK_ERR_INVALID_CMD             3   // Command is known, but invalid at this time
// Note 255 reserved for timeout error codes


class Communication{
public:
  Communication();

  /**
   * Start communication with the PC via UART
   * @param serial Reference to serial port to use
   * @Param baud Baud rate
   */
  void begin(HardwareSerial &serial, unsigned int baud);

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
   * @param msg_len Where to store length of data in msg buffer (reference)
   * @param msg_id Where to store id of received message (reference)
   * @return True if a message was received; else false. msg, msg_id, and msg_len are only assigned if true.
   */
  bool readMessage(uint8_t *msg, unsigned int &msg_len, unsigned int &msg_id);

  /**
   * Send acknowledgement message to PC
   * @param msg_id Id of message being acknowledged
   * @param error_code Error code for message receipt
   * @param result Extra data to append to acknowledge message
   * @param result_len Length of extra data
   */
  void acknowledge(uint16_t msg_id, uint8_t error_code, uint8_t *result, unsigned int result_len);

private:
  HardwareSerial *serial;
  uint8_t read_buf[MAX_MSG_LEN + 2 + 2];  // MSG + 2 id + 2 CRC
  unsigned int read_buf_count;
  bool parse_started, parse_escaped;
  uint16_t curr_msg_id;
};


#endif // __COMM_HPP__
