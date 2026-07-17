/*
 * adc.h
 *
 *  Created on: Jul 13, 2026
 *      Author: narayanamoorthy.r
 */

#ifndef ADC_H_
#define ADC_H_


#include "gpio.h"
#include "uart.h"

void gpioa_adc_init(uint8_t);
uint16_t adc_read(void);
void adc_interrupt(void);
#endif /* ADC_H_ */
