#include "exti.h"


static void GPIO_Configuration(void);

/**
 * @brief ��ʼ���ⲿ�ж�
 * @param void
 * @note ��ʼ��PD0-3Ϊ�ⲿ�ж�
 * @returns void
 */
void SYSTEM_EXTI_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_Configuration();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); // ѡ��GPIOA_1�ܽ������ⲿ�ж���·
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2); // ѡ��GPIOA_2�ܽ������ⲿ�ж���·
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3); // ѡ��GPIOA_3�ܽ������ⲿ�ж���·
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4); // ѡ��GPIOA_4�ܽ������ⲿ�ж���·

	// EXTI2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;		  // EXTI2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  // �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��NVIC�Ĵ���

	// EXTI3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;		  // EXTI3�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��NVIC�Ĵ���

	// EXTI4 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;		  // EXTI4�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		  // �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��NVIC�Ĵ���

	// EXTI1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;		  // EXTI1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // �����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��NVIC�Ĵ���

	EXTI_InitStructure.EXTI_Line = EXTI_Line1 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4; // �ⲿ�ж���·
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // �����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // ��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief �ⲿ�ж�1����
 * @param void
 * @returns void
 */

void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) == 1)
	{
		Encode_Value[0]++;
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}

/**
 * @brief �ⲿ�ж�2����
 * @param void
 * @returns void
 */
void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) == 1)
	{
		Encode_Value[1]++;
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}

/**
 * @brief �ⲿ�ж�3����
 * @param void
 * @returns void
 */
void EXTI3_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line3) == 1)
	{
		Encode_Value[2]++;
	}
	EXTI_ClearITPendingBit(EXTI_Line3);
}

/**
 * @brief �ⲿ�ж�4����
 * @param void
 * @returns void
 */
void EXTI4_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line4) == 1)
	{
		Encode_Value[3]++;
	}
	EXTI_ClearITPendingBit(EXTI_Line4);
}
