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
#define PxIE(n) 		P##n##IE		// Port n Interrupt Enable
#define PxIFG(n) 		P##n##IFG		// Port n Interrupt Flag

// Pin definitions
#define LEAK_DETECT     BIT5	// P1.5 | Digital Input; Controlled by the leak sensor circuit; High = leak, Low = no leak
#define LED_STRIP_DAT   BIT6	// P4.6 | SPI Data Output (MOSI); Data line to the LED strip

#define SYS_ENABLE      BIT5	// P2.5 | Digital Output; Output driven high to keep system SSR on
#define THR_KILLED      BIT0	// P3.0 | Digital Input (with pullup); Signal from the thruster SSR that can be read to determine the arming state of the thrusters
#define SW_KILL         BIT6	// P3.6 | Digital Output; Driven high to arm thrusters in software

#define SDA             BIT2	// P1.2 | USB0SDA; I2C data pin
#define SCL             BIT3	// P1.3 | UCB0SCL; I2C clock pin

#define UART0_TX        BIT7	// P1.7 | UCA0TX; Application UART transmit from MEB
#define UART0_RX        BIT6	// P1.6 | UCA0RX; Application UART receive to MEB

#define UART_DBG_TX     BIT3	// P4.3 | UCA1TX; Debug UART transmit from MEB
#define UART_DBG_RX     BIT2	// P4.2 | UCA1RX; Debug UART recive to MEB

#define SPI_SCLK        BIT5	// P4.5 | UCB1SCLK; SPI clock
#define SPI_MOSI        BIT6	// P4.6 | UCB1MOSI; SPI data out from MEB
#define SPI_MISO        BIT7	// P4.7 | UCB1MISO; SPI data in to MEB 

#endif
