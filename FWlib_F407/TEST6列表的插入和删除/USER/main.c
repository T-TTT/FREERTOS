#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h"
#include "key.h"

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void StartTask(void *pvParameters);


//�������ȼ�
#define LED0_TASK_PRIO		2
//�����ջ��С	
#define LED0_STK_SIZE 		128  
//������
TaskHandle_t LED0Task_Handler;
//������
void LED0Task(void *pvParameters);

//�������ȼ�
#define LIST_TASK_PRIO		3
//�����ջ��С	
#define LIST_STK_SIZE 		128  
//������
TaskHandle_t ListTask_Handler;
//������
void ListTask(void *pvParameters);




//����һ���б�������б���
List_t TestList;
ListItem_t ListItem1;
ListItem_t ListItem2;
ListItem_t ListItem3;


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
	
	xTaskCreate((TaskFunction_t 	 ) LED0Task,
				(char *				 ) "LED0Task",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) LED0_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) LED0_TASK_PRIO,
				(TaskHandle_t *		 ) &LED0Task_Handler );
	
	
	xTaskCreate((TaskFunction_t 	 ) ListTask,
				(char *				 ) "ListTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
				(uint16_t			 ) LIST_STK_SIZE,
				(void *				 ) NULL,
				(UBaseType_t		 ) LIST_TASK_PRIO,
				(TaskHandle_t *		 ) &ListTask_Handler );
				

    vTaskDelete(StartTask_Handler); //ɾ����ʼ����	
	taskEXIT_CRITICAL();
}


void LED0Task(void *pvParameters)
{
	uint16_t num=0;
    while(1)
    {
		num++;
		LED0=!LED0;
		vTaskDelay(1000);
		//printf("����1ִ�� %d ��\n\r",num);
    }	

}
void ListTask(void *pvParameters)
{
	
//	uint16_t num=0;
//    while(1)
//    {
//		num++;
//		LED1=!LED1;
//		vTaskDelay(1000);
//		printf("����2ִ�� %d ��\n\r",num);
//    }	
	//��ʼ��
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	
	//��ֵ������ʵ��۲�
	ListItem1.xItemValue = 40;
	ListItem2.xItemValue = 60;
	ListItem3.xItemValue = 50;
	
	
	//��ӡ�б����б���
	printf("/***************�б�����б�ĵ�ַ*******************/\r\n");
	printf("��Ŀ\t\t\t��ַ\t\t\t\r\n");
	printf("TestList\t\t\t %#x \t\t\t\r\n",(int)&TestList);
	printf("TestList->pxIndex\t\t %#x \t\t\t\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd\t\t %#x \t\t\t\r\n",(int)&TestList.xListEnd);
	printf("TestListItem1\t\t\t %#x \t\t\t\r\n",(int)&ListItem1);
	printf("TestListItem2\t\t\t %#x \t\t\t\r\n",(int)&ListItem2);
	printf("TestListItem3\t\t\t %#x \t\t\t\r\n",(int)&ListItem3);
	printf("/******************�밴��K_UP����*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);
	
	//����б���1���б���
	vListInsert(&TestList,&ListItem1);
	
	printf("/***************����б���1���б���*******************/\r\n");
	printf("��Ŀ\t\t\t��ַ\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);

	printf("/******************�밴��K_UP����*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);	
	
	
	//����б���2���б���
	vListInsert(&TestList,&ListItem2);
	
	printf("/***************����б���2���б���*******************/\r\n");
	printf("��Ŀ\t\t\t��ַ\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);
	
	printf("TestListItem2>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem2.pxNext);
	printf("TestListItem2->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem2.pxPrevious);
	printf("/******************�밴��K_UP����*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);	
	
	
	//����б���3���б���
	vListInsert(&TestList,&ListItem3);
	
	printf("/***************����б���3���б���*******************/\r\n");
	printf("��Ŀ\t\t\t��ַ\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);
	
	printf("TestListItem2>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem2.pxNext);
	printf("TestListItem2->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem2.pxPrevious);
	
	printf("TestListItem3>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem3.pxNext);
	printf("TestListItem3->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem3.pxPrevious);
	printf("/******************�밴��K_UP����*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);	
	
	
	//ɾ���б���2���б���
	uxListRemove(&ListItem2);
	
	printf("/***************ɾ���б���2���б���*******************/\r\n");
	printf("��Ŀ\t\t\t��ַ\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);
	

	printf("TestListItem3>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem3.pxNext);
	printf("TestListItem3->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem3.pxPrevious);
	
	printf("/******************�밴��K_UP����*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);		
	

	//β������б���2���б���
	vListInsertEnd(&TestList,&ListItem2);
	
	printf("/***************β������б���2���б���*******************/\r\n");
	printf("��Ŀ\t\t\t��ַ\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);
	
	printf("TestListItem2>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem2.pxNext);
	printf("TestListItem2->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem2.pxPrevious);
	
	printf("TestListItem3>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem3.pxNext);
	printf("TestListItem3->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem3.pxPrevious);
	printf("/******************����*********************/\r\n");
	while(1)
	{
		LED1=!LED1;
		vTaskDelay(1000);
	}
}

