#include <control.h>

float angle = 0;
float GyroZ_last,GyroZ;
uint8_t turnleft_flag=0,turnright_flag=0;//���յ���ת���־λ

/**
 * @brief   MPU6050 Z����ٶȴ���
 * @param   void
 * @note    ������Ư�������ٶ�ת��Ϊʵ������ֵ
 * @return  void
*/
void MPU6050_data_processing(void)
{
    GyroZ_last = GyroZ;
    GyroZ = (mpu6050_gyro_z - Zero_Drift)/16.4;
    GyroZ = GyroZ*0.9 + GyroZ_last*0.1;
}



/**
 * @brief   ԭ����ת90��
 * @param   void
 * @note    ������ת�����ַ�ת
 * @return  void
*/
void Turn_left(void)
{

    //��ȡ����������
    mpu6050_get_gyro();

    //���õ��ת��
    Motor_SetDirection(0, 0);
    Motor_SetDirection(1, 0);
    Motor_SetDirection(2, 1);
    Motor_SetDirection(3, 1);

    //������ٶ�
    MPU6050_data_processing();
    

    //�����ǰ�װӰ����ٶȷ���Ĭ������ת���ٶ�Ϊ��
    if(angle<=90&&turnleft_flag==1)
    {
         //�Ƕȼ���
        angle+=GyroZ;

        //����PWMռ�ձȣ����Ƶ��ת��
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
 * @brief   ԭ����ת90��
 * @param   void
 * @note    ���ַ�ת��������ת
 * @return  void
*/
void Turn_right(void)
{
    //��ȡ����������
    mpu6050_get_gyro();

    //���õ��ת��
    Motor_SetDirection(0, 1);
    Motor_SetDirection(1, 1);
    Motor_SetDirection(2, 0);
    Motor_SetDirection(3, 0);

    //������ٶ�
    MPU6050_data_processing();
    

    //�����ǰ�װӰ����ٶȷ���Ĭ������ת���ٶ�Ϊ��
    if(angle>-90&&turnright_flag==1)

    {   
        //�Ƕȼ���
        angle+=GyroZ;
        
        //����PWMռ�ձȣ����Ƶ��ת��
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





















