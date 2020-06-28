#include "exti.h"
#include "delay.h" 
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
	   
//�ⲿ�жϳ�ʼ������
//��ʼ��PE2~4,PA0Ϊ�ж�����.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	KEY_Init(); //������Ӧ��IO�ڳ�ʼ��
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);//PE3 ���ӵ��ж���3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);//PE4 ���ӵ��ж���4

	
	/* ����EXTI_Line3,4 */
	EXTI_InitStructure.EXTI_Line =  EXTI_Line3 | EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
	EXTI_Init(&EXTI_InitStructure);//����
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//�ⲿ�ж�3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//�ⲿ�ж�4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x07;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//����
	   
}

extern QueueHandle_t Key_Queue;	//���о��

//�ⲿ�ж�4�������
void EXTI4_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken0;
	u8 key = 1;
	delay_xms(20);	//����
	if(KEY0==0)	 
	{				 
		xQueueSendFromISR(Key_Queue,&key,&xHigherPriorityTaskWoken0);//������з�������
		
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken0);//�����Ҫ�Ļ�����һ�������л�
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line4);//���LINE4�ϵ��жϱ�־λ  
}


void EXTI3_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken1;
	u8 key = 2;
	delay_xms(20);	//����
	if(KEY1==0)	 
	{				 
		xQueueSendToFrontFromISR(Key_Queue,&key,&xHigherPriorityTaskWoken1);//������з�������
		
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken1);//�����Ҫ�Ļ�����һ�������л�
	}		 
	 EXTI_ClearITPendingBit(EXTI_Line3);//���LINE4�ϵ��жϱ�־λ  
}






