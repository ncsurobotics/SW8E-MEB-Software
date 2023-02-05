
#ifndef __PINMAP_H__
#define __PINMAP_H__

#ifdef ENERGIA_MSP_EXP430G2ET2553LP
// Pinmap for G2553 launchpad

#define SYS_POWER   P2_4            // Keep high to maintain system power
#define KILL_STAT   P2_3            // Input signal indicating thruster kill status
#define LEAK_SEN    P1_0            // Leak sensor input

#define RED_LED     P1_3            // Red status LED
#define GRN_LED     P1_4            // Green status LED
#define BLU_LED     P2_5            // Blue status LED
#define YLW_LED     P2_6            // Yellow status LED
#define WHT_LED     P2_7            // White status LED

#define SYS_LED     GRN_LED         // System power LED
#define TEMP_LED    RED_LED         // Overtemperature LED
#define LEAK_LED    BLU_LED         // Leak detected LED
#define THRU_LED    YLW_LED         // Thruster status LED
#define EXTRA_LED   WHT_LED         // Extra LED

// Note: P1_5, P1_6, P1_7 reserved for SPI
// Note: P2_1, P2_2 reserved for I2C
// Note: P1_1, P1_2 reserved for UART

#endif

#ifdef ENERGIA_MSP_EXP430FR2433LP

// Pinmap for FR2433 launchpad

#define SYS_POWER   P2_1            // Keep high to maintain system power
#define KILL_STAT   P2_0            // Input signal indicating thruster kill status
#define LEAK_SEN    P1_0            // Leak sensor input

#define RED_LED     P1_6            // Red status LED
#define GRN_LED     P1_7            // Green status LED
#define BLU_LED     P3_1            // Blue status LED
#define YLW_LED     P1_1            // Yellow status LED
#define WHT_LED     P2_2            // White status LED

#define SYS_LED     GRN_LED         // System power LED
#define TEMP_LED    RED_LED         // Overtemperature LED
#define LEAK_LED    BLU_LED         // Leak detected LED
#define THRU_LED    YLW_LED         // Thruster status LED
#define EXTRA_LED   WHT_LED         // Extra LED

// Note: P2_5, P2_6, P2_4 reserved for SPI
// Note: P1_2, P1_3 reserved for I2C
// Note: P1_4, P1_5 reserved for UART

#endif

#endif __PINMAP_H__
