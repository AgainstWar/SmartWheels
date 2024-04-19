#ifndef _ENCODE_H
#define _ENCODE_H

#include "stm32f10x.h"
#include "motor.h"

extern u32 Encode_Value[4];
extern float speed[4];
extern const float diameter;
extern const float code_disc;

void Encode_Init(void);

#endif
