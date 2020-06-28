#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h"


//���������ջ�Ϳ��ƿ�
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t IdleTaskTCB;


//��ʱ�������ջ�Ϳ��ƿ�
static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
static StaticTask_t TimerTaskTCB;


//�������������ڴ�
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
									StackType_t **ppxIdleTaskStackBuffer, 
									uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &IdleTaskTCB;
	*ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

//��ʱ���������������ڴ�
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
									StackType_t **ppxIdleTaskStackBuffer, 
									uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &TimerTaskTCB;
	*ppxIdleTaskStackBuffer = TimerTaskStack;
    *pulIdleTaskStackSize = configTIMER_TASK_STACK_DEPTH;	
	
}

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void StartTask(void *pvParameters);
//�����ջ
StackType_t StartTaskStack[START_STK_SIZE];
//������ƿ�
StaticTask_t StartTaskTCB;




//�������ȼ�
#define LED0_TASK_PRIO		2
//�����ջ��С	
#define LED0_STK_SIZE 		50  
//������
TaskHandle_t LED0Task_Handler;
//������
void LED0Task(void *pvParameters);
//�����ջ
StackType_t  LED0TaskStack[LED0_STK_SIZE];
//������ƿ�
StaticTask_t LED0TaskTCB;



//�������ȼ�
#define LED1_TASK_PRIO		3
//�����ջ��С	
#define LED1_STK_SIZE 		50  
//������
TaskHandle_t LED1Task_Handler;
//������
void LED1Task(void *pvParameters);
//�����ջ
StackType_t  LED1TaskStack[LED1_STK_SIZE];
//������ƿ�
StaticTask_t LED1TaskTCB;


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	uart_init(115200);     	//��ʼ������
	LED_Init();		        //��ʼ��LED�˿�
	
	StartTask_Handler=xTaskCreateStatic((TaskFunction_t ) StartTask,
										(char*          ) "StartTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
										(uint32_t       ) START_STK_SIZE,
										(void *         ) NULL,
										(UBaseType_t    ) START_TASK_PRIO,
										(StackType_t *  ) StartTaskStack,
										(StaticTask_t * ) &StartTaskTCB );

					
	vTaskStartScheduler();		//��ʼ�������
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
										
										
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
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

