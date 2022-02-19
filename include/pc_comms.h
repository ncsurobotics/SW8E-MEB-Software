/*
 * pc_comms.h
 *
 *  Created on: Aug 21, 2021
 *      Author: chiny
 */

#ifndef INCLUDE_PC_COMMS_H_
#define INCLUDE_PC_COMMS_H_

// Standard messages
#define HELP_MSG "  help - show this message\r\n" \
                 "  on - power on sensors\r\n" \
                 "  off - power off sensors\r\n" \
                 "  temp - read temperature\r\n"
#define UNKNOWN_MSG "Unknown command. Run 'help' for a list of commands.\r\n"
#define START_MSG "\r\n\r\n NewMemberProjFirmware\r\n"
#define SENSOR_ON_MSG "Sensors are now on.\r\n"\
                      "Temp sensor my take up to 5 seconds to start.\r\n"
#define SENSOR_OFF_MSG "Sensors are now off.\r\n"
#define PROMPT ">"


#define uca0_rx_buffer_len 128

// Data buffer initialization
volatile char uca0_rx_buffer[uca0_rx_buffer_len + 1];
extern volatile unsigned int uca0_rx_buffer_pos;

void init_pc_uart(void);

// ISR for data receive
__interrupt void pc_comm_rx_isr(void);

void send_byte_to_pc(char byte);
void send_string_to_pc(const char *message);

void pc_comm_process(void);


#endif /* INCLUDE_PC_COMMS_H_ */
