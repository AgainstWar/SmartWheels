#include "LCD.h"
#include "SysTick.h"

static void LCD_Desplay(uint_16 x)
{
	while(x--);
}

/**
 * @brief	LCD1602引脚初始化
 * @param	void
 * @note NULL
 * @return void
*/
static void GPIO_InitConifg(void)
{
	GPIO_InitTypeDef GPIO_Structure;
	// 开启时钟
	LCD_CON_RCC_FUNC(CON_GPIO_CLK, ENABLE);
	LCD_DATA_RCC_FUNC(DATA_GPIO_CLK, ENABLE);
	
	// 控制端
	GPIO_Structure.GPIO_Pin = CON_RS_PIN;
	GPIO_Structure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
	GPIO_Structure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CON_GPIO, &GPIO_Structure);
	
	GPIO_Structure.GPIO_Pin = CON_RW_PIN;
	GPIO_Init(CON_GPIO, &GPIO_Structure);
	
	GPIO_Structure.GPIO_Pin = CON_E_PIN;
	GPIO_Init(CON_GPIO, &GPIO_Structure);
	
	// 输出端
	GPIO_Structure.GPIO_Pin = DATA0_PIN;
	GPIO_Init(DATA_GPIO, &GPIO_Structure);
	GPIO_Structure.GPIO_Pin = DATA1_PIN;
	GPIO_Init(DATA_GPIO, &GPIO_Structure);
	GPIO_Structure.GPIO_Pin = DATA2_PIN;
	GPIO_Init(DATA_GPIO, &GPIO_Structure);
	GPIO_Structure.GPIO_Pin = DATA3_PIN;
	GPIO_Init(DATA_GPIO, &GPIO_Structure);
	GPIO_Structure.GPIO_Pin = DATA4_PIN;
	GPIO_Init(DATA_GPIO, &GPIO_Structure);
	GPIO_Structure.GPIO_Pin = DATA5_PIN;
	GPIO_Init(DATA_GPIO, &GPIO_Structure);
	GPIO_Structure.GPIO_Pin = DATA6_PIN;
	GPIO_Init(DATA_GPIO, &GPIO_Structure);
	GPIO_Structure.GPIO_Pin = DATA7_PIN;
	GPIO_Init(DATA_GPIO, &GPIO_Structure);
}

/**
 * @brief	LCD1602屏幕初始化
 * @param	void
 * @note NULL
 * @return void
*/
void LCD_Init(void)
{
	GPIO_InitConifg();
	LCD_WriteCon(CLOSE_CURSOR);         // 设置光标闪烁
	LCD_WriteCon(SHOW_TWO_5X10);         // 显示两行，5X10
	LCD_WriteCon(AFTER_CR);             // 光标左移，屏幕不移动
	LCD_WriteCon(CLEAR);                // 清屏
	LCD_WriteCon(SHOW_CURSOR_TWINKLE);	//显示光标
}

/**
 * @brief	对LCD1602写入数据
 * @param	data 需要写入的数据
 * @note NULL
 * @return void
*/
void LCD_WriteData(uint_8 data)
{
	GPIO_ResetBits(CON_GPIO, CON_E_PIN);
	
	// 10, 写入数据
	GPIO_SetBits(CON_GPIO, CON_RS_PIN);
	GPIO_ResetBits(CON_GPIO, CON_RW_PIN);
	
	// 写入数据
	GPIO_Write(DATA_GPIO, data<<8);
	LCD_Desplay(0xffff);
	
	// 写入使能
	GPIO_SetBits(CON_GPIO, CON_E_PIN);
	LCD_Desplay(0xffff);
	
	GPIO_ResetBits(CON_GPIO, CON_E_PIN);
}

/**
 * @brief	对LCD1602写入指令
 * @param	data 相关指令，具体可查看LCD.h中的宏定义
 * @note NULL
 * @return void
*/
void LCD_WriteCon(uint_8 data)
{
	GPIO_ResetBits(CON_GPIO, CON_E_PIN);
	
	// 00, 写入命令
	GPIO_ResetBits(CON_GPIO, CON_RS_PIN);
	GPIO_ResetBits(CON_GPIO, CON_RW_PIN);
	
	// 写入数据
	GPIO_Write(DATA_GPIO, data<<8);
	LCD_Desplay(0xffff);
	
	// 写入使能
	GPIO_SetBits(CON_GPIO, CON_E_PIN);
	LCD_Desplay(0xffff);
	
	GPIO_ResetBits(CON_GPIO, CON_E_PIN);
}	

/******************************************************************************
 * 函数名称:void LCD1602_Set_Cursor(u8 x, u8 y)				      		*
 * 函数功能:							 					      *
 * 输入参数:								      				*
 * 返回值  :无														          *
 * 其他说明:	 				 				     			  				*
 ******************************************************************************/
/**
 * @brief	设置LCD1602显示位置
 * @param	x 横坐标 y 纵坐标	
 * @note NULL
 * @return void
*/
void LCD1602_Set_Cursor( u8 x, u8 y )
{
	u8 addr;

	if ( y == 0 )
		addr = 0x00 + x;
	else
		addr = 0x40 + x;
	LCD_WriteData( addr | 0x80 );
}

/**
 * @brief	LCD1602显示数字
 * @param	num 需要显示的数字，0~9999；默认显示坐标为第一行第一列
 * @note NULL
 * @return void
*/
void LCD1602_Show_num(uint16_t num)
{
	uint16_t a=0,b=0,c=0,d=0;
	a=num/1000;
	b=(num/100)%10;
	c=(num/10)%10;
	d=num%10;
	LCD1602_Set_Cursor(0,0);
	LCD_WriteData(a+0x30);
	LCD_WriteData(b+0x30);
	LCD_WriteData(c+0x30);
	LCD_WriteData(d+0x30);
}


