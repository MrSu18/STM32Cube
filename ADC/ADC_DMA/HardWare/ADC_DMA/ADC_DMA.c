#include "ADC_DMA.h"
#include "printf.h"

uint32_t ADC_Value[ADC_Value_Num];//存储DMA读到的ADC值
int AD[ADC_Num];//存储最后面输出的ADC的值

uint8_t ADC_Collet_DMA(int AD[])
{
	uint8_t i,j;
	//把上一次的数值清零
	for(i=0;i<ADC_Num;i++)
	{
		AD[i]=0;
	}
	//开始这次数值的赋值
	for(i=0;i<ADC_Value_Num;i++)
	{
		j=i%ADC_Num;//j对i求余这样就可以保证AD_Value中的数组依次给ADC真正变量赋值
		AD[j] += ADC_Value[i];
	}
	//开始均值滤波
	for(i=0;i<ADC_Num;i++)
	{
		AD[i] /= (ADC_Value_Num/ADC_Num);
	}
	return 1;
}


