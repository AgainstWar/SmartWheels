#include <motor.h>

u8 Direction[4] = {1, 1, 1, 1};
u8 Last_Speed[4] = {0, 0, 0, 0};
u8 Speed[4] = {0};

/**
 * @brief 初始化电机
 * @param void
 * @returns void
 */
void Motor_Init(void)
{
    // 初始化GPIO
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // 初始化电机驱动端口
    GPIO_ResetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    // 初始化PWM
    PWM_Init();
    // 设置电机方向
    Motor_SetDirection(0, 1);
    Motor_SetDirection(1, 1);
    Motor_SetDirection(2, 1);
    Motor_SetDirection(3, 1);
    // 设置电机速度
    Motor_Speed(0, 0);
    Motor_Speed(1, 0);
    Motor_Speed(2, 0);
    Motor_Speed(3, 0);
}

/**
 * @brief 设置电机速度
 * @param motor 电机编号 0-3
 * @param speed 电机速度 0-100
 * @note 电机速度为0-100，0为停止，100为最大速度
 */
void Motor_Speed(u8 motor, u16 speed)
{
    if (motor >= 0 && motor <= 3)
    {
        Last_Speed[motor] = speed;
        PWM_SetDutyCycle(motor + 1, speed);
    }
}

/**
 * @brief 设置电机方向
 * @param motor 电机编号 0-3
 * @param direction 电机转动方向 1为正转，0为反转
 */
void Motor_SetDirection(u8 motor, u8 direction)
{
    if (motor > 3 || motor < 0)
    {
        return;
    }
    Direction[motor] = direction;
    switch (motor)
    {

    case 0:
    {
        if (direction == 0)
            GPIO_SetBits(GPIOD, GPIO_Pin_1),
                GPIO_ResetBits(GPIOD, GPIO_Pin_0);
        if (direction == 1)
            GPIO_SetBits(GPIOD, GPIO_Pin_0),
                GPIO_ResetBits(GPIOD, GPIO_Pin_1);
    }
    break;

    case 1:
    {
        if (direction == 0)
            GPIO_SetBits(GPIOD, GPIO_Pin_3),
                GPIO_ResetBits(GPIOD, GPIO_Pin_2);
        if (direction == 1)
            GPIO_ResetBits(GPIOD, GPIO_Pin_3),
                GPIO_SetBits(GPIOD, GPIO_Pin_2);
    }
    break;

    case 2:
    {
        if (direction == 0)
            GPIO_SetBits(GPIOD, GPIO_Pin_5),
                GPIO_ResetBits(GPIOD, GPIO_Pin_4);
        if (direction == 1)
            GPIO_ResetBits(GPIOD, GPIO_Pin_5),
                GPIO_SetBits(GPIOD, GPIO_Pin_4);
    }
    break;

    case 3:
    {
        if (direction == 0)
            GPIO_SetBits(GPIOD, GPIO_Pin_7),
                GPIO_ResetBits(GPIOD, GPIO_Pin_6);
        if (direction == 1)
            GPIO_ResetBits(GPIOD, GPIO_Pin_7),
                GPIO_SetBits(GPIOD, GPIO_Pin_6);
    }
    break;

    default:
        break;
    }
}
