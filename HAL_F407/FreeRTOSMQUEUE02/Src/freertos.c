/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
osThreadId MailHandle;
osThreadId MailConsumerHandle;
osMailQId MailQ01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
typedef struct
{
	uint32_t var1;
	uint32_t var2;
	uint8_t var3;
}Amail_TypeDef;
/* USER CODE END FunctionPrototypes */

void MailTask(void const * argument);
void MailConsumerTask(void const * argument);

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


  /* Create the queue(s) */
  /* definition and creation of MailQ01 */
  osMailQDef(MailQ01, 15, Amail_TypeDef);
  MailQ01Handle = osMailCreate(osMailQ(MailQ01), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Mail */
  osThreadDef(Mail, MailTask, osPriorityNormal, 0, 128);
  MailHandle = osThreadCreate(osThread(Mail), NULL);

  /* definition and creation of MailConsumer */
  osThreadDef(MailConsumer, MailConsumerTask, osPriorityNormal, 0, 128);
  MailConsumerHandle = osThreadCreate(osThread(MailConsumer), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_MailTask */
/**
  * @brief  Function implementing the Mail thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_MailTask */
void MailTask(void const * argument)
{
  /* USER CODE BEGIN MailTask */
	Amail_TypeDef Mail[3] = {
	{0x01,0x02,0x03},
	{0x02,0x03,0x04},
	{0x03,0x04,0x05},
	};
  /* Infinite loop */
  for(;;)
  {
	  osMailPut(MailQ01Handle,&Mail[0]);
	  osDelay(1000);
	  osMailPut(MailQ01Handle,&Mail[1]);
	  osDelay(1000);
	  osMailPut(MailQ01Handle,&Mail[2]);
	  osDelay(3000);
  }	
  /* USER CODE END MailTask */
}

/* USER CODE BEGIN Header_MailConsumerTask */
/**
* @brief Function implementing the MailConsumer thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MailConsumerTask */
void MailConsumerTask(void const * argument)
{
  /* USER CODE BEGIN MailConsumerTask */
	osEvent event;
	Amail_TypeDef *pMail;
	uint32_t cnt;
  /* Infinite loop */
  for(;;)
  {
	  event = osMailGet(MailQ01Handle,osWaitForever);			//获取邮件
	  if(event.status == osEventMail)							//是否是邮件事件
	  {
		  pMail = event.value.p;								//获取邮件指针
		  cnt = pMail->var3;									//提取var3的值，也可以是其他；
		  while(cnt--)
		  {
			  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET);
		  	  osDelay(100);
			  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_SET);
			  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET);
		  	  osDelay(100);
		  }
	  }
   
  }
  /* USER CODE END MailConsumerTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
