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

void usart2_init(uint32_t baudrate)
{

	if(baudrate > 0U){

	RCC_AHB1ENR |=  (1U << 0);  //Enable clock for GPIOA

	RCC_APB1ENR |=  (1U << 17); //Enable clock for usart2

	GPIOA_MODER &= ~((3U << (2*2)) | (3U << (3*2))); //reset the PA2(00) and PA3(00) pins

	GPIOA_MODER |=  ((2U << (2*2)) | (2U << (3*2))); //set  PA2(10) and PA3(10) to AF mode

	GPIOA_AFRL  &= ~((0xFU << (2*4)) | (0xFU << (3*4))); //reset the PA2(0000) and PA3(0000) AFRL register values

	GPIOA_AFRL  |=  ((0x7U << (2*4)) | (0x7U << (3*4))); //set the PA2(0111) and PA3(0111) for AF7 because datasheet says AF7 as USART2

	USART2_BRR = (TIMER_CLK_HZ + (baudrate / 2U)) / baudrate; //BRR = PCLK / Baud Rate (rounded to nearest)

	USART2_CR1 |= (1U << 3); //set TE bit Tx enable

	USART2_CR1 |= (1U << 2); //set RE bit RX enable

	USART2_CR1 |= (1U << 13); //set UE bit USART2 enable

	}

}

void usart2_tx_ch(char ch)
{

	while(!(USART2_SR & (1U<<7))); //Wait until TXE bit is 1

	USART2_DR = ch; //assign data register to required character

}

void usart2_tx_str(const char *str)
{

    while (*str) {           // Loop until null terminator

        usart2_tx_ch(*str);  // Send current character

        str++;               // Move to next character

    }

}

void usart2_tx_uint(uint32_t num)
{

    char buf[11];

    int i = 0;

    if (num == 0)
    {

        usart2_tx_ch('0');

        usart2_tx_ch('\n');

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

    usart2_tx_ch('\n');

}

char usart2_rx_ch(void)
{

	while(!(USART2_SR & (1U<<5))); //Wait until RXEN bit is 1

	return (char)USART2_DR; //retun received character

}

void usart2_rx_str(char *buffer)
{
    uint32_t i = 0;

    char ch;

    while (1)
    {
        ch = usart2_rx_ch();

        if (ch == '\r' || ch == '\n')
        {
            buffer[i] = '\0';
            break;
        }

        if (i < RX_BUFFER_SIZE)
        {
            buffer[i++] = ch;
        }
    }
}

uint32_t usart2_rx_uint(void)
{
    char buffer[12];
    uint32_t value = 0;
    uint32_t i = 0;

    usart2_rx_str(buffer);    // Receive "12345"

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

void usart2_rx_interrupt(void){

	USART2_CR1 |= (1U << 5); //set RXNE bit RX enable

	NVIC_ISER1 |= (1U << 6);

}

void USART2_IRQHandler(void){

//  Receive char value using uart interrupt

//	if(USART2_SR & (1U << 5)){ //Wait until RXEN bit is 1
//
//		char data = (char)USART2_DR;
//
//		usart2_tx_ch(data);
//
//		usart2_tx_ch('\n');
//
//		count++;
//
//		if(data == '1'){
//
//			gpioa_digitalWrite(5, HIGH);
//
//			gpioa_digitalWrite(6, HIGH);
//
//		}
//		else if(data == '0'){
//
//			gpioa_digitalWrite(5, LOW);
//
//			gpioa_digitalWrite(6, LOW);
//
//		}
//		else if(data == 't'){
//
//			usart2_tx_uint(count);
//
//		}
//
//	}

//  Receive string value using uart interrupt

//	if(USART2_SR & (1U << 5)){
//
//			usart2_rx_str(buffer);
//
//			usart2_tx_str("Received: ");
//
//			usart2_tx_str(buffer);
//
//			usart2_tx_str("\n");
//
//			if (strcmp(buffer, "LEDON") == 0)
//			{
//				gpioa_digitalWrite(5, HIGH);
//
//			    gpioa_digitalWrite(6, HIGH);
//
//			    usart2_tx_str("LED IS ON\n");
//
//			}
//
//			else if (strcmp(buffer, "LEDOFF") == 0)
//			{
//			    gpioa_digitalWrite(5, LOW);
//
//			    gpioa_digitalWrite(6, LOW);
//
//			    usart2_tx_str("LED IS OFF\n");
//
//			}
//			else
//			{
//			    usart2_tx_str("Unknown Command\r\n");
//			}
//	}

//  Receive int value using uart interrupt and work pwm

	if(USART2_SR & (1U << 5)){
		PWM_val = usart2_rx_uint();
		usart2_tx_str("Received: ");
		usart2_tx_uint(PWM_val);
		gpiob_pwm_write(0, PWM_val);
	}

}

