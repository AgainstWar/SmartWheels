#include "exti.h"

void GPIO_Configuration(void);

/**
 * @brief 初始化外部中断
 * @param void
 * @note 初始化外部中断1-4
 * @returns void
 */
void SYSTEM_EXTI_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_Configuration();

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); // 选择GPIOA_1管脚用作外部中断线
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2); // 选择GPIOA_2管脚用作外部中断线
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource3); // 选择GPIOA_3管脚用作外部中断线
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4); // 选择GPIOA_4管脚用作外部中断线

	// EXTI2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;		  // EXTI2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		  // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化NVIC寄存器

	// EXTI3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;		  // EXTI3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化NVIC寄存器

	// EXTI4 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;		  // EXTI4中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;		  // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化NVIC寄存器

	// EXTI1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;		  // EXTI1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // 子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化NVIC寄存器

	EXTI_InitStructure.EXTI_Line = EXTI_Line1 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4; // 外部中断线
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // 下拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief 外部中断1函数
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
 * @brief 外部中断2函数
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
 * @brief 外部中断3函数
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
 * @brief 外部中断4函数
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
