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

typedef enum {
    Black   = 0x000000, // turns LED off
    White   = 0xFFFFFF,
    Red     = 0xFF0000,
    Yellow  = 0xF08000,
    Green   = 0x00FF00,
    Blue    = 0x0000FF,
    Magenta = 0xFF005E
} color_generic_t;

//---------------
// MAPPINGS
//---------------
/* Assignments here correspond with places in the LED arrays */
enum led_assignment {
    Sys_On_F = 0,
    Sys_On_B = 7,
    Arm_F = 1,
    Arm_B = 6,
    Programmable_0_F = 2,
    Programmable_0_B = 5,
    Programmable_1_F = 3,
    Programmable_1_B = 4
};

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
