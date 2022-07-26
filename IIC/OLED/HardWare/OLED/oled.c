#include "oled.h"
#include "myiic.h"
#include "delay.h"
#include "oled_font.h"
#include <stdio.h>//sprinft����

const unsigned char OLED_init_cmd[25]=
{
    /*0xae,0X00,0X10,0x40,0X81,0XCF,0xff,0xa1,0xa4,
    0xA6,0xc8,0xa8,0x3F,0xd5,0x80,0xd3,0x00,0XDA,0X12,
    0x8d,0x14,0xdb,0x40,0X20,0X02,0xd9,0xf1,0xAF*/
    0xAE,//�ر���ʾ
    0xD5,//����ʱ�ӷ�Ƶ����,��Ƶ��
    0x80,  //[3:0],��Ƶ����;[7:4],��Ƶ��

    0xA8,//��������·��
    0X3F,//Ĭ��0X3F(1/64)
    0xD3,//������ʾƫ��
    0X00,//Ĭ��Ϊ0
    0x40,//������ʾ��ʼ�� [5:0],����.                              
    0x8D,//��ɱ�����
    0x14,//bit2������/�ر�
    0x20,//�����ڴ��ַģʽ
    0x02,//[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
    0xA1,//���ض�������,bit0:0,0->0;1,0->127;
    0xC8,//����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
    0xDA,//����COMӲ����������
    0x12,//[5:4]����            
    0x81,//�Աȶ�����
    Brightness,//1~255;Ĭ��0X7F (��������,Խ��Խ��)
    0xD9,//����Ԥ�������
    0xf1,//[3:0],PHASE 1;[7:4],PHASE 2;
    0xDB,//����VCOMH ��ѹ����
    0x30,//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
    0xA4,//ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
    0xA6,//������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ        
    0xAF,//������ʾ     
};

