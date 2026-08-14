/*
 * adc.c
 *
 *  Created on: Jul 13, 2026
 *      Author: narayanamoorthy.r
 */

#include <stdint.h>
#include "platform.h"
#include "timer.h"
#include "adc.h"

void gpioa_adc_init(uint8_t pin)
{

	RCC_AHB1ENR |=  (1U   << 0);  //Enable clock GPIOA

	RCC_APB2ENR |=  (1U   << 8);  //enable clock for ADC1

	GPIOA_MODER &= ~(3U   << (pin*2));  // reset the moder PA0 bits

	GPIOA_MODER |=  (3U   << (pin*2));  //set PA0 as analog mode

	ADC_SQR1 	&= ~(0XF  << 20); // Regular sequence length = 1 conversion (L = 0)

	ADC_SQR3    =  pin;   //set SQ1 = channel 0 (PA0)

	ADC_CR2 	|=  (1U   << 0);    //Enable ADON bit

}

uint16_t adc_read(void)
{

	ADC_CR2 |= (1U << 30); //start conversion

	while (!(ADC_SR & (1U << 1))); // Wait until End Of Conversion (EOC) flag is set

	return (uint16_t)ADC_DR; //return converted data

}

void adc_temp_sensor_init(void)
{
    /* Enable ADC1 Clock */
    RCC_APB2ENR |= (1U << 8);

    /* Enable Internal Temperature Sensor */
    ADC_CCR |= (1U << 23);      // TSVREFE

    /* Regular sequence length = 1 */
    ADC_SQR1 &= ~(0xF << 20);

    /* Select Channel 16 (Temperature Sensor) */
    ADC_SQR3 = 16;

    /* Sampling time for Channel 16 = 480 cycles */
    ADC_SMPR1 &= ~(7U << 18);
    ADC_SMPR1 |=  (7U << 18);

    /* Enable ADC */
    ADC_CR2 |= (1U << 0);
}

uint16_t adc_temp_read(void)
{
    /* Start Conversion */
    ADC_CR2 |= (1U << 30);

    /* Wait for EOC */
    while (!(ADC_SR & (1U << 1)));

    return (uint16_t)ADC_DR;
}

void adc_interrupt(void)
{
	ADC_CR2 	   |=  (1U   << 30); //start conversion

	ADC_CR1 	   |=  (1U   << 5);  //Enable ADC interrupt bit

	NVIC_ISER0 	   |=  (1U   << ADC_IRQn); //Enable ADC interrupt in ISER0 in NVIC

}

void ADC_IRQHandler(void)
{
	if((ADC_SR & (1U << 1))) // Wait until End Of Conversion (EOC) flag is set
	{

		usart_tx_uint(&USART2_SR, &USART2_DR, (uint16_t)ADC_DR); //Print the ADC value in USART2

		tim2_delay(1000);

		ADC_CR2 |= (1U << 30); //restart conversion

	}
}

