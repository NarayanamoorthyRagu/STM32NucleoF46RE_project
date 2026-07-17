/*
 * platform.h
 *
 *  Created on: Jul 3, 2026
 *      Author: narayanamoorthy.r
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include <stdint.h>
#define RCC_BASE          0x40023800UL  //Reset and clock register base
#define GPIOA_BASE 		  0x40020000UL
#define GPIOB_BASE 		  0x40020400UL
#define GPIOC_BASE 		  0x40020800UL
#define TIM2_BASE 		  0x40000000UL
#define TIM3_BASE 		  0x40000400UL
#define USART2_BASE       0x40004400UL
#define SYSCFG_BASE       0x40013800UL  //System configure base
#define EXTI_BASE         0x40013C00UL  //External interrupt base
#define ADC_BASE          0x40012000UL
#define DMA2_BASE         0x40026400UL

#define AHB1ENR_OFFSET    0x30
#define APB1ENR_OFFSET    0x40
#define APB2ENR_OFFSET    0x44

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
#define TIM_DIER_OFFSET   0x0C
#define TIM_CCER_OFFSET   0x20
#define TIM_CCMR2_OFFSET  0x1C
#define TIM_CCR3_OFFSET   0x3C

#define SYSCFG_EXTICR4_OFFSET   0x14

#define EXTI_IMR_OFFSET   0x00
#define EXTI_FTSR_OFFSET  0x0C
#define EXTI_RTSR_OFFSET  0x08
#define EXTI_PR_OFFSET    0x14

#define USART_SR		  0x00
#define USART_DR		  0x04
#define USART_BRR 		  0x08
#define USART_CR1         0x0C

#define ADC_CR1_OFFSET	  0x04
#define ADC_CR2_OFFSET	  0x08
#define ADC_SQR3_OFFSET   0x34
#define ADC_SQR1_OFFSET   0x2C
#define ADC_SR_OFFSET     0x00
#define ADC_DR_OFFSET     0x4C
#define ADC_CCR_OFFSET    0x04
#define ADC_SMPR2_OFFSET   0x10

#define DMA_LISR_OFFSET  0X00
#define DMA_HISR_OFFSET  0x04
#define DMA_LIFCR_OFFSET 0x08
#define DMA_HIFCR_OFFSET 0x0C
#define DMA_S0CR_OFFSET   (0x10 + (0x18 * 0))
#define DMA_S0NDTR_OFFSET (0x14 + (0x18 * 0))
#define DMA_S0PAR_OFFSET  (0x18 + (0x18 * 0))
#define DMA_S0M0AR_OFFSET (0x1C + (0x18 * 0))
#define DMA_S0FCR_OFFSET  (0x24 + (0x24 * 0))

#define RCC_AHB1ENR       (*(volatile uint32_t *)(RCC_BASE   + AHB1ENR_OFFSET))
#define RCC_APB1ENR       (*(volatile uint32_t *)(RCC_BASE   + APB1ENR_OFFSET))
#define RCC_APB2ENR       (*(volatile uint32_t *)(RCC_BASE   + APB2ENR_OFFSET))

#define GPIOA_MODER       (*(volatile uint32_t *)(GPIOA_BASE + GPIO_MODER_OFFSET))
#define GPIOC_MODER       (*(volatile uint32_t *)(GPIOC_BASE + GPIO_MODER_OFFSET))
#define GPIOC_IDR         (*(volatile uint32_t *)(GPIOC_BASE + GPIO_IDR_OFFSET))
#define GPIOC_PUPDR       (*(volatile uint32_t *)(GPIOC_BASE + GPIO_PUPDR_OFFSET))
#define GPIOA_ODR         (*(volatile uint32_t *)(GPIOA_BASE + GPIO_ODR_OFFSET))
#define GPIOA_AFRL        (*(volatile uint32_t *)(GPIOA_BASE + GPIO_AFRL_OFFSET))

#define GPIOB_MODER       (*(volatile uint32_t *)(GPIOB_BASE + GPIO_MODER_OFFSET))
#define GPIOB_AFRL        (*(volatile uint32_t *)(GPIOB_BASE + GPIO_AFRL_OFFSET))

#define TIM2_CR1          (*(volatile uint32_t *)(TIM2_BASE + TIM_CR1_OFFSET))
#define TIM2_SR           (*(volatile uint32_t *)(TIM2_BASE + TIM_SR_OFFSET))
#define TIM2_PSC          (*(volatile uint32_t *)(TIM2_BASE + TIM_PSC_OFFSET))
#define TIM2_ARR          (*(volatile uint32_t *)(TIM2_BASE + TIM_ARR_OFFSET))
#define TIM2_CNT 		  (*(volatile uint32_t *)(TIM2_BASE + TIM_CNT_OFFSET))
#define TIM2_EGR 		  (*(volatile uint32_t *)(TIM2_BASE + TIM_EGR_OFFSET))
#define TIM2_DIER         (*(volatile uint32_t *)(TIM2_BASE + TIM_DIER_OFFSET))

#define TIM3_PSC          (*(volatile uint32_t *)(TIM3_BASE + TIM_PSC_OFFSET))
#define TIM3_ARR          (*(volatile uint32_t *)(TIM3_BASE + TIM_ARR_OFFSET))
#define TIM3_CCMR2		  (*(volatile uint32_t *)(TIM3_BASE + TIM_CCMR2_OFFSET))
#define TIM3_CCER		  (*(volatile uint32_t *)(TIM3_BASE + TIM_CCER_OFFSET))
#define TIM3_EGR 		  (*(volatile uint32_t *)(TIM3_BASE + TIM_EGR_OFFSET))
#define TIM3_CR1          (*(volatile uint32_t *)(TIM3_BASE + TIM_CR1_OFFSET))
#define TIM3_CCR3         (*(volatile uint32_t *)(TIM3_BASE + TIM_CCR3_OFFSET))

#define USART2_BRR 		  (*(volatile uint32_t *)(USART2_BASE + USART_BRR))
#define USART2_CR1 		  (*(volatile uint32_t *)(USART2_BASE + USART_CR1))
#define USART2_SR 		  (*(volatile uint32_t *)(USART2_BASE + USART_SR))
#define USART2_DR 		  (*(volatile uint32_t *)(USART2_BASE + USART_DR))

#define SYSCFG_EXTICR4 	  (*(volatile uint32_t *)(SYSCFG_BASE + SYSCFG_EXTICR4_OFFSET))

#define EXTI_IMR    	  (*(volatile uint32_t *)(EXTI_BASE + EXTI_IMR_OFFSET))
#define EXTI_FTSR         (*(volatile uint32_t *)(EXTI_BASE + EXTI_FTSR_OFFSET))
#define EXTI_RTSR         (*(volatile uint32_t *)(EXTI_BASE + EXTI_RTSR_OFFSET))
#define EXTI_PR           (*(volatile uint32_t *)(EXTI_BASE + EXTI_PR_OFFSET))

#define ADC_CR1           (*(volatile uint32_t *)(ADC_BASE + ADC_CR1_OFFSET))
#define ADC_CR2           (*(volatile uint32_t *)(ADC_BASE + ADC_CR2_OFFSET))
#define ADC_SQR3		  (*(volatile uint32_t *)(ADC_BASE + ADC_SQR3_OFFSET))
#define ADC_SQR1		  (*(volatile uint32_t *)(ADC_BASE + ADC_SQR1_OFFSET))
#define ADC_SR   		  (*(volatile uint32_t *)(ADC_BASE + ADC_SR_OFFSET))
#define ADC_DR   		  (*(volatile uint32_t *)(ADC_BASE + ADC_DR_OFFSET))
#define ADC_CCR   		  (*(volatile uint32_t *)(ADC_BASE + ADC_CCR_OFFSET))
#define ADC_SMPR2   	  (*(volatile uint32_t *)(ADC_BASE + ADC_SMPR2_OFFSET))

#define DMA2_LISR		  (*(volatile uint32_t *)(DMA2_BASE + DMA_LISR_OFFSET))
#define DMA2_HISR  		  (*(volatile uint32_t *)(DMA2_BASE + DMA_HISR_OFFSET))
#define DMA2_LIFCR        (*(volatile uint32_t *)(DMA2_BASE + DMA_LIFCR_OFFSET))
#define DMA2_HIFCR        (*(volatile uint32_t *)(DMA2_BASE + DMA_HIFCR_OFFSET))
#define DMA2_S0CR         (*(volatile uint32_t *)(DMA2_BASE + DMA_S0CR_OFFSET))
#define DMA2_S0NDTR       (*(volatile uint32_t *)(DMA2_BASE + DMA_S0NDTR_OFFSET))
#define DMA2_S0PAR        (*(volatile uint32_t *)(DMA2_BASE + DMA_S0PAR_OFFSET))
#define DMA2_S0M0AR       (*(volatile uint32_t *)(DMA2_BASE + DMA_S0M0AR_OFFSET))
#define DMA2_S0FCR		  (*(volatile uint32_t *)(DMA2_BASE + DMA_S0FCR_OFFSET))

#define NVIC_ISER0  	  (*(volatile uint32_t *)0xE000E100)
#define NVIC_ISER1  	  (*(volatile uint32_t *)0xE000E104)

#define TIMER_CLK_HZ            16000000U      // TIM2 input clock (16 MHz)
#define TIMER_TICK_MS_DELAY     1000U          // 1 ms timer tick
#define TIMER_TICK_US_PWM       1000000U       // 1 us timer tick

#define INPUT   0
#define OUTPUT  1
#define ANALOG  3

#define LOW     0
#define HIGH    1

#define RX_BUFFER_SIZE    100
extern char buffer[RX_BUFFER_SIZE];

extern uint32_t count;
extern uint32_t PWM_val;

#endif /* PLATFORM_H_ */
