#ifndef _MPU6050_H
#define _MPU6050_H

#include "stm32f10x.h"


//=====================================================���� IIC ����====================================================
// #define MPU6050_SCL_PIN 	( PB_6 )				    // ���� IIC SCL ����
// #define MPU6050_SDA_PIN 	( PB_7 ) 					// ���� IIC SDA ����
#define MPU6050_IIC_DELAY 	( 0 )
//=====================================================���� IIC ����====================================================

// ����MPU6050�ڲ���ַ

#define	MPU6050_DEV_ADDR	    0xD0>>1	//IICд��ʱ�ĵ�ַ�ֽ����ݣ�+1Ϊ��ȡ

#define	SMPLRT_DIV				0x19	//�����ǲ����ʣ�����ֵ��0x07(125Hz)
#define	MPU6050_CONFIG          0x1A	//��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)
#define	GYRO_CONFIG				0x1B	//�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
#define	ACCEL_CONFIG	        0x1C	//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz)
#define INT_PIN_CFG	  			0x37    //����6050����I2CΪֱͨģʽ�Ĵ���
#define	ACCEL_XOUT_H	        0x3B
#define	ACCEL_XOUT_L	        0x3C
#define	ACCEL_YOUT_H	        0x3D
#define	ACCEL_YOUT_L	        0x3E
#define	ACCEL_ZOUT_H	        0x3F
#define	ACCEL_ZOUT_L	        0x40
#define	GYRO_XOUT_H				0x43
#define	GYRO_XOUT_L				0x44	
#define	GYRO_YOUT_H				0x45
#define	GYRO_YOUT_L				0x46
#define	GYRO_ZOUT_H				0x47
#define	GYRO_ZOUT_L				0x48
#define User_Control 			0x6A    //�ر�6050�Ը���I2C�豸�Ŀ���
#define	PWR_MGMT_1				0x6B	//��Դ����������ֵ��0x00(��������)
#define	WHO_AM_I				0x75	//IIC��ַ�Ĵ���(Ĭ����ֵ0x68��ֻ��)


extern int16_t mpu6050_gyro_x,mpu6050_gyro_y,mpu6050_gyro_z;    //���ٶȱ���
extern int16_t mpu6050_acc_x,mpu6050_acc_y,mpu6050_acc_z;       //���ٶȱ���
extern int8_t Zero_Drift;

uint8_t  mpu6050_init(void);               //��ʼ��MPU6050
void   mpu6050_get_accdata(void);          //��ȡ���ٶ�����
void   mpu6050_get_gyro(void);             //��ȡ���ٶ�����



#endif 

