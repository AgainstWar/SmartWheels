#ifndef _pwm_H
#define _pwm_H

#include "stm32f10x.h"

void PWM_Init(void);
void PWM_SetDutyCycle(u16 channel, s16 duty);

#endif
