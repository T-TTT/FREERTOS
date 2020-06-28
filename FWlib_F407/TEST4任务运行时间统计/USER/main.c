

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

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void StartTask(void *pvParameters);


//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		128 
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void LED0Task(void *pvParameters);

//任务优先级
#define LED1_TASK_PRIO		3
//任务堆栈大小	
#define LED1_STK_SIZE 		128  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void LED1Task(void *pvParameters);


//任务优先级
#define RUNTIME_TASK_PRIO		4
//任务堆栈大小	
#define RUNTIME_STK_SIZE 		128 
//任务句柄
TaskHandle_t RUNTIMETask_Handler;
//任务函数
void RUNTIMETask(void *pvParameters);


char RunTimeInfo[400];		//保存任务运行时间信息

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
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
	

	
	xTaskCreate((TaskFunction_t 	 ) LED0Task,
				(char *				 ) "LED0Task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) LED0_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) LED0_TASK_PRIO,
				(TaskHandle_t *		 ) &LED0Task_Handler );
	
	
	xTaskCreate((TaskFunction_t 	 ) LED1Task,
				(char *				 ) "LED1Task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) LED1_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) LED1_TASK_PRIO,
				(TaskHandle_t *		 ) &LED1Task_Handler );
				
	xTaskCreate((TaskFunction_t 	 )  RUNTIMETask,
				(char *				 ) "RUNTIMETask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) RUNTIME_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) RUNTIME_TASK_PRIO,
				(TaskHandle_t *		 ) &RUNTIMETask_Handler );				

				

    vTaskDelete(StartTask_Handler); //删除开始任务	
	taskEXIT_CRITICAL();
}


void LED0Task(void *pvParameters)
{
	uint16_t num=0;
    while(1)
    {
		num++;
		LED0=!LED0;
		vTaskDelay(1000);
		printf("LED0任务执行 %d 次\n\r",num);
    }	

}
void LED1Task(void *pvParameters)
{
	uint16_t num=0;
    while(1)
    {
		num++;
		LED1=!LED1;
		vTaskDelay(1000);
		printf("LED1任务执行 %d 次\n\r",num);
    }
}

void RUNTIMETask(void *pvParameters)
{
	u8 key = 0;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			vTaskGetRunTimeStats(RunTimeInfo);
			printf("任务名\t\t\t运行时间\t运行所占百分百\t\r\n");
			printf("%s\r\n",RunTimeInfo);
		}
		vTaskDelay(10);
	}


}

