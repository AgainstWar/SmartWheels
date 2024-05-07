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
};
extern enum dir direction;
 

void Turn_left(void);
void Turn_right(void);
void Move_forward(void);
void Move_back(void);
void unit_distancemov(uint8_t gradient);


extern uint8_t turnleft_flag , turnright_flag ;
extern uint8_t ditance_gradientmov_flag;
extern uint8_t expect_encoderval;
extern uint8_t displacement;
extern s16 PID_Increasement(int8_t Expect_Encode_Value,int8_t num);


void Movement(void);
 



#endif
