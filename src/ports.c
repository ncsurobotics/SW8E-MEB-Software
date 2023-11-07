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

    // LED Strip Data line (P4.6)
    PIN_SET_SEL(LED_STRIP_DAT_PORT, LED_STRIP_DAT_PIN, 0b01)
    PIN_SET_DIR(LED_STRIP_DAT_PORT, LED_STRIP_DAT_PIN, OUTPUT)

    // System Power Enable (P2.5)
    PIN_SET_SEL(SYS_ENABLE_PORT, SYS_ENABLE_PIN, 0b00)
    PIN_SET_DIR(SYS_ENABLE_PORT, SYS_ENABLE_PIN, OUTPUT)
    PIN_SET_OUT(SYS_ENABLE_PORT, SYS_ENABLE_PIN, HIGH)

    // Thruster arm state (P3.0)
    PIN_SET_SEL(THR_KILLED_PORT, THR_KILLED_PIN, 0b00)
    PIN_SET_DIR(THR_KILLED_PORT, THR_KILLED_PIN, INPUT)
    PIN_SET_REN(THR_KILLED_PORT, THR_KILLED_PIN, ENABLE)
    PIN_SET_OUT(THR_KILLED_PORT, THR_KILLED_PIN, HIGH)

    // Software Kill (P3.6)
    PIN_SET_SEL(SW_KILL_PORT, SW_KILL_PIN, 0b00)
    PIN_SET_DIR(SW_KILL_PORT, SW_KILL_PIN, OUTPUT)
    
    // SDA I2C data line (P1.2)
    PIN_SET_SEL(SDA_PORT, SDA_PIN, 0b01)

    // SCL I2C clock line (P1.3)
    PIN_SET_SEL(SCL_PORT, SCL_PIN, 0b01)
    PIN_SET_DIR(SCL_PORT, SCL_PIN, OUTPUT)

    // UART0 Transmit line (P1.7)
    PIN_SET_SEL(UART0_TX_PORT, UART0_TX_PIN, 0b01)
    PIN_SET_DIR(UART0_TX_PORT, UART0_TX_PIN, OUTPUT)

    // UART0 Receive line (P1.6)
    PIN_SET_SEL(UART0_RX_PORT, UART0_RX_PIN, 0b01)
    PIN_SET_DIR(UART0_RX_PORT, UART0_RX_PIN, INPUT)

    // UART Debug Transmit line (P4.3)
    PIN_SET_SEL(UART_DBG_TX_PORT, UART_DBG_TX_PIN, 0b01)
    PIN_SET_DIR(UART_DBG_TX_PORT, UART_DBG_TX_PIN, OUTPUT)

    // UART Debug Receive line (P4.2)
    PIN_SET_SEL(UART_DBG_RX_PORT, UART_DBG_RX_PIN, 0b01)
    PIN_SET_DIR(UART_DBG_RX_PORT, UART_DBG_RX_PIN, INPUT)

    // SPI Clock line (P4.5)
    PIN_SET_SEL(SPI_SCLK_PORT, SPI_SCLK_PIN, 0b01)
    PIN_SET_DIR(SPI_SCLK_PORT, SPI_SCLK_PORT, OUTPUT)

    // SPI data out (P4.6)
    PIN_SET_SEL(SPI_MOSI_PORT, SPI_MOSI_PIN, 0b01)
    PIN_SET_DIR(SPI_MOSI_PORT, SPI_MOSI_PIN, OUTPUT)

    // SPI data in (P4.7)
    PIN_SET_SEL(SPI_MISO_PORT, SPI_MISO_PIN, 0b01)
    PIN_SET_DIR(SPI_MISO_PORT, SPI_MISO_PIN, INPUT)
}
