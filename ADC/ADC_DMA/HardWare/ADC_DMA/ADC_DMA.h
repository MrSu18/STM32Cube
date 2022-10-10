#include "stdint.h"//uint8等变量的宏定义

//宏定义方便修改
#define ADC_Num 5
#define ADC_Value_Num 50//ADC暂存DMA读取到的值的数组大小

extern uint32_t ADC_Value[ADC_Value_Num];//存储DMA读到的ADC值
extern int AD[ADC_Num];//存储最后面输出的ADC的值
/*
* 函数功能：用DMA的方式读取ADC的值
* 参   数：ADC数组首地址,数组长度
* 返 回值：1:成功，0：失败
* 作  者：LJF
*/
uint8_t ADC_Collet_DMA(int AD[]);
