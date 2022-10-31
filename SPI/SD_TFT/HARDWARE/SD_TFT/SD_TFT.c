#include "SD_TFT.h"
#include "spi.h"//hspi1
#include "main.h"//CS���ŵĺ궨��
#include "stm32f1xx_hal_spi.h"//SPI��д

//��װSPI�ӻ���ӻ�ͨ�ŵ�ʹ���źţ�1:�� 0:��
#define SPI_CS(N) HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, N==1?GPIO_PIN_SET:GPIO_PIN_RESET)

uint8_t SD_Type=0;//SD��������
uint8_t SD_Send_Buffer[512]={0};
uint8_t SD_Receive_Buffer[512]={0};

/***********************************************
* @brief : SPI��дһ���ֽ�
* @param : tx_data:���͵����� 
* @return: ��ȡ�����ֽ�����
* @date  : 2022.10.28
* @author: ������
************************************************/
uint8_t SPI_ReadWriteByte(uint8_t tx_data)
{
    uint8_t receive_buffer;
    HAL_SPI_TransmitReceive(&hspi1,&tx_data,&receive_buffer,1,1);
    return receive_buffer;
}
uint8_t SD_SPI_ReadWriteByte(uint8_t data)
{
    return SPI_ReadWriteByte(data);
}

//�ȴ�sd��׼�����
uint8_t SD_WaitReady(void)
{
    uint32_t t=0;
    do
    {
        if(SD_SPI_ReadWriteByte(0XFF)==0XFF)return 0;//OK
        t++;
    }while(t<0XFFFFF);//�����˵ȴ�ʱ��
    return 1;
}

uint8_t SD_Select(void)
{
    if(SD_WaitReady()==0)return 0;//�ȴ��ɹ�
    return 1;//�ȴ�ʧ��
}

/***********************************************
* @brief : ��SD������һ������
* @param : uint8_t cmd:���� 
*          uint32_t arg  �������
*          uint8_t crc   crcУ��ֵ
* @return: SD�����ص���Ӧ
* @date  : 2022.10.28
* @author: ������
************************************************/
uint8_t SD_SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t r1;
    uint8_t Retry=0;
    if(SD_Select())return 0XFF;//ƬѡʧЧ
    //����
    SD_SPI_ReadWriteByte(cmd | 0x40);//�ֱ�д������
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte((arg >> 16)&0xff);
    SD_SPI_ReadWriteByte((arg >> 8)&0xff);
    SD_SPI_ReadWriteByte((arg)&0xff);
    SD_SPI_ReadWriteByte(crc);
    if(cmd==CMD12)SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
    //�ȴ���Ӧ����ʱ�˳�
    Retry=0x1F;
    do
    {
        r1=SD_SPI_ReadWriteByte(0xFF);
    }while((r1&0x80) && Retry--);
    //����״ֵ̬
    return r1;
}

