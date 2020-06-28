#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h"
#include "queue.h"
#include "timers.h"

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void StartTask(void *pvParameters);


//任务优先级
#define TIMER_TASK_PRIO		2
//任务堆栈大小	
#define TIMER_STK_SIZE 		128 
//任务句柄
TaskHandle_t timerTask_Handler;
//任务函数
void timerTask(void *pvParameters);


//定时器句柄和回调函数
TimerHandle_t AutoReloadTimer_Handle;
TimerHandle_t OneShotTimer_Handle;
void AutoReloadCallback( TimerHandle_t xTimer );
void OneShotCallback( TimerHandle_t xTimer );



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
	
	
	//定时器创建
	AutoReloadTimer_Handle = xTimerCreate((char *                 ) "AutoReloadTimer",		//名字
										  (TickType_t             ) 1000,					//周期
										  (UBaseType_t            ) pdTRUE,					//周期型
										  (void *                 ) 1,						//id号
										  (TimerCallbackFunction_t) AutoReloadCallback );	//回调函数名
										  
										  
	OneShotTimer_Handle   =  xTimerCreate((char *                 ) "OneShotTimer",			
										  (TickType_t             ) 2000,
										  (UBaseType_t            ) pdFALSE,				//单次型
										  (void *                 ) 2,
										  (TimerCallbackFunction_t) OneShotCallback );
										  
										  
	//任务创建									  
	xTaskCreate((TaskFunction_t 	 ) timerTask,
				(char *				 ) "timerTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) TIMER_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) TIMER_TASK_PRIO,
				(TaskHandle_t *		 ) &timerTask_Handler );
	
	
			
    vTaskDelete(StartTask_Handler); //删除开始任务	
	taskEXIT_CRITICAL();
				
}


void timerTask(void *pvParameters)
{
	u8 key;
	BaseType_t err;
    while(1)
    {
		key = KEY_Scan(0);
		switch (key)
		{
			case WKUP_PRES:
					err = xTimerStart(AutoReloadTimer_Handle,0);
					if(err == pdFAIL)
					{
						printf("周期性定时器开启失败\r\n");
					}
					else
						printf("周期性定时器开启成功\r\n");
					break;
					
			case KEY0_PRES:
					err = xTimerStart(OneShotTimer_Handle,0);
					if(err == pdFAIL)
					{
						printf("单次定时器开启失败\r\n");
					}
					else
						printf("单次性定时器开启成功\r\n");
					break;
			case KEY1_PRES:
					err = xTimerStop(AutoReloadTimer_Handle,0);
					if(err == pdFAIL)
					{
						printf("周期性定时器关闭失败\r\n");
					}
					else
						printf("周期性定时器关闭成功\r\n");
					break;
		}
		vTaskDelay(10);
    }	

}


//周期定时器回调函数
void AutoReloadCallback( TimerHandle_t xTimer )		
{
	static u8 timers=0;
	timers++;
	printf("AutoReloadTimer running : %d\r\n",timers);
}

//单次定时器回调函数
void OneShotCallback( TimerHandle_t xTimer )		
{
	static u8 timers=0;
	timers++;
	printf("OneShotTimer running : %d\r\n",timers);
}


