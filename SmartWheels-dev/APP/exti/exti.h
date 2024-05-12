#ifndef _exti_H
#define _exti_H

#include "system.h"
#include "stm32f10x.h"
#include "encode.h"
#include "motor.h"
#include "control.h"

extern uint8_t turnleft_flag , turnright_flag ;
extern uint8_t ditance_gradientmov_flag;
extern uint8_t expect_encoderval;

extern s16 PID_Increasement(int8_t Expect_Encode_Value,int8_t num);

void SYSTEM_EXTI_Init(void);

#endif
