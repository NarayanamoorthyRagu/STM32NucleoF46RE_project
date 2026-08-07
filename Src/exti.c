/*
 * exti.c
 *
 *  Created on: Jul 9, 2026
 *      Author: narayanamoorthy.r
 */
#include "platform.h"
#include "gpio.h"
#include "uart.h"
#include "exti.h"

static uint8_t led_status = 1;

void exti13_init(void)
{

	RCC_APB2ENR    |=  (1U << 14); //Enable clock for SYSCFG

	SYSCFG_EXTICR4 &= ~(0xF << 4); //reset the SYSCFG_EXTICR4 13 pin

	SYSCFG_EXTICR4 |=  (0x2 << 4); //set pc13 as interrupt pin pc[x]=0010=2

	EXTI_IMR 	   &= ~(1 << 13);  //reset the pc13 pin interrupt pin

	EXTI_IMR 	   |=  (1 << 13);  //set the pc13 pin interrupt pin make not masked

	EXTI_FTSR 	   &= ~(1 << 13);  //reset the pc13 pin falling trigger pin (because button status high to low)

	EXTI_FTSR 	   |=  (1 << 13);  //set the pc13 pin falling trigger pin (because button status high to low)

	EXTI_RTSR 	   &= ~(1 << 13);  //reset the pc13 pin rising trigger pin(because we cant use rising edge)

	NVIC_ISER1 	   |=  (1 << 8);   //Enable interrupt for external interrupt in nvic

}

void EXTI15_10_IRQHandler(void)
{

    if (EXTI_PR & (1 << 13)) //wait for pc13 pending register to overflow
    {
        /* Clear pending flag */
        EXTI_PR |= (1 << 13);

        gpioa_toggle(5);

        if (led_status)

            usart2_tx_str("Led_on\r\n");

        else

            usart2_tx_str("Led_off\r\n");

        led_status = !led_status;
    }
}


