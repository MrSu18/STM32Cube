#include "printf.h"

int fputc(int ch, FILE *f)
{
  uint8_t temp[1] = {ch};
  HAL_UART_Transmit(&huart1, temp, 1, 2);//huart1需要根据你的配置修改
  return ch;
}
