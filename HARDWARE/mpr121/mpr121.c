#include "mpr121.h" 
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//24CXX驱动 代码(适合24C01~24C16,24C32~256未经过测试!有待验证!)		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

#define MPR121_DEBUG (1)

u8 MPR121_ADDR = (0x5A << 1);

#define SENSORS       13
//#define TOU_THRESH    0x1F
//#define REL_THRESH    0x1A
#define PROX_THRESH   0x3f
#define PREL_THRESH   0x3c

// variables: capacitive sensing
u8 touchStates[SENSORS];    // holds the current touch/prox state of all sensors
u8 activeSensors[SENSORS] = {1,1,1,1,1,1,1,1,1,1,1,1,0}; // holds which sensors are active (0=inactive, 1=active)
u8 newData = 0;         // flag that is set to true when new data is available from capacitive sensor
int irqpin = 2;               // pin that connects to notifies when data is available from capacitive sensor


#ifdef MPR121_ENABLE

u8 MPR121_ReadOneByte(u8 RegAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  

    IIC_Send_Byte(MPR121_ADDR);   //发送器件地址0XA0,写数据 	 

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
	IIC_Send_Byte(MPR121_ADDR | 0x01);           //进入接收模式			   
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
u8 MPR121_WriteOneByte(u8 RegAddr, u8 d)
{				   	  	    																 
    IIC_Start();  
	IIC_Send_Byte(MPR121_ADDR);   //发送器件地址0XA0,写数据 
	if(0 != IIC_Wait_Ack()) {
        printf("\r\n L:%d %s(0x%02x) error.", __LINE__, __FUNCTION__, RegAddr);
        return 1;
	}	   
    IIC_Send_Byte(RegAddr%256);   //发送低地址
	if(0 != IIC_Wait_Ack()) {
        printf("\r\n L:%d %s(0x%02x) error.", __LINE__, __FUNCTION__, RegAddr);
        return 1;
	}	 										  		   
	IIC_Send_Byte(d);     //发送字节							   
	if(0 != IIC_Wait_Ack()) {
        printf("\r\n L:%d %s(0x%02x) error.", __LINE__, __FUNCTION__, RegAddr);
        return 1;
	}  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	

    return 0;
}

//在TTP229里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void MPR121_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		MPR121_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在TTP229里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
u32 MPR121_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=MPR121_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}


//在TTP229里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
u32 MPR121_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
    u32 i = 0;
	while(NumToRead)
	{
		*pBuffer++=MPR121_ReadOneByte(ReadAddr++);	
		NumToRead--;
        i++;
	}
    return i;
}  
//在TTP229里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
u32 MPR121_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
    u32 i = 0;
	while(NumToWrite--)
	{
		MPR121_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
        i++;
	}
    return i;
}

/*
(11)       (9)        (7)       (4)         (2)      (0)
zoom+   focus+   menu    bright+   mirror   situation

(10)       (8)         (6)      (5)         (3)     (1) 
zoom-   focus-    mode   bright-    freeze   power
*/

#if 1
#define ZOOM_PLUS 11
#define ZOOM_MINUS 10
#define FOCUS_PLUS 9
#define FOCUS_MINUS 8
#define MENU 7
#define MODE 6
#define BRIGHT_PLUS 4
#define BRIGHT_MINUS 5
#define MIRROR 2
#define FREEZE 3
#define SITUATION 0
#define POWER 1
#else
#define ZOOM_PLUS (1<<11)
#define ZOOM_MINUS (1<<10)
#define FOCUS_PLUS (1<<9)
#define FOCUS_MINUS0 (1<<8)
#define MENU (1<<7)
#define MODE (1<<6)
#define BRIGHT_PLUS (1<<4)
#define BRIGHT_MINUS (1<<5)
#define MIRROR (1<<2)
#define FREEZE (1<<3)
#define SITUATION (1<<0)
#define POWER (1<<1)
#endif


TP_INFO_t tp_info_list[SENSORS];
#if 0
= {
	{TP_ID_0,  "F0",	0, TP_state_release, 0, 0, 0},
    {TP_ID_1,  "F1",	1, TP_state_release, 0, 0, 0},
    {TP_ID_2,  "F2",	2, TP_state_release, 0, 0, 0},
    {TP_ID_3,  "F3",	3, TP_state_release, 0, 0, 0},
	{TP_ID_4,  "F4",	4, TP_state_release, 0, 0, 0},
	{TP_ID_5,  "F5",	5, TP_state_release, 0, 0, 0},
	{TP_ID_6,  "F6",	6, TP_state_release, 0, 0, 0},
	{TP_ID_7,  "F7",	7, TP_state_release, 0, 0, 0},
	{TP_ID_8,  "F8",	8, TP_state_release, 0, 0, 0},
	{TP_ID_9,  "F9",	9, TP_state_release, 0, 0, 0},
	{TP_ID_10,  "F10",	10, TP_state_release, 0, 0, 0},
	{TP_ID_11,  "F11",	11, TP_state_release, 0, 0, 0},
};
#endif


