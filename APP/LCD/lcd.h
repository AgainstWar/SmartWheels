#ifndef _LCD_H
#define _LCD_H

#include "stm32f10x.h"
#include "system.h"
#include "SysTick.h"

#define LCD_RS PCout(1)
#define LCD_RW PCout(2)
#define LCD_EN PCout(0)

#define LCD_BUSY PCin(15)

void Lcd1602_Pin_Init(void);
void Lcd1602_Init(void);
void Lcd1602_Write_Cmd(int cmd);
void Lcd1602_Write_Data(int data);
void Lcd1602_DisplayChar(u8 row, u8 col, int ch);
void Lcd1602_DisplayString(u8 row, u8 col, u8 *str);

void Lcd1602_Check_Busy(void);

#endif
