/*
 * uart.c
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */
#include "platform.h"
#include "uart.h"

void usart2_init(void){
	RCC_AHB1ENR |=  (1U << 0);  //Enable clock for GPIOA
	RCC_APB1ENR |=  (1U << 17); //Enable clock for usart2

	GPIOA_MODER &= ~((3U << (2*2)) | (3U << (3*2))); //reset the PA2 and PA3 pins
	GPIOA_MODER |=  ((2U << (2*2)) | (2U << (3*2))); //set  PA2 and PA3 to AF mode

	GPIOA_AFRL  &= ~((0xFU << (2*4)) | (0xFU << (3*4))); //reset the PA2 and PA3 AFRL register values
	GPIOA_AFRL  |=  ((0x7U << (2*4)) | (0x7U << (3*4))); //set the pa2 and pa3 for AF7 because datasheet says AF7 as USART2

	USART2_BRR = 0x0683; //set baudrate to 9600

	USART2_CR1 |= (1U << 3); //set TE bit
	USART2_CR1 |= (1U << 2); //set RE bit

	USART2_CR1 |= (1U << 13); //set UE bit
}

void usart2_tx_ch(char ch){
	while(!(USART2_SR & (1U<<7))); //Wait until TXE bit is 1
	USART2_DR = ch; //assign data register to required character
}

void usart2_tx_uint(uint32_t num)
{
    char buf[11];
    int i = 0;

    if (num == 0)
    {
        usart2_tx_ch('0');
        return;
    }

    while (num > 0)
    {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i > 0)
    {
        usart2_tx_ch(buf[--i]);
    }
}

void usart2_tx_str(const char *str) {
    while (*str) {           // Loop until null terminator
        usart2_tx_ch(*str);  // Send current character
        str++;               // Move to next character
    }
}

char usart2_rx_ch(void){
	while(!(USART2_SR & (1U<<5))); //Wait until RXEN bit is 1
	return (char)USART2_DR; //retun received character
}

char *usart2_rx_str(char *str){
	char *p = str;
	while (1) {
		char ch = usart2_rx_ch();
		if (ch == '\r' || ch == '\n')
			{
				break;
			}
		*p++ = ch;
	}
	*p='\0';
	return str;
}

