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
 
 

// Leak Detection (P1.5)
PIN_SET_SEL(LEAK_DETECT_PORT, LEAK_DETECT_PIN, 0);
 
// LED Strip Data line (Px.x)
 
// System Power Enable (Px.x)
 
// Thruster arm state (Px.x)
 
// Software Kill (Px.x)
 
// SDA I2C data line (Px.x)
 
// SCL I2C clock line (Px.x)
 
// UART0 Transmit line (Px.x)

// UART0 Recieve line (Px.x)
 
// UART Debug Transmit line (Px.x)

// UART Debug Recieve line (Px.x)

// SPI Clock line (Px.x)

// SPI data out (Px.x)

// SPI data in (Px.x)