/************************************************************
 * Copyright (C), 2009-2011,  Tech. Co., Ltd.
 * FileName:		// 文件名
 * Author:			// 作者
 * Date:			// 日期
 * Description:		// 模块描述
 * Version:			// 版本信息
 * Function List:	// 主要函数及其功能
 *     1. -------
 * History:			// 历史修改记录
 *     <author>  <time>   <version >   <desc>
 *     xxxxx    16/10/12     1.0     build this moudle
 ***********************************************************/

#include "lt8618sx.h"

#ifdef LT8618SX_ENABLE
// LT8618SX RGB 输入，HDMI输出，寄存器设置

#define   LT8618SX_ADR		0x76                    // IIC 地址，CI2CA 管脚为高
//#define   LT8618SX_ADR		0x72                    // IIC 地址，CI2CA 管脚为低

/***************************************************

   要注意的是IIC地址0x76的最低位bit0是读写标志位，如果是Linux系统的IIC，最高位是读写标志位，IIC地址需要右移一位，IIC地址变成0x3B

 ****************************************************/

//extern HDMI_WriteI2C_Byte( u8 RegAddr, u8 data );   // IIC 写入操作

// 1、Reset LT8618SX

// 2、LT8618SX Initial设置：

u16	hfp, hs_width, hbp, h_act, h_tal, v_act, v_tal, vfp, vs_width, vbp;
u8		HDMI_VIC   = 0x00;
u8		HDMI_Y	   = 0x00;
bool	hs_pol, vs_pol;

#define _D0_D15_In	0x40 // BT1120 从LT8618 的D0 ~ D15 输入
#define _D8_D23_In	0x70 // BT1120 从LT8618 的D8 ~ D23 输入

bool	Use_DDRCLK;// 1: DDR mode; 0: SDR (normal) mode

unsigned long lt8618_task_t_sec = 0;  ////单位秒
bool lt8618_init = false;

/***************************************************

   要注意的是IIC地址0x76的最低位bit0是读写标志位，如果是Linux系统的IIC，最高位是读写标志位，IIC地址需要右移一位，IIC地址变成0x3B

 ****************************************************/

#if 1

//在TTP229指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 HDMI_ReadI2C_Byte(u8 RegAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  

    IIC_Send_Byte(LT8618SX_ADR);   //发送器件地址0XA0,写数据 	 

	if(0 != IIC_Wait_Ack()) {
        printf("\r\n L:%d %s(0x%02x) error.", __LINE__, __FUNCTION__, RegAddr);
        return 0;
	}
    IIC_Send_Byte(RegAddr%256);   //发送低地址
	if(0 != IIC_Wait_Ack()) {
        printf("\r\n L:%d %s(0x%02x) error.", __LINE__, __FUNCTION__, RegAddr);
        return 0;
	}   
	IIC_Start();  	 	   
	IIC_Send_Byte(LT8618SX_ADR | 0x01);           //进入接收模式			   
	if(0 != IIC_Wait_Ack()) {
        printf("\r\n L:%d %s(0x%02x) error.", __LINE__, __FUNCTION__, RegAddr);
        return 0;
	}
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在TTP229指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
bool HDMI_WriteI2C_Byte(u8 RegAddr, u8 d)
{				   	  	    																 
    IIC_Start();  
	IIC_Send_Byte(LT8618SX_ADR);   //发送器件地址0XA0,写数据 
	if(0 != IIC_Wait_Ack()) {
        printf("\r\n L:%d %s(0x%02x) error.", __LINE__, __FUNCTION__, RegAddr);
        return false;
	}	   
    IIC_Send_Byte(RegAddr%256);   //发送低地址
	if(0 != IIC_Wait_Ack()) {
        printf("\r\n L:%d %s(0x%02x) error.", __LINE__, __FUNCTION__, RegAddr);
        return false;
	}	 										  		   
	IIC_Send_Byte(d);     //发送字节							   
	if(0 != IIC_Wait_Ack()) {
        printf("\r\n L:%d %s(0x%02x) error.", __LINE__, __FUNCTION__, RegAddr);
        return false;
	}  		    	   
    IIC_Stop();//产生一个停止条件 
	Wait10Ms(1);	

    return true;
}
//在TTP229里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
bool HDMI_WriteI2C_ByteN(u8 RegAddr, u8 *d,u16 N)
{  	
	u8 t;
	for(t=0;t<N;t++)
	{
		HDMI_WriteI2C_Byte(RegAddr+t, d[t]);
	}
    return true;
}

