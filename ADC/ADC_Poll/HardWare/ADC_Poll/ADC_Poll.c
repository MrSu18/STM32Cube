#include "ADC_Poll.h"

/*
** 函数功能：ADC采集引脚电压（轮询采集）
** 滤波方式：无
** 参数：adc通道的句柄，存储ADC的数据
** 返回值：无
*/
uint32_t Poll_ADC(ADC_HandleTypeDef hadcx)
{
    uint32_t AD_Value;

    HAL_ADC_Start(&hadcx);//启动ADC
    HAL_ADC_PollForConversion(&hadcx, 50);//等待转换完成，第二个参数表示超时时间，单位ms	
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadcx),HAL_ADC_STATE_REG_EOC))//判断是否转化完成
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
** 函数功能：平均滤波ADC采集到的数据
** 参数：adc通道的句柄，存储ADC的数据
** 返回值：滤波后的数值
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
