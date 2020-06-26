

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
#define LOW_TASK_PRIO		2
//任务堆栈大小	
#define LOW_STK_SIZE 		256 
//任务句柄
TaskHandle_t lowTask_Handler;
//任务函数
void lowTask(void *pvParameters);

//任务优先级
#define MED_TASK_PRIO		3
//任务堆栈大小	
#define MED_STK_SIZE 		256 
//任务句柄
TaskHandle_t medTask_Handler;
//任务函数
void medTask(void *pvParameters);

//任务优先级
#define HEIGHT_TASK_PRIO		4
//任务堆栈大小	
#define HEIGHT_STK_SIZE 		256 
//任务句柄
TaskHandle_t heightTask_Handler;
//任务函数
void heightTask(void *pvParameters);




//信号量句柄
SemaphoreHandle_t MutexSemaphore;



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
	
	MutexSemaphore = xSemaphoreCreateMutex();
	if(MutexSemaphore == NULL)
	{
		printf("互斥信号量创建失败");
	}
	
	
	xTaskCreate((TaskFunction_t 	 ) lowTask,
				(char *				 ) "lowTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) LOW_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) LOW_TASK_PRIO,
				(TaskHandle_t *		 ) &lowTask_Handler );
	
	
	xTaskCreate((TaskFunction_t 	 ) medTask,
				(char *				 ) "medTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) MED_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) MED_TASK_PRIO,
				(TaskHandle_t *		 ) &medTask_Handler );
				
	xTaskCreate((TaskFunction_t 	 ) heightTask,
				(char *				 ) "heightTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) HEIGHT_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) HEIGHT_TASK_PRIO,
				(TaskHandle_t *		 ) &heightTask_Handler );
						
		
    vTaskDelete(StartTask_Handler); //删除开始任务	
	taskEXIT_CRITICAL();
}


void lowTask(void *pvParameters)
{
	static u32 times;
    while(1)
    {
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);
		printf("low task running!\r\n");
		for(times=0;times<20000;times++)
		{
			delay_us(2);
		}
		xSemaphoreGive(MutexSemaphore);
		vTaskDelay(1000);

    }	
}
void medTask(void *pvParameters)
{

    while(1)
    {
		printf("mid task running!\r\n");
		vTaskDelay(1000);
    }
}

void heightTask(void *pvParameters)
{
	while(1)
	{
		printf("high task Pend Sem!\r\n");
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);
		
		xSemaphoreGive(MutexSemaphore);
		printf("high task running!\r\n");
		vTaskDelay(1000);
	
	}


}

