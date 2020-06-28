#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h"
#include "key.h"

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void StartTask(void *pvParameters);


//任务优先级
#define LED0_TASK_PRIO		2
//任务堆栈大小	
#define LED0_STK_SIZE 		128  
//任务句柄
TaskHandle_t LED0Task_Handler;
//任务函数
void LED0Task(void *pvParameters);

//任务优先级
#define LIST_TASK_PRIO		3
//任务堆栈大小	
#define LIST_STK_SIZE 		128  
//任务句柄
TaskHandle_t ListTask_Handler;
//任务函数
void ListTask(void *pvParameters);




//定义一个列表和三个列表项
List_t TestList;
ListItem_t ListItem1;
ListItem_t ListItem2;
ListItem_t ListItem3;


int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init(168);		//初始化延时函数
	uart_init(115200);     	//初始化串口
	LED_Init();		        //初始化LED端口
	KEY_Init();
	
	xTaskCreate(	(TaskFunction_t 	 ) StartTask,
					(char *				 ) "StartTask",		/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
					(uint16_t			 ) START_STK_SIZE,
					(void *				 ) NULL,
					(UBaseType_t		 ) START_TASK_PRIO,
					(TaskHandle_t *		 ) &StartTask_Handler );

					
	vTaskStartScheduler();		//开始任务调度
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
				

    vTaskDelete(StartTask_Handler); //删除开始任务	
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
		//printf("任务1执行 %d 次\n\r",num);
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
//		printf("任务2执行 %d 次\n\r",num);
//    }	
	//初始化
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	
	//赋值，方便实验观察
	ListItem1.xItemValue = 40;
	ListItem2.xItemValue = 60;
	ListItem3.xItemValue = 50;
	
	
	//打印列表与列表项
	printf("/***************列表项和列表的地址*******************/\r\n");
	printf("项目\t\t\t地址\t\t\t\r\n");
	printf("TestList\t\t\t %#x \t\t\t\r\n",(int)&TestList);
	printf("TestList->pxIndex\t\t %#x \t\t\t\r\n",(int)TestList.pxIndex);
	printf("TestList->xListEnd\t\t %#x \t\t\t\r\n",(int)&TestList.xListEnd);
	printf("TestListItem1\t\t\t %#x \t\t\t\r\n",(int)&ListItem1);
	printf("TestListItem2\t\t\t %#x \t\t\t\r\n",(int)&ListItem2);
	printf("TestListItem3\t\t\t %#x \t\t\t\r\n",(int)&ListItem3);
	printf("/******************请按下K_UP继续*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);
	
	//添加列表项1到列表中
	vListInsert(&TestList,&ListItem1);
	
	printf("/***************添加列表项1到列表中*******************/\r\n");
	printf("项目\t\t\t地址\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);

	printf("/******************请按下K_UP继续*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);	
	
	
	//添加列表项2到列表中
	vListInsert(&TestList,&ListItem2);
	
	printf("/***************添加列表项2到列表中*******************/\r\n");
	printf("项目\t\t\t地址\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);
	
	printf("TestListItem2>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem2.pxNext);
	printf("TestListItem2->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem2.pxPrevious);
	printf("/******************请按下K_UP继续*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);	
	
	
	//添加列表项3到列表中
	vListInsert(&TestList,&ListItem3);
	
	printf("/***************添加列表项3到列表中*******************/\r\n");
	printf("项目\t\t\t地址\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);
	
	printf("TestListItem2>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem2.pxNext);
	printf("TestListItem2->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem2.pxPrevious);
	
	printf("TestListItem3>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem3.pxNext);
	printf("TestListItem3->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem3.pxPrevious);
	printf("/******************请按下K_UP继续*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);	
	
	
	//删除列表项2到列表中
	uxListRemove(&ListItem2);
	
	printf("/***************删除列表项2到列表中*******************/\r\n");
	printf("项目\t\t\t地址\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);
	

	printf("TestListItem3>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem3.pxNext);
	printf("TestListItem3->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem3.pxPrevious);
	
	printf("/******************请按下K_UP继续*********************/\r\n");
	while(KEY_Scan(0)!=WKUP_PRES) delay_ms(10);		
	

	//尾部添加列表项2到列表中
	vListInsertEnd(&TestList,&ListItem2);
	
	printf("/***************尾部添加列表项2到列表中*******************/\r\n");
	printf("项目\t\t\t地址\t\t\t\r\n");
	printf("TestList->xListEnd->pxPre\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxPrevious);
	printf("TestList->xListEnd->pxNext\t %#x \t\t\t\r\n",(int)TestList.xListEnd.pxNext);
	
	
	printf("TestListItem1>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem1.pxNext);
	printf("TestListItem1->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem1.pxPrevious);
	
	printf("TestListItem2>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem2.pxNext);
	printf("TestListItem2->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem2.pxPrevious);
	
	printf("TestListItem3>pxNext\t\t %#x \t\t\t\r\n",(int)ListItem3.pxNext);
	printf("TestListItem3->pxPre\t\t %#x \t\t\t\r\n",(int)ListItem3.pxPrevious);
	printf("/******************结束*********************/\r\n");
	while(1)
	{
		LED1=!LED1;
		vTaskDelay(1000);
	}
}