/***********************************************
* @brief : SD����SPIͨ�ų�ʼ��
* @param : ��
* @return: �Ƿ��ʼ���ɹ�
* @date  : 2022.10.28
* @author: ������
************************************************/
uint8_t SD_Spi_Init(void)
{
    uint8_t r1;      // ���SD���ķ���ֵ
    uint16_t retry;  // �������г�ʱ����
    uint8_t buf[4];
    uint16_t i;
	SPI_SetSpeed(0);//��ʼ�����ٶ�״̬
    for(i=0;i<10;i++)
		SD_SPI_ReadWriteByte(0XFF);//��������74������
    SPI_CS(0);//����Ƭѡ�źű�ʾѡ�дӻ�
    retry=20;
    do
    {
        r1=SD_SendCmd(CMD0,0,0x95);//����IDLEģʽ
    }while((r1!=0X01) && retry--);
    SD_Type=0;//Ĭ���޿�
    if(r1==0X01)
    {
        if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
        {
            for(i=0;i<4;i++)buf[i]=SD_SPI_ReadWriteByte(0XFF);  //Get trailing return value of R7 resp
            if(buf[2]==0X01&&buf[3]==0XAA)//���Ƿ�֧��2.7~3.6V
            {
                retry=0XFFFE;
                do
                {
                    SD_SendCmd(CMD55,0,0X01);   //����CMD55
                    r1=SD_SendCmd(CMD41,0x40000000,0X01);//����CMD41
                }while(r1&&retry--);
                if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//����SD2.0���汾��ʼ
                {
                    for(i=0;i<4;i++)
						buf[i]=SD_SPI_ReadWriteByte(0XFF);//�õ�OCRֵ
                    if(buf[0]&0x40)
						SD_Type=SD_TYPE_V2HC;    //���CCS
                    else 
						SD_Type=SD_TYPE_V2;
                }
            }
        }
		else//SD V1.x/ MMC V3
        {
            SD_SendCmd(CMD55,0,0X01);       //����CMD55
            r1=SD_SendCmd(CMD41,0,0X01);    //����CMD41
            if(r1<=1)
            {
                SD_Type=SD_TYPE_V1;
                retry=0XFFFE;
                do //�ȴ��˳�IDLEģʽ
                {
                    SD_SendCmd(CMD55,0,0X01);   //����CMD55
                    r1=SD_SendCmd(CMD41,0,0X01);//����CMD41
                }while(r1&&retry--);
            }else//MMC����֧��CMD55+CMD41ʶ��
            {
                SD_Type=SD_TYPE_MMC;//MMC V3
                retry=0XFFFE;
                do //�ȴ��˳�IDLEģʽ
                {
                    r1=SD_SendCmd(CMD1,0,0X01);//����CMD1
                }while(r1&&retry--);
            }
            if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)SD_Type=SD_TYPE_ERR;//����Ŀ�
        }
    }
    SPI_SetSpeed(1);
    if(SD_Type)	return 0;
    else if(r1)	return r1;
    else	    return 255;//��������
}

/***********************************************
* @brief : ����SPIͨ������
* @param : mode 'h'������ 'l'������
* @return: ��
* @date  : 2022.10.28
* @author: ������
************************************************/
void SPI_SetSpeed(uint8_t mode)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
    //����ٶ���������0�������ģʽ����0�����ģʽ
    if(mode=='l')
    {
        hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    }
    else
    {
        hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    }
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
}

//�ȴ�SD����Ӧ
//Response:Ҫ�õ��Ļ�Ӧֵ
//����ֵ:0,�ɹ��õ��˸û�Ӧֵ
//����,�õ���Ӧֵʧ��
uint8_t SD_GetResponse(uint8_t Response)
{
    uint16_t Count=0xFFFF;//�ȴ�����
    while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)Count--;//�ȴ��õ�׼ȷ�Ļ�Ӧ
    if (Count==0)return MSD_RESPONSE_FAILURE;//�õ���Ӧʧ��
    else return MSD_RESPONSE_NO_ERROR;//��ȷ��Ӧ
}

//��sd����ȡһ�����ݰ�������
//buf:���ݻ�����
//len:Ҫ��ȡ�����ݳ���.
//����ֵ:0,�ɹ�;����,ʧ��;
uint8_t SD_RecvData(uint8_t*buf,uint16_t len)
{
    if(SD_GetResponse(0xFE))return 1;//�ȴ�SD������������ʼ����0xFE
    while(len--)//��ʼ��������
    {
        *buf=SPI_ReadWriteByte(0xFF);
        buf++;
    }
    //������2��αCRC��dummy CRC��
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);
    return 0;//��ȡ�ɹ�
}

//��sd��д��һ�����ݰ������� 512�ֽ�
//buf:���ݻ�����
//cmd:ָ��
//����ֵ:0,�ɹ�;����,ʧ��;
uint8_t SD_SendBlock(uint8_t*buf,uint8_t cmd)
{
    uint16_t t;
    if(SD_WaitReady())return 1;//�ȴ�׼��ʧЧ
    SD_SPI_ReadWriteByte(cmd);
    if(cmd!=0XFD)//���ǽ���ָ��
    {
        for(t=0;t<512;t++)SPI_ReadWriteByte(buf[t]);//����ٶ�,���ٺ�������ʱ��
        SD_SPI_ReadWriteByte(0xFF);//����crc
        SD_SPI_ReadWriteByte(0xFF);
        t=SD_SPI_ReadWriteByte(0xFF);//������Ӧ
        if((t&0x1F)!=0x05)return 2;//��Ӧ����
    }
    return 0;//д��ɹ�
}

