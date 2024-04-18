#ifndef _pwm_H
#define _pwm_H

#include "stm32f10x.h"
void PWM_Init(void);
void PWM_SetDutyCycle(uint16_t channel, uint16_t duty);

#endif
