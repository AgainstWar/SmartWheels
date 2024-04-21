#include <control.h>

float angle = 0;
float GyroZ_last,GyroZ;
uint8_t turnleft_flag=0,turnright_flag=0;//接收到的转向标志位

/**
 * @brief   MPU6050 Z轴角速度处理
 * @param   void
 * @note    抑制零漂，将角速度转换为实际物理值
 * @return  void
*/
void MPU6050_data_processing(void)
{
    GyroZ_last = GyroZ;
    GyroZ = (mpu6050_gyro_z - Zero_Drift)/16.4;
    GyroZ = GyroZ*0.9 + GyroZ_last*0.1;
}



/**
 * @brief   原地左转90°
 * @param   void
 * @note    右轮正转，左轮反转
 * @return  void
*/
void Turn_left(void)
{

    //获取陀螺仪数据
    mpu6050_get_gyro();

    //设置电机转向
    Motor_SetDirection(0, 0);
    Motor_SetDirection(1, 0);
    Motor_SetDirection(2, 1);
    Motor_SetDirection(3, 1);

    //处理加速度
    MPU6050_data_processing();
    

    //陀螺仪安装影响角速度方向，默认向左转角速度为正
    if(angle<=90&&turnleft_flag==1)
    {
         //角度计算
        angle+=GyroZ;

        //设置PWM占空比，控制电机转速
        PWM_SetDutyCycle(1, 50);
        PWM_SetDutyCycle(2, 50);
        PWM_SetDutyCycle(3, 50);
        PWM_SetDutyCycle(4, 50);
    }
    else if (angle>90)
    {
        turnleft_flag=0;
        angle=0;
        PWM_SetDutyCycle(1, 0);
        PWM_SetDutyCycle(2, 0);
        PWM_SetDutyCycle(3, 0);
        PWM_SetDutyCycle(4, 0);
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
    //获取陀螺仪数据
    mpu6050_get_gyro();

    //设置电机转向
    Motor_SetDirection(0, 1);
    Motor_SetDirection(1, 1);
    Motor_SetDirection(2, 0);
    Motor_SetDirection(3, 0);

    //处理加速度
    MPU6050_data_processing();
    

    //陀螺仪安装影响角速度方向，默认向左转角速度为正
    if(angle>-90&&turnright_flag==1)

    {   
        //角度计算
        angle+=GyroZ;
        
        //设置PWM占空比，控制电机转速
        PWM_SetDutyCycle(1, 50);
        PWM_SetDutyCycle(2, 50);
        PWM_SetDutyCycle(3, 50);
        PWM_SetDutyCycle(4, 50);
    }
    else if (angle<-90)
    {
        turnright_flag=0;
        angle=0;
        PWM_SetDutyCycle(1, 0);
        PWM_SetDutyCycle(2, 0);
        PWM_SetDutyCycle(3, 0);
        PWM_SetDutyCycle(4, 0);
    }
}





















