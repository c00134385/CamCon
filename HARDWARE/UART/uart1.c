/*----------------------------------------------------------------------------*/
/* uart1.c                                                                    */
/*                                                                            */
/* ���stdio.h��printf�ײ�����                                                */
/* 2014/6/10 manwjh ����                                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/* include */
#include "stdio.h"
#include "stm32f10x_it.h"
//
#include "uart1.h"

/* external */


/* public */


/* private */
static void USART1_Interrupts_Config(void);

#define RX_BUFFER_LEN 1
static unsigned char RxBuffer[RX_BUFFER_LEN+1];

#define TX_BUFFER_LEN	1
static unsigned char TxBuffer[TX_BUFFER_LEN];

static int RxCounter,TxCounter;

/* debug */



/*----------------------------------------------------------------------------*/
/*                                  Functions                                 */
/*----------------------------------------------------------------------------*/
void UART1_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef  USART_ClockInitStructure;
    
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 |RCC_APB2Periph_USART1, ENABLE  );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE  );
    /*
    *  USART1_TX -> PA9 , USART1_RX ->	PA10
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);		   
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //
    USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;                         // ʱ�ӵ͵�ƽ�
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;                               // ʱ�ӵ͵�ƽ
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;                             // ʱ�ӵڶ������ؽ������ݲ���
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;                     // ���һλ���ݵ�ʱ�����岻��SCLK���
    /* Configure the USART1 synchronous paramters */
    USART_ClockInit(USART1, &USART_ClockInitStructure);                                        // ʱ�Ӳ�����ʼ������
                                                                                                                                             
    USART_InitStructure.USART_BaudRate =57600;                                          // ������Ϊ��115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                          // 8λ����
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                               // ��֡��β����1��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No ;                                 // ��żʧ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;      // Ӳ��������ʧ��
    
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                      // ����ʹ��+����ʹ��
    /* Configure USART1 basic and asynchronous paramters */
    USART_Init(USART1, &USART_InitStructure);
        
    /* Enable USART1 */
    USART_ClearFlag(USART1, USART_IT_RXNE);                      //���жϣ�����һ�����жϺ����������ж�
    USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);                //ʹ��USART1�ж�Դ
    USART_Cmd(USART1, ENABLE);                                   //USART1�ܿ��أ�����
    //
    USART1_Interrupts_Config();
    //
    RxCounter = 0;
    TxCounter = 0;
}


static void USART1_Interrupts_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;   //ͨ������Ϊ����1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //�ж�ռ�ȵȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�ж���Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //���ж�   
    NVIC_Init(&NVIC_InitStructure);
}


int fputc(int ch, FILE *f)
{ 
    USART1->DR = (u8) ch; 

    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;

    return ch; 
}


int fgetc(FILE *f)
{
   while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET));
    
   return (USART_ReceiveData(USART1));
}


/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
  //�жϽ���
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    if( RxCounter >= RX_BUFFER_LEN )
          RxCounter = 0;
    RxBuffer[RxCounter++] = (USART_ReceiveData(USART1) & 0x7F);
  }
  //�жϷ���
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {
    USART_SendData(USART1, TxBuffer[TxCounter++]);                    
    if(TxCounter >= TX_BUFFER_LEN)
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
  }
}



