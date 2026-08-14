/*
 * uart.h
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void usart1_init(uint32_t baudrate);

void usart2_init(uint32_t baudrate);

uint8_t usart_available(volatile uint32_t*);

void usart_tx_ch(volatile uint32_t*, volatile uint32_t*, char);

void usart_tx_str(volatile uint32_t*, volatile uint32_t*, const char*);

void usart_tx_uint(volatile uint32_t*, volatile uint32_t*, uint32_t);

char usart_rx_ch(volatile uint32_t*, volatile uint32_t*);

void usart_rx_str(volatile uint32_t*, volatile uint32_t*, char*);

uint32_t usart_rx_uint(volatile uint32_t*, volatile uint32_t*);

void usart1_rx_interrupt(void);

void usart2_rx_interrupt(void);

#endif /* UART_H_ */