//在TTP229里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
bool HDMI_ReadI2C_ByteN(u8 RegAddr,u8 *p_data,u8 N)
{  	
	u8 t;
	for(t=0;t<N;t++)
	{
		p_data[t] = HDMI_ReadI2C_Byte(RegAddr + t); 	 				   
	}
    return true;			    
}

#else
//extern HDMI_WriteI2C_Byte( u8 RegAddr, u8 data );   // IIC 写入操作
u8 HDMI_ReadI2C_Byte(u8 RegAddr)
{
	FunctionalState state;
	u8 data = 0x5e;
    
    state = I2C_ReadBytes(&data, 1, RegAddr, LT8618SX_ADR);
    if(state != ENABLE) {
        printf("\r\n HDMI_ReadI2C_Byte fail.");
        //FLAG_I2C_ERROR = TRUE;
    }

    return data;
}

bool HDMI_ReadI2C_ByteN(u8 RegAddr,u8 *p_data,u8 N)
{
	bool flag = TRUE;
    FunctionalState state;

    state = I2C_ReadBytes(p_data, N, RegAddr, LT8618SX_ADR);
    if(state != ENABLE) {
        printf("\r\n HDMI_ReadI2C_ByteN fail.");
        flag = FALSE;
    }
    	
	return flag;
}


bool HDMI_WriteI2C_Byte(u8 RegAddr, u8 d)
{
	bool flag = TRUE;
    FunctionalState state;

    state = I2C_WriteByte(d, RegAddr, LT8618SX_ADR);
    if(state != ENABLE) {
        printf("\r\n HDMI_WriteI2C_Byte fail. RegAddr:0x%02x", RegAddr);
        flag = FALSE;
    }
    	
	return flag;
}

bool HDMI_WriteI2C_ByteN(u8 RegAddr, u8 *d,u16 N)
{
	bool flag = TRUE;
    FunctionalState state;

    state = I2C_WriteBytes(d, N, RegAddr, LT8618SX_ADR);
    if(state != ENABLE) {
        printf("\r\n HDMI_WriteI2C_ByteN fail.");
        flag = FALSE;
    }
    	
	return flag;
}
#endif

bool LT8618SX_Configure(void)
{
// HDMI reset/det is unused.
#if 0
    GPIO_InitTypeDef  GPIO_InitStructure;
 	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	 //使能PB,PE端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //LED0-->PB.5 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
    GPIO_SetBits(GPIOA, GPIO_Pin_7);						 //PB.5 输出高

    // HDMI_DET
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;//PA0 6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOa0,1,8
 
    printf("\r\nHDMI LT8618 Init.\r\n");

    GP_HDMI_RST=0;

    Wait10Ms(100);

#endif    
    
    printf("\r\nHDMI LT8618 Init done.\r\n");
	return 1;
}


// 1、Reset LT8618SX

void LT8618SX_Reset(void) {
    //hw reset
    GP_HDMI_RST=0;
    Wait10Ms(50);
    GP_HDMI_RST=1;
    Wait10Ms(100);
    lt8618_init = false;
}


