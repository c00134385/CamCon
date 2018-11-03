/*----------------------------------------------------------------------------*/
/* uart2.c                                                                    */
/*                                                                            */
/* gprs������д�Ĵ��ڽ��ղ���,���䲿��������Ҫ��.                             */
/* ���ղ���packge(timeout & ��ͷ��β                                          */
/* 2014/6/10 manwjh ����                                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/* include */
#include "stm32f10x_it.h"
#include "stdio.h"
#include "string.h"
//#include "systick.h"
//
#include "uart2.h"

/* external */


/* public */
#define UART2_RX_BUFFER_LEN 100
static unsigned char Uart2_RxBuffer[UART2_RX_BUFFER_LEN];

#define UART2_TX_BUFFER_LEN	4
static unsigned char Uart2_TxBuffer[UART2_TX_BUFFER_LEN+1];

static int Uart2_RxCounter,Uart2_TxCounter;
static int Uart2_RxReadPointer = 0;

static unsigned long rec_systick_mark=0;

static unsigned char Uart2_temp_buffer[UART2_RX_BUFFER_LEN];
static int Uart2_temp_rx_counter = 0;

static uart2_input_byte_callback input_callback = NULL; 


int gps_rec_count;

//int uart2_rx_complete = 0;

/* private */
void USART2_Interrupts_Config(void);
#define GPS_PKG_TIMEOUT     40   //?x20ms

/* debug */


/*----------------------------------------------------------------------------*/
/*                                  Functions                                 */
/*----------------------------------------------------------------------------*/

int uart2_comm_get_len(void)
{
    #if 0
    int rx_cnt=gps_rec_count,i;

    if( rx_cnt==0 )
        return 0;

    do{
        rx_cnt = gps_rec_count;
        //Wait10Ms(1);
        for( i=0; i<10000; i++ ){
		    if( gps_rec_count>rx_cnt )
		        break;
		}
    }while( gps_rec_count>rx_cnt );
    
    return gps_rec_count;
    #else

    if(Uart2_RxCounter == Uart2_RxReadPointer) {
        return 0;
    } else if(Uart2_RxCounter > Uart2_RxReadPointer) {
        return (Uart2_RxCounter - Uart2_RxReadPointer);
    } else {
        return (UART2_RX_BUFFER_LEN - Uart2_RxReadPointer + Uart2_RxCounter);
    }
    #endif
}

int uart2_comm_message_available(void) {
    int ret = 0;
    int rxIndex = Uart2_RxReadPointer;
    while(rxIndex != Uart2_RxCounter) {
        if(Uart2_RxBuffer[rxIndex++] == 0xFF) {
            ret = 1;
            break;
        }
        if(rxIndex >= UART2_RX_BUFFER_LEN) {
            rxIndex = 0;
        }
    }
    return ret;
}

int uart2_comm_read(unsigned char *buff, int length)
{
    int i = 0;
    for(i = 0; i < length; i++) {
        buff[i] = Uart2_RxBuffer[Uart2_RxReadPointer++];
        if(Uart2_RxReadPointer >= UART2_RX_BUFFER_LEN) {
            Uart2_RxReadPointer = 0;
        }
        if(buff[i] == 0xff) {
            i++;
            break;
        }
    }
	return i;
}

void uart2_comm_write(unsigned char *buff, int length)
{
    int i;
    unsigned char *p =buff;
    
    for( i=0; i<length; i++){
        uart2_fputc(*p++);
    }
    //gprs_tx_cnt = gprs_tx_length;
    //sonylens_tx_cnt = sonylens_tx_length;
}

void uart2_comm_clr(void)
{
    //gps_rec_count = 0;
    //memset(Uart2_RxBuffer, 0, UART2_RX_BUFFER_LEN);
    //Uart2_RxCounter = 0;
    Uart2_RxReadPointer = Uart2_RxCounter;
}

void uart2_print_rx_data(void) {
    int i = 0;
    printf("\r\n rx_data_index: %d-%d", Uart2_RxCounter, Uart2_RxReadPointer);
    while(i < UART2_RX_BUFFER_LEN) {
        if(i == 0) {
            printf("\r\n rx_data: 0x%02x", Uart2_RxBuffer[i]);
        } else {
            printf(" 0x%02x", Uart2_RxBuffer[i]);
        }
        
        i ++;
        if(i%10 == 0) {
            printf("\r\n");
        }
    }
}

void uart2_set_input_byte_callback(uart2_input_byte_callback cb) {
    input_callback = cb;
}


