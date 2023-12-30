#include "su_common_delay.h"

/***********************************************
* @brief : 用sistick让定时器进行us延迟
* @param : uint32_t us: 需要延迟的实践
* @return: void
* @date  : 2023/12/3
* @author: 刘骏帆
************************************************/
void System_Delay_us(uint32_t us)
{       
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000000);
    HAL_Delay(us-1);
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
}

/***********************************************
* @brief : ms延迟
* @param : uint32_t ms: 需要延迟的实践
* @return: void
* @date  : 2023/12/3
* @author: 刘骏帆
************************************************/
void System_Delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}
