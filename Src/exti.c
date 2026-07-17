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

void exti13_init(void){
	RCC_APB2ENR    |=  (1U << 14);  //Enable clock for SYSCFG
	SYSCFG_EXTICR4 &= ~(0xF << 4);
	SYSCFG_EXTICR4 |=  (0x2 << 4);
	EXTI_IMR 	   &= ~(1 << 13);
	EXTI_IMR 	   |=  (1 << 13);
	EXTI_FTSR 	   &= ~(1 << 13);
	EXTI_FTSR 	   |=  (1 << 13);
	EXTI_RTSR 	   &= ~(1 << 13);
	NVIC_ISER1 	   |=  (1 << 8);
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI_PR & (1 << 13))
    {
        /* Clear pending flag */
        EXTI_PR |= (1 << 13);

        //led(led_status);

        if (led_status)
            usart2_tx_str("Led_on\r\n");
        else
            usart2_tx_str("Led_off\r\n");

        led_status = !led_status;
    }
}


