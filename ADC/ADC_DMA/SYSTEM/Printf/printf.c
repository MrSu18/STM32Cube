#include "printf.h"
#include <stdlib.h>
#include <string.h>
#include "sys.h"

/*WBN*/
uint8_t aRxBuffer;				//接收中断缓冲
uint8_t Uart1_RxBuff[256];		//接收缓冲
uint8_t Uart1_Rx_Cnt = 0;		//接收缓冲计数
uint8_t	cAlmStr[] = "ERROR\r\n";

//重定向printf
int fputc(int ch, FILE *f)
{
  uint8_t temp[1] = {ch};
  HAL_UART_Transmit(&huart1, temp, 1, 2);//huart1需要根据你的配置修改
  return ch;
}

//串口中断函数，直接把控制逻辑写在这里，串口接收到数据之后会在这里行进处理
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  	UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
 
	if(Uart1_Rx_Cnt >= 255)  //溢出判断，如果溢出了则进行报错处理
	{
		Uart1_Rx_Cnt = 0;								//接收缓冲技术置0
		memset(Uart1_RxBuff,0x00,sizeof(Uart1_RxBuff));	//清空数组
	}
	else
	{
		Uart1_RxBuff[Uart1_Rx_Cnt++] = aRxBuffer;   //接收数据转存
		if((Uart1_RxBuff[Uart1_Rx_Cnt-1] == 0x0A)&&(Uart1_RxBuff[Uart1_Rx_Cnt-2] == 0x0D)) //判断结束位
		{	
			Uart1_Rx_Cnt = 0;								//接收缓冲技术置0
			memset(Uart1_RxBuff,0x00,sizeof(Uart1_RxBuff)); //清空数组
		}
	}
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);   //再开启接收中断
}

