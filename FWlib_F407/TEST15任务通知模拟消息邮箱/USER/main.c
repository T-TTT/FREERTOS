

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
#include "limits.h"


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void StartTask(void *pvParameters);


//任务优先级
#define KEYSCAN_TASK_PRIO		2
//任务堆栈大小	
#define KEYSCAN_STK_SIZE 		128 
//任务句柄
TaskHandle_t KEYSTask_Handler;
//任务函数
void KEYSTask(void *pvParameters);

//任务优先级
#define KEYPRO_TASK_PRIO		3
//任务堆栈大小	
#define KEYPRO_STK_SIZE 		128  
//任务句柄
TaskHandle_t KEYPROTask_Handler;
//任务函数
void KEYPROTask(void *pvParameters);



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
	
	
	
	xTaskCreate((TaskFunction_t 	 ) KEYSTask,
				(char *				 ) "KEYSTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) KEYSCAN_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) KEYSCAN_TASK_PRIO,
				(TaskHandle_t *		 ) &KEYSTask_Handler );
	
	
	xTaskCreate((TaskFunction_t 	 ) KEYPROTask,
				(char *				 ) "KEYPROTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) KEYPRO_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) KEYPRO_TASK_PRIO,
				(TaskHandle_t *		 ) &KEYPROTask_Handler );
						

				
    vTaskDelete(StartTask_Handler); //删除开始任务	
	taskEXIT_CRITICAL();
}


void KEYSTask(void *pvParameters)
{
	u8 key;
    while(1)
    {
		key = KEY_Scan(0);
		if((KEYPROTask_Handler != NULL)&&(key))					//判断队列有效与按键按下
		{
			xTaskNotify( KEYPROTask_Handler, (uint32_t) key,eSetValueWithOverwrite);
		}
		vTaskDelay(10);
    }	
}
void KEYPROTask(void *pvParameters)
{
	u8 key=1;
	BaseType_t err;
    while(1)
    {
		err = xTaskNotifyWait( 0, ULONG_MAX, (uint32_t*)&key, portMAX_DELAY );
		if(err == pdPASS)
		{
			printf("KEY的值等于%d\n\r",key);
			switch(key)
			{
				case WKUP_PRES:
					LED0 =!LED0;
					break;
				case KEY0_PRES:
					LED1 =!LED1;
					break;
				case KEY1_PRES:
					printf("哈哈\r\n");
					break;
			}
		}
    }
}