void LT8618SX_Video_Check( void )
{
	u8 temp;

	Wait10Ms( 10 );

	{
		HDMI_WriteI2C_Byte( 0xff, 0x82 );   //video check
		temp = HDMI_ReadI2C_Byte( 0x70 );   //hs vs polarity
		if( temp & 0x02 )
		{
			vs_pol = 1;
		}
		if( temp & 0x01 )
		{
			hs_pol = 1;
		}

		vs_width = HDMI_ReadI2C_Byte( 0x71 );

		hs_width   = HDMI_ReadI2C_Byte( 0x72 );
		hs_width   = ( hs_width << 8 ) + HDMI_ReadI2C_Byte( 0x73 );

		vbp	   = HDMI_ReadI2C_Byte( 0x74 );
		vfp	   = HDMI_ReadI2C_Byte( 0x75 );

		hbp	   = HDMI_ReadI2C_Byte( 0x76 );
		hbp	   = ( hbp << 8 ) + HDMI_ReadI2C_Byte( 0x77 );

		hfp	   = HDMI_ReadI2C_Byte( 0x78 );
		hfp	   = ( hfp << 8 ) + HDMI_ReadI2C_Byte( 0x79 );

		v_tal  = HDMI_ReadI2C_Byte( 0x7a );
		v_tal  = ( v_tal << 8 ) + HDMI_ReadI2C_Byte( 0x7b );

		h_tal  = HDMI_ReadI2C_Byte( 0x7c );
		h_tal  = ( h_tal << 8 ) + HDMI_ReadI2C_Byte( 0x7d );

		v_act  = HDMI_ReadI2C_Byte( 0x7e );
		v_act  = ( v_act << 8 ) + HDMI_ReadI2C_Byte( 0x7f );

		h_act  = HDMI_ReadI2C_Byte( 0x80 );
		h_act  = ( h_act << 8 ) + HDMI_ReadI2C_Byte( 0x81 );
	}

	if( ( h_act > 710 ) && ( h_act < 730 ) )
	{
		if( ( v_act > 470 ) && ( v_act < 490 ) )
		{
			//	Debug_Printf("\r\nVideo_Check = video_720x480_60Hz ");
		//	Sync_mode  = VESA_720x480p_60_27MHz;
			HDMI_VIC   = 3;
		}else
		if( ( v_act > 566 ) && ( v_act < 586 ) )
		{
		//	Sync_mode  = VESA_720x576p_50_27MHz;
			HDMI_VIC   = 18;
		}else
		{
		//	Sync_mode  = VIDEO_NOSYNC;
			HDMI_VIC   = 0x00;
		}
	}else
	if( ( h_act > 1430 ) && ( h_act < 1450 ) )
	{
		if( ( v_act > 230 ) && ( v_act < 250 ) )
		{
			//	Debug_Printf("\r\nVideo_Check = video_720x480_60Hz ");
		//	Sync_mode  = VESA_720x480i_60_27MHz;
			HDMI_VIC   = 7;
		}else
		if( ( v_act > 276 ) && ( v_act < 296 ) )
		{
		//	Sync_mode  = VESA_720x576i_50_27MHz;
			HDMI_VIC   = 22;
		}else
		{
		//	Sync_mode  = VIDEO_NOSYNC;
			HDMI_VIC   = 0x00;
		}
	}else
	if( ( h_act > 1270 ) && ( h_act < 1290 ) && ( v_act > 710 ) && ( v_act < 730 ) )
	{
		if( ( h_tal > 1640 ) && ( h_tal < 1660 ) )
		{
			//	Debug_Printf("\r\nVideo_Check = video_720x480_60Hz ");
		//	Sync_mode  = VESA_1280x720p_60;
			HDMI_VIC   = 4;
		}else
		if( ( h_tal > 1970 ) && ( h_tal < 1980 ) )
		{
		//	Sync_mode  = VESA_1280x720p_50;
			HDMI_VIC   = 19;
		}else
		if( ( h_tal > 3290 ) && ( h_tal < 3310 ) )
		{
		//	Sync_mode  = VESA_1280x720p_30;
			HDMI_VIC   = 62;
		}else
		if( ( h_tal > 3950 ) && ( h_tal < 3970 ) )
		{
		//	Sync_mode  = VESA_1280x720p_25;
			HDMI_VIC   = 61;
		}else
		{
		//	Sync_mode  = VIDEO_NOSYNC;
			HDMI_VIC   = 0x00;
		}
	}else
	if( ( h_act > 1910 ) && ( h_act < 1930 ) )
	{
		if( ( v_act > 1070 ) && ( v_act < 1090 ) )
		{
			if( ( h_tal > 2190 ) && ( h_tal < 2210 ) )
			{
				//	Debug_Printf("\r\nVideo_Check = video_720x480_60Hz ");
			//	Sync_mode  = VESA_1920x1080p_60;
				HDMI_VIC   = 16;
			}else
			if( ( h_tal > 2630 ) && ( h_tal < 2650 ) )
			{
			//	Sync_mode  = VESA_1920x1080p_50;
				HDMI_VIC   = 31;
			}else
			{
			//	Sync_mode  = VIDEO_NOSYNC;
				HDMI_VIC   = 0x00;
			}
		}else
		if( ( v_act > 530 ) && ( v_act < 550 ) )
		{
			if( ( h_tal > 2190 ) && ( h_tal < 2210 ) )
			{
				//	Debug_Printf("\r\nVideo_Check = video_720x480_60Hz ");
			//	Sync_mode  = VESA_1920x1080i_60;
				HDMI_VIC   = 5;
			}else
			if( ( h_tal > 2630 ) && ( h_tal < 2650 ) )
			{
			//	Sync_mode  = VESA_1920x1080i_50;
				HDMI_VIC   = 20;
			}else
			{
			//	Sync_mode  = VIDEO_NOSYNC;
				HDMI_VIC   = 0x00;
			}
		}else
		{
		//	Sync_mode  = VIDEO_NOSYNC;
			HDMI_VIC   = 0x00;
		}
	}

	printf("\r\n hs:%d", hs_width);
    printf("\r\n hbp:%d", hbp);
    printf("\r\n hactive:%d", h_act);
    printf("\r\n hfp:%d", hfp);
    printf("\r\n htotal:%d", h_tal);

    printf("\r\n");
    
    printf("\r\n vs:%d", vs_width);
    printf("\r\n vbp:%d", vbp);
    printf("\r\n vactive:%d", v_act);
    printf("\r\n vfp:%d", vfp);
    printf("\r\n vtotal:%d", v_tal);

    printf("\r\n");

    printf("\r\n HDMI_VIC:%d", HDMI_VIC);
    
    printf("\r\n\r\n");
}


