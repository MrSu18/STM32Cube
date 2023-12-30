#ifndef _SU_DRIVER_SOFT_IIC_H
#define _SU_DRIVER_SOFT_IIC_H

//HAL库的头文件
#include "gpio.h"
#include "main.h"
//本库的头文件
#include "su_common_typedef.h"
#include "su_common_delay.h"

//引脚宏定义
#ifdef  SOFT_IIC_SDA_Pin	
#define SOFT_IIC_SDA_PIN	SOFT_IIC_SDA_Pin
#define SOFT_IIC_SDA_PORT SOFT_IIC_SDA_GPIO_Port
#define SOFT_IIC_SCL_PIN	SOFT_IIC_SCL_Pin
#define SOFT_IIC_SCL_PORT SOFT_IIC_SCL_GPIO_Port
//如果用户没有使用cubemx的user_lable定义引脚则用默认引脚
#else 
#define SOFT_IIC_SDA_PIN	GPIO_PIN_6
#define SOFT_IIC_SDA_GPIO_PORT GPIOB
#define SOFT_IIC_SCL_PIN	GPIO_PIN_7
#define SOFT_IIC_SCL_GPIO_PORT GPIOB
#endif

//IO操作
#define SOFT_IIC_SDA(N)   HAL_GPIO_WritePin(SOFT_IIC_SDA_PORT, SOFT_IIC_SDA_PIN, N==1?GPIO_PIN_SET:GPIO_PIN_RESET) //SDA
#define SOFT_IIC_SCL(N)   HAL_GPIO_WritePin(SOFT_IIC_SCL_PORT, SOFT_IIC_SCL_PIN, N==1?GPIO_PIN_SET:GPIO_PIN_RESET) //SCL
#define READ_SDA    HAL_GPIO_ReadPin(SOFT_IIC_SDA_PORT,SOFT_IIC_SDA_PIN)  //输入SDA

//函数声明
void Soft_IIC_Init(void);//模拟iic初始化
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 

#endif
