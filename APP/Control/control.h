#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f10x.h"


enum dir
{
    N ,
    S ,
    W ,
    E ,
    run ,
    stop,
    R,
};

void Turn(void);
void Move_forward(void);
void Move_back(void);
void unit_distancemov(void);
void MPU6050_data_processing(void);
s16 PID_Increment(int8_t Expect_Encode_Value, int8_t num);
void Movement(void);
void Motor_Control(uint8_t num);

extern uint8_t time_sum ;
extern uint16_t distance_gradientmov_flag;
extern uint8_t average_value;
extern uint8_t speed[4];
extern uint8_t speed_pwm[4];
extern enum dir direction;
extern float GyroZ;
extern uint8_t turn_flag;
extern u8 cnt;
extern float angle;

#endif
