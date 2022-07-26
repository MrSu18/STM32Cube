#include "oled.h"
#include "myiic.h"
#include "delay.h"
#include "oled_font.h"
#include <stdio.h>//sprinft函数

const unsigned char OLED_init_cmd[25]=
{
    /*0xae,0X00,0X10,0x40,0X81,0XCF,0xff,0xa1,0xa4,
    0xA6,0xc8,0xa8,0x3F,0xd5,0x80,0xd3,0x00,0XDA,0X12,
    0x8d,0x14,0xdb,0x40,0X20,0X02,0xd9,0xf1,0xAF*/
    0xAE,//关闭显示
    0xD5,//设置时钟分频因子,震荡频率
    0x80,  //[3:0],分频因子;[7:4],震荡频率

    0xA8,//设置驱动路数
    0X3F,//默认0X3F(1/64)
    0xD3,//设置显示偏移
    0X00,//默认为0
    0x40,//设置显示开始行 [5:0],行数.                              
    0x8D,//电荷泵设置
    0x14,//bit2，开启/关闭
    0x20,//设置内存地址模式
    0x02,//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
    0xA1,//段重定义设置,bit0:0,0->0;1,0->127;
    0xC8,//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
    0xDA,//设置COM硬件引脚配置
    0x12,//[5:4]配置            
    0x81,//对比度设置
    Brightness,//1~255;默认0X7F (亮度设置,越大越亮)
    0xD9,//设置预充电周期
    0xf1,//[3:0],PHASE 1;[7:4],PHASE 2;
    0xDB,//设置VCOMH 电压倍率
    0x30,//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;
    0xA4,//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
    0xA6,//设置显示方式;bit0:1,反相显示;0,正常显示        
    0xAF,//开启显示     
};

