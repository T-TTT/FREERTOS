#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h"

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void StartTask(void *pvParameters);


//任务优先级
#define LED0_TASK_PRIO		3
//任务堆栈大小	
#define LED0_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void LED0Task(void *pvParameters);

//任务优先级
#define LED1_TASK_PRIO		2
//任务堆栈大小	
#define LED1_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void LED1Task(void *pvParameters);



int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
	
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
				

    vTaskDelete(StartTask_Handler); //删除开始任务	
	taskEXIT_CRITICAL();
}


void LED0Task(void *pvParameters)
{
	uint16_t num=0;
    while(1)
    {
		num++;
//		if(num == 5)
//		{
//			vTaskDelete(LED1Task_Handler);
//			printf("任务2已删除\n\r");
//			LED1=1;
//		}
		LED0=!LED0;
		
		printf("姓名：陈伟腾；Task1执行 %d 次\n\r",num);
		vTaskDelay(1000);
    }	

}
void LED1Task(void *pvParameters)
{
	uint16_t num=0;
    while(1)
    {
		num++;
		LED1=!LED1;
		printf("学号：18010123；Task2执行 %d 次\n\r",num);
		vTaskDelay(2000);
    }
}

