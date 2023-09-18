/**
 * @file ports.h
 * @author William Kelso (wpkelso)
 * @version 0.1
 * 
 * @section DESCRIPTION
 *
 */

#ifndef PORTS_H
#define PORTS_H

#define LEAK_DETECT     // Digital Input; Controlled by the leak sensor circuit; High = leak, Low = no leak
#define LED_STRIP_DAT   // SPI Data Output (MOSI); Data line to the LED strip

#define SYS_ENABLE      // Digital Output; Output driven high to keep system SSR on
#define THR_KILLED      // Digital Input (with pullup); Signal from the thruster SSR that can be read to determine the arming state of the thrusters
#define SW_KILL         // Digital Output; Driven high to arm thrusters in software

#define SDA             // USB0SDA; I2C data pin
#define SCL             // UCB0SCL; I2C clock pin

#define UART0_TX        // UCA0TX; Application UART transmit from MEB
#define UART0_RX        // UCA0RX; Application UART receive to MEB

#define UART_DBG_TX     // UCA1TX; Debug UART transmit from MEB
#define UART_DBG_RX     // UCA1RX; Debug UART recive to MEB

#define SPI_SCLK        // UCB1SCLK; SPI clock
#define SPI_MOSI        // UCB1MOSI; SPI data out from MEB
#define SPI_MISO        // UCB1MISO; SPI data in to MEB 

#endif
