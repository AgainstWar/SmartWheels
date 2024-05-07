#include <control.h>
#include "usart.h"
#include "math.h"
#include "stdlib.h"
#include "LED.h"
#include "SysTick.h"

enum dir
{
    N = 0,
    S = 1,
    W = 2,
    E = 3,
    run = 4,
};
enum dir direction;

#define ENCODER_VALUE 67        // 0.6m对应的编码器值
uint8_t expect_encoderval = 20; // 编码器期望值
uint8_t average_value = 0;      // 4个编码器平均值

// 陀螺仪数据变量
float angle = 0;
float GyroZ_last = 0;
float GyroZ = 0;

uint16_t distance_gradientmov_flag = 0; // 通过串口数据计算得到的行动距离数据

uint8_t turn_flag = 0;    // 转向标志位
uint8_t displacement = 0; // 移动位移

// 增量式PID变量
s16 ek[4] = {0};                  // 4个电机各自的当前误差
s16 ek1[4] = {0};                 // 4个电机各自的前一次误差
s16 ek2[4] = {0};                 // 4个电机各自的前前次误差
uint16_t Increament[4] = {0};     // 计算得到的增量值
uint16_t Increament_Out[4] = {0}; // 增量输出

// 增量式PID 参数数组
double PID_Para[4][3] = {
    //  P  I  D
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
    if (angle <= 90 && turn_flag == 1)
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
        turn_flag = 0;
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
    if (angle > -90 && turn_flag == 2)

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
        turn_flag = 0;
        angle = 0;
        Motor_Speed(0, 0);
        Motor_Speed(1, 0);
        Motor_Speed(2, 0);
        Motor_Speed(3, 0);
    }
}

/**
 * @brief 前进
 * @param void
 * @return void
 * @note 4电机正转
 */
void Move_forward(void)
{
    // 设置电机转向
    Motor_SetDirection(0, 1);
    Motor_SetDirection(1, 1);
    Motor_SetDirection(2, 1);
    Motor_SetDirection(3, 1);
}

/**
 * @brief 后退
 * @return void
 * @note 4电机反转
 */
void Move_back(void)
{
    // 设置电机转向
    Motor_SetDirection(0, 0);
    Motor_SetDirection(1, 0);
    Motor_SetDirection(2, 0);
    Motor_SetDirection(3, 0);
}

/**
 * @brief   增量式PID
 * @param   Expect_Encode_Value    编码器期望值
 * @param   num                    电机编号0-3
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
 * @brief   运动计算距离后停止
 * @param   gradient 梯度0-10,对应 distance_gradientmov_flag 标志位
 * @note    理论计算0.6m距离单个编码器累计值约为67
 * @return  void
 */
void unit_distancemov(uint8_t gradient)
{
    int i = 0;
    // 计算四个编码器平均编码值
    average_value = (Encode_Value[0] + Encode_Value[1] + Encode_Value[2] + Encode_Value[3]) / 4;

    if (gradient == 0)
    {
        Motor_Speed(0, 0);
        Motor_Speed(1, 0);
        Motor_Speed(2, 0);
        Motor_Speed(3, 0);
    }
    else if (gradient != 0)
    {
        displacement += average_value; // 编码器值累加计算路程
        // PID计算占空比
        for (i = 0; i < 4; i++)
        {
            speed[i] = PID_Increasement(expect_encoderval, i);
        }
        Motor_Speed(0, speed[0]);
        Motor_Speed(1, speed[1]);
        Motor_Speed(2, speed[2]);
        Motor_Speed(3, speed[3]);

        if (displacement > gradient * ENCODER_VALUE)
        {
            gradient = 0;     // 标志位清零,小车停止
            displacement = 0; // 位移计数清零
        }
    }
}

/**
 * @brief   运动控制函数
 * @param   void
 * @return  void
 * @note    读取USART数据后运动响应，主函数中直接调用即可
 */
void Movement(void)
{
    u8 length = 0;
    int i = 0;
    char num[2] = {0};

    // 使用USART数据对“direction”赋值
    if (USART1_RX_STA & 0x8000)
    {
        uint16_t current_data[8] = {0};  // 当前接收数据
        length = USART1_RX_STA & 0x3fff; // 获取数据长度
        for (i = 0; i < length; i++)
        {
            current_data[i] = USART1_RX_BUF[i]; // 复制获取数据
        }

        // 判断运动方向
        //  N-0b00 S-0b01 E-0b11 W-0b10
        if (current_data[0] == 'N')
        {
            direction = (enum dir)N;
        }
        else if (current_data[0] == 'S')
        {
            direction = (enum dir)S;
        }
        else if (current_data[0] == 'W')
        {
            direction = (enum dir)W;
        }
        else if (current_data[0] == 'E')
        {
            direction = (enum dir)E;
        }

        // 计算运动距离
        for (i = 0; i < 2; i++)
        {
            num[i] = current_data[i + 1]; // 提取十进制数字字符串
        }
        distance_gradientmov_flag = atoi(num);
        // atoi()函数将字符串数字转变为整型十进制数
        USART_SendData(USART1, USART1_RX_BUF[(char)speed[0]]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
            ;              // 等待发送结束
        USART1_RX_STA = 0; // 清零
    }
    // 检测运动方向
    switch (direction)
    {
    // 前进
    case N:
    {
        Encode_Clr(); // 编码器计数清零
        LED2 = !LED2;
        Move_forward();
        direction = run; // 退出状态机
    }
    break;
    // 后退
    case S:
    {
        Encode_Clr();
        LED1 = !LED1;
        Move_back();
        direction = run;
    }
    break;
    // 右
    case E:
    {
        Encode_Clr();
        turn_flag = 2;
        Turn_right();
        LED1 = !LED1;
        LED2 = !LED2;
        direction = run;
    }
    break;
    // 左
    case W:
    {
        Encode_Clr();
        turn_flag = 1;
        Turn_left();
        LED1 = !LED1;
        LED2 = !LED2;

        direction = run;
    }
    break;
    default:
        break;
    }

    // 移动计算得出的距离
    if (direction == run)
    {
        unit_distancemov(distance_gradientmov_flag);
    }
}
