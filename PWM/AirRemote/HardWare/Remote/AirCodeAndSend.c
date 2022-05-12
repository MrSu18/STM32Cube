#include "AirCodeAndSend.h"

/*定时器计数值：378-1 分频：5-1*/
void delay_us(uint32_t i)
{
	uint32_t temp;
	SysTick->LOAD=9*i;
	SysTick->CTRL=0X01;
	SysTick->VAL=0;
	 do
	{
		temp=SysTick->CTRL;
	}
	while((temp&0x01)&&(!(temp&(1<<16))));
	SysTick->CTRL=0;
	SysTick->VAL=0;
}


void Send_1()//发送高电平
{
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,189);
  delay_us(540);
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
  delay_us(1620);
}
void Send_0()//发送低电平
{
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,189);
  delay_us(540);
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
  delay_us(540);
}

void Send_L()//发送L码
{
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,189);
  delay_us(4400);
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
  delay_us(4400);
}

void Send_S()//发送S码
{
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,189);
  delay_us(540);
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
  delay_us(5220);
}

void Send_Stop()//发送stop码
{
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,189);
  delay_us(540);
  __HAL_TIM_SetCompare(&htim3,TIM_CHANNEL_2,0);
}

void Send_Code(uint8_t data)//发送编码
{
  for (int i = 7; i >=0; i--)
  {
    if (data&(1<<i))
    {
      Send_1();
    }
    else
    {
      Send_0();
    }
  }
}

void Air_Contorl(uint8_t B,uint8_t C)
{
  Send_L();
  Send_Code(0xB2);
  Send_Code(~0xB2);
  Send_Code(B);
  Send_Code(~B);
  Send_Code(C);
  Send_Code(~C);
  Send_S();

  Send_L();
  Send_Code(0xB2);
  Send_Code(~0xB2);
  Send_Code(B);
  Send_Code(~B);
  Send_Code(C);//制冷26度
  Send_Code(~C);
  Send_Stop();
}
