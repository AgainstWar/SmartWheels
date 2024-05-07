#include "stm32f10x.h"
#include "pwm.h"
#include "motor.h"
#include "SysTick.h"
#include "exti.h"
#include "time.h"
#include "MPU6050.h"
#include "usart.h"
#include "control.h"
#include "LED.h"

void system_initiation(void);
int i = 0;
int main(void)
{
	system_initiation();
	while (1)
	{
		Movement();
	}
}

void system_initiation(void)
{
	SYSTEM_EXTI_Init();
	LED_Init();			 // 初始化LED
	SysTick_Init(72);	 // 系统频率72MHz
	USART1_Init(115200); // 串口初始化 波特率115200
	TIM2_Init(999, 71);	 // 定时器2初始化
	mpu6050_init();		 // 陀螺仪初始化
	Motor_Init();		 // 电机相关函数初始化
	Encode_Init();		 // 编码器相关函数初始化
}
