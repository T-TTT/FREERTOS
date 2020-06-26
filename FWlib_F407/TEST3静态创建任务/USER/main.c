#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h"


//空闲任务堆栈和控制块
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t IdleTaskTCB;


//定时器任务堆栈和控制块
static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
static StaticTask_t TimerTaskTCB;


//空闲任务所需内存
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
									StackType_t **ppxIdleTaskStackBuffer, 
									uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &IdleTaskTCB;
	*ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

//定时器服务任务所需内存
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
									StackType_t **ppxIdleTaskStackBuffer, 
									uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &TimerTaskTCB;
	*ppxIdleTaskStackBuffer = TimerTaskStack;
    *pulIdleTaskStackSize = configTIMER_TASK_STACK_DEPTH;	
	
}

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void StartTask(void *pvParameters);
//任务堆栈
StackType_t StartTaskStack[START_STK_SIZE];
//任务控制块
StaticTask_t StartTaskTCB;




//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void LED0Task(void *pvParameters);
//任务堆栈
StackType_t  LED0TaskStack[LED0_STK_SIZE];
//任务控制块
StaticTask_t LED0TaskTCB;



//任务优先级
#define LED1_TASK_PRIO		3
//任务堆栈大小	
#define LED1_STK_SIZE 		50  
//任务句柄
TaskHandle_t LED1Task_Handler;
//任务函数
void LED1Task(void *pvParameters);
//任务堆栈
StackType_t  LED1TaskStack[LED1_STK_SIZE];
//任务控制块
StaticTask_t LED1TaskTCB;


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
	
	StartTask_Handler=xTaskCreateStatic((TaskFunction_t ) StartTask,
										(char*          ) "StartTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
										(uint32_t       ) START_STK_SIZE,
										(void *         ) NULL,
										(UBaseType_t    ) START_TASK_PRIO,
										(StackType_t *  ) StartTaskStack,
										(StaticTask_t * ) &StartTaskTCB );

					
	vTaskStartScheduler();		//开始任务调度
}


void StartTask(void *pvParameters)
{
	taskENTER_CRITICAL();
	
	LED0Task_Handler = xTaskCreateStatic((TaskFunction_t ) LED0Task,
										(char*          ) "LED0Task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
										(uint32_t       ) LED0_STK_SIZE,
										(void *         ) NULL,
										(UBaseType_t    ) LED0_TASK_PRIO,
										(StackType_t *  ) LED0TaskStack,
										(StaticTask_t * ) &LED0TaskTCB );


	LED1Task_Handler = xTaskCreateStatic((TaskFunction_t ) LED1Task,
										(char*          ) "LED1Task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
										(uint32_t       ) LED1_STK_SIZE,
										(void *         ) NULL,
										(UBaseType_t    ) LED1_TASK_PRIO,
										(StackType_t *  ) LED1TaskStack,
										(StaticTask_t * ) &LED1TaskTCB );										
										
										
    vTaskDelete(StartTask_Handler); //删除开始任务	
	taskEXIT_CRITICAL();
}


void LED0Task(void *pvParameters)
{
    while(1)
    {
        LED0=~LED0;
        vTaskDelay(500);
    }	

}
void LED1Task(void *pvParameters)
{
    while(1)
    {
        LED1=0;
        vTaskDelay(800);
        LED1=1;
        vTaskDelay(800);
    }
}

