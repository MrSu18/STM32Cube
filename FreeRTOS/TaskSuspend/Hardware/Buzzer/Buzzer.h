#include "main.h"
#include "gpio.h"

#define ON  0
#define OFF 1

#define Buzzer(N) HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,N?GPIO_PIN_SET:GPIO_PIN_RESET)
