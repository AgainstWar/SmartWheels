#include "stm32f10x.h"
#include "MPU6050.h"
#include "SysTick.h"

int8_t Zero_Drift;
int16_t mpu6050_gyro_x, mpu6050_gyro_y, mpu6050_gyro_z;
int16_t mpu6050_acc_x, mpu6050_acc_y, mpu6050_acc_z;

#define GET_MPU6050_SDA GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)
#define MPU6050_SCL_LOW() GPIO_SetBits(GPIOB, GPIO_Pin_6)    // IO口输出低电平
#define MPU6050_SCL_HIGH() GPIO_ResetBits(GPIOB, GPIO_Pin_6) // IO口输出高电平
#define MPU6050_SDA_LOW() GPIO_SetBits(GPIOB, GPIO_Pin_7)    // IO口输出低电平
#define MPU6050_SDA_HIGH() GPIO_ResetBits(GPIOB, GPIO_Pin_7) // IO口输出高电平

#define ack 1    // 主应答
#define no_ack 0 // 从应答

void MPU6050_Z_Zero_Drift_Calculation(void);

/**
 * @brief   模拟IIC初始化
 * @param   NULL
 * @return  void
 * @note 如果IIC通讯失败可以尝试增加j的值
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
static void mpu6050_simiic_sendack(unsigned char ack_dat) // 主应答(包含ack:SDA=0和no_ack:SDA=0)
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

    if (GET_MPU6050_SDA) // 应答为高电平，异常，通信失败
    {

        MPU6050_SCL_LOW();
        return 0;
    }

    MPU6050_SCL_LOW();
    mpu6050_simiic_delay();
    return 1;
}

/**
 * @brief   模拟IIC发送数据
 * @param   c		发送的数据（可以是数据也可是地址），发送完后接收从应答
 * @return  void
 * @note    不考虑从应答位
 */
static void mpu6050_send_ch(uint8_t c)
{
    uint8_t i = 8;
    while (i--)
    {
        if (c & 0x80)
            MPU6050_SDA_HIGH(); // SDA 输出数据
        else
            MPU6050_SDA_LOW();
        c <<= 1;
        mpu6050_simiic_delay();
        MPU6050_SCL_HIGH(); // SCL 拉高，采集信号
        mpu6050_simiic_delay();
        MPU6050_SCL_LOW(); // SCL 时钟线拉低
    }
    mpu6050_sccb_waitack();
}

/**
 * @brief 字节接收程序
 * @param ack_x
 * @note  接收器件传来的数据，此程序应配合|主应答函数|使用
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
        MPU6050_SCL_LOW(); // 置时钟线为低，准备接收数据位
        mpu6050_simiic_delay();
        MPU6050_SCL_HIGH(); // 置时钟线为高，使数据线上数据有效
        mpu6050_simiic_delay();
        c <<= 1;
        if (GET_MPU6050_SDA)
        {
            c += 1; // 读数据位，将接收的数据存c
        }
    }

    MPU6050_SCL_LOW();
    mpu6050_simiic_delay();
    mpu6050_simiic_sendack(ack_x);

    return c;
}

/**
 *  @brief      模拟IIC写数据到设备寄存器函数
 *  @param      dev_add			设备地址(低七位地址)
 *  @param      reg				寄存器地址
 *  @param      dat				写入的数据
 *  @return     void
 *  @since      v1.0
 */
static void mpu6050_simiic_write_reg(uint8_t dev_add, uint8_t reg, uint8_t dat)
{
    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x00); // 发送器件地址加写位
    mpu6050_send_ch(reg);                   // 发送从机寄存器地址
    mpu6050_send_ch(dat);                   // 发送需要写入的数据
    mpu6050_simiic_stop();
}

/**
 *  @brief      模拟IIC从设备寄存器读取数据
 *  @param      dev_add			设备地址(低七位地址)
 *  @param      reg				寄存器地址
 *  @param      type			选择通信方式是IIC  还是 SCCB
 *  @return     uint8_t			返回寄存器的数据
 *  @since      v1.0
 */
