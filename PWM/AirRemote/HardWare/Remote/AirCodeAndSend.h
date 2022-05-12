#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include<stdio.h>

void Send_1(void);//发送高电平
void Send_0(void);//发送低电平

void Send_L(void);//发送L码
void Send_S(void);//发送S码
void Send_Stop(void);//发送stop码

void Send_Code(uint8_t data);//发送编码
void Air_Contorl(uint8_t B,uint8_t C);
