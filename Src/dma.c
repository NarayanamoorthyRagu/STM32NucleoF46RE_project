/*
 * dma.c
 *
 *  Created on: Jul 17, 2026
 *      Author: narayanamoorthy.r
 */
#include "platform.h"
#include "timer.h"
#include "adc.h"

/*----------------------------------------------------------
 * DMA Initialization
 *---------------------------------------------------------*/
void dma2_stream0_init(void){

	RCC_AHB1ENR |=  (1U << 0);  //Enable clock for DMA2

	DMA2_S0CR   &= ~(1U << 0);  //Stream disabled

	while(DMA2_S0CR & (1U << 0)); //Wait until stream is disable

	DMA2_LIFCR   =   0x3D;  //clear the Stream0 bits

	DMA2_S0PAR   =   (uint32_t)&ADC_DR;

	//DMA2_S0M0AR  =   (uint32_t)adc_buffer;

	DMA2_S0NDTR = 1;

}

/*----------------------------------------------------------
 * ADC + DMA
 * Configures DMA2 Stream0 Channel0 for ADC1
 *---------------------------------------------------------*/
void adc_dma_init(volatile uint16_t *buffer, uint16_t length);

/*----------------------------------------------------------
 * DMA Control
 *---------------------------------------------------------*/
void dma2_stream0_enable(void);

void dma2_stream0_disable(void);

/*----------------------------------------------------------
 * DMA Status
 *---------------------------------------------------------*/
uint8_t dma2_stream0_transfer_complete(void);

void dma2_stream0_clear_flags(void);

