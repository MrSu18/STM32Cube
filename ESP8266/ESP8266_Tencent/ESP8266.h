#include "headfile.h"

#define MAX_QUEUE_LENGTH 215

typedef struct Usart_Aqueue
{
    char RxBuffer[MAX_QUEUE_LENGTH];//������
    int front;//��ͷ
    int rear;//��β
    int length;//���г���
}Usart_Aqueue;

extern Usart_Aqueue esp_rx_queue;//������Ϣ����
extern char one_frame_finish_flag;//һ֡ͨѶ�����ı�־����1���������Դ����ַ���0��δ����
extern uint8 bicycle_power;//�������ر�־����
extern uint8 people_station;//�����ڵĵ�λ����

void ESPContectTCCloud(void);//ESP8266������Ѷ�ƺ���
void ESPUartCallback(void);//���ڴ������ݵĻص�����
void DealESPString(void);//����һ֮֡����ESP�·��ַ�������
void ESPReportCloud(int distance);//esp�ϱ�distance�ĺ���
