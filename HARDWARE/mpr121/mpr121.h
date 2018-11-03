#ifndef __MPR121_H
#define __MPR121_H
#include "myiic.h"   
#include "sys.h" 
#include "includes.h"

#ifdef MPR121_ENABLE


// MPR121 Register Defines
#define MHD_R	0x2B
#define NHD_R	0x2C
#define	NCL_R 	0x2D
#define	FDL_R	0x2E
#define	MHD_F	0x2F
#define	NHD_F	0x30
#define	NCL_F	0x31
#define	FDL_F	0x32
#define	ELE0_T	0x41
#define	ELE0_R	0x42
#define	ELE1_T	0x43
#define	ELE1_R	0x44
#define	ELE2_T	0x45
#define	ELE2_R	0x46
#define	ELE3_T	0x47
#define	ELE3_R	0x48
#define	ELE4_T	0x49
#define	ELE4_R	0x4A
#define	ELE5_T	0x4B
#define	ELE5_R	0x4C
#define	ELE6_T	0x4D
#define	ELE6_R	0x4E
#define	ELE7_T	0x4F
#define	ELE7_R	0x50
#define	ELE8_T	0x51
#define	ELE8_R	0x52
#define	ELE9_T	0x53
#define	ELE9_R	0x54
#define	ELE10_T	0x55
#define	ELE10_R	0x56
#define	ELE11_T	0x57
#define	ELE11_R	0x58
#define	FIL_CFG	0x5D
#define	ELE_CFG	0x5E
#define GPIO_CTRL0	0x73
#define	GPIO_CTRL1	0x74
#define GPIO_DATA	0x75
#define	GPIO_DIR	0x76
#define	GPIO_EN		0x77
#define	GPIO_SET	0x78
#define	GPIO_CLEAR	0x79
#define	GPIO_TOGGLE	0x7A
#define	ATO_CFG0	0x7B
#define	ATO_CFGU	0x7D
#define	ATO_CFGL	0x7E
#define	ATO_CFGT	0x7F
#define	SOFT_RESET	0x80


#define PRO_T 			0x59 // Touch Threshold register address
#define PRO_R 			0x5A // Touch Threshold register address

#define	PROX_MHDR		0x36 // ELEPROX Max Half Delta Rising register address - 0xFF
#define	PROX_NHDAR 		0x37 // ELEPROX Noise Half Delta Amount Rising register address - 0xFF
#define	PROX_NCLR 		0x38 // ELEPROX Noise Count Limit Rising register address - 0x00
#define	PROX_FDLR 		0x39 // ELEPROX Filter Delay Limit Rising register address - 0x00
#define	PROX_MHDF 		0x3A // ELEPROX Max Half Delta Falling register address - 0x01
#define	PROX_NHDAF		0x3B // ELEPROX Noise Half Delta Amount Falling register address - 0x01
#define	PROX_NCLF  		0x3C // ELEPROX Noise Count Limit Falling register address - 0xFF
#define	PROX_NDLF   	0x3D // ELEPROX Filter Delay Limit Falling register address - 0xFF
#define	PROX_DEB     	0x5B // ELEPROX Debounce register address

#define	PROX_NHDAT   	0x3E // ELEPROX Noise Half Delta Amount Touched register address - 0x00
#define	PROX_NCLT    	0x3F // ELEPROX Noise Count Limit Touched register address - 0x00
#define	PROX_FDLT    	0x40 // ELEPROX Filter Delay Limit Touched register address - 0x00

// Global Constants
#define TOU_THRESH	(0x0F)//0x0F
#define	REL_THRESH	(0x0A)//0x0A

#define TP_ID_0    (0)
#define TP_ID_1    (1)
#define TP_ID_2    (2)
#define TP_ID_3    (3)
#define TP_ID_4    (4)
#define TP_ID_5    (5)
#define TP_ID_6    (6)
#define TP_ID_7    (7)
#define TP_ID_8    (8)
#define TP_ID_9    (9)
#define TP_ID_10    (10)
#define TP_ID_11    (11)
#define TP_ID_12    (12)
#define TP_ID_UNKNOWN    (-1)


typedef enum TP_STATE_e{
    TP_state_release = 0,
    TP_state_touch,
}TP_STATE_t;

typedef struct TP_INFO {
    u8 id;
    char* name;

	u8 bitPos;
    TP_STATE_t state;

	unsigned long key_press_time;
    unsigned long key_release_time;
    unsigned long key_holding_time;
    bool event_pressed;
    bool event_released;
} TP_INFO_t;



					  
u8 MPR121_ReadOneByte(u8 RegAddr);							//指定地址读取一个字节
u8 MPR121_WriteOneByte(u8 RegAddr, u8 d);		//指定地址写入一个字节
void MPR121_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//指定地址开始写入指定长度的数据
u32 MPR121_ReadLenByte(u16 ReadAddr,u8 Len);					//指定地址开始读取指定长度数据
u32 MPR121_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度的数据
u32 MPR121_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//从指定地址开始读出指定长度的数据
u8 MPR121_Check(void);  //检查器件
void mpr121_init(void);
void mpr121_notify(void);
void mpr121_scan(void);
void mpr121_process(void);
u16 mpr121_read_ele_filtered_data(u8 id);
void mpr121_get_ele_data(void);

#endif

#endif
















