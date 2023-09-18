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

int main(){
    
    //---------------
    // Main Loop
    //---------------
    
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
            // No flags set. Enter LPM0. Interrupts will exitLPM0 when flag set
            LPM0;
        }
    }
    
    return 0;
}