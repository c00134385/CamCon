#include "exti.h"
#include "mpr121.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//外部中断0服务程序
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

    //	按键端口初始化

	
	//初始化KEY0-->GPIOA.13,KEY1-->GPIOA.15  上拉输入
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTE时钟

	//初始化 WK_UP-->GPIOA.0	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.8


  //GPIOA.0	  中断线以及中断初始化配置 上升沿触发 PA0  WK_UP
 	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5); 
 	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	//EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_Init(&EXTI_InitStructure);		//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

  	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键WK_UP所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
}

//外部中断0服务程序 
void EXTI0_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);

	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
}
void EXTI1_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);

	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE0上的中断标志位  
}

//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);	 
	EXTI_ClearITPendingBit(EXTI_Line2);  //清除LINE2上的中断标志位  
}
//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);	 
	EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位  
}

void EXTI4_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);	 
	EXTI_ClearITPendingBit(EXTI_Line4);  //清除LINE4上的中断标志位  
}

void EXTI9_5_IRQHandler(void)
{
	//printf("\r\n %s()~~~~~~~~~~~~~~~~", __FUNCTION__);
	
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET)//判断某个线上的中断是否发生 
	{
		#ifdef MPR121_ENABLE
        mpr121_notify();
		#endif
		//printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line5);   //清除 LINE 上的中断标志位
	}
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line6);   //清除 LINE 上的中断标志位
	}
	if(EXTI_GetITStatus(EXTI_Line7)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line7);   //清除 LINE 上的中断标志位
	}
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line8);   //清除 LINE 上的中断标志位
	}
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line9);   //清除 LINE 上的中断标志位
	}     		 
}

void EXTI15_10_IRQHandler(void)
{
	printf("\r\n %s()", __FUNCTION__);
	if(EXTI_GetITStatus(EXTI_Line10)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line10);   //清除 LINE 上的中断标志位
	}
	if(EXTI_GetITStatus(EXTI_Line11)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line11);   //清除 LINE 上的中断标志位
	}
	if(EXTI_GetITStatus(EXTI_Line12)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line12);   //清除 LINE 上的中断标志位
	}
	if(EXTI_GetITStatus(EXTI_Line13)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line13);   //清除 LINE 上的中断标志位
	}
	if(EXTI_GetITStatus(EXTI_Line14)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line14);   //清除 LINE 上的中断标志位
	}
	if(EXTI_GetITStatus(EXTI_Line15)!=RESET)//判断某个线上的中断是否发生 
	{
		printf("\r\n L:%d %s()",__LINE__, __FUNCTION__);
		EXTI_ClearITPendingBit(EXTI_Line15);   //清除 LINE 上的中断标志位
	}  
	//delay_ms(10);//消抖
		 
	EXTI_ClearITPendingBit(EXTI_Line12);  //清除LINE4上的中断标志位  
	EXTI_ClearITPendingBit(EXTI_Line13);
	EXTI_ClearITPendingBit(EXTI_Line14);
	EXTI_ClearITPendingBit(EXTI_Line15);
} 
