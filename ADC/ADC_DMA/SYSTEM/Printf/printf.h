#include "usart.h"
#include <stdio.h>

//定义串口中断处理的变量	/*WBN*/
extern uint8_t aRxBuffer;			//接收中断缓冲
extern uint8_t Uart1_RxBuff[256];	//接收缓冲
extern uint8_t Uart1_Rx_Cnt;		//接收缓冲计数
extern uint8_t	cAlmStr[];

