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
#include "usart.h"
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
osMailQId	myMail01Handle;
/* USER CODE END Variables */
osThreadId MailHandle;
osThreadId MailConsumerHandle;
osThreadId myTask03Handle;
osSemaphoreId myBinarySem01Handle;


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void MailTask(void const * argument);
void MailConsumerTask(void const * argument);
void StartTask03(void const * argument);

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

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  osMailQDef(myMail01,16,uint16_t);
  myMail01Handle = osMailCreate(osMailQ(myMail01),NULL);
  /* USER CODE END RTOS_QUEUES */
	
  /* Create the thread(s) */
  /* definition and creation of Mail */
  osThreadDef(Mail, MailTask, osPriorityNormal, 0, 128);
  MailHandle = osThreadCreate(osThread(Mail), NULL);

  /* definition and creation of MailConsumer */
  osThreadDef(MailConsumer, MailConsumerTask, osPriorityNormal, 0, 128);
  MailConsumerHandle = osThreadCreate(osThread(MailConsumer), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityIdle, 0, 128);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

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
	uint8_t Msg[]="USART1 Senging message!\n";
  /* Infinite loop */
  for(;;)
  {
		HAL_UART_Transmit_IT((&huart1),Msg,sizeof(Msg));							//中断方式发送数据
		osSemaphoreRelease(myBinarySem01Handle);									//释放二值信号量
    osDelay(1000);
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
  /* Infinite loop */
  for(;;)
  {
	  osSemaphoreWait(myBinarySem01Handle,100);
	  HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_9);
	  osDelay(1);
  }
  /* USER CODE END MailConsumerTask */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
	osEvent retval;
	uint8_t *rdata;
  /* Infinite loop */
  for(;;)
  {
	  retval = osMailGet(myMail01Handle,100);
	  if(retval.status == osEventMail)
	  {
		printf("Receiving!!\n\r");
		rdata = retval.value.p;
		printf("var1 : %s\n\r",rdata);
	  }
  }
  /* USER CODE END StartTask03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
