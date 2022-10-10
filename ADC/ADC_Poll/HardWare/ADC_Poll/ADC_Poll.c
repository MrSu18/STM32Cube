#include "ADC_Poll.h"

/*
** �������ܣ�ADC�ɼ����ŵ�ѹ����ѯ�ɼ���
** �˲���ʽ����
** ������adcͨ���ľ�����洢ADC������
** ����ֵ����
*/
uint32_t Poll_ADC(ADC_HandleTypeDef hadcx)
{
    uint32_t AD_Value;

    HAL_ADC_Start(&hadcx);//����ADC
    HAL_ADC_PollForConversion(&hadcx, 50);//�ȴ�ת����ɣ��ڶ���������ʾ��ʱʱ�䣬��λms	
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadcx),HAL_ADC_STATE_REG_EOC))//�ж��Ƿ�ת�����
    {
        AD_Value=HAL_ADC_GetValue(&hadcx);
        
        return AD_Value;
    }
    else
    {
        return 99;
    }
}

/*
** �������ܣ�ƽ���˲�ADC�ɼ���������
** ������adcͨ���ľ�����洢ADC������
** ����ֵ���˲������ֵ
*/
uint32_t Filiter(ADC_HandleTypeDef hadcx)
{
    uint32_t AD_Value=0;
    for(uint8_t i=0;i<10;i++)
    {
        AD_Value=AD_Value+Poll_ADC(hadcx);
    }
    return AD_Value/10;
}
