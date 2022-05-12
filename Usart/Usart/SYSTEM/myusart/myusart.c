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

char RxBuffer[RXBUFFERSIZE];  //接收数据
uint8_t aRxBuffer;			  //接收中断缓冲
uint8_t Usart_Rx_Cnt = 0;     //接收缓冲计数

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);
	if(Usart_Rx_Cnt > 200)  //溢出判断
	{
		Usart_Rx_Cnt = 0;
		memset(RxBuffer,0,sizeof(RxBuffer));
	}
	else
	{
		RxBuffer[Usart_Rx_Cnt++] = aRxBuffer;
		//可以在此处增加自己的逻辑函数，但是最好处理串口接收到的数据最好放在外面，否则容易卡死
	}

	//因为接收中断使用了一次即关闭，所以在最后加入这行代码即可实现无限使用
	//发现数据发的过多时程序会进不去中断，怀疑是这里再次使能失败
	while(HAL_UART_Receive_IT(&RECEVEIUART, (uint8_t *)&aRxBuffer, 1)==HAL_BUSY);  
}
