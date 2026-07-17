/*
 * dma.h
 *
 *  Created on: Jul 17, 2026
 *      Author: narayanamoorthy.r
 */

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>

/*----------------------------------------------------------
 * DMA Initialization
 *---------------------------------------------------------*/
void dma2_stream0_init(void);

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

#endif /* DMA_H_ */
