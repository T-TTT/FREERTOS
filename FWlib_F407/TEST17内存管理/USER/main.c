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
#include "stdio.h"



//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void StartTask(void *pvParameters);


//任务优先级
#define MALLOC_TASK_PRIO		2
//任务堆栈大小	
#define MALLOC_STK_SIZE 		256 
//任务句柄
TaskHandle_t mallocTask_Handler;
//任务函数
void mallocTask(void *pvParameters);



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
									  
	//任务创建									  
	xTaskCreate((TaskFunction_t 	 ) mallocTask,
				(char *				 ) "mallocTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) MALLOC_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) MALLOC_TASK_PRIO,
				(TaskHandle_t *		 ) &mallocTask_Handler );
			
    vTaskDelete(StartTask_Handler); //删除开始任务	
	taskEXIT_CRITICAL();			
}


void mallocTask(void *pvParameters)
{
	u8 key;
	u8 *Buffer;
	u16 timer=0;
    while(1)
    {
		key = KEY_Scan(0);
		
		switch(key)
		{
			case WKUP_PRES:
				Buffer = pvPortMalloc(30);
				printf("申请到的内存地址为%#x\r\n",(int)Buffer);
				printf("剩余内存大小：%d\r\n",xPortGetFreeHeapSize());
				break;
			case KEY1_PRES:
				vPortFree(Buffer);
				printf("内存释放后的内存地址为%#x\r\n",(int)Buffer);
			    Buffer = NULL;
				break;
			case KEY0_PRES:
				timer++;
				if(Buffer != NULL)
				{
					sprintf((char*)Buffer,"User Timers = %d",timer);
					printf("%s\r\n",Buffer);
				}
				else
					printf("内存无效！需要申请\r\n");
				break;		
		}
		vTaskDelay(10);

    }	
}





