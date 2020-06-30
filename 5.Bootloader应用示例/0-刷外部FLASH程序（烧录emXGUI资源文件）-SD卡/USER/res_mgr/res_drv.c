/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   led应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
 
#include <string.h>
#include <stdlib.h>

#include "./fatfs/drivers/fatfs_flash_spi.h"
#include "./res_mgr/res_drv.h"
#include "RES_MGR.h"



/*=========================================================================================*/

#define RES_BASE 0x00200000


int GetResOffset(u32 res_base,const char *res_name);

/*=========================================================================================*/

int RES_GetOffset(const char *res_name)
{
	int offset;

	offset=GetResOffset(RES_BASE,res_name);

	if(offset>0)
	{
		return RES_BASE+offset;
	}
	return -1;
}


/*=========================================================================================*/

//static GUI_MUTEX *mutex_lock=NULL;

void RES_DevInit(void)
{
//	mutex_lock=GUI_MutexCreate();

	TM_FATFS_FLASH_SPI_disk_initialize();

	RES_DevTest();
}

uint32_t RES_DevGetID(void)
{
	uint32_t id;
//	GUI_MutexLock(mutex_lock,5000);

	id =SPI_FLASH_ReadID();
//	GUI_MutexUnlock(mutex_lock);
	return id;
}

int8_t RES_DevWrite(u8 *buf,u32 addr,u32 size)
{
//	GUI_MutexLock(mutex_lock,5000);
	SPI_FLASH_SectorErase(addr&0xFFFFF000);
	SPI_FLASH_BufferWrite(buf,addr,size);
//	GUI_MutexUnlock(mutex_lock);
	return 1;
}

int8_t RES_DevRead(u8 *buf,u32 addr,u32 size)
{
//	GUI_MutexLock(mutex_lock,5000);

	SPI_FLASH_BufferRead(buf,addr,size);
//	GUI_MutexUnlock(mutex_lock);
	return 1;
}

int RES_DevEraseSector(u32 addr)
{
//	GUI_MutexLock(mutex_lock,5000);
	SPI_FLASH_SectorErase(addr&0xFFFFF000);
//	GUI_MutexUnlock(mutex_lock);
	return 4096;
}

/*=========================================================================================*/

void RES_DevTest(void)
{
	int i;
	char *buf;
	volatile u32 id;

	buf =malloc(4096);
	i=0;
	RES_DevInit();
	id =RES_DevGetID();
  
  printf("0x%x",id);
  
//	while(1)
	{

#if 0
		RES_DevEraseSector(i);

		memset(buf,i>>12,4096);
		RES_DevWrite(buf,i,4096);
#endif
		memset(buf,0,4096);
		RES_DevRead((u8 *)buf,i,4096);

		i += 4096;
//		GUI_msleep(100);

	}
	free(buf);
}


/*********************************************END OF FILE**********************/
