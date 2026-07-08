/*
 * platform.h
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdint.h>

#define RCC_BASE          0x40023800UL
#define GPIOA_BASE 		  0x40020000UL
#define GPIOC_BASE 		  0x40020800UL
#define TIM2_BASE 		  0x40000000UL
#define USART2_BASE       0x40004400UL

#define AHB1ENR_OFFSET    0x30
#define APB1ENR_OFFSET    0x40

#define GPIO_MODER_OFFSET 0x00
#define GPIO_ODR_OFFSET   0x14
#define GPIO_IDR_OFFSET   0x10
#define GPIO_PUPDR_OFFSET 0x0C
#define GPIO_AFRL_OFFSET  0x20

#define TIM_CR1_OFFSET 	  0x00
#define TIM_SR_OFFSET 	  0x10
#define TIM_EGR_OFFSET 	  0x14
#define TIM_CNT_OFFSET 	  0x24
#define TIM_PSC_OFFSET    0x28
#define TIM_ARR_OFFSET    0x2C

#define USART_SR		  0x00
#define USART_DR		  0x04
#define USART_BRR 		  0x08
#define USART_CR1         0x0C

#define RCC_AHB1ENR       (*(volatile uint32_t *)(RCC_BASE   + AHB1ENR_OFFSET))
#define RCC_APB1ENR       (*(volatile uint32_t *)(RCC_BASE   + APB1ENR_OFFSET))
#define GPIOA_MODER       (*(volatile uint32_t *)(GPIOA_BASE + GPIO_MODER_OFFSET))
#define GPIOC_MODER       (*(volatile uint32_t *)(GPIOC_BASE + GPIO_MODER_OFFSET))
#define GPIOC_IDR         (*(volatile uint32_t *)(GPIOC_BASE + GPIO_IDR_OFFSET))
#define GPIOC_PUPDR       (*(volatile uint32_t *)(GPIOC_BASE + GPIO_PUPDR_OFFSET))
#define GPIOA_ODR         (*(volatile uint32_t *)(GPIOA_BASE + GPIO_ODR_OFFSET))
#define GPIOA_AFRL        (*(volatile uint32_t *)(GPIOA_BASE + GPIO_AFRL_OFFSET))

#define TIM2_CR1          (*(volatile uint32_t *)(TIM2_BASE + TIM_CR1_OFFSET))
#define TIM2_SR           (*(volatile uint32_t *)(TIM2_BASE + TIM_SR_OFFSET))
#define TIM2_PSC          (*(volatile uint32_t *)(TIM2_BASE + TIM_PSC_OFFSET))
#define TIM2_ARR          (*(volatile uint32_t *)(TIM2_BASE + TIM_ARR_OFFSET))
#define TIM2_CNT 		  (*(volatile uint32_t *)(TIM2_BASE + TIM_CNT_OFFSET))
#define TIM2_EGR 		  (*(volatile uint32_t *)(TIM2_BASE + TIM_EGR_OFFSET))

#define USART2_BRR 		  (*(volatile uint32_t *)(USART2_BASE + USART_BRR))
#define USART2_CR1 		  (*(volatile uint32_t *)(USART2_BASE + USART_CR1))
#define USART2_SR 		  (*(volatile uint32_t *)(USART2_BASE + USART_SR))
#define USART2_DR 		  (*(volatile uint32_t *)(USART2_BASE + USART_DR))

#define TIMER_CLK_HZ      16000000U   // TIM2 input clock (16 MHz)
#define TIMER_TICK_HZ     1000U       // 1 ms timer tick

#endif /* PLATFORM_H_ */
