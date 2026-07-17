/*
 * gpio.h
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

void gpioa_init(uint8_t pin,uint8_t mode);

void gpioc_init(uint8_t pin,uint8_t mode);

void gpioa_toggle(uint8_t pin);

void gpioa_digitalWrite(uint8_t pin,uint8_t operation);

uint8_t gpioc_digitalRead(uint8_t pin);

#endif /* GPIO_H_ */
