#include <control.h>
#include "encode.h"
#include "motor.h"
#include "pwm.h"
#include "usart.h"
#include "math.h"
#include "stdlib.h"
#include "LED.h"
#include "SysTick.h"
#include "math.h"
#include "MPU6050.h"
#include "system.h"

enum dir direction = stop;  //初始化默认停止
enum dir direction_last=stop;

uint8_t cnt=0;

uint8_t expect_speed = 80; // 速度期望值；单位 mm/s

uint8_t time_sum =0 ;       // 匀速运动时间 单位 s
uint8_t average_value = 0;  // 4个电机实际速度平均值
uint8_t speed_pwm[4] = {0}; // 存储占空比
// 陀螺仪数据变量
float angle = 0;
float GyroZ_last = 0;
float GyroZ = 0;

uint16_t distance_gradientmov_flag = 0; // 通过串口数据计算得到的行动距离数据

uint8_t turn_flag = 0;    // 转向标志位


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
   GyroZ= Gz;
   GyroZ_last = GyroZ;
   GyroZ = (Gz - Zero_Drift) / 16.4;
   GyroZ = GyroZ * 0.9 + GyroZ_last * 0.1;
}

/**
 * @brief   原地转向90°
 * @param   void
 * @return  void
 */
void Turn(void)
{
   
    // 角度计算
    angle += (GyroZ*0.008);
      
    switch(turn_flag)
    {
        case 1://左转   // 陀螺仪安装影响角速度方向，默认向左转角速度为正
        {
                if (angle>90)
                {
                    if(cnt%100==0)//静止1秒左右
                    {
                        cnt=0;
                        direction=(enum dir)run;
                        turn_flag=0;//转向标志位清零
                        angle=0;
                    }
                else 
                    {
                    
                    // 设置电机速度
                    Motor_Speed(0, 0);
                    Motor_Speed(1, 0);
                    Motor_Speed(2, 0);
                    Motor_Speed(3, 0);
                    }
                }
                else
                {
                // 设置电机转向
                Motor_SetDirection(0, 0);
                Motor_SetDirection(1, 0);
                Motor_SetDirection(2, 1);
                Motor_SetDirection(3, 1);

                // 设置电机速度
                Motor_Speed(0, 90);
                Motor_Speed(1, 90);
                Motor_Speed(2, 90);
                Motor_Speed(3, 90);
                }
                
        }break;

        case 2://右转
        {
                if (angle<-90)
                {

                    if(cnt%100==0)
                    {
                        cnt=0;
                        direction=(enum dir)run;
                        turn_flag=0;//转向标志位清零
                        angle=0;
                    }
                else 
                    {
                        // 设置电机速度
                        Motor_Speed(0, 0);
                        Motor_Speed(1, 0);
                        Motor_Speed(2, 0);
                        Motor_Speed(3, 0);
                    }
                }
                else
                {
                // 设置电机转向
                Motor_SetDirection(0, 1);
                Motor_SetDirection(1, 1);
                Motor_SetDirection(2, 0);
                Motor_SetDirection(3, 0);

                // 设置电机速度
                Motor_Speed(0, 90);
                Motor_Speed(1, 90);
                Motor_Speed(2, 90);
                Motor_Speed(3, 90);
                }
                
        }break;
        default:break;
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

    ek[num] = Expect_Encode_Value - average_value;
    PID_P = PID_Para[num][0] * (ek[num] - ek1[num]);
    PID_I = PID_Para[num][1] * ek[num];
    PID_D = PID_Para[num][2] * (ek[num] - 2 * ek1[num] + ek2[num]);

    ek2[num] = ek1[num];
    ek1[num] = ek[num];

    Increment[num] = PID_P + PID_I + PID_D;
    return Increment[num];
}

/**
 * @brief   电机速度控制函数
 * @param   num 电机编号0-3
 * @note    该函数由PID计算出的增量对PUM占空比赋值进而对电机进行速度控制
 * @note    默认0，1号电机在左；2，3号电机在右
 * @return  void
 */
void Motor_Control(uint8_t num)
{
     // 计算四个电机平均速度值 单位  cm/s
    average_value = (real_speed[0] + real_speed[1] + real_speed[2] + real_speed[3]) / 4;

    speed_pwm[num] = PID_Increment(expect_speed, num);
    
    if(speed_pwm[num]>0)
    {
        Motor_SetDirection(num, 1);
    }
    else
    {
        Motor_SetDirection(num, 0);
    }
    
    Motor_Speed(num, abs(speed_pwm[num]));
     
}

/**
 * @brief   运动计算距离后停止
 * @param   gradient 梯度0-10,对应 distance_gradientmov_flag 标志位
 * @note    NULL
 * @return  void
 */
void unit_distancemov( void)
{
            if (direction_last==S)
            {
                Move_back();
                Motor_Speed(0,PID_Increment(expect_speed,0));
                Motor_Speed(1,PID_Increment(expect_speed,1));
                Motor_Speed(2,PID_Increment(expect_speed,2));
                Motor_Speed(3,PID_Increment(expect_speed,3));
            }
            else if(direction_last==W||direction_last==E||direction_last==N)
            {
                Move_forward();
                Motor_Control(0);//对0号电机闭环
                Motor_Control(1);
                Motor_Control(2);
                Motor_Control(3);                
            }
            
    switch (distance_gradientmov_flag)
    {
    
    case 0:
    {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
            System_Reset();
    }break;

    case 1:
    {
      
         if (time_sum == distance_gradientmov_flag*5)//匀速运动10秒
        {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
           
        }

    }break;

    case 2:
    {            
         
         if (time_sum == distance_gradientmov_flag*5)
        {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
            
        }

    }break;

    case 3:
    {           
         
         if (time_sum == distance_gradientmov_flag*5)
        {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
            
        }

    }break;

      case 4:
    {       
         
         if (time_sum == distance_gradientmov_flag*5)
        {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
            
        }
            
    }break;

    case 5:
    { 
         
         if (time_sum == distance_gradientmov_flag*5)
           {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
            
           }
           
    }break;

    case 6:
    {
         
         if (time_sum == distance_gradientmov_flag*5)
        {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
            
        }
            
    }break;

    case 7:
    {
         
         if (time_sum == distance_gradientmov_flag*5)
        {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
            
        }
            
    }break; 

      case 8:
    {
         
         if (time_sum == distance_gradientmov_flag*5)
        {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
            
        }
            
    }break;

    case 9:
    {

         if (time_sum ==distance_gradientmov_flag*5)
        {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=-0;
            direction = (enum dir)stop;
            
        }
            
    }break;

    case 10:
    {            
         
         if (time_sum == distance_gradientmov_flag*5)
        {
            distance_gradientmov_flag = 0;     // 标志位清零,小车停止 退出当前状态
            time_sum=0;
            direction = (enum dir)stop;
            
        }

    }break;
    
    default:break;
        
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
    // 检测并设置运动方向
    switch (direction)
    {
    // 前进
    case N:
    {
        Encode_Clr(); // 编码器计数清零
        Rspeed_Clr(); // 速度数据清零
        Move_forward();
        direction_last=N;
        direction = run; // 退出当前状态
    }
    break;
    // 后退
    case S:
    {
        Encode_Clr();
        Rspeed_Clr();
        Move_back();
        direction_last=S;
        direction = run;
    }
    break;
    // 左
    case W:
    {       
        Encode_Clr();
        Rspeed_Clr();
        direction_last=W;
        turn_flag = 1;     
    }break;
    // 右
    case E:
    {     

        Encode_Clr();
        Rspeed_Clr();
        direction_last=E;
        turn_flag = 2;      
    }
    break;
     // 移动计算得出的距离
    case run:
    {	
        unit_distancemov();
    }break;
    //停止运动
	case stop:
	{
		Motor_Speed(0, 0);
        Motor_Speed(1, 0);
        Motor_Speed(2, 0);
        Motor_Speed(3, 0);
        average_value=0;//均速数据清零
        Encode_Clr();
        Rspeed_Clr();
        angle=0;
	}break;
    default:break;
    }
}
