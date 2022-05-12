#include "usart.h"
#include <stdio.h>

#define RETAGETUART	huart1	 //重定向串口端口
#define	RECEVEIUART	huart1	 //接收串口端口
#define RXBUFFERSIZE  200     //最大接收字节数

//接收消息队列结构体
typedef struct RX_Queue
{
    uint8_t USART_RX_BUF[RXBUFFERSIZE];//数据域
    int front;//队头指针
    int rear;//队尾指针
    uint8_t length;//队列当前长度
}RX_Queue;

extern char RxBuffer[RXBUFFERSIZE];  //接收数据
extern uint8_t aRxBuffer;			  //接收中断缓冲
//extern uint8_t Uart1_Rx_Cnt;         //接收缓冲计数
