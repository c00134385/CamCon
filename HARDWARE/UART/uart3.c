/*----------------------------------------------------------------------------*/
/* uart3.c                                                                    */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/* 2014/6/11 manwjh 建立                                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/* include */
#include "stdio.h"
//#include "systick.h"
#include "string.h"
#include "stm32f10x_it.h"
//
#include "uart3.h"


/* external */


/* public */
#if 0
#define RX_BUFFER_LEN 400
char gprs_rx_buffer[RX_BUFFER_LEN+1];

#define TX_BUFFER_LEN	1
char gprs_tx_buffer[TX_BUFFER_LEN+1];

int gprs_rx_cnt = 0;       //RxCounter
int gprs_tx_cnt = 0;       //TxCounter
int gprs_tx_length = 0;    //TxLength
#else

#define RX_BUFFER_LEN 100
char sonylens_rx_buffer[RX_BUFFER_LEN+1];

#define TX_BUFFER_LEN	1
char sonylens_tx_buffer[TX_BUFFER_LEN+1];

int sonylens_rx_cnt = 0;       //RxCounter
int sonylens_tx_cnt = 0;       //TxCounter
int sonylens_tx_length = 0;    //TxLength


#endif

/* private */
static unsigned long rec_systick_mark=0;

static uart3_rx_callback rx_callback = NULL; 


void USART3_Interrupts_Config(void);


/* debug */



/*----------------------------------------------------------------------------*/
/*                                  Functions                                 */
/*----------------------------------------------------------------------------*/
int uart3_get_reclen(void)
{
    //int rx_cnt=gprs_rx_cnt,i;
    int rx_cnt=sonylens_rx_cnt,i;

    if( rx_cnt==0 )
        return 0;

    do{
        //rx_cnt = gprs_rx_cnt;
		rx_cnt = sonylens_rx_cnt;
        //Wait10Ms(1);
		for( i=0; i<10000; i++ ){
		    //if( gprs_rx_cnt>rx_cnt )
			if( sonylens_rx_cnt>rx_cnt )
		        break;
		}
    //}while( gprs_rx_cnt>rx_cnt );
    }while( sonylens_rx_cnt>rx_cnt );
    
    //return gprs_rx_cnt;
    return sonylens_rx_cnt;
}

unsigned char *uart3_get_rxbuf_ptr(void)
{
    ///return (unsigned char *)gprs_rx_buffer;
    return (unsigned char *)sonylens_rx_buffer;
}

int uart3_read(char *buff, int length)
{
    int rd_len = length;
    
    //if( length>gprs_rx_cnt )
        //rd_len = gprs_rx_cnt;
    if( length>sonylens_rx_cnt )
        rd_len = sonylens_rx_cnt;

	//memcpy( buff,gprs_rx_buffer,rd_len );
	//strcpy( buff,gprs_rx_buffer );
	strcpy( buff,sonylens_rx_buffer );

	return rd_len;
}

void uart3_clr(void)
{
    //gprs_rx_cnt = 0;
    //gprs_tx_cnt = 0;
    //gprs_tx_length = 0;
    sonylens_rx_cnt = 0;
    sonylens_tx_cnt = 0;
    sonylens_tx_length = 0;
    //gprs_rx_buffer[0]=0;
    //gprs_tx_buffer[0]=0;
    sonylens_rx_buffer[0]=0;
    sonylens_tx_buffer[0]=0;
}


int uart3_sendbf_finished(void)
{

	//return gprs_tx_length-gprs_tx_cnt;
	return sonylens_tx_length-sonylens_tx_cnt;
}


void UART3_Configuration(void)
{
	    GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        USART_ClockInitTypeDef  USART_ClockInitStructure;

        /* 打开UART3使能时钟,UART3在APB1上 */
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//要将UART3映射到GPIOB上,是否也需要同时打开GPIOB的使能时钟呢?
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE);
        //
        /*
        *  USART3_TX -> PB10 , USART3_RX ->	PB11
        */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	         
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(GPIOB, &GPIO_InitStructure);		   

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
        GPIO_Init(GPIOB, &GPIO_InitStructure);
        //
        USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;                    // 时钟低电平活动
        USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;                          // 时钟低电平
        USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;                        // 时钟第二个边沿进行数据捕获
        USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;                // 最后一位数据的时钟脉冲不从SCLK输出
        /* Configure the USART3 synchronous paramters */
        USART_ClockInit(USART3, &USART_ClockInitStructure);                            // 时钟参数初始化设置
                                                                                                                                                 
        USART_InitStructure.USART_BaudRate =9600;                                    // 波特率
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    // 8位数据
        USART_InitStructure.USART_StopBits = USART_StopBits_1;                         // 在帧结尾传输1个停止位
        USART_InitStructure.USART_Parity = USART_Parity_No ;                           // 奇偶失能
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// 硬件流控制失能
        
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                // 发送使能+接收使能
        /* Configure USART3 basic and asynchronous paramters */
        USART_Init(USART3, &USART_InitStructure);

        /* Enable USART3 */
        USART_ClearFlag(USART3, USART_IT_RXNE|USART_IT_TXE);                           //清中断，以免一启用中断后立即产生中断
        //USART_ITConfig(USART3,USART_IT_TXE, ENABLE);                     //使能USART3中断源
		USART_ITConfig(USART3,USART_IT_RXNE, ENABLE);
        USART_Cmd(USART3, ENABLE);                                                     //USART3总开关：开启
        
        /* Enable USART3 Interrupt */
        USART3_Interrupts_Config();
        
        /* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
          如下语句解决第1个字节无法正确发送出去的问题 */
        USART_ClearFlag(USART3, USART_FLAG_TC); /* 清发送外城标志，Transmission Complete flag */

}