//д����
void OLED_send_cmd(unsigned char o_command)
{
	IIC_Start();
    IIC_Send_Byte(0x78);//OLED��ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(0x00);//�Ĵ�����ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(o_command);
    IIC_Wait_Ack();
    IIC_Stop();
}
//д����
void OLED_send_data(unsigned char o_data)
{ 
	IIC_Start();
    IIC_Send_Byte(0x78);//OLED��ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(0x40);//�Ĵ�����ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(o_data);
    IIC_Wait_Ack();
    IIC_Stop();
}
/**********************************************************************************************************
*	�� �� ��: OLED_init
*	����˵��: ��ʼ��OLED
*	��    ��: ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void OLED_init(void)
{
	unsigned char i;
	IIC_Init();		//��ʼ��IIC
	for(i=0;i<25;i++)
	{
		OLED_send_cmd(OLED_init_cmd[i]);
	}
	OLED_clear();
}

//��ʾ�������
//���ù��λ��
void OLED_SetPos(unsigned char x, unsigned char y)
{ 
    OLED_send_cmd(0xb0+y);
    OLED_send_cmd(((x&0xf0)>>4)|0x10);
    OLED_send_cmd((x&0x0f)|0x01);
}
/**********************************************************************************************************
*	�� �� ��: OLED_ShowChar
*	����˵��: ��ָ��λ����ʾһ���ַ�,���������ַ������ֿ��е��ַ���
*	��    ��: 1. x,y����ʼ��ʾ����	x:0~127  y:0~63
			  2. ch[]���ַ���    
			  3. TextSize���ַ���С��1:6*8  2:8*16��
*	�� �� ֵ: ��
* 	ע    �⣺x�����ֺ���Ϊ����������
			  y=0,2,4,6��TextSize=2��  y=0,1,2,3,4,5,6,7��TextSize=1��
**********************************************************************************************************/
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size)
{      	
	unsigned char c=0,i=0;	
    c=chr-' ';//�õ�ƫ�ƺ��ֵ			
    if(x>OLED_X_MAX-1){x=0;y=y+2;}
    if(Char_Size ==2)
    {
        OLED_SetPos(x,y);	
        for(i=0;i<8;i++)
			OLED_send_data(F8X16[c*16+i]);
        OLED_SetPos(x,y+1);
        for(i=0;i<8;i++)
			OLED_send_data(F8X16[c*16+i+8]);
    }
    else 
    {	
        OLED_SetPos(x,y);
        for(i=0;i<6;i++)
			OLED_send_data(F6x8[c][i]);
    }
}
//m^n����
uint16_t oled_pow(unsigned char m,unsigned char n)
{
	u16 result=1;	 
	while(n--)result*=m;    
	return result;
}	
/**********************************************************************************************************
*	�� �� ��: OLED_ShowChar
*	����˵��: ��ָ��λ����ʾһ���ַ�,���������ַ������ֿ��е��ַ���
*	��    ��: 1. x,y��	��ʼ��ʾ����	x:0~127  y:0~63
			  2. num:	��ֵ(0~4294967295)
			  3. len:	���ֵ�λ��   
			  3. TextSize���ַ���С��1:6*8  2:8*16��
*	�� �� ֵ: ��
* 	ע    �⣺x�����ֺ���Ϊ����������
			  y=0,2,4,6��TextSize=2��  y=0,1,2,3,4,5,6,7��TextSize=1��
**********************************************************************************************************/	 		  
void OLED_ShowNum(unsigned char x,unsigned char y,uint16_t num,unsigned char len,unsigned char size2)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(size2 == 1) OLED_ShowChar(x+(size2/2+2)*t,y,' ',size2);//�˾�Ϊ�޸ľ�,�����������ȴ���1�ص�����				@�����ٳ�	
				else if(size2 == 2) OLED_ShowChar(x+(size2/2)*t,y,' ',size2);//�˾�Ϊ�޸ľ�,�����������ȴ���1�ص����� 		@�����ٳ�
				continue;
			}else enshow=1; 
		 	 
		}
		if(size2 == 1) 	OLED_ShowChar(x+(size2/2+2)*t,y,temp+'0',size2);//�˾�Ϊ�޸ľ�,�����������ȴ���1�ص�����		 		@�����ٳ�	
		else if(size2 == 2) 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2);//�˾�Ϊ�޸ľ�,�����������ȴ���1�ص�����		@�����ٳ�
		
	}
} 
/**********************************************************************************************************
*	�� �� ��: OLED_ShowFloat
*	����˵��: ��ʾ������
*	��    ��:  x,y����ʼ��ʾ����		//y=0,2,4,6��TextSize=2��  y=0,1,2,3,4,5,6,7��TextSize=1��
			double dat��Ҫ��ʾ�ĸ�����
			uint8_t num��С����ǰ��λ��
			uint8_t pointnum��С������λ��
			TextSize���ַ���С��1:6*8  2:8*16��
*	�� �� ֵ: ��
**********************************************************************************************************/
void OLED_ShowFloat(uint8_t x,uint8_t y,double dat,uint8_t num,uint8_t pointnum,uint8_t size)
{
	uint32_t  length;
	int8_t    buff[34];
	int8_t   start,end,point;

	if(6<pointnum)  pointnum = 6;
    if(10<num)      num = 10;
        
    if(0>dat)   length = (uint32_t)sprintf(&buff[0],"%f",dat);//����
    else
    {
        length = (uint32_t)sprintf( &buff[1],"%f",dat);
        length++;
    }
    point = (int8_t)(length - 7);         //����С����λ��
    start = point - num - 1;    //������ʼλ
    end = point + pointnum + 1; //�������λ
    while(0>start)//����λ����  ĩβӦ�����ո�
    {
        buff[end] = ' ';
        end++;
        start++;
    }
    
    if(0>dat)   buff[start] = '-';
    else        buff[start] = ' ';
    
    buff[end] = '\0';
    
	OLED_ShowStr(x,y,buff,size);//��ʾ����
}
/**********************************************************************************************************
*	�� �� ��: OLED_ShowStr
*	����˵��: ��ʾ�ַ��������ֿ��е��ַ���
*	��    ��: 1. x,y����ʼ��ʾ����		//y=0,2,4,6��TextSize=2��  y=0,1,2,3,4,5,6,7��TextSize=1��
			2. ch[]���ַ���    
			3. TextSize���ַ���С��1:6*8  2:8*16��
*	�� �� ֵ: ��
**********************************************************************************************************/
void OLED_ShowStr(unsigned char x,unsigned char y,unsigned char *chr,unsigned char Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
		OLED_ShowChar(x,y,chr[j],Char_Size);
		x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
//ͼƬ���
void Column_set(unsigned char column)
{
	OLED_send_cmd(0x10|(column>>4));    //�����е�ַ��λ
	OLED_send_cmd(0x00|(column&0x0f));  //�����е�ַ��λ   
}
void Page_set(unsigned char page)
{
	OLED_send_cmd(0xb0+page);
}
/**********************************************************************************************************
*	�� �� ��: OLED_clear
*	����˵��: ��������OLED����ʾ
*	��    ��: ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void OLED_clear(void)
{
	unsigned char page,column;
	for(page=0;page<8;page++)             //page loop
	{ 
		Page_set(page);
		Column_set(0);	
		//����һ��IIC֮��ѭ���귢�͵������ٽ����������Ż��ٶ�
		IIC_Start();
		IIC_Send_Byte(0x78);			//D/C#=0; R/W#=0
		IIC_Wait_Ack();
		IIC_Send_Byte(0x40);			//write data
		IIC_Wait_Ack();  
		for(column=0;column<OLED_X_MAX;column++)	//column loop
		{
			// OLED_send_data(0x00);
			IIC_Send_Byte(0X00);	
			IIC_Wait_Ack();
		}
		IIC_Stop();
	}
}

/**********************************************************************************************************
*	�� �� ��: OLED_full
*	����˵��: �������OLED����ʾ
*	��    ��: ��
*	�� �� ֵ: ��
**********************************************************************************************************/
void OLED_full(void)
{
	unsigned char page,column;
	for(page=0;page<8;page++)             //page loop
	{ 
		Page_set(page);
		Column_set(0);
		//����һ��IIC֮��ѭ���귢�͵������ٽ����������Ż��ٶ�
		IIC_Start();
		IIC_Send_Byte(0x78);			//D/C#=0; R/W#=0
		IIC_Wait_Ack();
		IIC_Send_Byte(0x40);			//write data
		IIC_Wait_Ack();  
		for(column=0;column<OLED_X_MAX;column++)	//column loop
		{
			// OLED_send_data(0xFF);
			IIC_Send_Byte(0XFF);	
			IIC_Wait_Ack();
		}
		IIC_Stop();
	}
}
/***************************************************************
 * 	��  ��:��ʾBMPͼƬ128��64
 *  ��  ����(x0,y0)ͼƬ��ʼ����
 * 	       (x1,y1)ͼƬ��������
 * 		   BMP[]��BMPͼ��
 * 	˵  ������ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
 * 			y1-y2=����BMP�����Ԫ������/x�Ŀ��
 * **************************************************************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;
	
	if(y1%8==0) y=y1/8;      
	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
		for(x=x0;x<x1;x++)
		{      
			OLED_send_data(BMP[j++]);	    	
		}
	}
} 
//-----------------------------------����ˢ�»�ͼ����---------------- 
//��Դ��https://www.bilibili.com/video/BV1wG411H7w7?spm_id_from=333.999.0.0&vd_source=9c46cef251d6f0cdd377d7025bba4c52
void OLED_DrawBMP_Fast(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
	unsigned int j=0;
	unsigned char x,y;

	if(y1%8==0) y=y1/8;      
	else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
		IIC_Start();
		IIC_Send_Byte(0x78);			//D/C#=0; R/W#=0
		IIC_Wait_Ack();
		IIC_Send_Byte(0x40);			//write data
		IIC_Wait_Ack();
		for(x=x0;x<x1;x++)
		{  
			IIC_Send_Byte(BMP[j++]);	
			IIC_Wait_Ack();
		}
		IIC_Stop();
	}
}