void uart2_print_rx_temp_data(void) {
    int i = 0;
    printf("\r\n Uart2_temp_rx_counter: %d", Uart2_temp_rx_counter);
    while(i < Uart2_temp_rx_counter) {
        if(i == 0) {
            printf("\r\n Uart2_temp_data: 0x%02x", Uart2_temp_buffer[i]);
        } else {
            printf(" 0x%02x", Uart2_temp_buffer[i]);
        }
        
        i ++;
        if(i%10 == 0) {
            printf("\r\n");
        }
    }
}


void UART2_Configuration(void)
{
	    GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        USART_ClockInitTypeDef  USART_ClockInitStructure;

        /* ��ʼ������Ӧ�ó������ */


        /* ��UART2ʹ��ʱ��,UART2��APB1�� */
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);//Ҫ��UART2ӳ�䵽GPIOA��,�Ƿ�Ҳ��Ҫͬʱ��GPIOA��ʹ��ʱ����?
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE);
        //
        /*
        *  USART2_TX -> PA2 , USART2_RX ->	PA3
        */
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	         
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(GPIOA, &GPIO_InitStructure);		   

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
        GPIO_Init(GPIOA, &GPIO_InitStructure);
        //
        USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;                    // ʱ�ӵ͵�ƽ�
        USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;                          // ʱ�ӵ͵�ƽ
        USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;                        // ʱ�ӵڶ������ؽ������ݲ���
        USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;                // ���һλ���ݵ�ʱ�����岻��SCLK���
        /* Configure the USART2 synchronous paramters */
        USART_ClockInit(USART2, &USART_ClockInitStructure);                            // ʱ�Ӳ�����ʼ������
                                                                                                                                                 
        USART_InitStructure.USART_BaudRate =9600;                                      // ������
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    // 8λ����
        USART_InitStructure.USART_StopBits = USART_StopBits_1;                         // ��֡��β����1��ֹͣλ
        USART_InitStructure.USART_Parity = USART_Parity_No ;                           // ��żʧ��
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// Ӳ��������ʧ��
        
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                // ����ʹ��+����ʹ��
        /* Configure USART2 basic and asynchronous paramters */
        USART_Init(USART2, &USART_InitStructure);

        /* Enable USART2 */
        USART_ClearFlag(USART2, USART_IT_RXNE);                                        //���жϣ�����һ�����жϺ����������ж�
        USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);                                  //ʹ��USART2�ж�Դ
        USART_Cmd(USART2, ENABLE);                                                     //USART2�ܿ��أ�����

        /* Enable USART2 Interrupt */
        USART2_Interrupts_Config();
        //
        Uart2_TxCounter = 0;
}

void USART2_Interrupts_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;   //ͨ������Ϊ����2
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //�ж�ռ�ȵȼ�  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        //�ж���Ӧ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //���ж�   
    NVIC_Init(&NVIC_InitStructure);
}


/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
$GPGGA��<1>��<2>��<3>��<4>��<5>��<6>��<7>��<8>��<9>��M��<10>��M��<11>��<12>*hh<CR><LF>
*/
void USART2_IRQHandler(void)
{
    unsigned char tmp;
    
    
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        tmp = USART_ReceiveData(USART2);
        #if 0
        //���ݰ�����10��û�ж�ȡ��ϳ�(�����Ǵ������ߵ�ԭ��)
        if( GetSysTick_10Ms()>rec_systick_mark+1000 )
            gps_rec_count = 0;
        rec_systick_mark = GetSysTick_10Ms();
        //��һ���ַ�������'$'��ʼ
        if( (gps_rec_count==0) ){
            if( tmp=='$' )
                RxBuffer[gps_rec_count++] = tmp;
            return;
        }
        //�洢
        if( gps_rec_count<RX_BUFFER_LEN ){
            RxBuffer[gps_rec_count++] = tmp;
        }
        #else
        if(NULL != input_callback) {
            input_callback(tmp);
        }

        #if 0
        Uart2_RxBuffer[Uart2_RxCounter++] = tmp & 0xFF;
        if(Uart2_RxCounter >= UART2_RX_BUFFER_LEN) {
            Uart2_RxCounter = 0;
        }

        Uart2_temp_buffer[Uart2_temp_rx_counter++] = tmp & 0xFF;
        if(tmp == 0xFF) {
            Uart2_temp_rx_counter = 0;
        }
        #endif

        #endif
    }
    
    if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
    {   
        /* Write one byte to the transmit data register */
        USART_SendData(USART2, Uart2_TxBuffer[Uart2_TxCounter++]);


        if(Uart2_TxCounter >= UART2_TX_BUFFER_LEN)
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);        
    }           
}

int uart2_fputc(int ch, FILE *f) 
{ 
    USART2->DR = (u8) ch; 
     
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) ;

    return ch; 
}

int uart2_fgetc(FILE *f)
{
  
   while(!(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET));

   return (USART_ReceiveData(USART2));
}

