#include "exti.h"
#include "mpr121.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//�ⲿ�ж�0�������
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

    //	�����˿ڳ�ʼ��

	
	//��ʼ��KEY0-->GPIOA.13,KEY1-->GPIOA.15  ��������
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTA,PORTEʱ��

	//��ʼ�� WK_UP-->GPIOA.0	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó����룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.8


  //GPIOA.0	  �ж����Լ��жϳ�ʼ������ �����ش��� PA0  WK_UP
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5); 
 	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	//EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_Init(&EXTI_InitStructure);		//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ���WK_UP���ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}

//�ⲿ�ж�0������� 
void EXTI0_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);

	EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ  
}
void EXTI1_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);

	EXTI_ClearITPendingBit(EXTI_Line1); //���LINE0�ϵ��жϱ�־λ  
}

//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);	 
	EXTI_ClearITPendingBit(EXTI_Line2);  //���LINE2�ϵ��жϱ�־λ  
}
//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);	 
	EXTI_ClearITPendingBit(EXTI_Line3);  //���LINE3�ϵ��жϱ�־λ  
}

void EXTI4_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);	 
	EXTI_ClearITPendingBit(EXTI_Line4);  //���LINE4�ϵ��жϱ�־λ  
}

void EXTI9_5_IRQHandler(void)
{
	//printf("\r\n %s()~~~~~~~~~~~~~~~~", __FUNCTION__);
	
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		#ifdef MPR121_ENABLE
        mpr121_notify();
		#endif
		//printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line5);   //��� LINE �ϵ��жϱ�־λ
	}
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line6);   //��� LINE �ϵ��жϱ�־λ
	}
	if(EXTI_GetITStatus(EXTI_Line7)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line7);   //��� LINE �ϵ��жϱ�־λ
	}
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line8);   //��� LINE �ϵ��жϱ�־λ
	}
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line9);   //��� LINE �ϵ��жϱ�־λ
	}     		 
}

void EXTI15_10_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);
	if(EXTI_GetITStatus(EXTI_Line10)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line10);   //��� LINE �ϵ��жϱ�־λ
	}
	if(EXTI_GetITStatus(EXTI_Line11)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line11);   //��� LINE �ϵ��жϱ�־λ
	}
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line12);   //��� LINE �ϵ��жϱ�־λ
	}
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line13);   //��� LINE �ϵ��жϱ�־λ
	}
	if(EXTI_GetITStatus(EXTI_Line14)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line14);   //��� LINE �ϵ��жϱ�־λ
	}
	if(EXTI_GetITStatus(EXTI_Line15)!=RESET)//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line15);   //��� LINE �ϵ��жϱ�־λ
	}  
	//delay_ms(10);//����
		 
	EXTI_ClearITPendingBit(EXTI_Line12);  //���LINE4�ϵ��жϱ�־λ  
	EXTI_ClearITPendingBit(EXTI_Line13);
	EXTI_ClearITPendingBit(EXTI_Line14);
	EXTI_ClearITPendingBit(EXTI_Line15);
} 
