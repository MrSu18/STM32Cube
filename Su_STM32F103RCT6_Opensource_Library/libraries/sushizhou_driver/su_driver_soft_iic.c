#include "su_driver_soft_iic.h"

/***********************************************
* @brief : 模拟iic初始化
* @param : void
* @return: void
* @date  : 2023/12/3
* @author: 刘骏帆
************************************************/
void Soft_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOB时钟
    //SDA和SCL初始化设置
    GPIO_Initure.Pin=SOFT_IIC_SDA_PIN|SOFT_IIC_SCL_PIN;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_OD;  //开漏输出，推挽输出输入不了
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
    HAL_GPIO_Init(SOFT_IIC_SDA_PORT,&GPIO_Initure);

    SOFT_IIC_SDA(1);
    SOFT_IIC_SCL(1);
}

//产生IIC起始信号
void IIC_Start(void)
{
	SOFT_IIC_SDA(1);	  	  
	SOFT_IIC_SCL(1);
	System_Delay_us(5);
 	SOFT_IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	System_Delay_us(5);
	SOFT_IIC_SCL(0);//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SOFT_IIC_SCL(0);
	SOFT_IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	System_Delay_us(4);
	SOFT_IIC_SCL(1); 
	SOFT_IIC_SDA(1);//发送I2C总线结束信号
	System_Delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;  
	SOFT_IIC_SDA(1);System_Delay_us(1);	   
	SOFT_IIC_SCL(1);System_Delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	SOFT_IIC_SCL(0);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	SOFT_IIC_SCL(0);
	SOFT_IIC_SDA(0);
	System_Delay_us(2);
	SOFT_IIC_SCL(1);
	System_Delay_us(2);
	SOFT_IIC_SCL(0);
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	SOFT_IIC_SCL(0);
	SOFT_IIC_SDA(1);
	System_Delay_us(2);
	SOFT_IIC_SCL(1);
	System_Delay_us(2);
	SOFT_IIC_SCL(0);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;       
    SOFT_IIC_SCL(0);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        SOFT_IIC_SDA((txd&0x80)>>7);
        txd<<=1; 	  
		System_Delay_us(2);   //对TEA5767这三个延时都是必须的
		SOFT_IIC_SCL(1);
		System_Delay_us(2); 
		SOFT_IIC_SCL(0);	
		System_Delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
    for(i=0;i<8;i++ )
	{
        SOFT_IIC_SCL(0); 
        System_Delay_us(2);
		SOFT_IIC_SCL(1);
        receive<<=1;
        if(READ_SDA)receive++;   
		System_Delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}
