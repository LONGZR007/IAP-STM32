/**
  *********************************************************************
  * @file    resource_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   管理外部资源的接口
  *          例如用来管理外部FLASH中的资源
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#include <string.h>
#include "./font/resource_port.h"
#include "./flash/bsp_spi_flash.h"

#if defined(CPU_MIMXRT1052DVL6B)
#define QSPIFLASH_ADDR 0x60000000         //外部FLASH的映射地址
#endif
/*=========================================================================================*/
/*访问资源设备的互斥信号量*/

u8 buf[100];

/**
  * @brief  从设备中读取内容
  * @param  buf 存储读取到的内容
  * @param  addr 读取的目标地址
  * @param  size 读取的数据量
  * @retval 是否读取正常
  */
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size)
{
#if defined(STM32F429_439xx)
	SPI_FLASH_BufferRead(buf,addr,size);
#elif defined(STM32H743xx)
  BSP_QSPI_Read(buf,addr,size);
#elif defined(CPU_MIMXRT1052DVL6B)
  
  memcpy(buf, (void *)(QSPIFLASH_ADDR+addr), size);
  
#endif      
	return 1;
}

/*=========================================================================================*/
#if 0
/**
  * @brief  资源设备测试
  * @param  无
  * @retval 无
  */
void RES_DevTest(void)
{
	char *buf;
	volatile u32 id;

	buf =GUI_VMEM_Alloc(4096);
	RES_DevInit();
	id =RES_DevGetID();

#if 0  
	while(1)
	{	
    int i = 0;

		RES_DevEraseSector(i);

		memset(buf,i>>12,4096);
		RES_DevWrite(buf,i,4096);

		memset(buf,0,4096);
		RES_DevRead((u8 *)buf,i,4096);

		i += 4096;
		GUI_msleep(100);

	}
#endif  
	GUI_VMEM_Free(buf);
}
#endif

#if 0
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize     (countof(Tx_Buffer123)-1)
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
uint8_t Tx_Buffer[] = "好stm32开发板\r\nhttp://firestm32.taobao.c感谢您选用野火stm32开发板\r\nhttp://firestm32.taobao.com感谢您选用野火stm32开发板\r\nhttp://firestm32.taobao.com感谢您选用野火stm32开发板\r\nhttp://firestm32.taobao.com感谢您选用野火stm32开发板\r\nhttp://firestm32.taobao.com";
uint8_t Rx_Buffer[BufferSize];
#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
__IO TestStatus TransferStatus1 = FAILED;

TestStatus Buffercmp1(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}
CatalogTypeDef dir;
int font_base; 
void RES_DevTest(void)
{
  u8 *Rx_Buffer = (u8*)GUI_VMEM_Alloc(BufferSize);
  //BSP_QSPI_Erase_Chip();
//  HAL_InitTick(5);
  BSP_QSPI_Write(Tx_Buffer123, FLASH_WriteAddress, BufferSize);
  HAL_InitTick(5);
  BSP_QSPI_Read(Rx_Buffer, FLASH_ReadAddress, BufferSize);
  TransferStatus1 = Buffercmp1(Tx_Buffer123, Rx_Buffer, BufferSize);
  if( PASSED == TransferStatus1 )
  {    
    LED_GREEN;
    GUI_DEBUG("\r\n16M串行flash(W25Q256)测试成功!\n\r");
  }
  else
  {        
    LED_RED;
    GUI_DEBUG("\r\n16M串行flash(W25Q256)测试失败!\n\r");
  }
  GUI_VMEM_Free(Rx_Buffer);  
}
#endif

/**
  * @brief  从FLASH中的目录查找相应的资源位置
  * @param  res_base 目录在FLASH中的基地址
  * @param  res_name[in] 要查找的资源名字
  * @retval -1表示找不到，其余值表示资源在FLASH中的基地址
  */
s32 RES_GetOffset(const char *res_name)
{
	int i,len;
	CatalogTypeDef dir;

	len =strlen(res_name);
	for(i=0;i<GUI_CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		RES_DevRead((u8*)&dir,GUI_RES_BASE+i,sizeof(CatalogTypeDef));

		if(strncasecmp(dir.name,res_name,len)==0)
		{
      /* dir.offset是相对基地址的偏移，此处返回绝对地址 */
			return dir.offset + GUI_RES_BASE;
		}
	}

	return -1;
}

/**
  * @brief  从FLASH中的目录查找相应资源的信息
  * @param  res_base 目录在FLASH中的基地址
  * @param  res_name[in] 要查找的资源名字
  * @param  dir[out] 要查找的资源名字
  * @note   此处dir.offset会被赋值为资源的绝对地址！！
  * @retval -1表示找不到，其余值表示资源在FLASH中的绝对地址
  */
s32 RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir)
{
	int i,len;
  
	len =strlen(res_name);
  /* 根据名字遍历目录 */
	for(i=0;i<GUI_CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		RES_DevRead((u8*)dir,GUI_RES_BASE+i,sizeof(CatalogTypeDef));

		if(strncasecmp(dir->name,res_name,len)==0)
		{
      /* dir.offset是相对基地址的偏移，此处返回绝对地址 */
      dir->offset += GUI_RES_BASE;
			return dir->offset ;
		}
	}
	return -1;
}

/********************************END OF FILE****************************/

