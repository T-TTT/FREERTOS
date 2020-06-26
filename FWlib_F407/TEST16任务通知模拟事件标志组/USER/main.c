#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"
#include "exti.h"
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
#define SETBIT_TASK_PRIO		2
//任务堆栈大小	
#define SETBIT_STK_SIZE 		256 
//任务句柄
TaskHandle_t setbitTask_Handler;
//任务函数
void setbitTask(void *pvParameters);


//任务优先级
#define GROUP_TASK_PRIO		3
//任务堆栈大小	
#define GROUP_STK_SIZE 		256 
//任务句柄
TaskHandle_t groupTask_Handler;
//任务函数
void groupTask(void *pvParameters);


#define			EVENTBIT_0   	(1<<0)
#define			EVENTBIT_1   	(1<<1)
#define			EVENTBIT_2   	(1<<2)
#define 		EVENTBIT_ALL	(EVENTBIT_1|EVENTBIT_2|EVENTBIT_0)


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
	KEY_Init();
	EXTIX_Init();
	
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
	

										  
	//任务创建									  
	xTaskCreate((TaskFunction_t 	 ) setbitTask,
				(char *				 ) "setbitTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) SETBIT_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) SETBIT_TASK_PRIO,
				(TaskHandle_t *		 ) &setbitTask_Handler );
	
	//任务创建									  
	xTaskCreate((TaskFunction_t 	 ) groupTask,
				(char *				 ) "groupTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) GROUP_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) GROUP_TASK_PRIO,
				(TaskHandle_t *		 ) &groupTask_Handler );	
			
    vTaskDelete(StartTask_Handler); //删除开始任务	
	taskEXIT_CRITICAL();
				
}


void setbitTask(void *pvParameters)
{
	u8 key;
    while(1)
    {
		key = KEY_Scan(0);
		if(groupTask_Handler!=NULL)
		{
			switch(key)
			{
				case WKUP_PRES:
					xTaskNotify(groupTask_Handler,EVENTBIT_1,eSetBits);
					break;
				case KEY1_PRES:
					xTaskNotify(groupTask_Handler,EVENTBIT_2,eSetBits);
					break;
			}
		}
		vTaskDelay(10);
    }	

}

void groupTask(void *pvParameters)
{
	u8 num;
	static u8 event0flag,event1flag,event2flag,eventvalue;
	BaseType_t err;
	u32 key;
    while(1)
    {
		err = xTaskNotifyWait( 0, ULONG_MAX, (uint32_t *)&key, portMAX_DELAY );
		if(err == pdPASS)
		{
			if(key & EVENTBIT_0)
			{
				event0flag = 1;
			}
			else if(key & EVENTBIT_1)
			{
				event1flag = 1;
			}
			else if(key & EVENTBIT_2)
			{
				event2flag = 1;
			}
		}
		
		eventvalue = event0flag|(event1flag<<1)|(event2flag<<2);
		printf("EventValue = %#x\r\n",eventvalue);
		if(eventvalue ==0x07)
		{
			num++;
			event0flag = 0;
			event1flag = 0;
			event2flag = 0;
			printf("num = %d\r\n",num);
		}
		vTaskDelay(10);
			

    }	
}



