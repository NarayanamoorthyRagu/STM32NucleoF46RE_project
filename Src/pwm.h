/*
 * pwm.h
 *
 *  Created on: Jul 15, 2026
 *      Author: narayanamoorthy.r
 */

#ifndef PWM_H_
#define PWM_H_

void gpiob_pwm_init(uint8_t pin);
void gpiob_pwm_write(uint8_t pin,uint32_t value);

#endif /* PWM_H_ */
