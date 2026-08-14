/*
 * gpio.c
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */
//platforms
#include "platform.h"
#include "gpio.h"

#include <stdint.h>

void gpioa_init(uint8_t pin,uint8_t mode)
{
    RCC_AHB1ENR   |=  (1U    <<  0);      //Enable clock for GPIOA

    GPIOA_MODER   &= ~(3U    << (pin*2)); //Reset the bits for respective pin in MODER(00)

    GPIOA_MODER   |=  (mode << (pin*2)); //Make respective pin as output in MODER(01)

    GPIOA_OSPEEDR &= ~(3U    << (pin*2));

}

void gpioc_init(uint8_t pin,uint8_t mode)
{
    RCC_AHB1ENR |=  (1 << 2); //Enable clock GPIOC

    GPIOC_MODER &= ~(3 << (pin*2)); //Reset the bits and set as input for respective pin(PC13) in MODER(00)

    GPIOA_MODER |=  (mode << (pin*2)); //Make respective pin as output in MODER(01)

    GPIOC_PUPDR &= ~(3 << (pin*2)); //Reset the bits for respective pin(PC13) in PUPDR(00)

    GPIOC_PUPDR |=  (1 << (pin*2)); //Enable internal pullup for respective pin(PC13) in PUPDR(01)
}

void gpioa_toggle(uint8_t pin)
{
    GPIOA_ODR ^= (1 << pin);
}

void gpioa_digitalWrite(uint8_t pin,uint8_t operation)
{
	(operation == 1) ? (GPIOA_BSRR |= (1 << pin)) : (GPIOA_BSRR |= (1 << (pin+16)));
}

uint8_t gpioc_digitalRead(uint8_t pin)
{
    return (GPIOC_IDR & (1 << pin)) ? 1 : 0;
}



