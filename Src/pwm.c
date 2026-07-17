/*
 * pwm.c
 *
 *  Created on: Jul 15, 2026
 *      Author: narayanamoorthy.r
 */
#include "gpio.h"
#include "platform.h"
#include "pwm.h"

void gpiob_pwm_init(uint8_t pin)
{
	RCC_APB1ENR |=  (1U << 1); //Enable clock for timer3

	RCC_AHB1ENR |=  (1U << 1);  //Enable clock for GPIOB

	GPIOB_MODER &= ~(3U   << (pin*2)); //reset the PA1(00) pin

	GPIOB_MODER |=  (2U   << (pin*2)); //set PA1(10) to AF mode

	GPIOB_AFRL  &= ~(0xFU << (pin*4)); //reset the PA1(0000) AFRL register values

	GPIOB_AFRL  |=  (0x2U << (pin*4)); //set the PA1(0001)  for AF2 because datasheet says AF2 as TIMER2

	TIM3_PSC = 15;

	TIM3_ARR = 999;

	TIM3_CCMR2 &= ~((7U << 4) | (1U << 3));

	TIM3_CCMR2 |=  ((6U << 4) | (1U << 3));

	TIM3_CCER   |=  (1U << 8);

	TIM3_EGR    =   (1U << 0);

	TIM3_CR1    |=  (1U << 0);

	TIM3_CCR3 = 0;    // LED OFF initially

}

void gpiob_pwm_write(uint8_t pin, uint32_t value)
{
    if (value > 999)
    {
        value = 999;
    }

    TIM3_CCR3 = value;
}

