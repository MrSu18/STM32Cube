#include "su_driver_gpio.h"

/***********************************************
* @brief : gpio时钟使能的封装
* @param : GPIO_TypeDef *GPIOx：GPIO的端口比如GPIOB
* @return: void
* @date  : 2023/1/2
* @author: 刘骏帆
************************************************/
//void GPIO_RCC_Enable(GPIO_TypeDef *GPIOx)
//{
//    switch(*GPIOx)
//	{
//		case GPIOA:__HAL_RCC_GPIOA_CLK_ENABLE();break;
//		case GPIOB:__HAL_RCC_GPIOB_CLK_ENABLE();break;
//		case GPIOC:__HAL_RCC_GPIOC_CLK_ENABLE();break;
//		case GPIOD:__HAL_RCC_GPIOD_CLK_ENABLE();break;
//		default:break;
//	}
//}
