/**
  *********************************************************************
  * @file    gui_resource_port.c
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
#include "board.h" 
#include	"emXGUI.h"
#include	"emXGUI_Arch.h"
#include "x_libc.h" 
#include "gui_resource_port.h"
#include "gui_mem_port.h"

#if defined(CPU_MIMXRT1052DVL6B)
#define QSPIFLASH_ADDR 0x60000000         //外部FLASH的映射地址
#endif
/*=========================================================================================*/
/*访问资源设备的互斥信号量*/
static GUI_MUTEX *mutex_lock=NULL;
u8 buf[100];
/**
  * @brief  初始化资源设备（外部FLASH）
  * @param  无
  * @retval 是否初始化正常
  */
BOOL RES_DevInit(void)
{  
	mutex_lock=GUI_MutexCreate();
  
#if defined(STM32F429_439xx)
	if(SPI_FLASH_Init() == 0)
  {
    return TRUE;
  }
#elif defined(STM32H743xx)
  if(QSPI_FLASH_Init() == 0)
  {
    QSPI_FLASH_WriteStatusReg(1,0X00);
    QSPI_FLASH_WriteStatusReg(2,0X00);
    QSPI_FLASH_WriteStatusReg(3,0X60);
    GUI_DEBUG("\r\nFlash Status Reg1 is 0x%02X", QSPI_FLASH_ReadStatusReg(1));	
    GUI_DEBUG("\r\nFlash Status Reg2 is 0x%02X", QSPI_FLASH_ReadStatusReg(2));
    GUI_DEBUG("\r\nFlash Status Reg3 is 0x%02X", QSPI_FLASH_ReadStatusReg(3));    
    //RES_DevTest();
    return TRUE;
  }
#elif defined(CPU_MIMXRT1052DVL6B)
    if(1)
    {
      //外部FLASH已经初始化过了，这里不需要再进行初始化
      //直接返回TRUE即可
      RES_DevRead(buf,16*1024*1024, 100);
      GUI_DEBUG("%s", buf);
      return TRUE;
    }
#endif  
  else
    return FALSE;

	
}

/**
  * @brief  读取设备ID
  * @param  无
  * @retval 设备ID
  */
U32 RES_DevGetID(void)
{
	U32 id;
	GUI_MutexLock(mutex_lock,5000);
#if defined(STM32F429_439xx)
	id =SPI_FLASH_ReadID();
#elif defined(STM32H743xx)
  id = QSPI_FLASH_ReadID();
#endif  
  
	GUI_MutexUnlock(mutex_lock);
	return id;
}

/**
  * @brief  向设备写入内容
  * @param  buf 要写入的内容
  * @param  addr 写入的目标地址
  * @param  size 写入的数据量（size不应超过BLOCK大小）
  * @retval 是否写入正常
  */
BOOL RES_DevWrite(u8 *buf,u32 addr,u32 size)
{
	GUI_MutexLock(mutex_lock,5000);
	
#if defined(STM32F429_439xx)
	SPI_FLASH_BufferWrite(buf,addr,size);
#elif defined(STM32H743xx)
  BSP_QSPI_Write(buf,addr,size);
#endif    
	GUI_MutexUnlock(mutex_lock);
	return TRUE;
}

/**
  * @brief  从设备中读取内容
  * @param  buf 存储读取到的内容
  * @param  addr 读取的目标地址
  * @param  size 读取的数据量
  * @retval 是否读取正常
  */
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size)
{
	GUI_MutexLock(mutex_lock,5000);
	
#if defined(STM32F429_439xx)
	SPI_FLASH_BufferRead(buf,addr,size);
#elif defined(STM32H743xx)
  BSP_QSPI_Read(buf,addr,size);
#elif defined(CPU_MIMXRT1052DVL6B)
  
   
  
  memcpy(buf, (void *)(QSPIFLASH_ADDR+addr), size);
  
#endif      
	GUI_MutexUnlock(mutex_lock);
	return TRUE;
}

/**
  * @brief  擦除扇区
  * @param  addr 要擦除的扇区地址
  * @retval 扇区的字节数
  */
int RES_DevEraseSector(u32 addr)
{
	GUI_MutexLock(mutex_lock,5000);
#if defined(STM32F429_439xx)  
	SPI_FLASH_SectorErase(addr&0xFFFFF000);
#elif defined(STM32H743xx)
  //BSP_QSPI_Erase_Block(addr&0xFFFFF000);
#endif     
	GUI_MutexUnlock(mutex_lock);
	return 0;
}


