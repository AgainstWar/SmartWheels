#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x.h"

typedef unsigned char uint_8;
typedef unsigned int  uint_16;

// 控制端引脚
#define CON_GPIO           GPIOC
#define LCD_CON_RCC_FUNC   RCC_APB2PeriphClockCmd
#define CON_GPIO_CLK   		 RCC_APB2Periph_GPIOC
#define CON_RS_PIN     		 GPIO_Pin_8
#define CON_RW_PIN     		 GPIO_Pin_9
#define CON_E_PIN      		 GPIO_Pin_10

// 数据传输端引脚
#define DATA_GPIO          GPIOB
#define LCD_DATA_RCC_FUNC  RCC_APB2PeriphClockCmd
#define DATA_GPIO_CLK  		 RCC_APB2Periph_GPIOB
#define DATA0_PIN      		 GPIO_Pin_8
#define DATA1_PIN      		 GPIO_Pin_9
#define DATA2_PIN      		 GPIO_Pin_10
#define DATA3_PIN      		 GPIO_Pin_11
#define DATA4_PIN      		 GPIO_Pin_12
#define DATA5_PIN      		 GPIO_Pin_13
#define DATA6_PIN      		 GPIO_Pin_14
#define DATA7_PIN      		 GPIO_Pin_15

// 常用指令
#define CLEAR                0x01   // 清屏
#define CURSOR_RESET         0x10   // 光标归位
#define AFTER_CL             0x04   // 写入数据后，光标左移，显示屏不移动
#define AFTER_CL_SR          0x05   // 写入数据后，光标左移，显示屏整体向右移动
#define AFTER_CR           	 0x06   // 写入数据后，光标右移，显示屏不移动
#define AFTER_CR_SR          0x07   // 写入数据后，光标右移，显示屏整体向右移动
#define CLOSE_CURSOR         0x0C   // 无光标
#define SHOW_CURSOR_TWINKLE  0x0f   // 光标闪烁
#define SHOW_CURSOR          0x0e   // 光标不闪烁
#define CURSOR_LEFT          0x10   // 光标左移一格
#define CURSOR_RIGHT         0x14   // 光标右移一格
#define SCREEN_LEFT          0x18   // 显示屏上字符全部左移一格，光标不动
#define SCREEN_RIGHT         0x1C   // 显示屏上所有字符右移一格，光标不动

#define SHOW_ONE_5X7         0x30   // 显示一行，5X7点阵 (默认数据总线为8位)
#define SHOW_ONE_5X10        0x34   // 显示一行，5X10
#define SHOW_TWO_5X7         0x38   // 显示两行，5X7
#define SHOW_TWO_5X10        0x3C   // 显示两行，5X10

void LCD_Init(void);
void LCD_WriteData(uint_8 data);
void LCD_WriteCon(uint_8 data);
void LCD1602_Set_Cursor( u8 x, u8 y );
void LCD1602_Show_num(uint16_t num);
#endif