void LT8618SX_Initial( void )
{
    u16 deviceId;
	u8 value;

    lt8618_init = true;
   
    //76 ff 80 00 // Register Bank change76 ee 01 00 // Debug enable76 00 02 ff // read ID
    HDMI_WriteI2C_Byte( 0xff, 0x80 );// register bank
    HDMI_WriteI2C_Byte( 0xee, 0x01 );// register bank
    if(HDMI_ReadI2C_ByteN(0x0, (u8*)&deviceId, sizeof(u16)))
    {
        printf("\r\nLT8618SX deviceId:0x%04x.\r\n", deviceId);
    }
    
    
    Use_DDRCLK = 0;// 1: DDR mode; 0: SDR (normal) mode
    

	//I2CADR = LT8618SX_ADR;              // 设置IIC地址

/*
76 ff 80 00
76 11 00 00
76 13 f1 00
76 13 f9 00
	*/

	HDMI_WriteI2C_Byte( 0xff, 0x80 );// register bank
	HDMI_WriteI2C_Byte( 0x11, 0x00 ); //reset MIPI Rx logic.
	HDMI_WriteI2C_Byte( 0x13, 0xf1 );
	HDMI_WriteI2C_Byte( 0x13, 0xf9 );
/*
76 ff 81 00
76 02 66 00
76 0a 06 00
76 15 06 00
*/	
	// TTL mode
	HDMI_WriteI2C_Byte( 0xff, 0x81 );// register bank
	HDMI_WriteI2C_Byte( 0x02, 0x66 );
	HDMI_WriteI2C_Byte( 0x0a, 0x06 );
	HDMI_WriteI2C_Byte( 0x15, 0x06 );
/*
76 4e a8 00
*/
	HDMI_WriteI2C_Byte( 0x4e, 0xa8 );
	
/*
76 ff 82 00
76 1b 77 00
76 1c ec 00 *
*/
	// TTL_Input_Digtal
	HDMI_WriteI2C_Byte( 0xff, 0x82 );// register bank
	HDMI_WriteI2C_Byte( 0x1b, 0x77 );
	HDMI_WriteI2C_Byte( 0x1c, 0xec );


	//76 ff 80 00
	//76 0a f0 00
	HDMI_WriteI2C_Byte( 0xff, 0x80 );// register bank
	HDMI_WriteI2C_Byte( 0x0a, 0xf0 );

	
	// digital
/*
76 ff 82 00
76 45 70 00
76 4f 40 00
76 50 00 00
76 51 42 00
76 48 08 00
76 47 01 ff
76 47 07 00
*/
	HDMI_WriteI2C_Byte( 0xff, 0x82 );
	HDMI_WriteI2C_Byte( 0x45, 0x70 );   //RGB channel swap
	HDMI_WriteI2C_Byte( 0x4f, 0x40);
	HDMI_WriteI2C_Byte( 0x50, 0x00 );
	HDMI_WriteI2C_Byte( 0x51, 0x42 );
	HDMI_WriteI2C_Byte( 0x48, 0x08 );   //Embedded sync mode input enable.
	if(HDMI_ReadI2C_ByteN(0x47, (u8*)&value, sizeof(u8)))
    {
        printf("\r\nLT8618SX 0x47: 0x%02x.\r\n", value);
    }
	HDMI_WriteI2C_Byte( 0x47, 0x07 );
	
	Wait10Ms( 100 );
	
	//LT8618SX_Video_Check( );

	// PLL
	/*
76 ff 81 00
76 23 40 00
76 24 64 00
76 26 55 00
76 29 04 00
76 25 01 00
76 2c 94 00
76 2d 99 00 // 74.25M
	*/
	HDMI_WriteI2C_Byte( 0xff, 0x81 );// register bank
	HDMI_WriteI2C_Byte( 0x23, 0x40 );
	HDMI_WriteI2C_Byte( 0x24, 0x64 ); //icp set
	HDMI_WriteI2C_Byte( 0x26, 0x55 );
	HDMI_WriteI2C_Byte( 0x29, 0x04 );
	HDMI_WriteI2C_Byte( 0x25, 0x01 );
	HDMI_WriteI2C_Byte( 0x2c, 0x94 );
	HDMI_WriteI2C_Byte( 0x2d, 0x99 );


/*
76 4d 00 00
76 27 60 00
76 28 00 00
*/
	HDMI_WriteI2C_Byte( 0x4d, 0x00 );
	HDMI_WriteI2C_Byte( 0x27, 0x60 );
	HDMI_WriteI2C_Byte( 0x28, 0x00 );

	if(HDMI_ReadI2C_ByteN(0x2b, (u8*)&value, sizeof(u8)))
    {
        printf("\r\nLT8618SX 0x2b: 0x%02x.\r\n", value);
    }
	if(HDMI_ReadI2C_ByteN(0x2e, (u8*)&value, sizeof(u8)))
    {
        printf("\r\nLT8618SX 0x2e: 0x%02x.\r\n", value);
    }

	HDMI_WriteI2C_Byte( 0x2e, 0x00 );
//-------------------------------------------


/*
76 ff 82 00
76 de 00 00
76 de c0 00
*/
	HDMI_WriteI2C_Byte( 0xff, 0x82 );// register bank
	HDMI_WriteI2C_Byte( 0xde, 0x00 );
	HDMI_WriteI2C_Byte( 0xde, 0xc0 );

/*
76 ff 80 00
76 16 f1 00
76 18 dc 00
76 18 fc 00
76 16 f3 00
*/
	HDMI_WriteI2C_Byte( 0xff, 0x80 );// register bank
	HDMI_WriteI2C_Byte( 0x16, 0xf1 );
	HDMI_WriteI2C_Byte( 0x18, 0xdc );
	HDMI_WriteI2C_Byte( 0x18, 0xfc );
	HDMI_WriteI2C_Byte( 0x16, 0xf3 );


	/*
76 ff 82 00
76 15 01 ff
76 ea 02 ff
	*/
	HDMI_WriteI2C_Byte( 0xff, 0x82 );// register bank
	HDMI_WriteI2C_Byte( 0x15, 0x01 );
	HDMI_WriteI2C_Byte( 0xea, 0x02 );

/*
76 ff 82 00
76 b9 18 00
*/
	HDMI_WriteI2C_Byte( 0xff, 0x82 );// register bank
	HDMI_WriteI2C_Byte( 0xb9, 0x18 );


/*
// AVI 
76 ff 84 00
76 43 46 00
76 44 10 00
76 45 19 00
76 47 00 00 // 1080P30 

//
76 10 2c 00
76 12 64 00
76 3d 0a 00
//
*/

	HDMI_WriteI2C_Byte( 0xff, 0x84 );// register bank
	HDMI_WriteI2C_Byte( 0x43, 0x46 );
	HDMI_WriteI2C_Byte( 0x44, 0x10 );
	HDMI_WriteI2C_Byte( 0x45, 0x19 );
	HDMI_WriteI2C_Byte( 0x47, 0x00 );

	HDMI_WriteI2C_Byte( 0x10, 0x2c);
	HDMI_WriteI2C_Byte( 0x12, 0x64 );
	HDMI_WriteI2C_Byte( 0x3d, 0x0a );
	
	/*
// BT timing 720P60
76 ff 82 00
76 20 07 00
76 21 80 00
76 22 00 00
76 23 58 00
76 24 00 00
76 25 2c 00
76 26 00 00
76 27 00 00
76 36 04 00
76 37 38 00
76 38 00 00
76 39 04 00
76 3a 00 00
76 3b 24 00
76 3c 00 00
76 3d 05 00
	*/
	HDMI_WriteI2C_Byte( 0xff, 0x82 );// register bank
	HDMI_WriteI2C_Byte( 0x20, 0x07 );
	HDMI_WriteI2C_Byte( 0x21, 0x80 );
	HDMI_WriteI2C_Byte( 0x22, 0x00 );
	HDMI_WriteI2C_Byte( 0x23, 0x58 );
	HDMI_WriteI2C_Byte( 0x24, 0x00 );
	HDMI_WriteI2C_Byte( 0x25, 0x2c );
	HDMI_WriteI2C_Byte( 0x26, 0x00 );
	HDMI_WriteI2C_Byte( 0x27, 0x00 );

	HDMI_WriteI2C_Byte( 0x36, 0x04 );
	HDMI_WriteI2C_Byte( 0x37, 0x38 );
	HDMI_WriteI2C_Byte( 0x38, 0x00 );
	HDMI_WriteI2C_Byte( 0x39, 0x04 );
	HDMI_WriteI2C_Byte( 0x3a, 0x00 );
	HDMI_WriteI2C_Byte( 0x3b, 0x24 );
	HDMI_WriteI2C_Byte( 0x3c, 0x00 );
	HDMI_WriteI2C_Byte( 0x3d, 0x05 );

/*
76 ff 81 00
76 30 ea 00
//
76 31 44 00
76 32 4a 00
76 33 0b 00

76 34 00 00
76 35 00 00
76 36 00 00
76 37 44 00
76 3f 0f 00

76 40 a0 00
76 41 a0 00
76 42 a0 00
76 43 a0 00
76 44 0a 00
*/

	HDMI_WriteI2C_Byte( 0xff, 0x81 );// register bank
	HDMI_WriteI2C_Byte( 0x30, 0xea );
	HDMI_WriteI2C_Byte( 0x31, 0x44 );
	HDMI_WriteI2C_Byte( 0x32, 0x4a );
	HDMI_WriteI2C_Byte( 0x33, 0x0b );
	
	HDMI_WriteI2C_Byte( 0x34, 0x00 );
	HDMI_WriteI2C_Byte( 0x35, 0x00 );
	HDMI_WriteI2C_Byte( 0x36, 0x00 );
	HDMI_WriteI2C_Byte( 0x37, 0x44 );
	HDMI_WriteI2C_Byte( 0x3f, 0x0f );

	HDMI_WriteI2C_Byte( 0x40, 0xa0 );
	HDMI_WriteI2C_Byte( 0x41, 0xa0 );
	HDMI_WriteI2C_Byte( 0x42, 0xa0 );
	HDMI_WriteI2C_Byte( 0x43, 0xa0 );
	HDMI_WriteI2C_Byte( 0x44, 0x0a );
	
}

