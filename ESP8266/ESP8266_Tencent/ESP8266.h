#include "headfile.h"

#define MAX_QUEUE_LENGTH 215

typedef struct Usart_Aqueue
{
    char RxBuffer[MAX_QUEUE_LENGTH];//数据域
    int front;//队头
    int rear;//队尾
    int length;//队列长度
}Usart_Aqueue;

extern Usart_Aqueue esp_rx_queue;//接收消息队列
extern char one_frame_finish_flag;//一帧通讯结束的标志变量1：结束可以处理字符串0：未结束
extern uint8 bicycle_power;//单车开关标志变量
extern uint8 people_station;//人所在的点位变量

void ESPContectTCCloud(void);//ESP8266连接腾讯云函数
void ESPUartCallback(void);//串口处理数据的回调函数
void DealESPString(void);//接收一帧之后处理ESP下发字符串函数
void ESPReportCloud(int distance);//esp上报distance的函数
