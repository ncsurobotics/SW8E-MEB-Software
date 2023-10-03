/**
 * @file ports.c
 * @author William Kelso (wpkelso)
 * @version 0.1
 *
 * @section DESCRIPTION
 *
 */
 
#include "ports.h"
#include "msp430.h"
 
 

// Leak Detection (Px_x)
PxSEL0(1) &= ~LEAK_DETECT;
PxSEL1(1) &= ~LEAK_DETECT;
PxDIR(1) &= ~LEAK_DETECT;
 
// LED Strip Data line (Px_x)
 
// System Power Enable (Px_x)
 
// Thruster arm state (Px_x)
 
// Software Kill (Px_x)
 
// SDA I2C data line (Px_x)
 
// SCL I2C clock line (Px_x)
 
// UART0 Transmit line (Px_x)

// UART0 Recieve line (Px_x)
 
// UART Debug Transmit line (Px_x)

// UART Debug Recieve line (Px_x)

// SPI Clock line (Px_x)

// SPI data out (Px_x)

// SPI data in (Px_x)