void LT8618SX_BT1120_Set( void )
{
    //AVI
	HDMI_WriteI2C_Byte( 0xff, 0x84 );
	HDMI_WriteI2C_Byte( 0x43, 0x56 - HDMI_VIC - ( ( HDMI_Y << 5 ) + 0x10 ) );   //AVI_PB0
	HDMI_WriteI2C_Byte( 0x44, ( HDMI_Y << 5 ) + 0x10 );                         //AVI_PB1
	HDMI_WriteI2C_Byte( 0x47, HDMI_VIC );      

//-------------------------------------------
	// HDMI_TX_Phy
	HDMI_WriteI2C_Byte( 0xff, 0x81 );// register bank
	HDMI_WriteI2C_Byte( 0x30, 0xea );
	HDMI_WriteI2C_Byte( 0x31, 0x44 );
	HDMI_WriteI2C_Byte( 0x32, 0x4a );
	HDMI_WriteI2C_Byte( 0x33, 0x0b );
	HDMI_WriteI2C_Byte( 0x34, 0x00 );
	HDMI_WriteI2C_Byte( 0x35, 0x00 );
	HDMI_WriteI2C_Byte( 0x36, 0x00 );
	HDMI_WriteI2C_Byte( 0x37, 0x44 );
	HDMI_WriteI2C_Byte( 0x3f, 0x0f );
	HDMI_WriteI2C_Byte( 0x40, 0xa0 );
	HDMI_WriteI2C_Byte( 0x41, 0xa0 );
	HDMI_WriteI2C_Byte( 0x42, 0xa0 );
	HDMI_WriteI2C_Byte( 0x43, 0xa0 );
	HDMI_WriteI2C_Byte( 0x44, 0x0a );

    //-------------------------------------------
	//LT8618SX_BT1120_Set
    HDMI_WriteI2C_Byte( 0xff, 0x82 );
	HDMI_WriteI2C_Byte( 0x20, (u8)( h_act / 256 ) );
	HDMI_WriteI2C_Byte( 0x21, (u8)( h_act % 256 ) );
	HDMI_WriteI2C_Byte( 0x22, (u8)( hfp / 256 ) );
	HDMI_WriteI2C_Byte( 0x23, (u8)( hfp % 256 ) );
	HDMI_WriteI2C_Byte( 0x24, (u8)( hs_width / 256 ) );
	HDMI_WriteI2C_Byte( 0x25, (u8)( hs_width % 256 ) );
	HDMI_WriteI2C_Byte( 0x26, 0x00 );
	HDMI_WriteI2C_Byte( 0x27, 0x00 );
	HDMI_WriteI2C_Byte( 0x36, (u8)( v_act / 256 ) );
	HDMI_WriteI2C_Byte( 0x37, (u8)( v_act % 256 ) );
	HDMI_WriteI2C_Byte( 0x38, (u8)( vfp / 256 ) );
	HDMI_WriteI2C_Byte( 0x39, (u8)( vfp % 256 ) );
	HDMI_WriteI2C_Byte( 0x3a, (u8)( vbp / 256 ) );
	HDMI_WriteI2C_Byte( 0x3b, (u8)( vbp % 256 ) );
	HDMI_WriteI2C_Byte( 0x3c, (u8)( vs_width / 256 ) );
	HDMI_WriteI2C_Byte( 0x3d, (u8)( vs_width % 256 ) );
}



#if 1
void LT8618SX_state_check( void ) 
{
    u8 value[20] = 0;
    HDMI_WriteI2C_Byte( 0xff, 0x82 );// register bank
    HDMI_ReadI2C_ByteN(0x45, value, 1);

    printf("\r\n video_check: 0x%02x", value[0]);
}
#endif

void LT8618SX_process( void )
{
    if(!lt8618_init) {
        
        LT8618SX_Reset();
        LT8618SX_Initial();
        
        //IIC_disable();
    } else {
        if( GetSysTick_Sec() > (lt8618_task_t_sec + 2) )
        {
            lt8618_task_t_sec = GetSysTick_Sec();
            //LT8618SX_Video_Check();
            //LT8618SX_state_check();
        }
    }
}

#endif
/************************************** The End Of File **************************************/


