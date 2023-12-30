#include "su_common_delay.h"

/***********************************************
* @brief : ��sistick�ö�ʱ������us�ӳ�
* @param : uint32_t us: ��Ҫ�ӳٵ�ʵ��
* @return: void
* @date  : 2023/12/3
* @author: ������
************************************************/
void System_Delay_us(uint32_t us)
{       
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000000);
    HAL_Delay(us-1);
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
}

/***********************************************
* @brief : ms�ӳ�
* @param : uint32_t ms: ��Ҫ�ӳٵ�ʵ��
* @return: void
* @date  : 2023/12/3
* @author: ������
************************************************/
void System_Delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
