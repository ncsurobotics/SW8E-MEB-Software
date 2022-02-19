/*
 * gpio_devices.c
 *
 *  Created on: Aug 7, 2021
 *      Author: chiny
 */

#include <gpio_devices.h>
#include <msp430.h>
void init_gpio(void){

    // Sets pins P2.3, P2.4, P2.5 (as outputs)
    P2DIR |= BIT3 | BIT4 | BIT5;
    // Sets P2.4 as the high pin(board power
    P2OUT = BIT4;
    //Disables pullup and pulldowns resistors
    P2REN = 0;

    //Configure rest of ports to use all pins as inputs
    P1DIR = 0;
    P1OUT = 0;
    P1REN = 0;
    P3DIR = 0;
    P3OUT = 0;
    P3REN = 0;

    //This is a timer configuration(for compiler) for power LED blink

    TACCTL0 |= CCIE; //Enable interrupt CCR0 overflow for the timer(Timer_A)

    // The TA0CTL is the control register that is used to control the timer
    // using the SMCLK(sub main clock)
    // using Up mode
    // /8 as clock divider
    //
   TACTL = TASSEL_2 + MC_1 + ID_3 + TACLR; // Using SMCLK(TASSEL_2), Up mode(MC_1), input divider is /8(ID_3), resets clock(TACLR)
   TACCR0 = 62500;

   //TA0CTL = TASSEL;// Setting TAR to 62500( 2Hz interrupt rate)

    /* Helpful link when trying to understand the Timer Programming
    http://www.ocfreaks.com/msp430-timer-programming-tutorial/
    */

}

void enable_sensors(){
    // Proving power to pins P2.3 and P2.4
    P2OUT |= BIT3 | BIT5;
}

void disable_sensors(){

    // Disabling power to pins P2.3 and P2.4(setting it as low)
    P2OUT &= ~(BIT3 | BIT5);
}

#pragma vector = TIMER0_A0_VECTOR //Sets the interrupt timer for the compiler
// Interrupts the program with LED blink
__interrupt void blink_power_led(void) {
    //Toggles LED's
    P2OUT ^= BIT4;
}





