#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f10x.h"
#include "encode.h"
#include "motor.h"
#include "pwm.h"
#include "MPU6050.h"

void Turn_left(void);
void Turn_right(void);
void Move_forward(void);
void Move_back(void);
void unit_distancemov(uint8_t gradient);



uint16_t PID_Increasement(int8_t Expect_Encode_Value, int8_t num);









#endif
