#include "time.h"
#include "stm32f10x.h"
#include "LED.h"
#include "SysTick.h"
#include "LCD.h"
#include "encode.h"

//单位时间计数变量
u16 counter = 0;
u16 time_cnt=0;

extern enum dir direction;
uint16_t Encode_Value1[4]={0},
		 Encode_Value2[4]={0},
		 Encode_Value3[4]={0};
uint8_t speed[4]={0},speed1[4]={0},speed2[4]={0};

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	   // 设置NVIC中断分组2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	   // 定时器中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //设置抢占优先级2
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

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 使能TIM2时钟
	TIM_DeInit(TIM2);									 // 复位TIM2

	TIM_TimeBaseInitStructure.TIM_Period = per;						// 自动装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;					// 分频系数 71-1kHz 35-2kHz
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 设置向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); // 开启定时器中断
	NVIC_Configuration();
	TIM_Cmd(TIM2, ENABLE);					   // 使能定时器

}

void TIM2_IRQHandler(void)//10ms 定时中断
{

	if (TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		if(direction==run)
		{
			time_cnt++;
		}
		
		counter++;
		//计算转速 Encode_Value3；由于采样间隔为1s，省略Encode_Value/1 计算
		if(counter==50)
		{
			speed[0]=real_speed[0];
			speed[1]=real_speed[1];
			speed[2]=real_speed[2];
			speed[3]=real_speed[3];

			Encode_Value1[0]=Encode_Value[0];//0号编码器采值
			Encode_Value1[1]=Encode_Value[1];//1号编码器采值
			Encode_Value1[2]=Encode_Value[2];//2号编码器采值
			Encode_Value1[3]=Encode_Value[3];//3号编码器采值
		}
		else if(counter==150)
		{
			speed1[0]=real_speed[0];
			speed1[1]=real_speed[1];
			speed1[2]=real_speed[2];
			speed1[3]=real_speed[3];

			Encode_Value2[0]=Encode_Value[0];
			Encode_Value2[1]=Encode_Value[1];
			Encode_Value2[2]=Encode_Value[2];
			Encode_Value2[3]=Encode_Value[3];
			counter=0;
		}

		if(Encode_Value2[0]<Encode_Value1[0])
		{
			Encode_Value3[0]=Encode_Value1[0] - Encode_Value2[0];//1s内脉冲数
		}
		else 
		{
			Encode_Value3[0]=Encode_Value2[0] - Encode_Value1[0];
		}

		if(Encode_Value2[1]<Encode_Value1[1])
		{
			Encode_Value3[1]=Encode_Value1[1] - Encode_Value2[1];//1s内脉冲数
		}
		else 
		{
			Encode_Value3[1]=Encode_Value2[1] - Encode_Value1[1];
		}

		if(Encode_Value2[2]<Encode_Value1[2])
		{
			Encode_Value3[2]=Encode_Value1[2] - Encode_Value2[2];//1s内脉冲数
		}
		else 
		{
			Encode_Value3[2]=Encode_Value2[2] - Encode_Value1[2];
		}

		if(Encode_Value2[3]<Encode_Value1[3])
		{
			Encode_Value3[3]=Encode_Value1[3] - Encode_Value2[3];//1s内脉冲数
		}
		else 
		{
			Encode_Value3[3]=Encode_Value2[3] - Encode_Value1[3];
		}
		
		speed2[0]=(Encode_Value3[0]*3.14*6)/20;//计算实际速度，单位 cm/s 
		speed2[1]=(Encode_Value3[1]*3.14*6)/20;
		speed2[2]=(Encode_Value3[2]*3.14*6)/20;
		speed2[3]=(Encode_Value3[3]*3.14*6)/20;

		real_speed[0]=(speed[0]+speed1[0]+speed2[0])/3;//均值滤波
		real_speed[1]=(speed[1]+speed1[1]+speed2[1])/3;
		real_speed[2]=(speed[2]+speed1[2]+speed2[2])/3;
		real_speed[3]=(speed[3]+speed1[3]+speed2[3])/3;
		
		if(time_cnt==100)//
		{
			time_cnt=0;
			time_sum++;
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	
	}
	
}

	