/**
  * @brief  等待设备的写入或擦除结束
  * @param  timeout 最大等待时间（ms）
  * @retval 是否超时
  */
BOOL RES_WaitForWriteEnd(u32 timeout)
{
	GUI_MutexLock(mutex_lock,5000);
//  SPI_FLASH_WaitForWriteEnd();
//	SPI_FLASH_BufferRead(buf,addr,size);
	GUI_MutexUnlock(mutex_lock);
	return TRUE;
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
  
//  GUI_Printf("0x%x",id);

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

	len =x_strlen(res_name);
	for(i=0;i<GUI_CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		RES_DevRead((u8*)&dir,GUI_RES_BASE+i,sizeof(CatalogTypeDef));

		if(x_strncasecmp(dir.name,res_name,len)==0)
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
  * @retval -1表示找不到，其余值表示资源在FLASH中的基地址
  */
s32 RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir)
{
	int i,len;
  
	len =x_strlen(res_name);
  /* 根据名字遍历目录 */
	for(i=0;i<GUI_CATALOG_SIZE;i+=sizeof(CatalogTypeDef))
	{
		RES_DevRead((u8*)dir,GUI_RES_BASE+i,sizeof(CatalogTypeDef));

		if(x_strncasecmp(dir->name,res_name,len)==0)
		{
      /* dir.offset是相对基地址的偏移，此处返回绝对地址 */
      dir->offset += GUI_RES_BASE;
			return dir->offset ;
		}
	}
	return -1;
}


/**
  * @brief  从资源设备加载内容
  * @param  file_name[in]: 文件名
  * @param  buf[out]：加载后得到的缓冲区
  * @param  size[out]：内容的大小
  * @note   buf是根据内容的大小动态从VMEM中申请的，
  *         使用完毕buf后，需要调用h文件中的Release_Content函数释放buf的空间
  * @retval FALSE:失败; TRUE:成功
*/
BOOL RES_Load_Content(char *file_name, char** buf, u32* size) 
{	 
    int content_offset;
    CatalogTypeDef dir;
    BOOL result = TRUE;
  
    content_offset = RES_GetInfo_AbsAddr(file_name, &dir);
    if(content_offset > 0)
    {    
      /* 文件内容空间 */
      *buf = (char *)GUI_VMEM_Alloc(dir.size);
      if(*buf != NULL)
      {
        /* 加载数据*/
        RES_DevRead((u8 *)*buf,content_offset,dir.size); 
          
        *size = dir.size;
      }
      else
        result = FALSE;
    }
    else 
      result = FALSE;
   
    return result;
}

#if(GUI_RES_FS_EN)
/**
  * @brief  从文件系统加载内容
  * @param  file_name[in]: 文件路径
  * @param  buf[out]：加载后得到的缓冲区
  * @param  size[out]：内容的大小
  * @note   buf是根据内容的大小动态从VMEM中申请的，
  *         使用完毕buf后，需要调用h文件中的Release_Content函数释放buf的空间
  * @retval FALSE:失败; TRUE:成功
*/
BOOL FS_Load_Content(char *file_name, char** buf, u32* size) 
{	 	
    /* file objects */
    FIL     *file;												
    FRESULT fresult;  
    BOOL result = TRUE;
    UINT br;
  
    /* 文件句柄空间 */
    file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));
      
    /* 打开文件 */		
    fresult = f_open(file, file_name, FA_OPEN_EXISTING | FA_READ );
    if (fresult != FR_OK)
    {      
      GUI_ERROR("Open file failed!");
      GUI_VMEM_Free(file);
      return FALSE;
    }    

    *size = f_size(file);    
    /* 文件内容空间 */
    *buf = (char *)GUI_VMEM_Alloc(*size);
    if(*buf != NULL)
    {    
      /* 加载整个图片文件 */
      fresult = f_read(file, *buf, *size, &br);
      /* 关闭文件 */
      f_close(file);
    }
    else
      result = FALSE;    
    
    /* 释放空间 */
    GUI_VMEM_Free(file);    
    
    return result;
}
#endif




/********************************END OF FILE****************************/

