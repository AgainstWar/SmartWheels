#include "stm32f10x.h"
#include "MPU6050.h"
#include "SysTick.h"



int8_t Zero_Drift;
int16_t mpu6050_gyro_x, mpu6050_gyro_y, mpu6050_gyro_z;
int16_t mpu6050_acc_x, mpu6050_acc_y, mpu6050_acc_z;

#define GET_MPU6050_SDA GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
#define MPU6050_SCL_LOW() GPIO_SetBits(GPIOB, GPIO_Pin_6)    // IO������͵�ƽ
#define MPU6050_SCL_HIGH() GPIO_ResetBits(GPIOB, GPIO_Pin_6) // IO������ߵ�ƽ
#define MPU6050_SDA_LOW() GPIO_SetBits(GPIOB, GPIO_Pin_7)    // IO������͵�ƽ
#define MPU6050_SDA_HIGH() GPIO_ResetBits(GPIOB, GPIO_Pin_7) // IO������ߵ�ƽ

#define ack 1    // ��Ӧ��
#define no_ack 0 // ��Ӧ��

void MPU6050_Z_Zero_Drift_Calculation(void);


/**
 * @brief   ģ��IIC��ʼ��
 * @param   NULL
 * @return  void
 * @note ���IICͨѶʧ�ܿ��Գ�������j��ֵ
 */
static void mpu6050_simiic_delay(void)
{
    uint16_t j = MPU6050_IIC_DELAY;
    while (j--)
        ;
}

// internal fuction
static void mpu6050_simiic_start(void)
{
    MPU6050_SDA_HIGH();
    MPU6050_SCL_HIGH();
    mpu6050_simiic_delay();
    MPU6050_SDA_LOW();
    mpu6050_simiic_delay();
    MPU6050_SCL_LOW();
}

// internal fuction
static void mpu6050_simiic_stop(void)
{
    MPU6050_SDA_LOW();
    MPU6050_SCL_LOW();
    mpu6050_simiic_delay();
    MPU6050_SCL_HIGH();
    mpu6050_simiic_delay();
    MPU6050_SDA_HIGH();
    mpu6050_simiic_delay();
}

// internal fuction
static void mpu6050_simiic_sendack(unsigned char ack_dat) // ��Ӧ��(����ack:SDA=0��no_ack:SDA=0)
{

    MPU6050_SCL_LOW();
    mpu6050_simiic_delay();
    if (ack_dat)
        MPU6050_SDA_LOW();
    else
        MPU6050_SDA_HIGH();

    MPU6050_SCL_HIGH();
    mpu6050_simiic_delay();
    MPU6050_SCL_LOW();
    mpu6050_simiic_delay();
}

// internal fuction
static int mpu6050_sccb_waitack(void)
{
    MPU6050_SCL_LOW();

    mpu6050_simiic_delay();

    MPU6050_SCL_HIGH();
    mpu6050_simiic_delay();

    if (GET_MPU6050_SDA) // Ӧ��Ϊ�ߵ�ƽ���쳣��ͨ��ʧ��
    {

        MPU6050_SCL_LOW();
        return 0;
    }

    MPU6050_SCL_LOW();
    mpu6050_simiic_delay();
    return 1;
}

/**
 * @brief   ģ��IIC��������
 * @param   c		���͵����ݣ�����������Ҳ���ǵ�ַ�������������մ�Ӧ��
 * @return  void
 * @note    �����Ǵ�Ӧ��λ
 */
static void mpu6050_send_ch(uint8_t c)
{
    uint8_t i = 8;
    while (i--)
    {
        if (c & 0x80)
            MPU6050_SDA_HIGH(); // SDA �������
        else
            MPU6050_SDA_LOW();
        c <<= 1;
        mpu6050_simiic_delay();
        MPU6050_SCL_HIGH(); // SCL ���ߣ��ɼ��ź�
        mpu6050_simiic_delay();
        MPU6050_SCL_LOW(); // SCL ʱ��������
    }
    mpu6050_sccb_waitack();
}

/**
 * @brief �ֽڽ��ճ���
 * @param ack_x
 * @note  �����������������ݣ��˳���Ӧ���|��Ӧ����|ʹ��
 * @note  internal fuction
 */
static uint8_t mpu6050_read_ch(uint8_t ack_x)
{
    uint8_t i;
    uint8_t c;
    c = 0;
    MPU6050_SCL_LOW();
    mpu6050_simiic_delay();
    MPU6050_SDA_HIGH();

    for (i = 0; i < 8; i++)
    {
        mpu6050_simiic_delay();
        MPU6050_SCL_LOW(); // ��ʱ����Ϊ�ͣ�׼����������λ
        mpu6050_simiic_delay();
        MPU6050_SCL_HIGH(); // ��ʱ����Ϊ�ߣ�ʹ��������������Ч
        mpu6050_simiic_delay();
        c <<= 1;
        if (GET_MPU6050_SDA)
        {
            c += 1; // ������λ�������յ����ݴ�c
        }
    }

    MPU6050_SCL_LOW();
    mpu6050_simiic_delay();
    mpu6050_simiic_sendack(ack_x);

    return c;
}

/**
 *  @brief      ģ��IICд���ݵ��豸�Ĵ�������
 *  @param      dev_add			�豸��ַ(����λ��ַ)
 *  @param      reg				�Ĵ�����ַ
 *  @param      dat				д�������
 *  @return     void
 *  @since      v1.0
 */
static void mpu6050_simiic_write_reg(uint8_t dev_add, uint8_t reg, uint8_t dat)
{
    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x00); // ����������ַ��дλ
    mpu6050_send_ch(reg);                   // ���ʹӻ��Ĵ�����ַ
    mpu6050_send_ch(dat);                   // ������Ҫд�������
    mpu6050_simiic_stop();
}

