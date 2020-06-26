#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h"

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void StartTask(void *pvParameters);


//�������ȼ�
#define LED0_TASK_PRIO		3
//�����ջ��С	
#define LED0_STK_SIZE 		50  
//������
TaskHandle_t LED0Task_Handler;
//������
void LED0Task(void *pvParameters);

//�������ȼ�
#define LED1_TASK_PRIO		2
//�����ջ��С	
#define LED1_STK_SIZE 		50  
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


void LED0Task(void *pvParameters)
{
	uint16_t num=0;
    while(1)
    {
		num++;
//		if(num == 5)
//		{
//			vTaskDelete(LED1Task_Handler);
//			printf("����2��ɾ��\n\r");
//			LED1=1;
//		}
		LED0=!LED0;
		
		printf("��������ΰ�ڣ�Task1ִ�� %d ��\n\r",num);
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
		printf("ѧ�ţ�18010123��Task2ִ�� %d ��\n\r",num);
		vTaskDelay(2000);
    }
}

