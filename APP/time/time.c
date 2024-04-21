#include "time.h"

u16 counter = 0;

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	   // 设置NVIC中断分组0
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	   // 定时器中断通道
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	   // IRQ通道使能

	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief 定时器2初始化
 * @param void
 * @note 更新频率计算公式= 72Mhz/((per+1)*(psc+1))
 * @returns void
 */
void TIM2_Init(u16 per, u16 psc) // 1ms: per=999,psc=71
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 使能TIM4时钟
	TIM_DeInit(TIM2);									 // 复位TIM4

	TIM_TimeBaseInitStructure.TIM_Period = per;						// 自动装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;					// 分频系数 71-1kHz 35-2kHz
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 设置向上计数模式

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 开启定时器中断
	TIM_Cmd(TIM2, ENABLE);					   // 使能定时器
	NVIC_Configuration();
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

		counter++;
		if (counter >= 1000)
		{
			counter = 0; // 清零计数器
			// 计算速度
			speed[0] = (Encode_Value[0] / code_disc) * 3.14 * diameter;
			speed[1] = (Encode_Value[1] / code_disc) * 3.14 * diameter;
			speed[2] = (Encode_Value[2] / code_disc) * 3.14 * diameter;
			speed[3] = (Encode_Value[3] / code_disc) * 3.14 * diameter;

			Encode_Value[0] = 0;
			Encode_Value[1] = 0;
			Encode_Value[2] = 0;
			Encode_Value[3] = 0;
		}
	}
}
