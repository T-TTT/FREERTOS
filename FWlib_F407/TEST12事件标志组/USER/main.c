#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "event_groups.h"

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void StartTask(void *pvParameters);


//�������ȼ�
#define SETBIT_TASK_PRIO		2
//�����ջ��С	
#define SETBIT_STK_SIZE 		256 
//������
TaskHandle_t setbitTask_Handler;
//������
void setbitTask(void *pvParameters);


//�������ȼ�
#define GROUP_TASK_PRIO		3
//�����ջ��С	
#define GROUP_STK_SIZE 		256 
//������
TaskHandle_t groupTask_Handler;
//������
void groupTask(void *pvParameters);



EventGroupHandle_t GroupHandle;
#define			EVENTBIT_0   	(1<<0)
#define			EVENTBIT_1   	(1<<1)
#define			EVENTBIT_2   	(1<<2)
#define 		EVENTBIT_ALL	(EVENTBIT_1|EVENTBIT_2)


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init(168);		//��ʼ����ʱ����
	uart_init(115200);     	//��ʼ������
	LED_Init();		        //��ʼ��LED�˿�
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
	
	//�����¼���־��
	GroupHandle = xEventGroupCreate();
	if(GroupHandle == NULL)
	{
		printf("�¼���־�鴴��ʧ��\r\n");
	}
	
										  
	//���񴴽�									  
	xTaskCreate((TaskFunction_t 	 ) setbitTask,
				(char *				 ) "setbitTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) SETBIT_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) SETBIT_TASK_PRIO,
				(TaskHandle_t *		 ) &setbitTask_Handler );
	
	//���񴴽�									  
	xTaskCreate((TaskFunction_t 	 ) groupTask,
				(char *				 ) "groupTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) GROUP_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) GROUP_TASK_PRIO,
				(TaskHandle_t *		 ) &groupTask_Handler );	
			
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
	taskEXIT_CRITICAL();
				
}


void setbitTask(void *pvParameters)
{
	u8 key;
    while(1)
    {
		key = KEY_Scan(0);
		
		switch(key)
		{
			case KEY1_PRES:
				xEventGroupSetBits( GroupHandle, EVENTBIT_1 );
				break;
			case KEY0_PRES:
				xEventGroupSetBits( GroupHandle, EVENTBIT_2 );
				break;
		}
		vTaskDelay(10);
    }	

}

void groupTask(void *pvParameters)
{
	EventBits_t EventValue;
    while(1)
    {
		if(GroupHandle != NULL)
		{
		EventValue =xEventGroupWaitBits((EventGroupHandle_t) GroupHandle,
										(EventBits_t	   ) EVENTBIT_ALL,
										(BaseType_t		   ) pdTRUE,
										(BaseType_t        ) pdTRUE,
										(TickType_t        ) portMAX_DELAY );		
	
		printf("All bit set 1;EventValue = %#x\r\n",EventValue);
		vTaskDelay(10);
		}
		else
		{
		vTaskDelay(10);	
		}
    }	

}



