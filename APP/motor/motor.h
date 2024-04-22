#ifndef _motor_H
#define _motor_H

#include "stm32f10x.h"
#include "pwm.h"



extern u8 Direction[4];
extern u8 Last_Speed[4];
extern u8 Speed[4]; 

void Motor_Init(void);
void Motor_SetDirection(u8 motor, u8 direction);
void Motor_Speed(u8 motor, u16 speed);

#endif
