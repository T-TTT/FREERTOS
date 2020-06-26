#include "exti.h"
#include "delay.h" 
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
	   
//外部中断初始化程序
//初始化PE2~4,PA0为中断输入.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	KEY_Init(); //按键对应的IO口初始化
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);//PE3 连接到中断线3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);//PE4 连接到中断线4

	
	/* 配置EXTI_Line3,4 */
	EXTI_InitStructure.EXTI_Line =  EXTI_Line3 | EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
	EXTI_Init(&EXTI_InitStructure);//配置
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//外部中断3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//外部中断4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x07;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	   
}

extern QueueHandle_t Key_Queue;	//队列句柄

//外部中断4服务程序
void EXTI4_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken0;
	u8 key = 1;
	delay_xms(20);	//消抖
	if(KEY0==0)	 
	{				 
		xQueueSendFromISR(Key_Queue,&key,&xHigherPriorityTaskWoken0);//向队列中发送数据
		
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken0);//如果需要的话进行一次任务切换
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line4);//清除LINE4上的中断标志位  
}


void EXTI3_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken1;
	u8 key = 2;
	delay_xms(20);	//消抖
	if(KEY1==0)	 
	{				 
		xQueueSendToFrontFromISR(Key_Queue,&key,&xHigherPriorityTaskWoken1);//向队列中发送数据
		
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken1);//如果需要的话进行一次任务切换
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line3);//清除LINE4上的中断标志位  
}






