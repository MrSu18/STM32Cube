#include "myusart.h"
#include <string.h>

//printf重定向
int fputc(int ch, FILE *f)
{
  uint8_t temp[1] = {ch};
  HAL_UART_Transmit(&RETAGETUART, temp, 1, 2);//huart1需要根据你的配置修改
  return ch;
}

int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
}

/***********************************************
* @brief : 串口接收回调函数
* @param : 串口端口
* @return: 消息循环队列
* @date  : 2022.9.19
* @author: 刘骏帆
************************************************/
RX_Queue usart_rx_queue;	//接收循环队列
uint8_t aRxBuffer;			  //接收中断缓冲
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/*======================串口信息进入消息队列=================*/
	if((usart_rx_queue.rear+1)%MAX_QUEUE_LENGTH==usart_rx_queue.front)//判断是否队满
	{
		usart_rx_queue.front=(usart_rx_queue.front+1)%MAX_QUEUE_LENGTH;//队头加一
	}
	else
	{
		usart_rx_queue.length++;
	}
	//无论如何队列的队尾都需要进队的，接收到的信息不能被放弃
	usart_rx_queue.USART_RX_BUF[usart_rx_queue.rear]=aRxBuffer;
	usart_rx_queue.rear=(usart_rx_queue.rear+1)%MAX_QUEUE_LENGTH;
	/*=========================================================*/
	//在以下部分添加通讯帧是否结束的逻辑判断代码
	
	
	//因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用
	//发现数据发的过多时程序会进不去中断，怀疑是这里再次使能失败
	while(HAL_UART_Receive_IT(&RECEVEIUART, (uint8_t *)&aRxBuffer, 1)==HAL_BUSY);  
}
