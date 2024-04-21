#include "encode.h"

u32 Encode_Value[4] = {0, 0, 0, 0};
float speed[4] = {0};
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
    // 编码器原始值初始化
    Encode_Value[0] = 0;
    Encode_Value[1] = 0;
    Encode_Value[2] = 0;
    Encode_Value[3] = 0;
}
