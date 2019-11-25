/**
  ******************************************************************************
  * @file    bsp_internalFlash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   内部FLASH读写测试范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./internalFlash/bsp_internalFlash.h"   
#include <stdio.h>

/**
  * @brief  根据地址和长度擦除所需扇区
  * @param  start_address ：要写入的起始地址
	* @param  len ：长度
  * @retval 无
  */
int erasure_sector(uint32_t start_address, uint32_t len)
{
	sector_t sector_inof;         // 记录扇区的信息
	uint32_t sector_size  = 0;    // 记录所需扇区的大小
	uint32_t address = start_address;
	
	sector_inof = GetSector(start_address);    // 获取第一个扇区的信息

  /* 擦除用户区域 (用户区域指程序本身没有使用的空间，可以自定义)**/
  /* 清除各种FLASH的标志位 */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	
	if ((sector_inof.start_addr + sector_inof.size) - (start_address + len) > 0)    // 第一个扇区就足够存下数据
	{
		/* VoltageRange_3 以“字”的大小进行操作 */ 
    if (FLASH_EraseSector(sector_inof.number, VoltageRange_3) != FLASH_COMPLETE)    // 擦除相应扇区
    { 
      /*擦除出错，返回，实际应用中可加入处理 */
			return -1;
    }
	}
	else     // 第一个扇区还不足以放下所有数据
	{
		sector_size += (sector_inof.start_addr + sector_inof.size) - start_address;    // 记录第一个扇区的大小
		address += sector_size;                // 偏移至下一个扇区的起始地址
		
		do
		{
			sector_inof = GetSector(address);    // 获取一个扇区的信息
			
			/* 擦除扇区 VoltageRange_3 以“字”的大小进行操作 */ 
			if (FLASH_EraseSector(sector_inof.number, VoltageRange_3) != FLASH_COMPLETE)    // 擦除相应扇区
			{ 
				/*擦除出错，返回，实际应用中可加入处理 */
				return -1;
			}
			
			address += sector_inof.size;        // 偏移至下一个扇区的起始地址
			sector_size += sector_inof.size;    // 记录擦除扇区的大小
			
			
		}while (sector_size >= len);          // 擦除的扇区可以存下所需的数据
	}
	
	return 0;
}

/**
  * @brief  根据地址和长度擦除所需扇区
  * @param  start_address ：要写入的起始地址
  * @param  *data : 需要保存的数据
	* @param  len ：长度
  * @retval 无
  */
int flash_write_data(uint32_t start_address, const void *data, uint32_t len)
{
	uint8_t *data_8 = (uint8_t *)data;
	uint32_t address = start_address;
	
	/* 以“字”的大小为单位写入数据 ******************************** */
	address = start_address;
  while (len)
  {
    if (FLASH_ProgramByte(address, *data_8) == FLASH_COMPLETE)
    {
			address++;
			data_8++;
      len -= len > 1 ? 1 : len;
    }
    else
    {
      /*写入出错，返回，实际应用中可加入处理 */
			return -1;
    }
  }
	
	return 0;
}

int save_data_flash(uint32_t start_address, const void *data, uint32_t len) 
{
	uint8_t *data_v = (uint8_t *)data;
	uint32_t address = start_address;
	uint32_t data_len = len;
	__IO uint8_t uw_data = 0;
	__IO uint8_t uwMemoryProgramStatus = 0;
	
	if (start_address % 4 != 0)    // 检查地址的合法性
	{
		return -1;    // 地址非法，返回
	}
	
	/* FLASH 解锁 ********************************/
  /* 使能访问FLASH控制寄存器 */
  FLASH_Unlock();
	
	if (erasure_sector(address, len))    // 擦除所需扇区
	{
		return -1;    // 擦除出错，返回
	}
	
	if (flash_write_data(start_address, data, len))    // 擦除所需扇区
	{
		return -1;    // 写数据出错，返回
	}

	/* 给FLASH上锁，防止内容被篡改*/
  FLASH_Lock(); 
	
	address = start_address;
	
	printf("起始地址 ： 0x%X 结束地址 ：0x%X \r\n", address, start_address + data_len);
	
	/* 数据校验 */
  while (address < start_address + data_len)
  {
    uw_data = *(__IO uint8_t*)address;
		
    if (uw_data != *data_v++)
    {
			printf("地址 ： 0x%X 实际内容是 ：0x%X  希望的内容是 ：0x%X \r\n", address, uw_data, *data_v);
      uwMemoryProgramStatus++;  
    }

    address++;
  }  
	
  /* 数据校验不正确 */
  if(uwMemoryProgramStatus)
  {    
		return -1;
  }
  else /*数据校验正确*/
  { 
		return 0;   
  }
}

/**
  * @brief  根据输入的地址给出它所在的sector
  *					例如：
						uwStartSector = GetSector(FLASH_USER_START_ADDR);
						uwEndSector = GetSector(FLASH_USER_END_ADDR);	
  * @param  Address：地址
  * @retval 地址所在的sector
  */
sector_t GetSector(uint32_t Address)
{
  sector_t sector;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_0;
    sector.number = FLASH_Sector_0; 
		sector.size = 16 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_1;
    sector.number = FLASH_Sector_1;  
		sector.size = 16 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_2;
    sector.number = FLASH_Sector_2;  
		sector.size = 16 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_3;
    sector.number = FLASH_Sector_3;  
		sector.size = 16 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_4;
    sector.number = FLASH_Sector_4;  
		sector.size = 64 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_5;
    sector.number = FLASH_Sector_5;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_6;
    sector.number = FLASH_Sector_6;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_7;
    sector.number = FLASH_Sector_7;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_8;
    sector.number = FLASH_Sector_8;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_9;
    sector.number = FLASH_Sector_9;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_10;
    sector.number = FLASH_Sector_10;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_11;
    sector.number = FLASH_Sector_11;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_12;
    sector.number = FLASH_Sector_12;  
		sector.size = 16 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_13;
    sector.number = FLASH_Sector_13;  
		sector.size = 16 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_14;
    sector.number = FLASH_Sector_14;  
		sector.size = 16 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_15;
    sector.number = FLASH_Sector_15;  
		sector.size = 16 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_16;
    sector.number = FLASH_Sector_16;  
		sector.size = 64 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_17;
    sector.number = FLASH_Sector_17;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_18;
    sector.number = FLASH_Sector_18;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_19;
    sector.number = FLASH_Sector_19;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_20;
    sector.number = FLASH_Sector_20;  
		sector.size = 128 * KB;
  } 
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_21;
    sector.number = FLASH_Sector_21;  
		sector.size = 128 * KB;
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
		sector.start_addr = ADDR_FLASH_SECTOR_22;
    sector.number = FLASH_Sector_22;  
		sector.size = 128 * KB;
  }
  else/*if ((Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23)) */
  {
		sector.start_addr = ADDR_FLASH_SECTOR_23;
    sector.number = FLASH_Sector_23;  
		sector.size = 16 * KB;
  }
  
  return sector;
}



