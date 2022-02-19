#include <msp430.h>
#include <gpio_devices.h>
#include <pc_comms.h>
#include <clocks.h>

// This function runs when the program starts
int main(void){
    // Disable watchdog timer
       WDTCTL = WDTPW | WDTHOLD;

    // Initialize devices
       init_clocks();
       init_gpio();
       init_pc_uart();

       while(1){
               // Will enter LPM0 while waiting for UART activity
               pc_comm_process();
        }

}