uint8_t mpu6050_simiic_read_reg(uint8_t dev_add, uint8_t reg)
{
    uint8_t dat;
    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x00); // 发送器件地址加写位
    mpu6050_send_ch(reg);                   // 发送从机寄存器地址

    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x01); // 发送器件地址加读位
    dat = mpu6050_read_ch(no_ack);          // 读取数据
    mpu6050_simiic_stop();

    return dat;
}

/**
 *  @brief      模拟IIC读取多字节数据
 *  @param      dev_add			设备地址(低七位地址)
 *  @param      reg				寄存器地址
 *  @param      dat_add			数据保存的地址指针
 *  @param      num				读取字节数量
 *  @param      type			选择通信方式是IIC  还是 SCCB
 *  @return     uint8_t			返回寄存器的数据
 *  @since      v1.0
 */
void mpu6050_simiic_read_regs(uint8_t dev_add, uint8_t reg, uint8_t *dat_add, uint8_t num)
{
    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x00); // 发送器件地址加写位
    mpu6050_send_ch(reg);                   // 发送从机寄存器地址

    mpu6050_simiic_start();
    mpu6050_send_ch((dev_add << 1) | 0x01); // 发送器件地址加读位
    while (--num)
    {
        *dat_add = mpu6050_read_ch(ack); // 读取数据
        dat_add++;
    }
    *dat_add = mpu6050_read_ch(no_ack); // 读取数据
    mpu6050_simiic_stop();
}

/**
 *@brief      MPU6050自检函数
 *@param      NULL
 *@return     void
 *@since      v1.0
 */
static uint8_t mpu6050_self1_check(void)
{
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, PWR_MGMT_1, 0x00); // 解除休眠状态
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, SMPLRT_DIV, 0x07); // 125HZ采样率
    if (0x07 != mpu6050_simiic_read_reg(MPU6050_DEV_ADDR, SMPLRT_DIV))
    {
        // printf("mpu6050 init error.\r\n");
        return 1;
        // 卡在这里原因有以下几点
        // 1 MPU6050坏了，如果是新的这样的概率极低
        // 2 接线错误或者没有接好
        // 3 可能你需要外接上拉电阻，上拉到3.3V
        // 4 可能没有调用模拟IIC的初始化函数
    }

    return 0;
}

/**
 *  @brief      初始化MPU6050
 *  @param      NULL
 *  @return     void
 */
uint8_t mpu6050_init(void)
{
    delay_ms(100); // 上电延时

    if (mpu6050_self1_check())
    {
        return 1;
    }
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, PWR_MGMT_1, 0x00);     // 解除休眠状态
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, SMPLRT_DIV, 0x07);     // 125HZ采样率
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, MPU6050_CONFIG, 0x04); //
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, GYRO_CONFIG, 0x18);    // 角速度满量程±2000dps
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, ACCEL_CONFIG, 0x10);   // 加速度量程±8g
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, User_Control, 0x00);
    mpu6050_simiic_write_reg(MPU6050_DEV_ADDR, INT_PIN_CFG, 0x02);
    MPU6050_Z_Zero_Drift_Calculation(); // Z轴零漂计算
    return 0;
}

/**
 * @brief   获取MPU6050加速度数据
 * @param   NULL
 * @return  void
 * @note	执行该函数后，直接查看对应的变量即可
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
 *  @brief      获取MPU6050陀螺仪数据
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
 *  @brief      计算MPU6050 Z轴零漂
 *  @param      NULL
 *  @return     void
 *  @note       函数调用于初始化中，上电即计算零漂值，并于后续抑制
 */
void MPU6050_Z_Zero_Drift_Calculation(void)
{
    uint8_t i = 0;
    for (i = 0; i < 150; i++)
    {
        mpu6050_get_gyro();
        Zero_Drift += mpu6050_gyro_z;
        delay_ms(2);
    }
    Zero_Drift /= 150;
}

