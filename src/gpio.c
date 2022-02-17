/*
 * gpio.c
 *
 *  Created on: Jan 29, 2022
 *      Author: chiny
 */
#include<gpio.h>
#include <msp430.h>
#include <ws2812.h>
void init_gpio(){
    // Setting all pins/ports as inputs
    P1DIR = 0;
    P1OUT = 0;
    P1REN = 0;
    P1SEL = 0;
    P1SEL2 = 0;
    P2DIR = 0;
    P2OUT = 0;
    P2REN = 0;
    P2SEL = 0;
    P2SEL2 = 0;
    // Setting the P2.3 as an input pin
    P2DIR &= ~(BIT3);
    P2REN &= ~(BIT3);
    initStrip();
}


void sensor(){
            // Performing AND operation of the P2IN register with BIT3(P2.3) to test if there is input
            if(P2IN & BIT3){
                // High(change led color)
                fillStrip(0xFF,0xFF,0x00);
                showStrip();
            }
            else{
                // Low(continue to read)
                clearStrip();
            }

}






