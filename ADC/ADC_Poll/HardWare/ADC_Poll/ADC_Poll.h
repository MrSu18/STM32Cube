#include "adc.h"//包含了hadc1啥的
#include "stdint.h"//包含了uint32_t

/*
** 函数功能：ADC采集引脚电压（轮询采集）
** 滤波方式：无
** 参数：adc通道的句柄，存储ADC的数据
** 返回值：无
*/
uint32_t Poll_ADC(ADC_HandleTypeDef hadcx);
