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
#include "lcd.h"
void u32tostr(unsigned long dat, char *str);
void system_initiation(void);
int i = 0;
int main(void)
{
	u8 buf[5];
	u8 buf1[5];
	u8 buf2[5];
	u8 buf3[5];
	system_initiation();
	while (1)
	{

	}
}

void system_initiation(void)
{
	SYSTEM_EXTI_Init();
	LED_Init();			 // 初始化LED
	Encode_Init();		 // 编码器相关函数初始化
	Motor_Init();		 // 电机相关函数初始化
	//mpu6050_init();		 // 陀螺仪初始化
	SysTick_Init(72);	 // 系统频率72MHz
	USART1_Init(115200); // 串口初始化 波特率115200
	TIM2_Init(999, 71);	 // 定时器2初始化
	Lcd1602_Init();		 // LCD初始化
}

void u32tostr(unsigned long dat, char *str)
{
	char temp[20];
	unsigned char i = 0, j = 0;
	i = 0;
	while (dat)
	{
		temp[i] = dat % 10 + 0x30;
		i++;
		dat /= 10;
	}
	j = i;
	for (i = 0; i < j; i++)
	{
		str[i] = temp[j - i - 1];
	}
	if (!i)
	{
		str[i++] = '0';
	}
	str[i] = 0;
}