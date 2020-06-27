/**
  ******************************************************************************
  * @file    ymodem_app.c
  * @author  long
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ymodem应用示例
  ******************************************************************************
***/

#include <string.h>
#include "./ymodem/ymodem.h"
#include "./ymodem_res_mgr/burn_resources.h"
#include "./usart/bsp_debug_usart.h"

/**
 * @brief   Ymodem 发送一个字符的接口.
 * @param   ch ：发送的数据
 * @return  返回发送状态
 */
int y_transmit_ch(uint8_t ch)
{
	Usart_SendByte(DEBUG_USART, ch);
	
	return 0;
}

#if 0

#include "./FATFS/ff.h"

/* 全局变量 */
static FIL fnew;												/* 文件对象 */
static FRESULT res_sd;                  /* 文件操作结果 */
static UINT fnum;            					  /* 文件成功读写数量 */

/**
 * @brief   文件名和大小接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_nanme_size_callback(void *ptr, char *file_name, uint32_t file_size)
{
  char buff[50];
  FIL **f_ptr = ptr;    /* 文件对象 */
  
  ptr = &fnew;
  
  sprintf(buff, "0:/%s", file_name);
  res_sd = f_open(*f_ptr, buff, FA_OPEN_APPEND | FA_WRITE );
  if ( res_sd != FR_OK )
  {	
    /* Flash 错误. */
    return -1;
  }
  
  return 0;
}

/**
 * @brief   文件数据接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_data_callback(void *ptr, char *file_data, uint32_t w_size)
{
  FIL **f_ptr = ptr;    /* 文件对象 */
  
  res_sd=f_write(*f_ptr, (uint8_t *)file_data, (uint32_t)w_size, &fnum);
  if(res_sd != FR_OK)
  {
    /* 不再读写，关闭文件 */
    f_close(*f_ptr);
    /* Flash 错误. */
    return -1;
  }
  
  return 0;
}

/**
 * @brief   一个文件接收完成回调.
 * @param   *ptr: 控制句柄.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_callback(void *ptr)
{
  FIL **f_ptr = ptr;    /* 文件对象 */
  f_close(*f_ptr);
  
  return 0;
}

#else

#include "./flash/bsp_spi_flash.h"

/**
  * @brief  从FLASH中的目录查找相应的资源位置
  * @param  res_base 目录在FLASH中的基地址
  * @param  res_name[in] 要查找的资源名字
  * @retval -1表示找不到，其余值表示资源在FLASH中的基地址
  */
int GetResOffset(const char *res_name)
{
  
	int i, len;
	CatalogTypeDef dir;

	len =strlen(res_name);
  for(i=0; i<CATALOG_SIZE; i+=sizeof(CatalogTypeDef))
	{
		SPI_FLASH_BufferRead((u8*)&dir, RESOURCE_BASE_ADDR + i, sizeof(CatalogTypeDef));
    
		if(strncasecmp(dir.name, res_name, len)==0)
		{
			return dir.offset + RESOURCE_BASE_ADDR;
		}
	}

	return -1;
}

static CatalogTypeDef w_data;

/**
 * @brief   烧录资源目录.
 * @param   *data: 目录数据.
 * @param   *size: 目录长度.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int burn_catalog_flash(uint8_t *data, uint32_t size)
{
  static uint32_t ctatlog = RESOURCE_BASE_ADDR;
  static CatalogTypeDef dir = 
  {
    .name[0] = 0,
    .offset = CATALOG_SIZE,
    .size = 0,
  };
  
  strcpy(dir.name, (char *)data);
  dir.size = size;
  
  w_data = dir;
  
  SPI_FLASH_BufferWrite((u8*)&dir, ctatlog, sizeof(CatalogTypeDef));
  
  dir.offset += size;
  ctatlog += sizeof(CatalogTypeDef);
  
  return 0;
}

/**
 * @brief   烧录资源数据.
 * @param   *data: 资源数据.
 * @param   *size: 资源长度.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int burn_res_flash(uint8_t *data, uint32_t size)
{
  SPI_FLASH_BufferWrite((u8*)data, w_data.offset + RESOURCE_BASE_ADDR, size);
  w_data.offset += size;
  
  return 0;
}

/**
 * @brief   文件名和大小接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_nanme_size_callback(void *ptr, char *file_name, uint32_t file_size)
{
  burn_catalog_flash((uint8_t *)file_name, file_size);
  
  return 0;
}

/**
 * @brief   文件数据接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_data_callback(void *ptr, char *file_data, uint32_t w_size)
{
  burn_res_flash((uint8_t *)file_data, w_size);
  
  return 0;
}

/**
 * @brief   一个文件接收完成回调.
 * @param   *ptr: 控制句柄.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_callback(void *ptr)
{
  return 0;
}

#endif
