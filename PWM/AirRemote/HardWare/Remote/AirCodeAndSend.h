#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include<stdio.h>

void Send_1(void);//���͸ߵ�ƽ
void Send_0(void);//���͵͵�ƽ

void Send_L(void);//����L��
void Send_S(void);//����S��
void Send_Stop(void);//����stop��

void Send_Code(uint8_t data);//���ͱ���
void Air_Contorl(uint8_t B,uint8_t C);
