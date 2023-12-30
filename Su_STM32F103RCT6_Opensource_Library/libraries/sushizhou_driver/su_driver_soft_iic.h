#ifndef _SU_DRIVER_SOFT_IIC_H
#define _SU_DRIVER_SOFT_IIC_H

//HAL���ͷ�ļ�
#include "gpio.h"
#include "main.h"
//�����ͷ�ļ�
#include "su_common_typedef.h"
#include "su_common_delay.h"

//���ź궨��
#ifdef  SOFT_IIC_SDA_Pin	
#define SOFT_IIC_SDA_PIN	SOFT_IIC_SDA_Pin
#define SOFT_IIC_SDA_PORT SOFT_IIC_SDA_GPIO_Port
#define SOFT_IIC_SCL_PIN	SOFT_IIC_SCL_Pin
#define SOFT_IIC_SCL_PORT SOFT_IIC_SCL_GPIO_Port
//����û�û��ʹ��cubemx��user_lable������������Ĭ������
#else 
#define SOFT_IIC_SDA_PIN	GPIO_PIN_6
#define SOFT_IIC_SDA_GPIO_PORT GPIOB
#define SOFT_IIC_SCL_PIN	GPIO_PIN_7
#define SOFT_IIC_SCL_GPIO_PORT GPIOB
#endif

//IO����
#define SOFT_IIC_SDA(N)   HAL_GPIO_WritePin(SOFT_IIC_SDA_PORT, SOFT_IIC_SDA_PIN, N==1?GPIO_PIN_SET:GPIO_PIN_RESET) //SDA
#define SOFT_IIC_SCL(N)   HAL_GPIO_WritePin(SOFT_IIC_SCL_PORT, SOFT_IIC_SCL_PIN, N==1?GPIO_PIN_SET:GPIO_PIN_RESET) //SCL
#define READ_SDA    HAL_GPIO_ReadPin(SOFT_IIC_SDA_PORT,SOFT_IIC_SDA_PIN)  //����SDA

//��������
void Soft_IIC_Init(void);//ģ��iic��ʼ��
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 

#endif
