/*
 * timer.c
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */
#include "platform.h"
#include "timer.h"
#include "gpio.h"
#include "uart.h"

void tim2_init(void)
{

	RCC_APB1ENR |=  (1U << 0); //Enable clock for timer2

	TIM2_CR1 	&= ~(1U << 0); // stop the timer

	TIM2_PSC =  (TIMER_CLK_HZ / TIMER_TICK_HZ) - 1U;

}

void tim2_delay(uint32_t ms)
{

	if(ms > 0U){

		TIM2_ARR = ms-1; // Set auto-reload value for required delay

		TIM2_CNT = 0; // Reset counter so each delay starts from 0

		TIM2_EGR = (1U<<0); //force the timer immediately to load the PSC and ARR

		TIM2_SR = 0; // reset the overflow status register

		TIM2_CR1 |= (1U<<0); // start the timer

		while(!(TIM2_SR & (1U<<0))); // Wait until timer overflow (UIF = 1)

		TIM2_CR1 &= ~(1U<<0); // stop the timer

		TIM2_SR = 0;  // reset the overflow status register

	}
}

void tim2_interrupt(uint32_t ms)
{

	if(ms==0)

		return;

	TIM2_ARR = ms - 1;

	TIM2_DIER |= (1<<0);      // UIE enable interuupt for tim2

	TIM2_CR1  |= (1<<0);      // Start the timer

	NVIC_ISER0 |= (1<<28);    // TIM2 IRQ

}

void TIM2_IRQHandler(void)
{

    if(TIM2_SR & (1<<0))
    {

        TIM2_SR &= ~(1<<0);      // Clear UIF

        gpioa_toggle(5);

        count++;

        usart2_tx_uint(count);

    }

}

