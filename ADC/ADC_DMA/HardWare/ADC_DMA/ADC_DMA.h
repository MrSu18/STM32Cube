#include "stdint.h"//uint8�ȱ����ĺ궨��

//�궨�巽���޸�
#define ADC_Num 5
#define ADC_Value_Num 50//ADC�ݴ�DMA��ȡ����ֵ�������С

extern uint32_t ADC_Value[ADC_Value_Num];//�洢DMA������ADCֵ
extern int AD[ADC_Num];//�洢����������ADC��ֵ
/*
* �������ܣ���DMA�ķ�ʽ��ȡADC��ֵ
* ��   ����ADC�����׵�ַ,���鳤��
* �� ��ֵ��1:�ɹ���0��ʧ��
* ��  �ߣ�LJF
*/
uint8_t ADC_Collet_DMA(int AD[]);
