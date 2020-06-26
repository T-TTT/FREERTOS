

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
#include "limits.h"


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
    while(1)
    {
		key = KEY_Scan(0);
		if((KEYPROTask_Handler != NULL)&&(key))					//�ж϶�����Ч�밴������
		{
			xTaskNotify( KEYPROTask_Handler, (uint32_t) key,eSetValueWithOverwrite);
		}
		vTaskDelay(10);
    }	
}
void KEYPROTask(void *pvParameters)
{
	u8 key=1;
	BaseType_t err;
    while(1)
    {
		err = xTaskNotifyWait( 0, ULONG_MAX, (uint32_t*)&key, portMAX_DELAY );
		if(err == pdPASS)
		{
			printf("KEY��ֵ����%d\n\r",key);
			switch(key)
			{
				case WKUP_PRES:
					LED0 =!LED0;
					break;
				case KEY0_PRES:
					LED1 =!LED1;
					break;
				case KEY1_PRES:
					printf("����\r\n");
					break;
			}
		}
    }
}

