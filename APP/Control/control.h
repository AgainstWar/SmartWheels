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

void unit_distancemov(void);

s16 PID_Increment(int8_t Expect_Encode_Value, int8_t num);

void Movement(void);
void Motor_Control(uint8_t num);

extern uint8_t time_sum ;
extern uint16_t distance_gradientmov_flag;
extern uint8_t average_value;
extern uint8_t speed[4];
extern uint8_t speed_pwm[4];
extern enum dir direction;
#endif
