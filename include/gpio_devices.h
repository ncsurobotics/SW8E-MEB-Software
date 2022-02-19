/*
 * gpio_devices.h
 *
 *  Created on: Aug 7, 2021
 *      Author: chiny
 */

#ifndef INCLUDE_GPIO_DEVICES_H_
#define INCLUDE_GPIO_DEVICES_H_

void init_gpio(void);

void enable_sensors();

void disable_sensors();

// Interrupt Service Routine for TIMER0_A0
__interrupt void blink_power_led(void);

#endif /* INCLUDE_GPIO_DEVICES_H_ */
