#include "su_driver_soft_iic.h"

/***********************************************
* @brief : ģ��iic��ʼ��
* @param : void
* @return: void
* @date  : 2023/12/3
* @author: ������
************************************************/
void Soft_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOB_CLK_ENABLE();   //ʹ��GPIOBʱ��
    //SDA��SCL��ʼ������
    GPIO_Initure.Pin=SOFT_IIC_SDA_PIN|SOFT_IIC_SCL_PIN;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_OD;  //��©���������������벻��
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
    HAL_GPIO_Init(SOFT_IIC_SDA_PORT,&GPIO_Initure);

    SOFT_IIC_SDA(1);
    SOFT_IIC_SCL(1);
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SOFT_IIC_SDA(1);	  	  
	SOFT_IIC_SCL(1);
	System_Delay_us(5);
 	SOFT_IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
	System_Delay_us(5);
	SOFT_IIC_SCL(0);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SOFT_IIC_SCL(0);
	SOFT_IIC_SDA(0);//STOP:when CLK is high DATA change form low to high
 	System_Delay_us(4);
	SOFT_IIC_SCL(1); 
	SOFT_IIC_SDA(1);//����I2C���߽����ź�
	System_Delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
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
	SOFT_IIC_SCL(0);//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	SOFT_IIC_SCL(0);
	SOFT_IIC_SDA(0);
	System_Delay_us(2);
	SOFT_IIC_SCL(1);
	System_Delay_us(2);
	SOFT_IIC_SCL(0);
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	SOFT_IIC_SCL(0);
	SOFT_IIC_SDA(1);
	System_Delay_us(2);
	SOFT_IIC_SCL(1);
	System_Delay_us(2);
	SOFT_IIC_SCL(0);
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;       
    SOFT_IIC_SCL(0);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        SOFT_IIC_SDA((txd&0x80)>>7);
        txd<<=1; 	  
		System_Delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		SOFT_IIC_SCL(1);
		System_Delay_us(2); 
		SOFT_IIC_SCL(0);	
		System_Delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}
