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

void pins_setup() {
    // Leak Detection (P1.5)
    PIN_SET_SEL(LEAK_DETECT_PORT, LEAK_DETECT_PIN, 0b00)
    PIN_SET_DIR(LEAK_DETECT_PORT, LEAK_DETECT_PIN, INPUT)
    PIN_SET_REN(LEAK_DETECT_PORT, LEAK_DETECT_PIN, ENABLE)
    PIN_SET_OUT(LEAK_DETECT_PORT, LEAK_DETECT_PIN, LOW)

    // LED Strip Data line (Px.x)

    // System Power Enable (P2.5)
    PIN_SET_SEL(SYS_ENABLE_PORT, SYS_ENABLE_PIN, 0b00)
    PIN_SET_DIR(SYS_ENABLE_PORT, SYS_ENABLE_PIN, OUTPUT)
    PIN_SET_OUT(SYS_ENABLE_PORT, SYS_ENABLE_PIN, HIGH)

    // Thruster arm state (P3.0)
    PIN_SET_SEL(THR_KILLED_PORT, THR_KILLED_PIN, 0b00)
    PIN_SET_DIR(THR_KILLED_PORT, THR_KILLED_PIN, INPUT)
    PIN_SET_REN(THR_KILLED_PORT, THR_KILLED_PIN, ENABLE)
    PIN_SET_OUT(THR_KILLED_PORT, THR_KILLED_PIN, HIGH)

    // Software Kill (Px.x)

    // SDA I2C data line (Px.x)

    // SCL I2C clock line (Px.x)

    // UART0 Transmit line (Px.x)

    // UART0 Receive line (Px.x)

    // UART Debug Transmit line (Px.x)

    // UART Debug Receive line (Px.x)

    // SPI Clock line (Px.x)

    // SPI data out (Px.x)

    // SPI data in (Px.x)
}
