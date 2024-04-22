#include "stm32f10x.h"
#include "pwm.h"
#include "motor.h"
#include "SysTick.h"
#include "exti.h"
#include "time.h"
#include "MPU6050.h"

void delay(u32 nms);

int main(void)
{
	Motor_Init();
	SYSTEM_EXTI_Init();
	TIM2_Init(999, 71);
	Encode_Init();
	mpu6050_init();
	while (1)
	{
		// Motor Test
		Motor_Speed(0, 80);
		Motor_Speed(1, 80);
		Motor_Speed(2, 80);
		Motor_Speed(3, 80);
		delay(10);
		Motor_SetDirection(0, 1);
		Motor_SetDirection(1, 0);
		Motor_SetDirection(2, 1);
		Motor_SetDirection(3, 0);
		delay(10);
		Motor_Speed(0, 50);
		delay(10);
		Motor_Speed(1, 20);
		delay(10);
		Motor_SetDirection(1, 1);
		delay(10);
	}
}

void delay(u32 nms)
{
	while (nms--)
	{
		u32 i = 72000;
		while (i--)
			;
	}
}
