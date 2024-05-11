#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f10x.h"
#include "encode.h"
#include "motor.h"
#include "pwm.h"
#include "MPU6050.h"


enum dir
{
    N ,
    S ,
    W ,
    E ,
    run ,
    stop,
};

void Turn_left(void);
void Turn_right(void);
void Move_forward(void);
void Move_back(void);

void unit_distancemov(uint8_t gradient,float  pit_time);

s16 PID_Increasement(int8_t Expect_Encode_Value, int8_t num);

void Movement(void);
void Motor_Control(uint8_t num);

extern uint16_t distance_gradientmov_flag;
extern uint8_t displacement;
extern uint8_t average_value;
extern uint8_t speed[4];
#endif
