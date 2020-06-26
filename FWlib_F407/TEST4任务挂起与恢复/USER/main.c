

/***********���������ָ�***********************
******K_UPΪLED1�������************************
*******K0ΪLED0��������LED1�жϻָ���������****
*******K1ΪLED0����ָ�*************************/

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h"
#include "key.h"
#include "exti.h"

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void StartTask(void *pvParameters);

//�������ȼ�
#define KEY_TASK_PRIO		3
//�����ջ��С	
#define KEY_STK_SIZE 		128 
//������
TaskHandle_t KEYTask_Handler;
//������
void KEYTask(void *pvParameters);




//�������ȼ�
#define LED0_TASK_PRIO		3
//�����ջ��С	
#define LED0_STK_SIZE 		128 
//������
TaskHandle_t LED0Task_Handler;
//������
void LED0Task(void *pvParameters);

//�������ȼ�
#define LED1_TASK_PRIO		4
//�����ջ��С	
#define LED1_STK_SIZE 		128  
//������
TaskHandle_t LED1Task_Handler;
//������
void LED1Task(void *pvParameters);



int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	uart_init(115200);     	//��ʼ������
	LED_Init();		        //��ʼ��LED�˿�
	KEY_Init();
	EXTIX_Init();
	
	
	xTaskCreate(	(TaskFunction_t 	 ) StartTask,
					(char *				 ) "StartTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
					(uint16_t			 ) START_STK_SIZE,
					(void *				 ) NULL,
					(UBaseType_t		 ) START_TASK_PRIO,
					(TaskHandle_t *		 ) &StartTask_Handler );

					
	vTaskStartScheduler();		//��ʼ�������
}


void StartTask(void *pvParameters)
{
	taskENTER_CRITICAL();
	
	xTaskCreate((TaskFunction_t 	 ) KEYTask,
				(char *				 ) "KEYTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) KEY_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) KEY_TASK_PRIO,
				(TaskHandle_t *		 ) &KEYTask_Handler );
	
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
				

    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
	taskEXIT_CRITICAL();
}

void KEYTask(void *pvParameters)
{
	uint8_t key;
	while(1)
	{
		key = KEY_Scan(0);
		switch(key)
		{
			case KEY0_PRES:
				vTaskSuspend(LED0Task_Handler);
				printf("*****LED0���񱻹���*****\n\r");
				break;
			case KEY1_PRES:
				vTaskResume(LED0Task_Handler);
				printf("*****LED0����ָ�����*****\n\r");
				break;
			case WKUP_PRES:
				vTaskSuspend(LED1Task_Handler);
				printf("*****LED1���񱻹���*****\n\r");
				break;
		}
		vTaskDelay(10);

	}


}


void LED0Task(void *pvParameters)
{
	uint16_t num=0;
    while(1)
    {
		num++;
		LED0=!LED0;
		vTaskDelay(1000);
		printf("LED0����ִ�� %d ��\n\r",num);
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
		printf("LED1����ִ�� %d ��\n\r",num);
    }
}

