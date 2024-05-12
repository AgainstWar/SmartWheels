#ifndef _time_H
#define _time_H

#include "system.h"
#include "stm32f10x.h"
#include "encode.h"
#include "control.h"



void TIM2_Init(u16 per, u16 psc);
extern 		uint16_t Encode_Value1[4],
		 		 Encode_Value2[4],
		 		 Encode_Value3[4];
#endif
