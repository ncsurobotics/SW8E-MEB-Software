/**
 * @file leds.h
 * @author William Kelso (wpkelso)
 * @version 0.1
 * 
 * @section DESCRIPTION
 *
 */
 
#include <stdint.h>

#ifndef LEDS_H
#define LEDS_H

//---------------
// COLORS
//---------------
/* Colors are defined in the format (0xRRGGBB) */
#define BLACK       (0x000000) // Turns the LED OFF
#define WHITE       (0xFFFFFF)
#define RED         (0xFF0000)
#define YELLOW      (0xF08000)
#define GREEN       (0x00FF00)
#define BLUE        (0x0000FF)
#define MAGENTA     (0xFF005E)

//---------------
// MAPPINGS
//---------------
#define PH0 (0)
#define PH1 (1)
#define PH2 (2)
#define PH3 (3)
#define PH4 (4)
#define PH5 (5)
#define PH6 (6)
#define PH7 (7)
#define PH8 (8)

//---------------
// FUNCTIONS
//---------------

/*
 * Configures the RGB values for a specified LED
 * @param led 
 * @param color
 */
void leds_set_single(int led, uint32_t color);

/*
 * Configures the RGB for all LEDs
 * @param color
 */
void leds_set_all(uint32_t color);

/*
 * Transmits the signals to the LEDs necessary for them to recognize the 1s and 0s as 1s and 0s
 * @param r
 * @param g
 * @param b
 */
void leds_write(uint8_t r, uint8_t g, uint8_t b);

/*
 * Sends the data required to write to the LEDs
 */
void leds_update();

/*
 * Turns off all the LEDs on the strip
 */
void leds_reset_all();

#endif