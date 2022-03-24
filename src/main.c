#include <msp430.h>
#include <ws2812.h>
#include <aht10.h>
#include <gpio.h>

void gradualFill(u_int n, u_char r, u_char g, u_char b);

int main(void) {
    float temp;
    WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
    if (CALBC1_16MHZ==0xFF)    // If calibration constant erased
    {
        while(1);              // do not load, trap CPU!!
    }

    // configure clock to 16 MHz
    BCSCTL1 = CALBC1_16MHZ;    // DCO = 16 MHz
    DCOCTL = CALDCO_16MHZ;

    // initialize LED strip
    //initStrip();  // ***** HAVE YOU SET YOUR NUM_LEDS DEFINE IN WS2812.H? ******

    // set strip color red
    //fillStrip(0xFF, 0x00, 0x00);

    // show the strip
    //showStrip();
    init();
    aht10_init();

    while(1){
        //sensor(); // test if there is leakage
        temp = aht10_read_temp();
        if(temp>10){
            fillStrip(0x00,0xFF,0x00);
        }
        else{
            clearStrip();
        }
    }


    // gradually fill for ever and ever
    /*while (1) {
        gradualFill(NUM_LEDS, 0x00, 0x00, 0xFF);  // blue
        gradualFill(NUM_LEDS, 0x00, 0x00, 0x00);  // black
        //waterLeak(NUM_LEDS);
    }*/
}

// added comment
void gradualFill(u_int n, u_char r, u_char g, u_char b){
    int i;
    for (i = 0; i < n; i++){        // n is number of LEDs
        setLEDColor(i, r, g, b);
        showStrip();
        _delay_cycles(1000000);       // lazy delay

    }
}
    // comment added

    // Auxboard main.c
    // Detect leakage and temperatcure in continuously(while loop)
    // Display LED colors if there is leakage
    // Display LED colors in the temperature reaches a certain threshold

    //Red color for temperature
    //Yellow for leakage








