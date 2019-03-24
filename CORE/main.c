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


unsigned long main_task_t_sec = 0;  ////µ¥Î»Ãë



static u8 Lock_Code[4]; // 
const u32 Lock_Code_Holder = 0x9136FB19;
BOOL checkIdCode(void)
{
	u32 CpuID[3];
	u32 code;
	BOOL ret = false;
	//get cpu unique id
	CpuID[0]=*(vu32*)(0x1ffff7e8);
	CpuID[1]=*(vu32*)(0x1ffff7ec);
	CpuID[2]=*(vu32*)(0x1ffff7f0);
	//encrypt algy
	code=(CpuID[0]>>1)+(CpuID[1]>>2)+(CpuID[2]>>3);
	code = ~code;
	code *=3;
	//printf("\r\n 0x%08x 0x%08x 0x%08x 0x%08x", CpuID[0], CpuID[1], CpuID[2], code);
	Lock_Code[0] = code >> 24;
	Lock_Code[1] = code >> 16;
	Lock_Code[2] = code >> 8;
	Lock_Code[3] = code;

	//STMFLASH_Read((u32)&Lock_Code_Holder,(u16*)code,4);
	if(code == Lock_Code_Holder)
	{
		//printf("\r\n L:%d write flash...", __LINE__);
		//STMFLASH_Write((u32)&Lock_Code_Holder,(u16*)Lock_Code,4);
		//printf("\r\n L:%d write flash finished!", __LINE__);
		ret = true;
	}
	else
	{
		//printf("\r\n L:%d Lock_Code_bak:%02x %02x %02x %02x", __LINE__, code[0], code[1], code[2], code[3]);
		ret = false;
	}
	return ret;
}

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
	IIC_Init();

	AT24CXX_Init();

	KEY_Init();

	LED_Init();
    

	//check = checkIdCode();
	if(!check)
	{
		printf("%c", 64);
		while(1)
		{
			printf(version_msg);
			delay_ms(250);
		}
	}
	

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
