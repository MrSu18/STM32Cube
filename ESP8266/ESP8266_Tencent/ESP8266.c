#include "ESP8266.h"
#include <string.h>
#include "SEEKFREE_18TFT.h"

//wifi信息
#define WIFI_NAME "\"Redmi\""//WIFI名称
#define WIFI_KEY  "\"liu20021217\""//WIFI密码
//腾讯云mqtt信息
#define DEVICE_NAME "\"esp8266\""//设备名称
#define DEVICE_KEY  "\"6YSO7Vt/UhmXBzAcW4iWmA==\""//设备密钥
#define PRODUCT_ID  "\"0QGB3SJYYS\""//产品ID
//主题
#define TOPIC   "\"$thing/down/property/0QGB3SJYYS/esp8266\""

/****************************************************************************
 * 函数功能：ESP连接腾讯云
 * 作   者：LJF
 * 注   意：若ESP连接过一次手机热点之后不进行修改连接腾讯云时尽量把连接热点的AT指令注释
 ****************************************************************************/
void ESPContectTCCloud(void)//ESP8266连接腾讯云函数
{
    //连接手机热点
//    printf("AT+CWMODE=1\r\n");
//    systick_delay_ms(STM0,800);
//    printf("AT+CWJAP=%s,%s",WIFI_NAME,WIFI_KEY);
//    systick_delay_ms(STM0,5000);
    //连接腾讯云
    printf("AT+TCDEVINFOSET=1,%s,%s,%s\r\n",PRODUCT_ID,DEVICE_NAME,DEVICE_KEY);
    systick_delay_ms(STM0,1000);
    printf("AT+TCMQTTCONN=1,5000,240,1,1\r\n");
    systick_delay_ms(STM0,2000);
    printf("AT+TCMQTTSUB=%s,1\r\n",TOPIC);
    systick_delay_ms(STM0,2000);
    printf("ATE0\r\n");//关闭esp8266回显
    systick_delay_ms(STM0,1000);
    //初始化消息队列
    memset(esp_rx_queue.RxBuffer,0,sizeof(esp_rx_queue.RxBuffer));
    esp_rx_queue.front=0;
    esp_rx_queue.rear=0;
    esp_rx_queue.length=0;
}

/****************************************************************************
 * 函数功能：串口中断回调函数，在此函数接收原始数据
 * 作   者：LJF
 ****************************************************************************/
Usart_Aqueue esp_rx_queue;//接收消息队列
uint8 aRxBuffer=0;            //接收中断缓冲
char one_frame_finish_flag=0;//一帧通讯结束的标志变量1：结束可以处理字符串0：未结束
void ESPUartCallback(void)//串口处理数据的回调函数
{
    static uint8 special_char_cnt=0;
    // 读取无线串口的数据 并且置位接收标志
    if(uart_query(UART_2, &aRxBuffer)==1 && one_frame_finish_flag==0)
    {
        //串口信息进入消息队列
        if((esp_rx_queue.rear+1)%MAX_QUEUE_LENGTH==esp_rx_queue.front)//判断是否队满
        {
            esp_rx_queue.front=(esp_rx_queue.front+1)%MAX_QUEUE_LENGTH;//队头加一
        }
        else
        {
            esp_rx_queue.length++;
        }
        esp_rx_queue.RxBuffer[esp_rx_queue.rear]=aRxBuffer;
        esp_rx_queue.rear=(esp_rx_queue.rear+1)%MAX_QUEUE_LENGTH;
        //下面的判断语句用于判断是否接收完一帧通信帧
        //观察esp下发的字符串若沿用之前的还用“作为一帧结束的标志，由于16个”太容易出现丢失，使得数据丢包，发现了大括号会比较适合，特殊并且出现间隔小
        if(aRxBuffer=='}')
        {
            special_char_cnt++;
            if(special_char_cnt==2)
            {
                special_char_cnt=0;
                one_frame_finish_flag=1;
            }
        }
    }
}
/****************************************************************************
 * 函数功能：处理串口中断接收的一帧数据从中提取出变量信息
 * 作   者：LJF
 * 注   意：得到的变量是字符，例如：bicycle_power '0' or '1' ,写switch的时候要对应字符0和1
 ****************************************************************************/
uint8 bicycle_power=0;//单车开关标志变量
uint8 people_station=0;//人所在的点位变量
void DealESPString(void)//接收一帧之后处理ESP下发字符串函数
{
    if(one_frame_finish_flag==1)
    {
        for (int i = esp_rx_queue.front,j=esp_rx_queue.length-3; j>0; i=(i+1)%MAX_QUEUE_LENGTH,j--)
        {
            //找到bicycle开关
            if (esp_rx_queue.RxBuffer[i]=='b' && esp_rx_queue.RxBuffer[(i+1)%MAX_QUEUE_LENGTH]=='i' && esp_rx_queue.RxBuffer[(i+2)%MAX_QUEUE_LENGTH]=='c')
            {
                for(i=i+2;j>0; i=(i+1)%MAX_QUEUE_LENGTH,j--)
                {
                    if(esp_rx_queue.RxBuffer[i]==':')
                    {
                        bicycle_power=esp_rx_queue.RxBuffer[(i+1)%MAX_QUEUE_LENGTH];//得到单车开关控制变量
                        //接收到才清零
                        memset(esp_rx_queue.RxBuffer,0,sizeof(esp_rx_queue.RxBuffer));
                        esp_rx_queue.front=0;
                        esp_rx_queue.rear=0;
                        esp_rx_queue.length=0;
                        break;
                    }
                }
                break;
            }
            //找到点位设定station
            else if(esp_rx_queue.RxBuffer[i]=='l' && esp_rx_queue.RxBuffer[(i+1)%MAX_QUEUE_LENGTH]=='o' && esp_rx_queue.RxBuffer[(i+2)%MAX_QUEUE_LENGTH]=='c')
            {
                for(i=i+2;j>0; i=(i+1)%MAX_QUEUE_LENGTH,j--)
                {
                    if(esp_rx_queue.RxBuffer[i]==':')
                    {
                        gpio_toggle(P21_4);
                        people_station=esp_rx_queue.RxBuffer[(i+1)%MAX_QUEUE_LENGTH];
                        memset(esp_rx_queue.RxBuffer,0,sizeof(esp_rx_queue.RxBuffer));
                        esp_rx_queue.front=0;
                        esp_rx_queue.rear=0;
                        esp_rx_queue.length=0;
                        break;
                    }
                }
                break;
            }
        }
        //为下一帧开始做准备
        one_frame_finish_flag=0;
    }
}
#define REPORT_TOPIC "\"$thing/up/property/0QGB3SJYYS/esp8266\""
#define JSON_START "\"{\\\"method\\\":\\\"report\\\"\\,\\\"clientToken\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"distance\\\":"
void ESPReportCloud(int distance)//esp上报distance的函数
{
    printf("AT+TCMQTTPUB=%s,0,%s%d}}\"\r\n",REPORT_TOPIC,JSON_START,distance);
}
