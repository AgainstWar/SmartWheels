#include <control.h>

#define ENCODER_VALUE 79618 // 0.5m对应的编码器值
uint8_t expect_encoderval;  // 编码器期望值
float angle = 0;
float GyroZ_last, GyroZ;
uint8_t turnleft_flag = 0, turnright_flag = 0; // 接收到的转向标志位
uint8_t ditance_gradientmov_flag = 0;          // 接收到的梯度距离移动标志位
uint8_t displacement;                          // 移动位移标志位

// 增量式PID变量
uint8_t ek[4] = {0};              // 4个电机各自的当前误差
uint8_t ek1[4] = {0};             // 4个电机各自的前一次误差
uint8_t ek2[4] = {0};             // 4个电机各自的前前次误差
uint16_t Increament[4] = {0};     // 计算得到的增量值
uint16_t Increament_Out[4] = {0}; // 增量输出

// 增量式PID 参数数组     P   I   D  0-3行对应电机编号，P、I、D对应各电机参数
double PID_Para[4][3] = {
    8, 2, 25, // 电机0参数
    8, 2, 25, // 电机1参数
    8, 2, 25, // 电机2参数
    8, 2, 25, // 电机3参数
};

/**
 * @brief   MPU6050 Z轴角速度处理
 * @param   void
 * @note    抑制零漂，将角速度转换为实际物理值
 * @return  void
 */
void MPU6050_data_processing(void)
{
    GyroZ_last = GyroZ;
    GyroZ = (mpu6050_gyro_z - Zero_Drift) / 16.4;
    GyroZ = GyroZ * 0.9 + GyroZ_last * 0.1;
}

/**
 * @brief   原地左转90°
 * @param   void
 * @note    右轮正转，左轮反转
 * @return  void
 */
void Turn_left(void)
{

    // 获取陀螺仪数据
    mpu6050_get_gyro();

    // 设置电机转向
    Motor_SetDirection(0, 0);
    Motor_SetDirection(1, 0);
    Motor_SetDirection(2, 1);
    Motor_SetDirection(3, 1);

    // 处理角速度
    MPU6050_data_processing();

    // 陀螺仪安装影响角速度方向，默认向左转角速度为正
    if (angle <= 90 && turnleft_flag == 1)
    {
        // 角度计算
        angle += GyroZ;

        // 设置电机速度
        Motor_Speed(0, 50);
        Motor_Speed(1, 50);
        Motor_Speed(2, 50);
        Motor_Speed(3, 50);
    }
    else if (angle > 90)
    {
        turnleft_flag = 0;
        angle = 0;
        Motor_Speed(0, 0);
        Motor_Speed(1, 0);
        Motor_Speed(2, 0);
        Motor_Speed(3, 0);
    }
}

/**
 * @brief   原地右转90°
 * @param   void
 * @note    右轮反转，左轮正转
 * @return  void
 */
void Turn_right(void)
{
    // 获取陀螺仪数据
    mpu6050_get_gyro();

    // 设置电机转向
    Motor_SetDirection(0, 1);
    Motor_SetDirection(1, 1);
    Motor_SetDirection(2, 0);
    Motor_SetDirection(3, 0);

    // 处理加速度
    MPU6050_data_processing();

    // 陀螺仪安装影响角速度方向，默认向左转角速度为正
    if (angle > -90 && turnright_flag == 1)

    {
        // 角度计算
        angle += GyroZ;

        // 设置电机速度
        Motor_Speed(0, 50);
        Motor_Speed(1, 50);
        Motor_Speed(2, 50);
        Motor_Speed(3, 50);
    }
    else if (angle < -90)
    {
        turnright_flag = 0;
        angle = 0;
        Motor_Speed(0, 0);
        Motor_Speed(1, 0);
        Motor_Speed(2, 0);
        Motor_Speed(3, 0);
    }
}

/**
 * @brief   增量式PID
 * @param   Expect_Encode_Value    编码器期望值
 * @param   num                    电机编号0-3
 * @note   在对应编码器中断程序中调用即可
 * @return  PID增量输出
 */
uint16_t PID_Increasement(int8_t Expect_Encode_Value, int8_t num)
{
    // 计算当前误差
    ek[num] = Expect_Encode_Value - Encode_Value[num];

    // 计算增量
    Increament[num] = PID_Para[num][0] * (ek[num] - ek1[num]) + PID_Para[num][1] * ek[num] + PID_Para[num][2] * (ek[num] - 2 * ek1[num] + ek2[num]);
    // 增量累加输出
    Increament_Out[num] += Increament[num];

    // 更新误差
    ek2[num] = ek1[num];
    ek1[num] = ek[num];

    // 限幅
    if (Increament_Out[num] > 100)
        Increament_Out[num] = 100;

    return Increament_Out[num];
}

/**
 * @brief   梯度距离运动
 * @note    理论计算0.5m距离单个编码器累计值约为79618
 * @note    在主函数中调用即可
 * @return  void
 */
void unit_distance(void)
{
    // 计算四个编码器平均编码值
    uint8_t average_value = 0;

    // 计算平均值
    average_value += (Encode_Value[0] / 4);
    average_value += (Encode_Value[1] / 4);
    average_value += (Encode_Value[2] / 4);
    average_value += (Encode_Value[3] / 4);

    if (displacement == 0)
    {
        Motor_Speed(0, 0);
        Motor_Speed(1, 0);
        Motor_Speed(2, 0);
        Motor_Speed(3, 0);
    }
    else if (displacement >= 1 && displacement <= 10)
    {
        average_value += average_value;
        if (average_value > displacement * ENCODER_VALUE)
        {
            ditance_gradientmov_flag = 0;
            displacement = 0;
            average_value = 0;
        }
    }
}

/**
 * @brief 前进
 * @param level 前进距离，从1到10
 * @return void
 * @note 4电机正转
 * @todo 编码器算距离/编码器陀螺仪融合算距离
 */
void Move_forward(u8 level)
{
    // 设置电机转向
    Motor_SetDirection(0, 1);
    Motor_SetDirection(1, 1);
    Motor_SetDirection(2, 1);
    Motor_SetDirection(3, 1);
}