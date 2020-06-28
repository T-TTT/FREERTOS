

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
#define TASK1_TASK_PRIO		2
//�����ջ��С	
#define TASK1_STK_SIZE 		256 
//������
TaskHandle_t Task1Task_Handler;
//������
void Task1Task(void *pvParameters);

//�������ȼ�
#define DATAPRO_TASK_PRIO		3
//�����ջ��С	
#define DATAPRO_STK_SIZE 		256 
//������
TaskHandle_t DATAPROTask_Handler;
//������
void DATAPROTask(void *pvParameters);


//�ź������
SemaphoreHandle_t CountingSemaphore;



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
	
	CountingSemaphore = xSemaphoreCreateCounting(255,0);
	if(CountingSemaphore == NULL)
	{
		printf("�������ź�������ʧ��");
	}
	
	
	xTaskCreate((TaskFunction_t 	 ) Task1Task,
				(char *				 ) "Task1Task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) TASK1_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) TASK1_TASK_PRIO,
				(TaskHandle_t *		 ) &Task1Task_Handler );
	
	
	xTaskCreate((TaskFunction_t 	 ) DATAPROTask,
				(char *				 ) "DATAPROTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) DATAPRO_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) DATAPRO_TASK_PRIO,
				(TaskHandle_t *		 ) &DATAPROTask_Handler );
						
		
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
	taskEXIT_CRITICAL();
}


void Task1Task(void *pvParameters)
{
	u8 key;
	UBaseType_t semavalue;
    while(1)
    {
		key = KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			if(NULL != CountingSemaphore)
				xSemaphoreGive(CountingSemaphore);
				semavalue = uxSemaphoreGetCount(CountingSemaphore);
				printf("��ǰʣ�೵λΪ%d\r\n",10-semavalue);		
		}
		LED0=!LED0;
		vTaskDelay(10);
    }	
}
void DATAPROTask(void *pvParameters)
{
	UBaseType_t semavalue;
	u8 key,i;
    while(1)
    {
		key = KEY_Scan(0);
		if(NULL != CountingSemaphore)
		{
			if(key==KEY1_PRES)
			{
				xSemaphoreTake(CountingSemaphore,portMAX_DELAY);
				semavalue = uxSemaphoreGetCount(CountingSemaphore);
				printf("��ǰʣ�೵λΪ%d\r\n",10-(int)semavalue);
			}
		}
//		if(NULL != CountingSemaphore)
//		{
////			xSemaphoreTake(CountingSemaphore,portMAX_DELAY);
////			semavalue = uxSemaphoreGetCount(CountingSemaphore);
////			printf("semavalue = %d\n\r",(int)semavalue);
//			
//			if(USART_RX_BUF[0] != '\0')
//			{
//			printf("USART_RX_BUF = %s \r\n",USART_RX_BUF);
//			memset(USART_RX_BUF,0,USART_REC_LEN);
//			USART_RX_STA = 0;
//			}
//		}
		i++;			
		if(i==50)
		{
			semavalue = uxSemaphoreGetCount(CountingSemaphore);
			printf("��ǰʣ�೵λΪ%d\r\n",10-(int)semavalue);
		}
		vTaskDelay(10);
    }
}

