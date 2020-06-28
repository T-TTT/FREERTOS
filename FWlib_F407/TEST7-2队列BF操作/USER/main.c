

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


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void StartTask(void *pvParameters);


//�������ȼ�
#define KEYSCAN_TASK_PRIO		2
//�����ջ��С	
#define KEYSCAN_STK_SIZE 		128 
//������
TaskHandle_t KEYSTask_Handler;
//������
void KEYSTask(void *pvParameters);

//�������ȼ�
#define KEYPRO_TASK_PRIO		3
//�����ջ��С	
#define KEYPRO_STK_SIZE 		128  
//������
TaskHandle_t KEYPROTask_Handler;
//������
void KEYPROTask(void *pvParameters);


QueueHandle_t Key_Queue;				//��������
QueueHandle_t Message_Queue;
#define KEYMSG_Q_NUM			3
#define MESSAGE_Q_NUM			4


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);								//��ʼ����ʱ����
	uart_init(115200);     							//��ʼ������
	LED_Init();		       							 //��ʼ��LED�˿�
	KEY_Init();
	TIM7_Int_Init(5000-1,16800-1);				//10k*5000=500ms
	
	
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
	
	Key_Queue = xQueueCreate(KEYMSG_Q_NUM,24);
	if(NULL == Key_Queue)
	{
		printf("���д���ʧ��");
	}
	
	Message_Queue = xQueueCreate(MESSAGE_Q_NUM,USART_REC_LEN);
	if(NULL == Message_Queue)
	{
		printf("���д���ʧ��");
	}
	
	xTaskCreate((TaskFunction_t 	 ) KEYSTask,
				(char *				 ) "KEYSTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) KEYSCAN_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) KEYSCAN_TASK_PRIO,
				(TaskHandle_t *		 ) &KEYSTask_Handler );
	
	
	xTaskCreate((TaskFunction_t 	 ) KEYPROTask,
				(char *				 ) "KEYPROTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) KEYPRO_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) KEYPRO_TASK_PRIO,
				(TaskHandle_t *		 ) &KEYPROTask_Handler );
						

				
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
	taskEXIT_CRITICAL();
}


void KEYSTask(void *pvParameters)
{
	u8 key;
	BaseType_t err;
    while(1)
    {
		key = KEY_Scan(0);
		if((Key_Queue != NULL)&&(key))					//�ж϶�����Ч�밴������
		{

			if(key == 1)
			{
				err=xQueueSendToBack(Key_Queue,&key,10);
			}
			else if(key == 2)
			{
				err=xQueueSendToFront(Key_Queue,&key,10);			
				//����������K1,������K2����K2��
				//��Ϊ�Ҷ���ֻ��24�֣�������ѡ���Ȱ�����K1���ڵڶ���K1û��ӡ�������ٰ���K2��
			}
			
			if(err != pdPASS)
			{
				printf("���з���ʧ��\r\n");
			}		
		}
		vTaskDelay(10);
    }	

}
void KEYPROTask(void *pvParameters)
{
	BaseType_t err;
	u8 key;
    while(1)
    {
		if(NULL != Key_Queue)
		{
		err = xQueueReceive(Key_Queue,&key,portMAX_DELAY);
			if(err ==pdTRUE)
			{
				printf("KEY��ֵ����%d\n\r",key);

			}
		}
		vTaskDelay(2000);
    }
}

