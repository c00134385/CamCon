#include "sys.h"
#include "delay.h"
#include "usart.h"

#include "myiic.h"
#include "at24cxx.h"
#include "cam_test.h"
#include "includes.h"

/* version */
const char version_msg[200]={
"\r\n\r\n"
"**********************************************\r\n"
"[Camera Project]\r\n"
"V1.0c\r\n"
"2017/9/2\r\n"
"MODEM:jy\r\n"
"**********************************************\r\n"
};


unsigned long main_task_t_sec = 0;  ////��λ��


#define UART2_INPUT_BUFFER_SIZE            (128) /* size of the local packet buffer */
static int uart2_buf_index = 0;
static unsigned char uart2_input_buf[UART2_INPUT_BUFFER_SIZE];


void uart2_callback(unsigned char byte) {
    uart2_input_buf[uart2_buf_index++] = byte;
    if(uart2_buf_index >= UART2_INPUT_BUFFER_SIZE) {
        uart2_buf_index = 0;
    }
    printf("%02x ", byte);
}

int main(void)
{
	u8 t=0;
	BOOL check = true;
	u32 zoom_value;
    BOOL cam_initialized = false;
	VISCA_result_e result;
        
	delay_init();	    	 //???????	  
	NVIC_Configuration(); 	 //??NVIC????2:2??????,2??????
	uart_init(57600);	 //??????9600
	check = sec_init();
	if(!check)
	{
		printf("%c%c%c%c", 64, 117, 117, 117);
		while(1)
		{
			//printf(version_msg);
			delay_ms(2500);
		}
	}
    
	IIC_Init();

	AT24CXX_Init();

	KEY_Init();

	LED_Init();
    

	
	

	printf(version_msg);
	
	UART2_Configuration();
    //uart2_set_input_byte_callback(uart2_callback);
	UART3_Configuration();

	#ifdef LT8618SX_ENABLE
	LT8618SX_Configure();
	#endif

	#ifdef MPR121_ENABLE
	mpr121_init();
	EXTIX_Init();
	#endif

    sonylens_init();

    //while(1);

    // inquire version
    //result = visca_set_zoom_wide_speed(1, 0);
    //printf("\r\n set monitor mode failed. result:%d", result);
	
	while(1)
	{
		
		#if 0
		if( GetSysTick_Sec() > (main_task_t_sec) )
	    {
	        main_task_t_sec = GetSysTick_Sec();
	        //printf("time_sec:%d msec:%ld \r\n",GetSysTick_Sec(),GetSysTick_10Ms());
			sonylens_get_zoom_value(&zoom_value);
			printf("\r\nzoom_value:0x%x",zoom_value);
	    }
		#endif

		sonylens_task();
	}
}
