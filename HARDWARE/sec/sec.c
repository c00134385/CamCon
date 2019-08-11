
#include "sec.h"


static u8 sec_code[4]; // 
const u32 sec_code_holder = 0x65A61B1E; //ng:0x877A67C4; //ok:0x87716773
BOOL sec_init(void)
{
	u32 cid[3];
	u32 code;
	BOOL ret = false;
	//get cid
	cid[0]=*(vu32*)(0x1ffff7e8);
	cid[1]=*(vu32*)(0x1ffff7ec);
	cid[2]=*(vu32*)(0x1ffff7f0);
	//encrypt algy
	code=(cid[0]>>1)+(cid[1]>>2)+(cid[2]>>3);
	code = ~code;
	code = (code / 3) * 5;
	//printf("\r\n 0x%08x 0x%08x 0x%08x 0x%08x", cid[0], cid[1], cid[2], code);
	sec_code[0] = code >> 24;
	sec_code[1] = code >> 16;
	sec_code[2] = code >> 8;
	sec_code[3] = code;

	//STMFLASH_Read((u32)&sec_code_holder,(u16*)code,4);
	if(code == sec_code_holder)
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


