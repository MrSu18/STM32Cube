/*
 * SDSPI.c
 *
 *  Created on: 2022年3月12日
 *      Author: Interstellar
 */

#include "SDSPI.h"
#include "spi.h"//HAL库的SPI

//定义使用SPI

uint8_t sd_type=0;//SD卡的类型

uint8_t sd_send_buffer[512]={0};
uint8_t sd_receive_buffer[512]={0};


//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPI_ReadWriteByte(uint8_t TxData)
{
    uint8_t receive_buffer;
    HAL_SPI_TransmitReceive(&hspi1, &TxData, &receive_buffer,1,1);
    //spi_mosi (SD_SPI,SD_SPI_CS,&TxData,&receive_buffer,1,1);
    return receive_buffer;
}
//SD卡指令操作部分

//函数再封装,SPI读写操作
uint8_t SD_SPI_ReadWriteByte(uint8_t data)
{
    return SPI_ReadWriteByte(data);
}

//等待sd卡准备完成
uint8_t SD_WaitReady(void)
{
    uint32_t t=0;
    do
    {
        if(SD_SPI_ReadWriteByte(0XFF)==0XFF)return 0;//OK
        t++;
    }while(t<0XFFFFF);//减少了等待时间
    return 1;
}

uint8_t SD_Select(void)
{
    if(SD_WaitReady()==0)return 0;//等待成功
    return 1;//等待失败
}

//等待SD卡回应
//Response:要得到的回应值
//返回值:0,成功得到了该回应值
//其他,得到回应值失败
uint8_t SD_GetResponse(uint8_t Response)
{
    uint16_t Count=0xFFFF;//等待次数
    while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)Count--;//等待得到准确的回应
    if (Count==0)return MSD_RESPONSE_FAILURE;//得到回应失败
    else return MSD_RESPONSE_NO_ERROR;//正确回应
}

//从sd卡读取一个数据包的内容
//buf:数据缓存区
//len:要读取的数据长度.
//返回值:0,成功;其他,失败;
uint8_t SD_RecvData(uint8_t*buf,uint16_t len)
{
    if(SD_GetResponse(0xFE))return 1;//等待SD卡发回数据起始令牌0xFE
    while(len--)//开始接收数据
    {
        *buf=SPI_ReadWriteByte(0xFF);
        buf++;
    }
    //下面是2个伪CRC（dummy CRC）
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);
    return 0;//读取成功
}

//向sd卡写入一个数据包的内容 512字节
//buf:数据缓存区
//cmd:指令
//返回值:0,成功;其他,失败;
uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
{
    uint16_t t;
    if(SD_WaitReady())return 1;//等待准备失效
    SD_SPI_ReadWriteByte(cmd);
    if(cmd!=0XFD)//不是结束指令
    {
        for(t=0;t<512;t++)SPI_ReadWriteByte(buf[t]);//提高速度,减少函数传参时间
        SD_SPI_ReadWriteByte(0xFF);//忽略crc
        SD_SPI_ReadWriteByte(0xFF);
        t=SD_SPI_ReadWriteByte(0xFF);//接收响应
        if((t&0x1F)!=0x05)return 2;//响应错误
    }
    return 0;//写入成功
}

//向SD卡发送一个命令
//输入: uint8_t cmd   命令
//      uint32_t arg  命令参数
//      uint8_t crc   crc校验值
//返回值:SD卡返回的响应
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
  uint8_t r1;
    uint8_t Retry=0;
    if(SD_Select())return 0XFF;//片选失效
    //发送
    SD_SPI_ReadWriteByte(cmd | 0x40);//分别写入命令
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte((arg >> 16)&0xff);
    SD_SPI_ReadWriteByte((arg >> 8)&0xff);
    SD_SPI_ReadWriteByte((arg)&0xff);
    SD_SPI_ReadWriteByte(crc);
    if(cmd==CMD12)SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
    //等待响应，或超时退出
    Retry=0x1F;
    do
    {
        r1=SD_SPI_ReadWriteByte(0xFF);
    }while((r1&0x80) && Retry--);
    //返回状态值
    return r1;
}

