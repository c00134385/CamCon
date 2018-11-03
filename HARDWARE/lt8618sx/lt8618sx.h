#ifndef __LT8618_H_
#define __LT8618_H_

#include "sys.h" 
#include "includes.h"


#ifdef LT8618SX_ENABLE
//-----------------------------------------------------------
//sbit GP_HDMI_RST = P0^4;
//sbit GP_HDMI_DET = P4^3;
#define GP_HDMI_RST PAout(7)// PB5
#define GP_HDMI_DET PAin(6)// PE5	

//-----------------------------------------------------------
typedef enum
{			
	VESA_1920x1080_60,
	VESA_1920x1080_50,
	VESA_1920x1080_30,
	VESA_1920x1080_25,
	VESA_1920x1080I_60,
	VESA_1920x1080I_50,
	VESA_1280x720_60,
	VESA_1280x720_50,
	VESA_1280x720_30,
	VESA_1280x720_25,
	SignnalOff
}
Resolution_Type;

bool LT8618SX_Configure(void);
//void LT8618SX_Reset(void);
//void LT8618SX_Initial( void );

//void LT8618SX_video_check( void );

void LT8618SX_process( void );
#endif

//-----------------------------------------------------------

#endif
