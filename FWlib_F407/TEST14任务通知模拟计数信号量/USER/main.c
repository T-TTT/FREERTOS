

/***********任务挂起与恢复***********************
******K_UP为LED1任务挂起************************
*******K0为LED0任务挂起和LED1中断恢复任务运行****
*******K1为LED0任务恢复*************************/

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h"
#include "key.h"
#include "exti.h"
#include "timer.h"
#include "queue.h"
#include "string.h"
#include "semphr.h"


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void StartTask(void *pvParameters);


//任务优先级
#define TASK1_TASK_PRIO		2
//任务堆栈大小	
#define TASK1_STK_SIZE 		256 
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void Task1Task(void *pvParameters);

//任务优先级
#define DATAPRO_TASK_PRIO		3
//任务堆栈大小	
#define DATAPRO_STK_SIZE 		256 
//任务句柄
TaskHandle_t DATAPROTask_Handler;
//任务函数
void DATAPROTask(void *pvParameters);


//信号量句柄
SemaphoreHandle_t CountingSemaphore;



int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);								//初始化延时函数
	uart_init(115200);     							//初始化串口
	LED_Init();		       							 //初始化LED端口
	KEY_Init();

	
	
	xTaskCreate(	(TaskFunction_t 	 ) StartTask,
					(char *				 ) "StartTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
					(uint16_t			 ) START_STK_SIZE,
					(void *				 ) NULL,
					(UBaseType_t		 ) START_TASK_PRIO,
					(TaskHandle_t *		 ) &StartTask_Handler );

					
	vTaskStartScheduler();		//开始任务调度
}


void StartTask(void *pvParameters)
{
	taskENTER_CRITICAL();
	
	CountingSemaphore = xSemaphoreCreateCounting(255,0);
	if(CountingSemaphore == NULL)
	{
		printf("计数型信号量创建失败");
	}
	
	
	xTaskCreate((TaskFunction_t 	 ) Task1Task,
				(char *				 ) "Task1Task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) TASK1_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) TASK1_TASK_PRIO,
				(TaskHandle_t *		 ) &Task1Task_Handler );
	
	
	xTaskCreate((TaskFunction_t 	 ) DATAPROTask,
				(char *				 ) "DATAPROTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) DATAPRO_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) DATAPRO_TASK_PRIO,
				(TaskHandle_t *		 ) &DATAPROTask_Handler );
						
		
    vTaskDelete(StartTask_Handler); //删除开始任务	
	taskEXIT_CRITICAL();
}


void Task1Task(void *pvParameters)
{
	u8 key;
    while(1)
    {
		key = KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			if( DATAPROTask_Handler != NULL)
			{
				xTaskNotifyGive(DATAPROTask_Handler);
			}
		}
		LED0=!LED0;
		vTaskDelay(10);
    }	
}
void DATAPROTask(void *pvParameters)
{
	u32 notifyvalue;

    while(1)
    {

		notifyvalue = ulTaskNotifyTake( pdFALSE, portMAX_DELAY );
		printf("notifyvalue  = %d \r\n",notifyvalue);
		vTaskDelay(1000);
    }
}

