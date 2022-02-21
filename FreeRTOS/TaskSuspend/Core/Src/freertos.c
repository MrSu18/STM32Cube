/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LED.h"
#include "Buzzer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId Key1TaskHandle;
osThreadId LED0Task03Handle;
osThreadId LED1Task04Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void KEYTask(void const * argument);
void LED0Task(void const * argument);
void LED1Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Key1Task */
  osThreadDef(Key1Task, KEYTask, osPriorityNormal, 0, 128);
  Key1TaskHandle = osThreadCreate(osThread(Key1Task), NULL);

  /* definition and creation of LED0Task03 */
  osThreadDef(LED0Task03, LED0Task, osPriorityIdle, 0, 128);
  LED0Task03Handle = osThreadCreate(osThread(LED0Task03), NULL);

  /* definition and creation of LED1Task04 */
  osThreadDef(LED1Task04, LED1Task, osPriorityIdle, 0, 128);
  LED1Task04Handle = osThreadCreate(osThread(LED1Task04), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_KEYTask */
/**
  * @brief  Function implementing the Key1Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_KEYTask */
void KEYTask(void const * argument)
{
  /* USER CODE BEGIN KEYTask */
	uint8_t keyflag=1;
  /* Infinite loop */
  for(;;)
  {
	  switch(keyflag)
	  {
		  case 1:
			  osDelay(20);
			  if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin))
			  {
				  vTaskSuspend(LED0Task03Handle);//挂起LED0任务
				  keyflag=0;
			  }
			  break;
		  case 0:
			  osDelay(20);
			  if(HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin))
			  {
				  vTaskResume(LED0Task03Handle);//恢复LED0任务
				  keyflag=1;
			  }
			  break;
		  default:
			  break;
	  }
	  osDelay(20);
	
  }
  /* USER CODE END KEYTask */
}

/* USER CODE BEGIN Header_LED0Task */
/**
* @brief Function implementing the LED0Task03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LED0Task */
void LED0Task(void const * argument)
{
  /* USER CODE BEGIN LED0Task */
  /* Infinite loop */
  for(;;)
  {
	  LED0(OFF);
      osDelay(500);
	  LED0(ON);
	  osDelay(500);
  }
  /* USER CODE END LED0Task */
}

/* USER CODE BEGIN Header_LED1Task */
/**
* @brief Function implementing the LED1Task04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LED1Task */
void LED1Task(void const * argument)
{
  /* USER CODE BEGIN LED1Task */
  /* Infinite loop */
  for(;;)
  {
      LED1(OFF);
      osDelay(100);
	  LED1(ON);
	  osDelay(100);
  }
  /* USER CODE END LED1Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