uint8_t SD_Spi_Init(void)
{
    uint8_t r1;      // 存放SD卡的返回值
    uint16_t retry;  // 用来进行超时计数
    uint8_t buf[4];
    uint16_t i;
    
    //spi_init(SD_SPI,SD_SPI_SCLK,SD_SPI_MOSI,SD_SPI_MISO,SD_SPI_CS,3,300*1000);//初始化慢速度状态

    for(i=0;i<10;i++)SD_SPI_ReadWriteByte(0XFF);//发送最少74个脉冲
    retry=20;

    do
    {
      r1=SD_SendCmd(CMD0,0,0x95);//进入IDLE模式
    }while((r1!=0X01) && retry--);
    sd_type=0;//默认无卡
    if(r1==0X01)
    {
        if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
        {
            for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);  //Get trailing return value of R7 resp
            if(buf[2]==0X01&&buf[3]==0XAA)//卡是否支持2.7~3.6V
            {
                retry=0XFFFE;
                do
                {
                    SD_SendCmd(CMD55,0,0X01);   //发送CMD55
                    r1=SD_SendCmd(CMD41,0x40000000,0X01);//发送CMD41
                }while(r1&&retry--);
                if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//鉴别SD2.0卡版本开始
                {
                    for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);//得到OCR值
                    if(buf[0]&0x40)sd_type=SD_TYPE_V2HC;    //检查CCS
                    else sd_type=SD_TYPE_V2;
                }
            }
        }else//SD V1.x/ MMC V3
        {
            SD_SendCmd(CMD55,0,0X01);       //发送CMD55
            r1=SD_SendCmd(CMD41,0,0X01);    //发送CMD41
            if(r1<=1)
            {
                sd_type=SD_TYPE_V1;
                retry=0XFFFE;
                do //等待退出IDLE模式
                {
                    SD_SendCmd(CMD55,0,0X01);   //发送CMD55
                    r1=SD_SendCmd(CMD41,0,0X01);//发送CMD41
                }while(r1&&retry--);
            }else//MMC卡不支持CMD55+CMD41识别
            {
                sd_type=SD_TYPE_MMC;//MMC V3
                retry=0XFFFE;
                do //等待退出IDLE模式
                {
                    r1=SD_SendCmd(CMD1,0,0X01);//发送CMD1
                }while(r1&&retry--);
            }
            if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)sd_type=SD_TYPE_ERR;//错误的卡
        }
    }
    spi_init(SD_SPI,SD_SPI_SCLK,SD_SPI_MOSI,SD_SPI_MISO,SD_SPI_CS,3,20*1000*1000);//高速
    if(sd_type)return 0;
    else if(r1)return r1;
    return 255;//其他错误
}

//获取SD卡的CID信息，包括制造商信息
//输入: uint8_t *cid_data(存放CID的内存，至少16Byte）
//返回值:0：NO_ERR
//       1：错误
uint8_t SD_GetCID(uint8_t *cid_data)
{
  uint8_t r1;
  //发CMD10命令，读CID
  r1=SD_SendCmd(CMD10,0,0x01);
  if(r1==0x00)
    {
        r1=SD_RecvData(cid_data,16);//接收16个字节的数据
  }
  if(r1)return 1;
  else return 0;
}

//获取SD卡的CSD信息，包括容量和速度信息
//输入:uint8_t *cid_data(存放CID的内存，至少16Byte）
//返回值:0：NO_ERR
//       1：错误
uint8_t SD_GetCSD(uint8_t *csd_data)
{
  uint8_t r1;
  r1=SD_SendCmd(CMD9,0,0x01);//发CMD9命令，读CSD
  if(r1==0x00)
    {
        r1=SD_RecvData(csd_data, 16);//接收16个字节的数据
  }
    if(r1)return 1;
    else return 0;
}

//获取SD卡的总扇区数（扇区数）
//返回值:0： 取容量出错
//       其他:SD卡的容量(扇区数/512字节)
//每扇区的字节数必为512，因为如果不是512，则初始化不能通过.
uint32_t SD_GetSectorCount(void)
{
    uint8_t csd[16];
    uint32_t Capacity;
    uint8_t n;
    uint16_t csize;
    //取CSD信息，如果期间出错，返回0
    if(SD_GetCSD(csd)!=0) return 0;

    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)  //V2.00的卡
    {
        csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
        Capacity = (uint32_t)csize << 10;//得到扇区数
    }else//V1.XX的卡
    {
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
        Capacity= (uint32_t)csize << (n - 9);//得到扇区数
    }
    return Capacity;
}

//读SD卡
//buf:数据缓存区
//sector:扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
    uint8_t r1;
    if(sd_type!=SD_TYPE_V2HC)sector <<= 9;//转换为字节地址
    if(cnt==1)
    {
        r1=SD_SendCmd(CMD17,sector,0X01);//读命令
        if(r1==0)//指令发送成功
        {
            r1=SD_RecvData(buf,512);//接收512个字节
        }
    }else
    {
        r1=SD_SendCmd(CMD18,sector,0X01);//连续读命令
        do
        {
            r1=SD_RecvData(buf,512);//接收512个字节
            buf+=512;
        }while(--cnt && r1==0);
        SD_SendCmd(CMD12,0,0X01);   //发送停止命令
    }
//    SD_DisSelect();//取消片选
    return r1;//
}

//写SD卡
//buf:数据缓存区
//sector:起始扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
    uint8_t r1;
    if(sd_type!=SD_TYPE_V2HC)sector *= 512;//转换为字节地址
    if(cnt==1)
    {
        r1=SD_SendCmd(CMD24,sector,0X01);//读命令
        if(r1==0)//指令发送成功
        {
            r1=SD_SendBlock(buf,0xFE);//写512个字节
        }
    }else
    {
        if(sd_type!=SD_TYPE_MMC)
        {
            SD_SendCmd(CMD55,0,0X01);
            SD_SendCmd(CMD23,cnt,0X01);//发送指令
        }
        r1=SD_SendCmd(CMD25,sector,0X01);//连续读命令
        if(r1==0)
        {
            do
            {
                r1=SD_SendBlock(buf,0xFC);//接收512个字节
                buf+=512;
            }while(--cnt && r1==0);
            r1=SD_SendBlock(0,0xFD);//接收512个字节
        }
    }
    return r1;//
}

