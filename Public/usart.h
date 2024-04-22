#ifndef __usart_H
#define __usart_H

#include "system.h" 


#define USART1_REC_LEN		200  	//�����������ֽ��� 200

extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         		//����״̬���


void USART1_Init(u32 bound);


#endif


