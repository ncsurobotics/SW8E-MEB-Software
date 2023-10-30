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
#define INPUT           0
#define OUTPUT          1

#define LOW             0
#define HIGH            1

#define RISING_EDGE     0
#define FALLING_EDGE    1

#define DISABLE         0
#define ENABLE          1

#define SET(func, pin)      ##func## |= ##pin##   // Set target to 1
#define CLEAR(func, pin)    ##func## &= ~##pin##  // Clear target to 0

#define PxDIR(n) 		P##n##DIR		// Port n Direction
#define PxOUT(n) 		P##n##OUT		// Port n Output
#define PxREN(n) 		P##n##REN		// Port n Resistor Enable
#define PxSEL0(n) 		P##n##SEL0		// Port n Function Select 0
#define PxSEL1(n) 		P##n##SEL1		// Port n Function Select 1
#define PxIES(n) 		P##n##IES		// Port n Interrupt Edge Select
#define PxIE(n) 		P##n##IE		// Port n Interrupt Enable
#define PxIFG(n) 		P##n##IFG		// Port n Interrupt Flag

#pragma endregion


#pragma region 

// Warning: There is no invalid input detection in the following macros.
// Use properly if you want proper results. (Default is to 0)

/* SET PIN I/O DIRECTION
 * 
 * Ex. usage for dummy pin
 * PIN_SET_DIR(DUMMY_PORT, DUMMY_PIN, INPUT)
 */
#define PIN_SET_DIR(port, pin, dir)

/* SET PIN OUTPUT VALUE
 * 
 * Ex. usage for dummy pin
 * PIN_SET_OUT(DUMMY_PORT, DUMMY_PIN, HIGH)
 */
#define PIN_SET_OUT(port, pin, value)

/* SET PIN PULLUP RESISTOR ENABLE
 *
 * Ex. usage for dummy pin
 * PIN_SET_REN(DUMMY_PORT, DUMMY_PIN, ENABLE)
 */
#define PIN_SET_REN(port, pin, value)

/* SET PIN FUNCTION (SEL)
 * 
 * Ex. usage for dummy pin
 * SET_PIN_SEL(DUMMY_PORT, DUMMY_PIN, 0b00)
 * SET_PIN_SEL(DUMMY_PORT, DUMMY_PIN, 0)
 */
#define PIN_SET_SEL(port, pin, sel) \
    if (sel & 0b01) { \
        SET(PxSEL0(port), ##pin##); \
    } else { \
        CLEAR(PxSEL0(port), ##pin##); \
    } \
    \
    if (sel & 0b10) { \
        SET(PxSEL1(port), ##pin##); \
    } else { \
        CLEAR(PxSEL1(port), ##pin##); \
    }

/* SET PIN IES
 * 
 * Ex. usage for dummy pin
 * SET_PIN_IES(DUMMY_PORT, DUMMY_PIN, RISING_EDGE)
 */
#define PIN_SET_IES(port, pin, edge)

/* SET INTERRUPT ENABLE
 *
 * Ex. usage for dummy pin
 * SET_PIN_IE(DUMMY_PORT, DUMMY_PIN, ENABLE)
 */
#define PIN_SET_IE(port, pin, value)

/* SET PIN 
 * 
 * Ex. usage for dummy pin
 * PIN_SET_IFG(DUMMY_PORT, DUMMY_PIN, HIGH)
 */
#define PIN_SET_IFG(port, pin, value)

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
