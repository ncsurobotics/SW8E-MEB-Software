// LED Strip update and writing code
// COLORS = RRGGBB
  // WHITE = 0xFFFFFF
  // RED = 0xFF0000
  // GREEN = 0x00FF00
  // BLUE = 0x0000FF
  // YELLOW = 0xF08000
  // MAGENTA = 0xFF005E
  // BLACK = 0x000000


#include <stdint.h>
#include <SPI.h>
#include "LEDs.h"


// LED Write and Update Function Definitions

// Configures the RGB values for a specified LED based on a passed-in 24-bit value in RGB format
void LED_Strip::set_One(int led, uint32_t value){
  if(led >= N_LEDS) return;
  reds[led] =  value >> 16;
  greens[led] = value >> 8;
  blues[led] = value;
}

// Configures the RGB Passes in an array of all the LED values and updates all the LEDs in the array at once
void LED_Strip::set_All(uint32_t value){
  for(int i = 0; i < N_LEDS; i++){
    set_One(i, value);
  }
  
}

// Transmits the signals to the LEDs necessary for them to recognize the 1s and 0s as 1s and 0s
void LED_Strip::write_LED(uint8_t r, uint8_t g, uint8_t b){
    // Combine in order g, r, b MSB first
    // String of 24-bit data to transmit to the LED Strip from MSB -> LSB
    uint32_t data = (((uint32_t)g) << 16) | (((uint32_t)r) << 8) | b;

    // Mask for grabbing the ith bit of the data variable, starting from the 23rd bit and going down to the 0th bit
    uint32_t mask = 0x800000;
    
    // Transmit code for each 1 or 0 (24 1's or 0's)
    // Transmit MSB first (23 bit first)
    for(int i = 23; i >= 0; --i){
 
        if(data & mask){
            // Bit i of the mask is a 1 - Transmits the 1 code (High 1/2 of the time, low 1/2 of the time)
            SPI.transfer(0xF0);
            asm("nop");
            asm("nop");
        }else{
            // Bit i of the mask is a 0 - transmit 0 code (High 1/8 of the time, low 7/8 of the time)
            SPI.transfer(0x80);
        }

        mask >>= 1; // Right shifts the mask each iteration to grab the value of the next bit
    }
    
}

// Sends the data required to write to the LEDs
void LED_Strip::update_LEDs(){
  for(int i = N_LEDS-1; i >= 0; --i){
        write_LED(reds[i], greens[i], blues[i]); 
    }
    delayMicroseconds(100);
}

// Turns off all the LEDs on the strip
void LED_Strip::reset_strip(){
  for(int i = N_LEDS-1; i >= 0; --i){
        write_LED(0, 0, 0); 
    }
    delayMicroseconds(100);
}
