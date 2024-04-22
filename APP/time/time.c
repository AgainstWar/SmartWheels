#include "time.h"


u16 counter = 0;

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	   // ����NVIC�жϷ���0
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	   // ��ʱ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	   // IRQͨ��ʹ��

	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief ��ʱ��2��ʼ��
 * @param void
 * @note ����Ƶ�ʼ��㹫ʽ= 72Mhz/((per+1)*(psc+1))
 * @returns void
 */
void TIM2_Init(u16 per, u16 psc) // 1ms: per=999,psc=71
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // ʹ��TIM4ʱ��
	TIM_DeInit(TIM2);									 // ��λTIM4

	TIM_TimeBaseInitStructure.TIM_Period = per;						// �Զ�װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;					// ��Ƶϵ�� 71-1kHz 35-2kHz
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // �������ϼ���ģʽ

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // ������ʱ���ж�
	TIM_Cmd(TIM2, ENABLE);					   // ʹ�ܶ�ʱ��
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
			counter = 0; // ���������
			// �����ٶ�
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
