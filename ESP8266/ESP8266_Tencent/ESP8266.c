#include "ESP8266.h"
#include <string.h>
#include "SEEKFREE_18TFT.h"

//wifi��Ϣ
#define WIFI_NAME "\"Redmi\""//WIFI����
#define WIFI_KEY  "\"liu20021217\""//WIFI����
//��Ѷ��mqtt��Ϣ
#define DEVICE_NAME "\"esp8266\""//�豸����
#define DEVICE_KEY  "\"6YSO7Vt/UhmXBzAcW4iWmA==\""//�豸��Կ
#define PRODUCT_ID  "\"0QGB3SJYYS\""//��ƷID
//����
#define TOPIC   "\"$thing/down/property/0QGB3SJYYS/esp8266\""

/****************************************************************************
 * �������ܣ�ESP������Ѷ��
 * ��   �ߣ�LJF
 * ע   �⣺��ESP���ӹ�һ���ֻ��ȵ�֮�󲻽����޸�������Ѷ��ʱ�����������ȵ��ATָ��ע��
 ****************************************************************************/
void ESPContectTCCloud(void)//ESP8266������Ѷ�ƺ���
{
    //�����ֻ��ȵ�
//    printf("AT+CWMODE=1\r\n");
//    systick_delay_ms(STM0,800);
//    printf("AT+CWJAP=%s,%s",WIFI_NAME,WIFI_KEY);
//    systick_delay_ms(STM0,5000);
    //������Ѷ��
    printf("AT+TCDEVINFOSET=1,%s,%s,%s\r\n",PRODUCT_ID,DEVICE_NAME,DEVICE_KEY);
    systick_delay_ms(STM0,1000);
    printf("AT+TCMQTTCONN=1,5000,240,1,1\r\n");
    systick_delay_ms(STM0,2000);
    printf("AT+TCMQTTSUB=%s,1\r\n",TOPIC);
    systick_delay_ms(STM0,2000);
    printf("ATE0\r\n");//�ر�esp8266����
    systick_delay_ms(STM0,1000);
    //��ʼ����Ϣ����
    memset(esp_rx_queue.RxBuffer,0,sizeof(esp_rx_queue.RxBuffer));
    esp_rx_queue.front=0;
    esp_rx_queue.rear=0;
    esp_rx_queue.length=0;
}

/****************************************************************************
 * �������ܣ������жϻص��������ڴ˺�������ԭʼ����
 * ��   �ߣ�LJF
 ****************************************************************************/
Usart_Aqueue esp_rx_queue;//������Ϣ����
uint8 aRxBuffer=0;            //�����жϻ���
char one_frame_finish_flag=0;//һ֡ͨѶ�����ı�־����1���������Դ����ַ���0��δ����
void ESPUartCallback(void)//���ڴ������ݵĻص�����
{
    static uint8 special_char_cnt=0;
    // ��ȡ���ߴ��ڵ����� ������λ���ձ�־
    if(uart_query(UART_2, &aRxBuffer)==1 && one_frame_finish_flag==0)
    {
        //������Ϣ������Ϣ����
        if((esp_rx_queue.rear+1)%MAX_QUEUE_LENGTH==esp_rx_queue.front)//�ж��Ƿ����
        {
            esp_rx_queue.front=(esp_rx_queue.front+1)%MAX_QUEUE_LENGTH;//��ͷ��һ
        }
        else
        {
            esp_rx_queue.length++;
        }
        esp_rx_queue.RxBuffer[esp_rx_queue.rear]=aRxBuffer;
        esp_rx_queue.rear=(esp_rx_queue.rear+1)%MAX_QUEUE_LENGTH;
        //������ж���������ж��Ƿ������һ֡ͨ��֡
        //�۲�esp�·����ַ���������֮ǰ�Ļ��á���Ϊһ֡�����ı�־������16����̫���׳��ֶ�ʧ��ʹ�����ݶ����������˴����Ż�Ƚ��ʺϣ����Ⲣ�ҳ��ּ��С
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
 * �������ܣ��������жϽ��յ�һ֡���ݴ�����ȡ��������Ϣ
 * ��   �ߣ�LJF
 * ע   �⣺�õ��ı������ַ������磺bicycle_power '0' or '1' ,дswitch��ʱ��Ҫ��Ӧ�ַ�0��1
 ****************************************************************************/
uint8 bicycle_power=0;//�������ر�־����
uint8 people_station=0;//�����ڵĵ�λ����
void DealESPString(void)//����һ֮֡����ESP�·��ַ�������
{
    if(one_frame_finish_flag==1)
    {
        for (int i = esp_rx_queue.front,j=esp_rx_queue.length-3; j>0; i=(i+1)%MAX_QUEUE_LENGTH,j--)
        {
            //�ҵ�bicycle����
            if (esp_rx_queue.RxBuffer[i]=='b' && esp_rx_queue.RxBuffer[(i+1)%MAX_QUEUE_LENGTH]=='i' && esp_rx_queue.RxBuffer[(i+2)%MAX_QUEUE_LENGTH]=='c')
            {
                for(i=i+2;j>0; i=(i+1)%MAX_QUEUE_LENGTH,j--)
                {
                    if(esp_rx_queue.RxBuffer[i]==':')
                    {
                        bicycle_power=esp_rx_queue.RxBuffer[(i+1)%MAX_QUEUE_LENGTH];//�õ��������ؿ��Ʊ���
                        //���յ�������
                        memset(esp_rx_queue.RxBuffer,0,sizeof(esp_rx_queue.RxBuffer));
                        esp_rx_queue.front=0;
                        esp_rx_queue.rear=0;
                        esp_rx_queue.length=0;
                        break;
                    }
                }
                break;
            }
            //�ҵ���λ�趨station
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
        //Ϊ��һ֡��ʼ��׼��
        one_frame_finish_flag=0;
    }
}
#define REPORT_TOPIC "\"$thing/up/property/0QGB3SJYYS/esp8266\""
#define JSON_START "\"{\\\"method\\\":\\\"report\\\"\\,\\\"clientToken\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"distance\\\":"
void ESPReportCloud(int distance)//esp�ϱ�distance�ĺ���
{
    printf("AT+TCMQTTPUB=%s,0,%s%d}}\"\r\n",REPORT_TOPIC,JSON_START,distance);
}
