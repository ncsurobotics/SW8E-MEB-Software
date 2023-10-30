/**
 * @file ports.h
 * @author William Kelso (wpkelso)
 * @version 0.1
 * 
 * @section DESCRIPTION
 * Definitions of pins used in MEBv2.0, as well as various preprocessor
 * macros that can be used to simplify pin management.
 */

#ifndef PORTS_H
#define PORTS_H

#pragma region 
// Macros for making pins *mildly* easier to manage
#define INPUT   0
#define OUTPUT  1
#define SET(func, pin) (##func## |= ##pin##) // set target to 1
#define CLEAR(func, pin) (##func## &= ~##pin##) // clear target to 0

#define PxDIR(n) 		P##n##DIR		// Port n Direction
#define PxOUT(n) 		P##n##OUT		// Port n Output
#define PxREN(n) 		P##n##REN		// Port n Resistor Enable
#define PxSEL0(n) 		P##n##SEL0		// Port n Function Select 0
#define PxSEL1(n) 		P##n##SEL1		// Port n Function Select 1
#define PxIES(n) 		P##n##IES		// Port n 
#define PxIE(n) 		P##n##IE		// Port n Interrupt Enable
#define PxIFG(n) 		P##n##IFG		// Port n Interrupt Flag
#pragma endregion


#pragma region 
// Warning: There is no invalid input detection in the following macros.
// Use properly if you want proper results. (Default is to 0)

/* SET PIN I/O DIRECTION
 * Ex. usage for Dummy Pin
 * 
 * SET_PIN_DIR(DUMMY_PORT, DUMMY_PIN, INPUT)
 */
// TODO: check the validity of this with the data sheet. Input and output might be switched
#define SET_PIN_DIR(port, pin, dir)
    #if dir
        SET(PxSDIR(##port##), ##pin##)
    #else
        CLEAR(PxDIR(##port##), ##pin##)
    #endif

/* SET PIN FUNCTION (SEL)
 * 
 * Ex. usage for Dummy Pin
 * SET_PIN_SEL(DUMMY_PORT, DUMMY_PIN, b00)
 * SET_PIN_SEL(DUMMY_PORT, DUMMY_PIN, 0)
 * 
 */
#define SET_PIN_SEL(port, pin, sel)
    #if sel & 0x0
        SET(PxSEL0(##port##), ##pin##)
    #else 
        CLEAR(PxSEL0(##port##), ##pin##)
    #endif

    #if sel & 0x1
        SET(PxSEL1(##port##), ##pin##)
    #else
        CLEAR(PxSEL1(##port##), ##pin##)
    #endif

/* SET PIN IES
 * 
 * 
 * 
 */
#pragma endregion

#pragma region 
/*
 * ---------------
 * Pin definitions
 * ---------------
 */

// P1.5 | Digital Input; Controlled by the leak sensor circuit; High = leak, Low = no leak
#define LEAK_DETECT_PORT    1
#define LEAK_DETECT_PIN     BIT5
// P4.6 | SPI Data Output (MOSI); Data line to the LED strip
#define LED_STRIP_DAT_PORT  4
#define LED_STRIP_DAT_PIN   BIT6



// P2.5 | Digital Output; Output driven high to keep system SSR on
#define SYS_ENABLE_PORT     2
#define SYS_ENABLE_PIN      BIT5
// P3.0 | Digital Input (with pullup); Signal from the thruster SSR that can be read to determine the arming state of the thrusters
#define THR_KILLED_PORT     3
#define THR_KILLED_PIN      BIT0
// P3.6 | Digital Output; Driven high to arm thrusters in software
#define SW_KILL_PORT        3
#define SW_KILL_PIN         BIT6



// P1.2 | USB0SDA; I2C data pin
#define SDA_PORT            1
#define SDA_PIN             BIT2
// P1.3 | UCB0SCL; I2C clock pin
#define SCL_PORT            1
#define SCL_PIN             BIT3



// P1.7 | UCA0TX; Application UART transmit from MEB
#define UART0_TX_PORT       1
#define UART0_TX_PIN        BIT7
// P1.6 | UCA0RX; Application UART receive to MEB
#define UART0_RX_PORT       1
#define UART0_RX_PIN        BIT6



// P4.3 | UCA1TX; Debug UART transmit from MEB
#define UART_DBG_TX_PORT    4
#define UART_DBG_TX_PIN     BIT3
// P4.2 | UCA1RX; Debug UART recive to MEB
#define UART_DBG_RX_PORT    4
#define UART_DBG_RX_PIN     BIT2



// P4.5 | UCB1SCLK; SPI clock
#define SPI_SCLK_PORT       4
#define SPI_SCLK_PIN        BIT5
// P4.6 | UCB1MOSI; SPI data out from MEB
#define SPI_MOSI_PORT       4
#define SPI_MOSI_PIN        BIT6
// P4.7 | UCB1MISO; SPI data in to MEB 
#define SPI_MISO_PORT       4
#define SPI_MISO_PIN        BIT7
#pragma endregion

#endif
