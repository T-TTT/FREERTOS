

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
#include "timer.h"
#include "queue.h"
#include "string.h"
#include "semphr.h"


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void StartTask(void *pvParameters);


//�������ȼ�
#define LOW_TASK_PRIO		2
//�����ջ��С	
#define LOW_STK_SIZE 		256 
//������
TaskHandle_t lowTask_Handler;
//������
void lowTask(void *pvParameters);

//�������ȼ�
#define MED_TASK_PRIO		3
//�����ջ��С	
#define MED_STK_SIZE 		256 
//������
TaskHandle_t medTask_Handler;
//������
void medTask(void *pvParameters);

//�������ȼ�
#define HEIGHT_TASK_PRIO		4
//�����ջ��С	
#define HEIGHT_STK_SIZE 		256 
//������
TaskHandle_t heightTask_Handler;
//������
void heightTask(void *pvParameters);




//�ź������
SemaphoreHandle_t MutexSemaphore;



int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);								//��ʼ����ʱ����
	uart_init(115200);     							//��ʼ������
	LED_Init();		       							 //��ʼ��LED�˿�
	KEY_Init();

	
	
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
	
	MutexSemaphore = xSemaphoreCreateMutex();
	if(MutexSemaphore == NULL)
	{
		printf("�����ź�������ʧ��");
	}
	
	
	xTaskCreate((TaskFunction_t 	 ) lowTask,
				(char *				 ) "lowTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) LOW_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) LOW_TASK_PRIO,
				(TaskHandle_t *		 ) &lowTask_Handler );
	
	
	xTaskCreate((TaskFunction_t 	 ) medTask,
				(char *				 ) "medTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) MED_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) MED_TASK_PRIO,
				(TaskHandle_t *		 ) &medTask_Handler );
				
	xTaskCreate((TaskFunction_t 	 ) heightTask,
				(char *				 ) "heightTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) HEIGHT_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) HEIGHT_TASK_PRIO,
				(TaskHandle_t *		 ) &heightTask_Handler );
						
		
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
	taskEXIT_CRITICAL();
}


void lowTask(void *pvParameters)
{
	static u32 times;
    while(1)
    {
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);
		printf("low task running!\r\n");
		for(times=0;times<20000;times++)
		{
			delay_us(2);
		}
		xSemaphoreGive(MutexSemaphore);
		vTaskDelay(1000);

    }	
}
void medTask(void *pvParameters)
{

    while(1)
    {
		printf("mid task running!\r\n");
		vTaskDelay(1000);
    }
}

void heightTask(void *pvParameters)
{
	while(1)
	{
		printf("high task Pend Sem!\r\n");
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);
		
		xSemaphoreGive(MutexSemaphore);
		printf("high task running!\r\n");
		vTaskDelay(1000);
	
	}


}

