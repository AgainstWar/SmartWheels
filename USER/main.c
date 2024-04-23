#include "stm32f10x.h"
#include "pwm.h"
#include "motor.h"
#include "SysTick.h"
#include "exti.h"
#include "time.h"
#include "MPU6050.h"
#include "usart.h"
#include "control.h"

void system_initiation(void);

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
	Motor_Init();
	SYSTEM_EXTI_Init();
	TIM2_Init(999, 71);
	Encode_Init();
	mpu6050_init();
	SysTick_Init(72); // 系统频率72MHz
}
