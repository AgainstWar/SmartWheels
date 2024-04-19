#include <pwm.h>

static void GPIO_Configuration(void);
static void TIM3_Configuration(void);

/*
 * 初始化PWM
 * @param void
 * @note 初始化TIM3的CH1-4通道，对应PA6、PA7、PA0、PA1
 * @returns void
 */
void PWM_Init(void)
{
    GPIO_Configuration();
    TIM3_Configuration();
    PWM_SetDutyCycle(1, 0);
    PWM_SetDutyCycle(2, 0);
    PWM_SetDutyCycle(3, 0);
    PWM_SetDutyCycle(4, 0);
}

static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 开启GPIOA|B的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 设置GPIO为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; // 查询数据手册，非重映射TIM3 CH1-2对应PA6和PA7
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; // 查询数据手册，非重映射TIM3 CH3-4对应PB0和PB1
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void TIM3_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // TIM3的时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 定时器基本配置
    TIM_TimeBaseStructure.TIM_Period = 999;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // PWM模式配置
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 50; // 初始占空比0%
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // 初始化TIM3的Channel1
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 初始化TIM3的Channel2
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 初始化TIM3的Channel3
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 初始化TIM3的Channel4
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 启动TIM3
    TIM_Cmd(TIM3, ENABLE);
}

/*
 * 设置PWM的占空比
 * @param channel PWM通道 1-4
 * @param duty 占空比 %0-100
 * @returns void
 */
void PWM_SetDutyCycle(u16 channel, u16 duty)
{
    duty = duty * 10;
    duty = duty > 1000 ? 1000 : duty;
    duty = duty < 0 ? 0 : duty;

    switch (channel)
    {
    case 1:
        TIM_SetCompare1(TIM3, duty);
        break;
    case 2:
        TIM_SetCompare2(TIM3, duty);
        break;
    case 3:
        TIM_SetCompare3(TIM3, duty);
        break;
    case 4:
        TIM_SetCompare4(TIM3, duty);
        break;
    default:
        break;
    }
}
