#ifndef _ENCODE_H
#define _ENCODE_H

#include "stm32f10x.h"
#include "motor.h"

extern u16 Encode_Value[4];
extern const float diameter;
extern const float code_disc;
extern uint8_t real_speed[4];

void Encode_Init(void);
void Encode_Clr(void);
#endif
