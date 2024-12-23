#include "encode.h"

u16 Encode_Value[4] = {0, 0, 0, 0};
uint8_t real_speed[4] = {0};  // 实际速度 单位 cm/s
const float diameter = 6E-2;  // 轮胎直径 6cm
const float code_disc = 20.0; // 编码盘格数

/**
 * @brief 编码器初始化
 * @param void
 * @note 编码器原始值初始化
 * @returns void
 */
void Encode_Init(void)
{
    // 编码器GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // 编码器原始值初始化
    Encode_Value[0] = 0;
    Encode_Value[1] = 0;
    Encode_Value[2] = 0;
    Encode_Value[3] = 0;
}

/**
 * @brief   编码器计数清零
 * @param   void
 * @note
 * @return void
 */
void Encode_Clr(void)
{
    Encode_Value[0] = 0;
    Encode_Value[1] = 0;
    Encode_Value[2] = 0;
    Encode_Value[3] = 0;
}

/**
 * @brief   速度数据清零
 * @param   void
 * @note
 * @return void
 */
void Rspeed_Clr(void)
{
    real_speed[0]=0;
    real_speed[1]=0;
    real_speed[2]=0;
    real_speed[3]=0;
}