//写命令
void OLED_send_cmd(unsigned char o_command)
{
	IIC_Start();
    IIC_Send_Byte(0x78);//OLED地址
    IIC_Wait_Ack();
    IIC_Send_Byte(0x00);//寄存器地址
    IIC_Wait_Ack();
    IIC_Send_Byte(o_command);
    IIC_Wait_Ack();
    IIC_Stop();
}
//写数据
void OLED_send_data(unsigned char o_data)
{ 
	IIC_Start();
    IIC_Send_Byte(0x78);//OLED地址
    IIC_Wait_Ack();
    IIC_Send_Byte(0x40);//寄存器地址
    IIC_Wait_Ack();
    IIC_Send_Byte(o_data);
    IIC_Wait_Ack();
    IIC_Stop();
}
/**********************************************************************************************************
*	函 数 名: OLED_init
*	功能说明: 初始化OLED
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void OLED_init(void)
{
	unsigned char i;
	IIC_Init();		//初始化IIC
	for(i=0;i<25;i++)
	{
		OLED_send_cmd(OLED_init_cmd[i]);
	}
	OLED_clear();
}

//显示数据相关
//设置光标位置
void OLED_SetPos(unsigned char x, unsigned char y)
{ 
    OLED_send_cmd(0xb0+y);
    OLED_send_cmd(((x&0xf0)>>4)|0x10);
    OLED_send_cmd((x&0x0f)|0x01);
}
/**********************************************************************************************************
*	函 数 名: OLED_ShowChar
*	功能说明: 在指定位置显示一个字符,包括部分字符（在字库中的字符）
*	形    参: 1. x,y：起始显示坐标	x:0~127  y:0~63
			  2. ch[]：字符串    
			  3. TextSize：字符大小（1:6*8  2:8*16）
*	返 回 值: 无
* 	注    意：x按照字号作为单个距离间隔
			  y=0,2,4,6（TextSize=2）  y=0,1,2,3,4,5,6,7（TextSize=1）
**********************************************************************************************************/
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char chr,unsigned char Char_Size)
{      	
	unsigned char c=0,i=0;	
    c=chr-' ';//得到偏移后的值			
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
//m^n函数
uint16_t oled_pow(unsigned char m,unsigned char n)
{
	u16 result=1;	 
	while(n--)result*=m;    
	return result;
}	
/**********************************************************************************************************
*	函 数 名: OLED_ShowChar
*	功能说明: 在指定位置显示一个字符,包括部分字符（在字库中的字符）
*	形    参: 1. x,y：	起始显示坐标	x:0~127  y:0~63
			  2. num:	数值(0~4294967295)
			  3. len:	数字的位数   
			  3. TextSize：字符大小（1:6*8  2:8*16）
*	返 回 值: 无
* 	注    意：x按照字号作为单个距离间隔
			  y=0,2,4,6（TextSize=2）  y=0,1,2,3,4,5,6,7（TextSize=1）
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
				if(size2 == 1) OLED_ShowChar(x+(size2/2+2)*t,y,' ',size2);//此句为修改句,消除变量长度大于1重叠问题				@布丁橘长	
				else if(size2 == 2) OLED_ShowChar(x+(size2/2)*t,y,' ',size2);//此句为修改句,消除变量长度大于1重叠问题 		@布丁橘长
				continue;
			}else enshow=1; 
		 	 
		}
		if(size2 == 1) 	OLED_ShowChar(x+(size2/2+2)*t,y,temp+'0',size2);//此句为修改句,消除变量长度大于1重叠问题		 		@布丁橘长	
		else if(size2 == 2) 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2);//此句为修改句,消除变量长度大于1重叠问题		@布丁橘长
		
	}
} 
/**********************************************************************************************************
*	函 数 名: OLED_ShowFloat
*	功能说明: 显示浮点数
*	形    参:  x,y：起始显示坐标		//y=0,2,4,6（TextSize=2）  y=0,1,2,3,4,5,6,7（TextSize=1）
			double dat：要显示的浮点数
			uint8_t num：小数点前的位数
			uint8_t pointnum：小数点后的位数
			TextSize：字符大小（1:6*8  2:8*16）
*	返 回 值: 无
**********************************************************************************************************/
void OLED_ShowFloat(uint8_t x,uint8_t y,double dat,uint8_t num,uint8_t pointnum,uint8_t size)
{
	uint32_t  length;
	int8_t    buff[34];
	int8_t   start,end,point;

	if(6<pointnum)  pointnum = 6;
    if(10<num)      num = 10;
        
    if(0>dat)   length = (uint32_t)sprintf(&buff[0],"%f",dat);//负数
    else
    {
        length = (uint32_t)sprintf( &buff[1],"%f",dat);
        length++;
    }
    point = (int8_t)(length - 7);         //计算小数点位置
    start = point - num - 1;    //计算起始位
    end = point + pointnum + 1; //计算结束位
    while(0>start)//整数位不够  末尾应该填充空格
    {
        buff[end] = ' ';
        end++;
        start++;
    }
    
    if(0>dat)   buff[start] = '-';
    else        buff[start] = ' ';
    
    buff[end] = '\0';
    
	OLED_ShowStr(x,y,buff,size);//显示数字
}
/**********************************************************************************************************
*	函 数 名: OLED_ShowStr
*	功能说明: 显示字符串（在字库中的字符）
*	形    参: 1. x,y：起始显示坐标		//y=0,2,4,6（TextSize=2）  y=0,1,2,3,4,5,6,7（TextSize=1）
			2. ch[]：字符串    
			3. TextSize：字符大小（1:6*8  2:8*16）
*	返 回 值: 无
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
//图片相关
void Column_set(unsigned char column)
{
	OLED_send_cmd(0x10|(column>>4));    //设置列地址高位
	OLED_send_cmd(0x00|(column&0x0f));  //设置列地址低位   
}
void Page_set(unsigned char page)
{
	OLED_send_cmd(0xb0+page);
}
/**********************************************************************************************************
*	函 数 名: OLED_clear
*	功能说明: 清屏整个OLED的显示
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void OLED_clear(void)
{
	unsigned char page,column;
	for(page=0;page<8;page++)             //page loop
	{ 
		Page_set(page);
		Column_set(0);	
		//开启一次IIC之后循环完发送的数据再结束，可以优化速度
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
*	函 数 名: OLED_full
*	功能说明: 填充整个OLED的显示
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
void OLED_full(void)
{
	unsigned char page,column;
	for(page=0;page<8;page++)             //page loop
	{ 
		Page_set(page);
		Column_set(0);
		//开启一次IIC之后循环完发送的数据再结束，可以优化速度
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
 * 	功  能:显示BMP图片128×64
 *  参  数：(x0,y0)图片起始坐标
 * 	       (x1,y1)图片结束坐标
 * 		   BMP[]：BMP图像
 * 	说  明：起始点坐标(x,y),x的范围0～127，y为页的范围0～7
 * 			y1-y2=整个BMP数组的元素总数/x的宽度
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
//-----------------------------------快速刷新画图函数---------------- 
//来源：https://www.bilibili.com/video/BV1wG411H7w7?spm_id_from=333.999.0.0&vd_source=9c46cef251d6f0cdd377d7025bba4c52
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
