/*
 * gpio.c
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */

#include "platform.h"
#include "gpio.h"

void gpioa_init(void)
{
    RCC_AHB1ENR |= (1<<0);

    GPIOA_MODER &= ~(3<<(5*2));
    GPIOA_MODER |=  (1<<(5*2));
}

void led_on(void)
{
    GPIOA_ODR |= (1<<5);
}

void led_off(void)
{
    GPIOA_ODR &= ~(1<<5);
}

void led_toggle(void)
{
    GPIOA_ODR ^= (1<<5);
}

void gpioc_init(void)
{
    RCC_AHB1ENR |= (1<<2);

    GPIOC_MODER &= ~(3<<(13*2));

    GPIOC_PUPDR &= ~(3<<(13*2));

    GPIOC_PUPDR |= (1<<(13*2));
}

uint8_t read_button(void)
{
    return !(GPIOC_IDR&(1<<13));
}


