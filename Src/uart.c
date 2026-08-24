/*
 * uart.c
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */
#include "platform.h"
#include "uart.h"
#include "gpio.h"
#include "pwm.h"
#include <string.h>

#include "platform.h"

USART_Handle_t USART1 =
{
    .SR = &USART1_SR,
    .DR = &USART1_DR
};

USART_Handle_t USART2 =
{
    .SR = &USART2_SR,
    .DR = &USART2_DR
};

//USART_Handle_t USART1 = {&USART1_SR, &USART1_DR};
//USART_Handle_t USART2 = {&USART2_SR, &USART2_DR};

void usart1_init(uint32_t baudrate)
{

	if(baudrate > 0U){

	RCC_APB2ENR |=  (1U << 4); //Enable clock for usart1

	gpioa_init(9,AF_MODE); //set  PA9(10) to AF mode

	gpioa_init(10,AF_MODE); //set  PA10(10) to AF mode

	GPIOA_AFRH  &= ~((0xFU << (1*4)) | (0xFU << (2*4))); //reset the PA9(0000) and PA10(0000) AFRL register values

	GPIOA_AFRH  |=  ((0x7U << (1*4)) | (0x7U << (2*4))); //set the PA9(0111) and PA10(0111) for AF7 because datasheet says AF7 as USART1

	USART1_BRR = (APB2_CLK_HZ  + (baudrate / 2U)) / baudrate; //BRR = PCLK / Baud Rate (rounded to nearest)

	USART1_CR1 |= (1U << 3); //set TE bit Tx enable

	USART1_CR1 |= (1U << 2); //set RE bit RX enable

	USART1_CR1 |= (1U << 13); //set UE bit USART1 enable

	}

}

uint8_t usart_available(USART_Handle_t* uart)
{
	return (*(uart->SR) & (1U << 5)) ? 1 : 0;
}

void usart2_init(uint32_t baudrate)
{

	if(baudrate > 0U){

	RCC_APB1ENR |=  (1U << 17); //Enable clock for usart2

	gpioa_init(2,AF_MODE); //set  PA2(10) to AF mode

	gpioa_init(3,AF_MODE); //set  PA3(10) to AF mode

	GPIOA_AFRL  &= ~((0xFU << (2*4)) | (0xFU << (3*4))); //reset the PA2(0000) and PA3(0000) AFRL register values

	GPIOA_AFRL  |=  ((0x7U << (2*4)) | (0x7U << (3*4))); //set the PA2(0111) and PA3(0111) for AF7 because datasheet says AF7 as USART2

	USART2_BRR = (APB1_CLK_HZ + (baudrate / 2U)) / baudrate; //BRR = PCLK / Baud Rate (rounded to nearest)

	USART2_CR1 |= (1U << 3); //set TE bit Tx enable

	USART2_CR1 |= (1U << 2); //set RE bit RX enable

	USART2_CR1 |= (1U << 13); //set UE bit USART2 enable

	}

}

void usart_tx_ch(USART_Handle_t* uart, char ch)
{

		while(!(*(uart->SR) & (1U<<7))); //Wait until TXE bit is 1

		*(uart->DR) = ch; //assign data register to required character

}

void usart_tx_str(USART_Handle_t* uart, const char *str)
{

    while (*str) {           // Loop until null terminator

    	usart_tx_ch(uart, *str); // Send current character

        str++;               // Move to next character

    }

}

void usart_tx_uint(USART_Handle_t* uart, uint32_t num)
{

    char buf[11];

    int i = 0;

    if (num == 0)
    {

    	usart_tx_ch(uart, '0'); // Send current character

    	usart_tx_ch(uart, '\n'); // Send current character

        return;

    }

    while (num > 0)
    {

        buf[i++] = (num % 10) + '0';

        num /= 10;

    }

    while (i > 0)
    {

    	usart_tx_ch(uart, buf[--i]); // Send current character

    }

    usart_tx_ch(uart, '\n'); // Send current character

}

char usart_rx_ch(USART_Handle_t* uart)
{

	while(!(*(uart->SR) & (1U<<5))); //Wait until RXEN is set

	return (char)*(uart->DR); //return received character

}

void usart_rx_str(USART_Handle_t* uart, char *buffer)
{
    uint32_t i = 0;

    char ch;

    while (1)
    {
        ch = usart_rx_ch(uart);

        if (ch == '\r' || ch == '\n')
        {
            buffer[i] = '\0';
            break;
        }

        if(i < (RX_BUFFER_SIZE - 1))
        {
            buffer[i++] = ch;
        }
    }
}

uint32_t usart_rx_uint(USART_Handle_t* uart)
{
    char buffer[12];
    uint32_t value = 0;
    uint32_t i = 0;

    usart_rx_str(uart, buffer);

    while (buffer[i] != '\0')
    {
        if (buffer[i] >= '0' && buffer[i] <= '9')
        {
            value = (value * 10) + (buffer[i] - '0');
        }
        else
        {
            break;      // Invalid character
        }

        i++;
    }

    return value;
}

//void usart1_rx_interrupt(void){
//
//	USART1_CR1 |= (1U << 5); // Enable RXNE interrupt
//
//	NVIC_ISER1 |= (1U << (USART1_IRQn-32));
//
//}
//
//void usart2_rx_interrupt(void){
//
//	USART2_CR1 |= (1U << 5); //set RXNE bit RX enable
//
//	NVIC_ISER1 |= (1U << (USART2_IRQn-32));
//
//}

void USART1_IRQHandler(void){

}

void USART2_IRQHandler(void){

}
