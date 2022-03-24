/*
 * pc_comms.c
 *
 *  Created on: Aug 21, 2021
 *      Author: chiny
 */
#include <pc_comms.h>
#include <msp430.h>
#include <string.h>
#include <stdio.h>
#include <aht10.h>
#include <gpio_devices.h>

volatile char uca0_rx_buffer[uca0_rx_buffer_len + 1];
volatile unsigned int uca0_rx_buffer_pos=0;

void init_pc_uart(void){
    //Setting registers for UART communication
    // We are essentially configuring pins P1.1 and P1.2 for the UART
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;
    // Configuring the UCA0 bus for UART mode
    UCA0CTL1 |= UCSSEL_2 + UCSWRST;
    UCA0BR0 = 104; // Baud rate setting for 9600 baud at 1MHz
    UCA0BR1 = 0; // Baud rate setting for 9600 baud at 1MHz
    UCA0MCTL = UCBRS_1; // Modulation  setting for 9600 baud at 1MHz
    UCA0CTL1 &= ~UCSWRST; // Initializes UART

    // Enables receive interrupt(call ISR to receive data from UART
    IE2 |= UCA0RXIE;
    send_string_to_pc(START_MSG);
    send_string_to_pc(PROMPT);

 }

#pragma vector=USCIAB0RX_VECTOR

// I guess an interrupt starts whenever we start entering characters
__interrupt void pc_comm_rx_isr(void){
    // if the buffer is full; discard the old data
    // This makes sure that a \n will be ignored
    if(uca0_rx_buffer_pos == uca0_rx_buffer_len){
       uca0_rx_buffer_pos = 0;
    }

    //Echo character
    if(UCA0RXBUF != '\n'){
        UCA0TXBUF = UCA0RXBUF;
    }
    if(UCA0RXBUF == 127){
            // PuTTY uses 127 for backspace
            // Don't put backspace in buffer
            // Remove previous character from buffer
            uca0_rx_buffer_pos--;
    }

    else if(UCA0RXBUF != '\n' && UCA0RXBUF != '\r'){
            // Put in buffer if not \n or \r (line feed of carriage return)
            uca0_rx_buffer[uca0_rx_buffer_pos] = UCA0RXBUF;
            uca0_rx_buffer_pos++;
     }

     // If complete command (enter was pressed) handle the received data
     if(UCA0RXBUF == '\r' || (UCA0RXBUF == '\n' && uca0_rx_buffer_pos > 0)){
         // Exit LPM0 after interrupt exits
          __bic_SR_register_on_exit(CPUOFF);
     }

        // Clear RX interrupt flag
        IFG2 &= ~UCA0RXIFG;
}

void send_byte_to_pc(char byte){
    while( !(IFG2 & UCA0TXIFG)); // waits for current TX to finish
    UCA0TXBUF = byte;
}


void send_string_to_pc(const char *message){
    unsigned int i;
    // For loop is for each and every char
    for(i = 0; i < strlen(message); ++i){
            while (!(IFG2&UCA0TXIFG)); // Wait for current TX to finish
            UCA0TXBUF = message[i];
    }

}





 void pc_comm_process(void){
     //Put in low power mode 0 (CPU off) with interrupts enabled
     // Waiting for something to happen that needs some action to be performed
      __bis_SR_register(LPM0_bits + GIE);
      // The ISR will exit LPM0 when a complete command has been received
      // In other words, these next lines only run after a
      // complete command has been received
      // Also echo '\n' after receiving \r b/c PuTTY is weird...
      send_byte_to_pc('\n');
      // Append null terminator (so string operations work as intended)
      uca0_rx_buffer[uca0_rx_buffer_pos] = '\0';

      if(strcmp((const char*)uca0_rx_buffer, "help") == 0){
              // Response to help command
          send_string_to_pc(HELP_MSG);
      }
      else if(strcmp((const char*)uca0_rx_buffer, "on") ==0){
          //Enable sensors
          enable_sensors();
          __delay_cycles(500000);
          IE2 &= ~UCA0RXIE; // Disable UART RX interrupt while I2C TX occurs
          aht10_init();
          IE2 |= UCA0RXIE;
          send_string_to_pc(SENSOR_ON_MSG);
          uca0_rx_buffer_pos = 0;
          while(UCA0RXBUF != 'b'){
              char temp_str[10];
              float temp = aht10_read_temp();
              sprintf(temp_str, "%d.%d\r\n", (int)temp, (int)((temp - (int)temp) * 100));
              send_string_to_pc(temp_str);
              __delay_cylcles(.5e6);
              if(strcmp((const char*)uca0_rx_buffer, "off") == 0){
                        // Send temperature data
                        disable_sensors();
                        send_string_to_pc(SENSOR_OFF_MSG);
                    }
          }

      }
      else if(strcmp((const char*)uca0_rx_buffer, "off") == 0){
          // Send temperature data
          disable_sensors();
          send_string_to_pc(SENSOR_OFF_MSG);
      }
      else if(strcmp((const char*)uca0_rx_buffer, "temp") == 0){
          // Will change this after implementing the driver
          char temp_str[10];
          float temp = aht10_read_temp();
          sprintf(temp_str, "%d.%d\r\n", (int)temp, (int)((temp - (int)temp) * 100));
          send_string_to_pc(temp_str);
      }
      else{
          // Unknown command
          send_string_to_pc(UNKNOWN_MSG);
      }
      // Show prompt again
       send_string_to_pc(PROMPT);
      // Clear rx buffer
       uca0_rx_buffer_pos = 0;
    }

