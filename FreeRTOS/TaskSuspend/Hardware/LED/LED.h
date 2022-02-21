#include "main.h"
#include "gpio.h"

#define ON  0
#define OFF 1

#define LED0(N) HAL_GPIO_WritePin(LED0_GPIO_Port,LED0_Pin,N?GPIO_PIN_SET:GPIO_PIN_RESET)
#define LED1(N) HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,N?GPIO_PIN_SET:GPIO_PIN_RESET)

