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

// Macros for making pins *mildly* easier to manage
#define PxDIR(n) 		P##n##DIR		// Port n Direction
#define PxOUT(n) 		P##n##OUT		// Port n Output
#define PxREN(n) 		P##n##REN		// Port n Resistor Enable
#define PxSEL0(n) 		P##n##SEL0		// Port n Function Select 0
#define PxSEL1(n) 		P##n##SEL1		// Port n Function Select 1
#define PxIES(n) 		P##n##IES		// Port n 
#define PxIE(n) 		P##n##IE		// Port n
#define PxIFG(n) 		P##n##IFG		// Port n Interrupt Flag

// Pin definitions
#define LEAK_DETECT     BIT0	// Digital Input; Controlled by the leak sensor circuit; High = leak, Low = no leak
#define LED_STRIP_DAT   BIT0	// SPI Data Output (MOSI); Data line to the LED strip

#define SYS_ENABLE      BIT0	// Digital Output; Output driven high to keep system SSR on
#define THR_KILLED      BIT0	// Digital Input (with pullup); Signal from the thruster SSR that can be read to determine the arming state of the thrusters
#define SW_KILL         BIT0	// Digital Output; Driven high to arm thrusters in software

#define SDA             BIT0	// USB0SDA; I2C data pin
#define SCL             BIT0	// UCB0SCL; I2C clock pin

#define UART0_TX        BIT0	// UCA0TX; Application UART transmit from MEB
#define UART0_RX        BIT0	// UCA0RX; Application UART receive to MEB

#define UART_DBG_TX     BIT0	// UCA1TX; Debug UART transmit from MEB
#define UART_DBG_RX     BIT0	// UCA1RX; Debug UART recive to MEB

#define SPI_SCLK        BIT0	// UCB1SCLK; SPI clock
#define SPI_MOSI        BIT0	// UCB1MOSI; SPI data out from MEB
#define SPI_MISO        BIT0	// UCB1MISO; SPI data in to MEB 

#endif
