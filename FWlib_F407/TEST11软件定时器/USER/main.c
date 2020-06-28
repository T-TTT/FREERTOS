#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h"
#include "queue.h"
#include "timers.h"

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void StartTask(void *pvParameters);


//�������ȼ�
#define TIMER_TASK_PRIO		2
//�����ջ��С	
#define TIMER_STK_SIZE 		128 
//������
TaskHandle_t timerTask_Handler;
//������
void timerTask(void *pvParameters);


//��ʱ������ͻص�����
TimerHandle_t AutoReloadTimer_Handle;
TimerHandle_t OneShotTimer_Handle;
void AutoReloadCallback( TimerHandle_t xTimer );
void OneShotCallback( TimerHandle_t xTimer );



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
	
	
	//��ʱ������
	AutoReloadTimer_Handle = xTimerCreate((char *                 ) "AutoReloadTimer",		//����
										  (TickType_t             ) 1000,					//����
										  (UBaseType_t            ) pdTRUE,					//������
										  (void *                 ) 1,						//id��
										  (TimerCallbackFunction_t) AutoReloadCallback );	//�ص�������
										  
										  
	OneShotTimer_Handle   =  xTimerCreate((char *                 ) "OneShotTimer",			
										  (TickType_t             ) 2000,
										  (UBaseType_t            ) pdFALSE,				//������
										  (void *                 ) 2,
										  (TimerCallbackFunction_t) OneShotCallback );
										  
										  
	//���񴴽�									  
	xTaskCreate((TaskFunction_t 	 ) timerTask,
				(char *				 ) "timerTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) TIMER_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) TIMER_TASK_PRIO,
				(TaskHandle_t *		 ) &timerTask_Handler );
	
	
			
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
	taskEXIT_CRITICAL();
				
}


void timerTask(void *pvParameters)
{
	u8 key;
	BaseType_t err;
    while(1)
    {
		key = KEY_Scan(0);
		switch (key)
		{
			case WKUP_PRES:
					err = xTimerStart(AutoReloadTimer_Handle,0);
					if(err == pdFAIL)
					{
						printf("�����Զ�ʱ������ʧ��\r\n");
					}
					else
						printf("�����Զ�ʱ�������ɹ�\r\n");
					break;
					
			case KEY0_PRES:
					err = xTimerStart(OneShotTimer_Handle,0);
					if(err == pdFAIL)
					{
						printf("���ζ�ʱ������ʧ��\r\n");
					}
					else
						printf("�����Զ�ʱ�������ɹ�\r\n");
					break;
			case KEY1_PRES:
					err = xTimerStop(AutoReloadTimer_Handle,0);
					if(err == pdFAIL)
					{
						printf("�����Զ�ʱ���ر�ʧ��\r\n");
					}
					else
						printf("�����Զ�ʱ���رճɹ�\r\n");
					break;
		}
		vTaskDelay(10);
    }	

}


//���ڶ�ʱ���ص�����
void AutoReloadCallback( TimerHandle_t xTimer )		
{
	static u8 timers=0;
	timers++;
	printf("AutoReloadTimer running : %d\r\n",timers);
}

//���ζ�ʱ���ص�����
void OneShotCallback( TimerHandle_t xTimer )		
{
	static u8 timers=0;
	timers++;
	printf("OneShotTimer running : %d\r\n",timers);
}