//��ȡSD����CID��Ϣ��������������Ϣ
//����: uint8_t *cid_data(���CID���ڴ棬����16Byte��
//����ֵ:0��NO_ERR
//       1������
uint8_t SD_GetCID(uint8_t *cid_data)
{
  uint8_t r1;
  //��CMD10�����CID
  r1=SD_SendCmd(CMD10,0,0x01);
  if(r1==0x00)
    {
        r1=SD_RecvData(cid_data,16);//����16���ֽڵ�����
  }
  if(r1)return 1;
  else return 0;
}

//��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
//����:uint8_t *cid_data(���CID���ڴ棬����16Byte��
//����ֵ:0��NO_ERR
//       1������
uint8_t SD_GetCSD(uint8_t *csd_data)
{
  uint8_t r1;
  r1=SD_SendCmd(CMD9,0,0x01);//��CMD9�����CSD
  if(r1==0x00)
    {
        r1=SD_RecvData(csd_data, 16);//����16���ֽڵ�����
  }
    if(r1)return 1;
    else return 0;
}

//��ȡSD����������������������
//����ֵ:0�� ȡ��������
//       ����:SD��������(������/512�ֽ�)
//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.
uint32_t SD_GetSectorCount(void)
{
    uint8_t csd[16];
    uint32_t Capacity;
    uint8_t n;
    uint16_t csize;
    //ȡCSD��Ϣ������ڼ��������0
    if(SD_GetCSD(csd)!=0) return 0;

    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)  //V2.00�Ŀ�
    {
        csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
        Capacity = (uint32_t)csize << 10;//�õ�������
    }else//V1.XX�Ŀ�
    {
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((uint16_t)csd[7] << 2) + ((uint16_t)(csd[6] & 3) << 10) + 1;
        Capacity= (uint32_t)csize << (n - 9);//�õ�������
    }
    return Capacity;
}

//��SD��
//buf:���ݻ�����
//sector:����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
uint8_t SD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
    uint8_t r1;
    if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;//ת��Ϊ�ֽڵ�ַ
    if(cnt==1)
    {
        r1=SD_SendCmd(CMD17,sector,0X01);//������
        if(r1==0)//ָ��ͳɹ�
        {
            r1=SD_RecvData(buf,512);//����512���ֽ�
        }
    }else
    {
        r1=SD_SendCmd(CMD18,sector,0X01);//����������
        do
        {
            r1=SD_RecvData(buf,512);//����512���ֽ�
            buf+=512;
        }while(--cnt && r1==0);
        SD_SendCmd(CMD12,0,0X01);   //����ֹͣ����
    }
    return r1;//
}

//дSD��
//buf:���ݻ�����
//sector:��ʼ����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
uint8_t SD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
    uint8_t r1;
    if(SD_Type!=SD_TYPE_V2HC)sector *= 512;//ת��Ϊ�ֽڵ�ַ
    if(cnt==1)
    {
        r1=SD_SendCmd(CMD24,sector,0X01);//������
        if(r1==0)//ָ��ͳɹ�
        {
            r1=SD_SendBlock(buf,0xFE);//д512���ֽ�
        }
    }
	else
    {
        if(SD_Type!=SD_TYPE_MMC)
        {
            SD_SendCmd(CMD55,0,0X01);
            SD_SendCmd(CMD23,cnt,0X01);//����ָ��
        }
        r1=SD_SendCmd(CMD25,sector,0X01);//����������
        if(r1==0)
        {
            do
            {
                r1=SD_SendBlock(buf,0xFC);//����512���ֽ�
                buf+=512;
            }while(--cnt && r1==0);
            r1=SD_SendBlock(0,0xFD);//����512���ֽ�
        }
    }
    return r1;//
}