void UART3_set_baudrate(int baudrate)
{
    USART_InitTypeDef USART_InitStructure;
    
    USART_Cmd(USART3, DISABLE);

    USART_InitStructure.USART_BaudRate = baudrate;                                    // 波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    // 8位数据
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                         // 在帧结尾传输1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No ;                           // 奇偶失能
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// 硬件流控制失能
    
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                // 发送使能+接收使能
    /* Configure USART3 basic and asynchronous paramters */
    USART_Init(USART3, &USART_InitStructure);

    /* Enable USART3 */
    USART_ClearFlag(USART3, USART_IT_RXNE|USART_IT_TXE);                           //清中断，以免一启用中断后立即产生中断
    //USART_ITConfig(USART3,USART_IT_TXE, ENABLE);                     //使能USART3中断源
	USART_ITConfig(USART3,USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);
}

void USART3_Interrupts_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;   //通道配置为串口2
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //中断占先等级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        //中断响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //打开中断   
    NVIC_Init(&NVIC_InitStructure);
}


/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
$GPGGA，<1>，<2>，<3>，<4>，<5>，<6>，<7>，<8>，<9>，M，<10>，M，<11>，<12>*hh<CR><LF>
*/

void USART3_IRQHandler(void)
{
    unsigned char tmp;
    
    //中断接收
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        tmp = USART_ReceiveData(USART3);
        //printf("%02x ", tmp);
        if(NULL != rx_callback) {
            rx_callback(tmp);
        }

        #if 0
        //数据包超过10秒没有读取则废除(可能是待机休眠等原因)
        if( GetSysTick_10Ms()>rec_systick_mark+1000 )
            //gprs_rx_cnt = 0;
            sonylens_rx_cnt = 0;
        rec_systick_mark = GetSysTick_10Ms();
        /* Read one byte from the receive data register */
        //if(gprs_rx_cnt >= RX_BUFFER_LEN)
            //gprs_rx_cnt = 0;
        if(sonylens_rx_cnt >= RX_BUFFER_LEN)
            sonylens_rx_cnt = 0;
        //gprs_rx_buffer[gprs_rx_cnt++] = tmp;
        //gprs_rx_buffer[gprs_rx_cnt] = 0x0; //总是自动填充下一个字节为'\0'
        sonylens_rx_buffer[sonylens_rx_cnt++] = tmp;
        sonylens_rx_buffer[sonylens_rx_cnt] = 0x0; //总是自动填充下一个字节为'\0'
        #endif
    }
    //中断发送    
    if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {   
        //if(gprs_tx_cnt >= gprs_tx_length)
        if(sonylens_tx_cnt >= sonylens_tx_length)
        {
            /* Disable the USART3 Transmit interrupt */
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }else
        {
            /* Write one byte to the transmit data register */
            //USART_SendData(USART3, gprs_tx_buffer[gprs_tx_cnt++]);
            USART_SendData(USART3, sonylens_tx_buffer[sonylens_tx_cnt++]);
        }
    }           
}

#if 0   //采用中断发送
void uart3_printf(char *bf)
{
    int len = strlen(bf);
    
    while( uart3_sendbf_finished()!=0 );        //等待上一批数据发送完成
    
    if( TX_BUFFER_LEN>len ){
        strcpy(gprs_tx_buffer,bf);
        gprs_tx_cnt = 0;
        gprs_tx_length = len;
        gprs_tx_buffer[gprs_tx_length] = 0;
        USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
}


void uart3_sendbf(unsigned char *bf, int len)
{
    while( uart3_sendbf_finished()!=0 );        //等待上一批数据发送完成
    
    if( TX_BUFFER_LEN>len ){
        memcpy(gprs_tx_buffer,bf,len);

        gprs_tx_cnt = 0;
        gprs_tx_length = len;
        gprs_tx_buffer[gprs_tx_length] = 0;
        USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
    }
}

void uart3_sendbf_f(int len)
{
    gprs_tx_cnt = 0;
    gprs_tx_length = len;
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

#else   //非中断方式
void uart3_sendbf_f(int len)
{
    //gprs_tx_cnt = 0;
    //gprs_tx_length = len;
    //gprs_tx_cnt = gprs_tx_length;
    sonylens_tx_cnt = 0;
    sonylens_tx_length = len;
    sonylens_tx_cnt = sonylens_tx_length;
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

void uart3_sendbf(unsigned char *bf, int len)
{
    int i;
    unsigned char *p =bf;
    
    for( i=0; i<len; i++){
        uart3_fputc(*p++);
    }
    //gprs_tx_cnt = gprs_tx_length;
    sonylens_tx_cnt = sonylens_tx_length;
}

void uart3_printf(char *bf)
{
    int len = strlen(bf);
    int i;
    char *p =bf;
    
    for( i=0; i<len; i++ ){
        uart3_fputc(*p++);
    }
    //gprs_tx_cnt = gprs_tx_length;
    sonylens_tx_cnt = sonylens_tx_length;
}
#endif



int uart3_fputc(int ch) 
{ 
    //USART_SendData(USART2, (u8) ch); 
    USART3->DR = (u8) ch; 
     
    /* Loop until the end of transmission */ 
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) 
    { 
    } 

    return ch; 
}



int uart3_fgetc(void)
{
  
   while(!(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET))
   {
   }
  
    
   return (USART_ReceiveData(USART3));
}

void uart3_set_rx_callback(uart3_rx_callback cb)
{
    rx_callback = cb;
}


