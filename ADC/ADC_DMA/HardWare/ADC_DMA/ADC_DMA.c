#include "ADC_DMA.h"
#include "printf.h"

uint32_t ADC_Value[ADC_Value_Num];//�洢DMA������ADCֵ
int AD[ADC_Num];//�洢����������ADC��ֵ

uint8_t ADC_Collet_DMA(int AD[])
{
	uint8_t i,j;
	//����һ�ε���ֵ����
	for(i=0;i<ADC_Num;i++)
	{
		AD[i]=0;
	}
	//��ʼ�����ֵ�ĸ�ֵ
	for(i=0;i<ADC_Value_Num;i++)
	{
		j=i%ADC_Num;//j��i���������Ϳ��Ա�֤AD_Value�е��������θ�ADC����������ֵ
		AD[j] += ADC_Value[i];
	}
	//��ʼ��ֵ�˲�
	for(i=0;i<ADC_Num;i++)
	{
		AD[i] /= (ADC_Value_Num/ADC_Num);
	}
	return 1;
}


