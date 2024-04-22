#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f10x.h"
#include "encode.h"
#include "motor.h"
#include "pwm.h"
#include "MPU6050.h"

void Turn_left(void);
void Turn_right(void);

uint16_t PID_Increasement(int8_t Expect_Encode_Value, int8_t num);

void Move_forward(u8 level);

#endif
