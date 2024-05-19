#ifndef _motor_H
#define _motor_H

#include "stm32f10x.h"
#include "pwm.h"



extern u8 Last_Speed[4];

void Motor_Init(void);
void Motor_SetDirection(s8 motor, s8 direction);
void Motor_Speed(s8 motor, s16 speed);

#endif
