#include "system.h"

/**
 * @brief 软件复位函数
 * @param void
 * @note  强制清除冗余指令，保证操作连续性
 * @return void
 */
void System_Reset(void) 
{
	__set_FAULTMASK(1); //关闭所有中断
	NVIC_SystemReset(); //进行软件复位
}