/**
 *  @brief      ģ��IIC���豸�Ĵ�����ȡ����
 *  @param      dev_add			�豸��ַ(����λ��ַ)
 *  @param      reg				�Ĵ�����ַ
 *  @param      type			ѡ��ͨ�ŷ�ʽ��IIC  ���� SCCB
 *  @return     uint8_t			���ؼĴ���������
 *  @since      v1.0
 */
uint8_t mpu6050_simiic_read_reg(uint8_t dev_add, uint8_t reg)
{
    uint8_t dat;
    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x00); // ����������ַ��дλ
    mpu6050_send_ch(reg);                   // ���ʹӻ��Ĵ�����ַ

    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x01); // ����������ַ�Ӷ�λ
    dat = mpu6050_read_ch(no_ack);          // ��ȡ����
    mpu6050_simiic_stop();

    return dat;
}

/**
 *  @brief      ģ��IIC��ȡ���ֽ�����
 *  @param      dev_add			�豸��ַ(����λ��ַ)
 *  @param      reg				�Ĵ�����ַ
 *  @param      dat_add			���ݱ���ĵ�ַָ��
 *  @param      num				��ȡ�ֽ�����
 *  @param      type			ѡ��ͨ�ŷ�ʽ��IIC  ���� SCCB
 *  @return     uint8_t			���ؼĴ���������
 *  @since      v1.0
 */
void mpu6050_simiic_read_regs(uint8_t dev_add, uint8_t reg, uint8_t *dat_add, uint8_t num)
{
    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x00); // ����������ַ��дλ
    mpu6050_send_ch(reg);                   // ���ʹӻ��Ĵ�����ַ

    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x01); // ����������ַ�Ӷ�λ
    while (--num)
    {
        *dat_add = mpu6050_read_ch(ack); // ��ȡ����
        dat_add++;
    }
    *dat_add = mpu6050_read_ch(no_ack); // ��ȡ����
    mpu6050_simiic_stop();
}

/**
 *@brief      MPU6050�Լ캯��
 *@param      NULL
 *@return     void
 *@since      v1.0
 */
static uint8_t mpu6050_self1_check(void)
{
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, PWR_MGMT_1, 0x00); // �������״̬
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, SMPLRT_DIV, 0x07); // 125HZ������
    if (0x07 != mpu6050_simiic_read_reg(MPU6050_DEV_ADDR, SMPLRT_DIV))
    {
        // printf("mpu6050 init error.\r\n");
        return 1;
        // ��������ԭ�������¼���
        // 1 MPU6050���ˣ�������µ������ĸ��ʼ���
        // 2 ���ߴ������û�нӺ�
        // 3 ��������Ҫ����������裬������3.3V
        // 4 ����û�е���ģ��IIC�ĳ�ʼ������
    }

    return 0;
}

/**
 *  @brief      ��ʼ��MPU6050
 *  @param      NULL
 *  @return     void
 */
uint8_t mpu6050_init(void)
{
    delay_ms(100); // �ϵ���ʱ

    if (mpu6050_self1_check())
    {
        return 1;
    }
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, PWR_MGMT_1, 0x00);     // �������״̬
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, SMPLRT_DIV, 0x07);     // 125HZ������
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, MPU6050_CONFIG, 0x04); //
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, GYRO_CONFIG, 0x18);    // ���ٶ������̡�2000dps
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, ACCEL_CONFIG, 0x10);   // ���ٶ����̡�8g
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, User_Control, 0x00);
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, INT_PIN_CFG, 0x02);
    MPU6050_Z_Zero_Drift_Calculation();                               // Z����Ư����
    return 0;
}

/**
 * @brief   ��ȡMPU6050���ٶ�����
 * @param   NULL
 * @return  void
 * @note	ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
 */
void mpu6050_get_accdata(void)
{
    uint8_t dat[6];

    mpu6050_simiic_read_regs(MPU6050_DEV_ADDR, ACCEL_XOUT_H, dat, 6);
    mpu6050_acc_x = (int16_t)(((uint16_t)dat[0] << 8 | dat[1]));
    mpu6050_acc_y = (int16_t)(((uint16_t)dat[2] << 8 | dat[3]));
    mpu6050_acc_z = (int16_t)(((uint16_t)dat[4] << 8 | dat[5]));
}

/**
 *  @brief      ��ȡMPU6050����������
 *  @param      NULL
 *  @return     void
 *  @since      v1.0
 */
void mpu6050_get_gyro(void)
{
    uint8_t dat[6];

    mpu6050_simiic_read_regs(MPU6050_DEV_ADDR, GYRO_XOUT_H, dat, 6);
    mpu6050_gyro_x = (int16_t)(((uint16_t)dat[0] << 8 | dat[1]));
    mpu6050_gyro_y = (int16_t)(((uint16_t)dat[2] << 8 | dat[3]));
    mpu6050_gyro_z = (int16_t)(((uint16_t)dat[4] << 8 | dat[5]));
}



/**
 *  @brief      ����MPU6050 Z����Ư
 *  @param      NULL
 *  @return     void
 *  @note       ���������ڳ�ʼ���У��ϵ缴������Ưֵ�����ں�������
*/
void MPU6050_Z_Zero_Drift_Calculation(void)
{
    uint8_t i=0;
    for(i=0;i<150;i++)
    {
       mpu6050_get_gyro();
       Zero_Drift+=mpu6050_gyro_z;
       delay_ms(2);
    }
    Zero_Drift/=150;
   
}