void mpr121_init(void)
{
	int i = 0;
	memset(tp_info_list, 0, sizeof(tp_info_list));
	for(i = 0; i < SENSORS; i++)
	{
		tp_info_list[i].id = TP_ID_0 + i;
		tp_info_list[i].bitPos = i;
		tp_info_list[i].state = TP_state_release;
	}
	
	//Reset MPR121 if not reset correctly
	MPR121_WriteOneByte(SOFT_RESET,0x63); //Soft reset
	MPR121_WriteOneByte(ELE_CFG,0x00); //Stop mode
    
  // Section A
  // This group controls filtering when data is > baseline.
  MPR121_WriteOneByte(MHD_R, 0x01);
  MPR121_WriteOneByte(NHD_R, 0x01);
  MPR121_WriteOneByte(NCL_R, 0x00);
  MPR121_WriteOneByte(FDL_R, 0x00);

  // Section B
  // This group controls filtering when data is < baseline.
  MPR121_WriteOneByte(MHD_F, 0x01);
  MPR121_WriteOneByte(NHD_F, 0x01);
  MPR121_WriteOneByte(NCL_F, 0xFF);
  MPR121_WriteOneByte(FDL_F, 0x02);
  
  // Section C
  // This group sets touch and release thresholds for each electrode
  MPR121_WriteOneByte(ELE0_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE0_R, REL_THRESH);
  MPR121_WriteOneByte(ELE1_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE1_R, REL_THRESH);
  MPR121_WriteOneByte(ELE2_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE2_R, REL_THRESH);
  MPR121_WriteOneByte(ELE3_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE3_R, REL_THRESH);
  MPR121_WriteOneByte(ELE4_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE4_R, REL_THRESH);
  MPR121_WriteOneByte(ELE5_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE5_R, REL_THRESH);
  MPR121_WriteOneByte(ELE6_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE6_R, REL_THRESH);
  MPR121_WriteOneByte(ELE7_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE7_R, REL_THRESH);
  MPR121_WriteOneByte(ELE8_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE8_R, REL_THRESH);
  MPR121_WriteOneByte(ELE9_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE9_R, REL_THRESH);
  MPR121_WriteOneByte(ELE10_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE10_R, REL_THRESH);
  MPR121_WriteOneByte(ELE11_T, TOU_THRESH);
  MPR121_WriteOneByte(ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  MPR121_WriteOneByte(FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Enable 6 Electrodes and set to run mode
  // Set ELE_CFG to 0x00 to return to standby mode
  MPR121_WriteOneByte(ELE_CFG, 0x0C);	// Enables all 12 Electrodes
  //MPR121_WriteOneByte(ELE_CFG, 0x06);		// Enable first 6 electrodes
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*MPR121_WriteOneByte(ATO_CFG0, 0x0B);
  MPR121_WriteOneByte(ATO_CFGU, 0xC9);	// USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   MPR121_WriteOneByte(ATO_CFGL, 0x82);	// LSL = 0.65*USL = 0x82 @3.3V
  MPR121_WriteOneByte(ATO_CFGT, 0xB5);*/	// Target = 0.9*USL = 0xB5 @3.3V
}


unsigned long mp121_task_t_sec = 0;  ////单位�





u8 touchpad_notify = 0;
u16 touch_status = 0;
u16 oor_status = 0;

u16 mpr121_read_ele_filtered_data(u8 id)
{
	u16 ret = -1;
	u8 data[2];
	if(id < 12)
	{
		MPR121_Read(0x04 + id * 2, data, 2);
		ret = data[1];
        ret <<= 8;
        ret |= data[0];
	}

	return ret;
}

void mpr121_get_ele_data(void)
{
	u8 i;
	u16 tmp_sig, tmp_bas, delta;
	u8 eleLSB, eleMSB;
	for (i=0; i<SENSORS; i++)
	{
		eleLSB = MPR121_ReadOneByte(0x04 + 2 * i);
		eleMSB = MPR121_ReadOneByte(0x04 + 2 * i + 1);
		tmp_sig = (((u16)eleMSB << 8) | eleLSB) & 0xFFFC;
		tmp_bas = MPR121_ReadOneByte(0x1E + i);
		tmp_bas <<= 2;

		delta = (tmp_sig > tmp_bas)?(tmp_sig - tmp_bas):(tmp_bas - tmp_sig);

		//tmp_sig=(((UINT16)readingArray[0x04+2*i])|(((UINT16)readingArray[0x04+1+2*i])<<8))&0xFFFC;
		//tmp_bas=((UINT16)readingArray[0x1e + i])<<2;
		//ele_delta[i]=abs((INT16)(tmp_sig-tmp_bas));

		printf("\r\n key:%d tmp_sig:%04x tmp_bas:%04x delta:%04x", i, tmp_sig, tmp_bas, delta);
	}
}

void mpr121_notify(void)
{
	u8 data[4];
	u32 ret;
	touchpad_notify++;

	mpr121_scan();
}

void mpr121_scan(void)
{
   	u8 data[16];
	u32 ret;
	u16 temp;
	u8 i;

	u8 tLSB;
    u8 tMSB;
    u16 touched = ((tMSB << 8) | tLSB);
	
    if(touchpad_notify > 0)
	{
		ret = MPR121_Read(0, data, 4);
		touched = ((u16)data[1] << 8) | data[0];
		touch_status = touched;

		temp = data[3];
        temp <<= 8;
        temp |= data[2];
		oor_status = temp;

		for (i = 0; i < SENSORS; i++) // Check what electrodes were pressed
		{
	      	if (activeSensors[i] == 0) continue;
		        // if current sensor was touched (check appropriate bit on touched var)
		        if(touched & (1<<i))
				{
					// if current pin was not previously touched send a serial message
					if(tp_info_list[i].state == TP_state_release)
					{
						printf("\r\n key(%d) : %d", i, 1);
						tp_info_list[i].state = TP_state_touch;
						tp_info_list[i].key_press_time = GetSysTick_10Ms();
						tp_info_list[i].event_pressed = true;
						tp_info_list[i].event_released = false;
					}
					else
					{
						tp_info_list[i].key_holding_time = GetSysTick_10Ms() - tp_info_list[i].key_press_time;
					}
		        } 
				else
				{
					// if current pin was just touched send serial message
					if(tp_info_list[i].state == TP_state_touch)
					{
						printf("\r\n key(%d) : %d", i, 0);
						tp_info_list[i].state = TP_state_release;
						tp_info_list[i].key_release_time = GetSysTick_10Ms();
						tp_info_list[i].event_pressed = false;
						tp_info_list[i].event_released = true;
					}
					
		        }        
		}

		//printf("\r\n L:%d touch_status:%04x. oor_status:%04x", __LINE__, touch_status, oor_status);
		printf("\r\n");

		touchpad_notify--;
	}
}

BOOL mpr121_is_holding(u8 id, long time)
{
	if(id < SENSORS)
	{
		if(tp_info_list[id].state == TP_state_touch && (GetSysTick_10Ms() - tp_info_list[id].key_press_time) > time)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

BOOL mpr121_is_pressed(u8 id)
{
	if(id < SENSORS)
	{
		if(tp_info_list[id].state == TP_state_touch)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

BOOL mpr121_is_released(u8 id)
{
	if(id < SENSORS)
	{
		if(tp_info_list[id].state == TP_state_release)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


void mpr121_process(void)
{
	int i = 0;
	#if 0
	for (i = 0; i < SENSORS; i++) // Check what electrodes were pressed
	{
      	if (activeSensors[i] == 0) continue;

		if(mpr121_is_pressed(i))
		{
			printf("\r\n key(%d) short press handle", i);
			tp_info_list[i].event_pressed = false;
		}

		if(mpr121_is_holding(i, 300))
		{
			printf("\r\n key(%d) long press handle", i);
			tp_info_list[i].event_pressed = false;
		}
	}
	#else


	if(mpr121_is_pressed(ZOOM_PLUS) && mpr121_is_pressed(POWER))
	{
		if(mpr121_is_holding(ZOOM_PLUS, 400) && mpr121_is_holding(POWER, 400))
		{
			tp_info_list[ZOOM_PLUS].event_pressed = false;
			tp_info_list[POWER].event_pressed = false;
			//sonylens_set_zoom_stop();
			//sonylens_set_focus_stop();
			sonylens_reset_1080P25();
			return;
		}

		return;
	}
	
	if(mpr121_is_pressed(ZOOM_MINUS) && mpr121_is_pressed(POWER))
	{
		if(mpr121_is_holding(ZOOM_MINUS, 400) && mpr121_is_holding(POWER, 400))
		{
			tp_info_list[ZOOM_MINUS].event_pressed = false;
			tp_info_list[POWER].event_pressed = false;
			//sonylens_set_zoom_stop();
			//sonylens_set_focus_stop();
			sonylens_reset_720P25();
			return;
		}

		return;
	}
	
	if(mpr121_is_pressed(ZOOM_PLUS) && tp_info_list[ZOOM_PLUS].event_pressed)
	{
		tp_info_list[ZOOM_PLUS].event_pressed = false;
		if(!sonylens_remote_is_menu_on())
        {
            sonylens_set_zoom_tele();
        }
        else
        {
            sonylens_remote_navi_left();
        }
	}

	if(mpr121_is_released(ZOOM_PLUS) && tp_info_list[ZOOM_PLUS].event_released)
	{
		tp_info_list[ZOOM_PLUS].event_released = false;
		if(!sonylens_remote_is_menu_on())
        {
            sonylens_set_zoom_stop();
        }
	}

	if(mpr121_is_pressed(ZOOM_MINUS) && tp_info_list[ZOOM_MINUS].event_pressed)
	{
		tp_info_list[ZOOM_MINUS].event_pressed = false;
		if(!sonylens_remote_is_menu_on())
        {
            sonylens_set_zoom_wide();
        }
        else
        {
            sonylens_remote_navi_right();
        }
	}

	if(mpr121_is_released(ZOOM_MINUS) && tp_info_list[ZOOM_MINUS].event_released)
	{
		tp_info_list[ZOOM_MINUS].event_released = false;
		if(!sonylens_remote_is_menu_on())
        {
            sonylens_set_zoom_stop();
        }
	}


	if(mpr121_is_pressed(FOCUS_PLUS) && tp_info_list[FOCUS_PLUS].event_pressed)
	{
		tp_info_list[FOCUS_PLUS].event_pressed = false;
		if(!sonylens_remote_is_menu_on())
        {
            sonylens_set_focus_near();
        }
        else
        {
            sonylens_remote_navi_down();
        }
	}

	if(mpr121_is_released(FOCUS_PLUS) && tp_info_list[FOCUS_PLUS].event_released)
	{
		tp_info_list[FOCUS_PLUS].event_released = false;
		if(!sonylens_remote_is_menu_on())
		{
			sonylens_set_focus_stop();
		}
	}

	if(mpr121_is_pressed(FOCUS_MINUS) && tp_info_list[FOCUS_MINUS].event_pressed)
	{
		tp_info_list[FOCUS_MINUS].event_pressed = false;
		if(!sonylens_remote_is_menu_on())
        {
            sonylens_set_focus_far();
        }
        else
        {
            sonylens_remote_navi_up();
        }
	}

	if(mpr121_is_released(FOCUS_MINUS) && tp_info_list[FOCUS_MINUS].event_released)
	{
		tp_info_list[FOCUS_MINUS].event_released = false;
		if(!sonylens_remote_is_menu_on())
		{
			sonylens_set_focus_stop();
		}
	}

	if(mpr121_is_pressed(MENU) && tp_info_list[MENU].event_pressed)
	{
		if(sonylens_remote_is_menu_on())
		{
			tp_info_list[MENU].event_pressed = false;
			sonylens_menu_action();
		}
		else
		{
			if(mpr121_is_holding(MENU, 150))
			{
				tp_info_list[MENU].event_pressed = false;
				sonylens_main_menu_on();
			}
		}
	}

	if(mpr121_is_pressed(MODE))
	{
		if(tp_info_list[MODE].event_pressed)
		{
			tp_info_list[MODE].event_pressed = false;
			sonylens_control_f7();
		}
	}

	if(mpr121_is_pressed(BRIGHT_PLUS))
	{
		if(tp_info_list[BRIGHT_PLUS].event_pressed)
		{
			tp_info_list[BRIGHT_PLUS].event_pressed = false;
			sonylens_control_f5();
		}	
	}

	if(mpr121_is_pressed(BRIGHT_MINUS))
	{
		if(tp_info_list[BRIGHT_MINUS].event_pressed)
		{
			tp_info_list[BRIGHT_MINUS].event_pressed = false;
			sonylens_control_f6();
		}
	}

	if(mpr121_is_pressed(MIRROR))
	{
		if(tp_info_list[MIRROR].event_pressed)
		{
			tp_info_list[MIRROR].event_pressed = false;
			sonylens_control_f8();
		}
		
	}

	if(mpr121_is_pressed(FREEZE))
	{
		if(tp_info_list[FREEZE].event_pressed)
		{
			tp_info_list[FREEZE].event_pressed = false;
			sonylens_control_f2();
		}
	}

	if(mpr121_is_pressed(SITUATION))
	{
		if(tp_info_list[SITUATION].event_pressed)
		{
			tp_info_list[SITUATION].event_pressed = false;
			sonylens_control_f4();
		}
	}

	if(mpr121_is_holding(POWER, 200) && !mpr121_is_pressed(ZOOM_PLUS) && !mpr121_is_pressed(ZOOM_MINUS))
	{
		if(tp_info_list[POWER].event_pressed)
		{
			tp_info_list[POWER].event_pressed = false;
	        sonylens_control_f1();
		}

		return;
	}

	#endif
}

#endif


