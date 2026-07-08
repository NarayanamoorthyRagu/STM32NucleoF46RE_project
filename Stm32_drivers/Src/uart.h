/*
 * uart.h
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void usart2_init(void);

void usart2_tx_ch(char);

void usart2_tx_uint(uint32_t);

void usart2_tx_str(const char*);

char usart2_rx_ch(void);

char *usart2_rx_str(char*);

#endif /* UART_H_ */
