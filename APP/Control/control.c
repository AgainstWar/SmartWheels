#include <control.h>
#include "usart.h"
#include "math.h"
#include "stdlib.h"
#include "LED.h"
#include "SysTick.h"
#include "math.h"

enum dir direction;

#define unit_distance 60  // 单位距离 单位 m
uint8_t expect_speed = 8; // 速度期望值；单位 cm/s
// 电机满占空比即5V供电“匀速”运转时，real_speed取值参考区间 [10，16] (未闭环)
// 3.3V供电时    real_speed取值参考区间 [6，10] (未闭环)
// 5V和3.3V供电时，16与10出现概率较少，基本稳定为10和6，推测为开发板供电电源波动引起的噪声

uint8_t average_value = 0;  // 4个电机实际速度平均值
uint8_t speed_pwm[4] = {0}; // 存储占空比
// 陀螺仪数据变量
float angle = 0;
float GyroZ_last = 0;
float GyroZ = 0;

uint16_t distance_gradientmov_flag = 0; // 通过串口数据计算得到的行动距离数据

uint8_t turn_flag = 0;    // 转向标志位
uint8_t displacement = 0; // 移动位移

// 增量式PID变量
s16 ek[4] = {0};        // 4个电机各自的当前误差
s16 ek1[4] = {0};       // 4个电机各自的前一次误差
s16 ek2[4] = {0};       // 4个电机各自的前前次误差
s16 Increment[4] = {0}; // PID计算得到的增量值

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
s16 PID_Increment(int8_t Expect_Encode_Value, int8_t num)
{
    int PID_P = 0;
    int PID_I = 0;
    int PID_D = 0;
    // TODO: 将四个电机的速度平均值作为输入，对四个电机分别进行闭环，PID的输出存在正负，

    ek[num] = Expect_Encode_Value - Encode_Value[num];
    PID_P = PID_Para[num][0] * (ek[num] - ek1[num]);
    PID_I = PID_Para[num][1] * ek[num];
    PID_D = PID_Para[num][2] * (ek[num] - 2 * ek1[num] + ek2[num]);

    ek2[num] = ek1[num];
    ek1[num] = ek[num];

    Increment[num] = PID_P + PID_I + PID_D;
    return Increment[num];
}

/**
 * @brief   电机方向控制函数
 * @param   num 电机编号0-3
 * @note    该函数由PID计算出的增量对电机进行方向控制
 * @return  void
 */
void Motor_Control(uint8_t num)
{
    // TODO: 电机输出口要根据PID输出进行换向
    speed_pwm[num] = PID_Increment(expect_speed, num);

    if (speed_pwm[num] >= 0)
    {
        Motor_SetDirection(num, 1); // 正转
    }
    else
    {
        Motor_SetDirection(num, 0); // 反转
    }

    Motor_Speed(num, abs(speed_pwm[num]));
}

/**
 * @brief   运动计算距离后停止
 * @param   gradient 梯度0-10,对应 distance_gradientmov_flag 标志位
 * @param   pit_time 定时器中断时间　单位　ｓ;例如中断时间为5ms，则pit_time = 0.005
 * @note    NULL
 * @return  void
 */
void unit_distancemov(uint8_t gradient, float pit_time)
{
    // 计算四个电机平均速度值 单位 cm/s
    average_value = (real_speed[0] + real_speed[1] + real_speed[2] + real_speed[3]) / 4;

    if (gradient == 0)
    {
        Motor_Speed(0, 0);
        Motor_Speed(1, 0);
        Motor_Speed(2, 0);
        Motor_Speed(3, 0);
    }
    else if (gradient != 0)
    {
        displacement += (average_value * pit_time); // 实际速度积分得路程

        // PID计算以及控制电机转向和速度设置
        Motor_Control(0);
        Motor_Control(1);
        Motor_Control(2);
        Motor_Control(3);

        Motor_Speed(0, 70);
        Motor_Speed(1, 70);
        Motor_Speed(2, 70);
        Motor_Speed(3, 70);
        if (displacement > gradient * unit_distance)
        {
            gradient = 0;     // 标志位清零,小车停止
            displacement = 0; // 位移计数清零
            direction = (enum dir)stop;
            LED1 = !LED1;
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
        distance_gradientmov_flag = atoi(num); // atoi()函数将字符串数字转变为整型十进制数
        USART1_RX_STA = 0;                     // 清零
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
}
