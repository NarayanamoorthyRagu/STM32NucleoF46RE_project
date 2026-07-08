/*
 * gpio.h
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */

#ifndef GPIO_H_
#define GPIO_H_
#include <stdint.h>

void gpioa_init(void);

void led_on(void);

void led_off(void);

void led_toggle(void);

void gpioc_init(void);

uint8_t read_button(void);

#endif /* GPIO_H_ */
