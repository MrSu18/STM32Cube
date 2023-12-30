#ifndef _SU_DEVICE_OLED_H
#define _SU_DEVICE_OLED_H

//HAL���ͷ�ļ�
#include "gpio.h"
#include "main.h"
//�����ͷ�ļ�
#include "su_common_typedef.h"
#include "su_common_delay.h"

//�궨��
#define	Brightness	0x7f //����OLED����  Խ��Խ��    ��Χ0-0XFF  
#define OLED_X_MAX 	128	 //OLED��ʾ�Ŀ�Ⱥ궨��0~127
#define OLED_Y_MAX 	64

//��������
void OLED_send_cmd(uint8_t o_command);	
void OLED_send_data(uint8_t o_data);		
void OLED_clear(void);			
void OLED_full(void);			
void OLED_init(void);				
//�������
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize);	
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size);
void OLED_ShowNum(uint8_t x,uint8_t y,uint16_t num,uint8_t len,uint8_t size);
void OLED_ShowFloat(uint8_t x,uint8_t y,double dat,uint8_t num,uint8_t pointnum,uint8_t size);
//ͼƬ���
void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[]);
void OLED_DrawBMP_Fast(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[]);
void OLED_DrawBMP_Faster(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[]);

#endif
