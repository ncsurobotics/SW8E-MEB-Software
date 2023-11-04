/**
 * @file main.c
 * @author William Kelso (wpkelso)
 * @version 0.1
 * 
 * @section DESCRIPTION
 *
 */

#include <msp430.h>
#include <stdbool.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
/// Flags from interrupt service routines
////////////////////////////////////////////////////////////////////////////////

// Can make this wider if necessary
volatile uint8_t flags = 0;

#define TIMING_10MS         BIT0        // Set every 10ms
#define TIMING_100MS        BIT1        // Set every 100ms
#define TIMING_500MS        BIT2        // Set every 500ms
#define TIMING_1S           BIT3        // Set every 1s
#define AHT10_DONE          BIT4        // AHT10 I2C done (success or fail)
#define AHT10_FAIL          BIT5        // AHT10 I2C failed

#define SET_FLAG(x)         flags |= (x)
#define CHECK_FLAG(x)       (flags & x)
#define CLEAR_FLAG(x)       flags &= ~(x)

int main() {
    //---------------
    // Main Loop
    //---------------
    
    // This general structure is lifted directly from MB3hel's AHT10 driver for the MSP430 platform (github.com/MB3hel/AHT10Driver)
    // The idea is that we run tasks in chunks depending on how often we want to schedule them, and go into low power mode if nothing currently needs to run
    while(true) {
        if (CHECK_FLAG(TIMING_10MS)) {
            CLEAR_FLAG(TIMING_100MS);
            // Run every 10ms
            
        } else if (CHECK_FLAG(TIMING_100MS)) {
            CLEAR_FLAG(TIMING_100MS);
            // Run every 100ms
            
        } else if (CHECK_FLAG(TIMING_500MS)) {
            CLEAR_FLAG(TIMING_500MS);
            // Run every 500ms
            
        } else if (CHECK_FLAG(TIMING_1S)) {
            CLEAR_FLAG(TIMING_1S);
            // Run ever 1sec
            
        } else {
            // No flags set. Enter LPM0. Interrupts will exit LPM0 when flag set
            LPM0;
        }
    }
    
    return 0;
}
