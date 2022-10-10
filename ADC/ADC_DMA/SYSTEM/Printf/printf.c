#include "printf.h"
#include <stdlib.h>
#include <string.h>
#include "sys.h"

/*WBN*/
uint8_t aRxBuffer;				//�����жϻ���
uint8_t Uart1_RxBuff[256];		//���ջ���
uint8_t Uart1_Rx_Cnt = 0;		//���ջ������
uint8_t	cAlmStr[] = "ERROR\r\n";

//�ض���printf
int fputc(int ch, FILE *f)
{
  uint8_t temp[1] = {ch};
  HAL_UART_Transmit(&huart1, temp, 1, 2);//huart1��Ҫ������������޸�
  return ch;
}

//�����жϺ�����ֱ�Ӱѿ����߼�д��������ڽ��յ�����֮����������н�����
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  	UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
 
	if(Uart1_Rx_Cnt >= 255)  //����жϣ�������������б�����
	{
		Uart1_Rx_Cnt = 0;								//���ջ��弼����0
		memset(Uart1_RxBuff,0x00,sizeof(Uart1_RxBuff));	//�������
	}
	else
	{
		Uart1_RxBuff[Uart1_Rx_Cnt++] = aRxBuffer;   //��������ת��
		if((Uart1_RxBuff[Uart1_Rx_Cnt-1] == 0x0A)&&(Uart1_RxBuff[Uart1_Rx_Cnt-2] == 0x0D)) //�жϽ���λ
		{	
			Uart1_Rx_Cnt = 0;								//���ջ��弼����0
			memset(Uart1_RxBuff,0x00,sizeof(Uart1_RxBuff)); //�������
		}
	}
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);   //�ٿ��������ж�
}

