/*
 * gpio.c
 *
 *  Created on: Jan 29, 2022
 *      Author: chiny
 */
#include<gpio.h>
#include <msp430.h>
#include <ws2812.h>
void init(){
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


    // Setting the P2.7 as an input pin
    P2DIR &= ~(BIT7);
    P2REN &= ~(BIT7);
    initStrip();
}


void sensor(){
            // Performing AND operation of the P2IN register with BIT3(P2.7) to test if there is input
            if(P2IN & BIT7){
                // High(change led color)
                //fillStrip(0xFF,0x00,0x00); // yellow for leakage
                setLEDColor(2,0x00,0x00,0xFF);
                setLEDColor(3,0x00,0x00,0xFF);
                showStrip();
            }
            else{
                // Low(continue to read)
                clearStrip();
            }
}






