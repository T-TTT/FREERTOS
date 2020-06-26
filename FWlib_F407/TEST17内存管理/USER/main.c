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
#include "stdio.h"



//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void StartTask(void *pvParameters);


//�������ȼ�
#define MALLOC_TASK_PRIO		2
//�����ջ��С	
#define MALLOC_STK_SIZE 		256 
//������
TaskHandle_t mallocTask_Handler;
//������
void mallocTask(void *pvParameters);



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
									  
	//���񴴽�									  
	xTaskCreate((TaskFunction_t 	 ) mallocTask,
				(char *				 ) "mallocTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) MALLOC_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) MALLOC_TASK_PRIO,
				(TaskHandle_t *		 ) &mallocTask_Handler );
			
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
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
				printf("���뵽���ڴ��ַΪ%#x\r\n",(int)Buffer);
				printf("ʣ���ڴ��С��%d\r\n",xPortGetFreeHeapSize());
				break;
			case KEY1_PRES:
				vPortFree(Buffer);
				printf("�ڴ��ͷź���ڴ��ַΪ%#x\r\n",(int)Buffer);
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
					printf("�ڴ���Ч����Ҫ����\r\n");
				break;		
		}
		vTaskDelay(10);

    }	
}





