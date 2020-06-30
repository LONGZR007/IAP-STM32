/**
  *********************************************************************
  * @file    gui_fs_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   文件系统接口，需要实现FileSystem_Init函数
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
#include "gui_fs_port.h"
#include "ff.h"

#if defined(STM32F429_439xx)
#include "sdio/bsp_sdio_sd.h"
#elif defined(STM32H743xx)
#include "./sd_card/bsp_sdio_sd.h"
#elif defined(CPU_MIMXRT1052DVL6B)
#include "./bsp/sd_fatfs_test/bsp_sd_fatfs_test.h"

#define BUFFER_SIZE (100U)

SDK_ALIGN(uint8_t g_bufferWrite[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
SDK_ALIGN(uint8_t g_bufferRead[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
#endif  


	/* FatFs文件系统对象 */
static FATFS fs __EXRAM;									

void FileSystem_Test(void);



/**
  * @brief  文件系统初始化
  * @param  无
  * @retval TRUE  FALSE
  */
BOOL FileSystem_Init(void)
{ 
  /* 文件操作结果 */
                

	/* 这个函数不允许使用rtt 的 printf打印
		 只能使用printf	*/
	
	/* 打开中断，如果不加互斥量可以在board.c中进行初始化
		 则必须打开中断，在任务中初始化的时候则不需要打开中断 */
//	rt_base_t level = 0;
//	rt_hw_interrupt_enable(level);	

  /* 禁用WiFi模块 */
#if defined(STM32F429_439xx)
  BL8782_PDN_INIT();
#elif defined(STM32H743xx)
  WIFI_PDN_INIT();
#endif  
	

#if defined(STM32F429_439xx) || defined(STM32H743xx)
  
  int i=50;
  while(i--)    // 多挂几次
  {
    //在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
    FRESULT res_sd; 
    res_sd = f_mount(&fs,"0:",1);
    
    if(res_sd == FR_NO_FILESYSTEM)
    {
      printf("》SD卡还没有文件系统...\r\n");
      /* 格式化 */
      res_sd=f_mkfs("0:",0,0);							
      
      if(res_sd == FR_OK)
      {
        printf("》SD卡已成功格式化文件系统。\r\n");
        /* 格式化后，先取消挂载 */
        res_sd = f_mount(NULL,"0:",1);			
        /* 重新挂载	*/			
        res_sd = f_mount(&fs,"0:",1);
      }
      else
      {
  //			LED_RED;
        printf("《《格式化失败。》》\r\n");
  //			while(1);
      }
      //9return FALSE;
    }
    else if(res_sd!=FR_OK)
    {
      printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
      printf("！！可能原因：没有接入SD卡。\r\n");
      //return FALSE;

    }
    else
    {
      printf("》文件系统挂载成功\r\n");
      
  #if 0
      /* 文件系统测试 */
      FileSystem_Test();
  #endif 
      
      /* 尝试进行unicode编码转换，
        当使用extern_cc936时，可测试是否存在cc936资源*/
      ff_convert('a',1);
      
      break;
    }
  }
#elif defined(CPU_MIMXRT1052DVL6B)
  f_mount_test(&fs);
#endif
  if (i>=0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#if 0
FIL fnew;													/* 文件对象 */
UINT fnum;            					  /* 文件成功读写数量 */

uint8_t WriteBuffer[] = "你好啊！野火";
uint8_t ReadBuffer[128];

/**
  * @brief  文件系统读写测试
  * @param  无
  * @retval TRUE  FALSE
  */
void FileSystem_Test(void)
{
  /* 文件操作结果 */
  FRESULT res_sd;               

	/*----------------------- 文件系统测试：写测试 -----------------------------*/
	/* 打开文件，如果文件不存在则创建它 */
	printf("\r\n****** 即将进行文件写入测试... ******\r\n");	
	res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt",FA_CREATE_ALWAYS | FA_WRITE );
	if ( res_sd == FR_OK )
	{
		printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
    /* 将指定存储区内容写入到文件内 */
		res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
    if(res_sd==FR_OK)
    {
      printf("》文件写入成功，写入字节数据：%d\n",fnum);
      printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
    }
    else
    {
      printf("！！文件写入失败：(%d)\n",res_sd);
    }    
		/* 不再读写，关闭文件 */
    f_close(&fnew);
	}
	else
	{	
//		LED_RED;
		printf("！！打开/创建文件失败。\r\n");
	}
	
/*------------------- 文件系统测试：读测试 ------------------------------------*/
	printf("****** 即将进行文件读取测试... ******\r\n");
	res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ); 	 
	if(res_sd == FR_OK)
	{
//		LED_GREEN;
		printf("》打开文件成功。\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(res_sd==FR_OK)
    {
      printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
      printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("！！文件读取失败：(%d)\n",res_sd);
    }	
		/* 不再读写，关闭文件 */
		f_close(&fnew);	
	}
	else
	{
//		LED_RED;
		printf("！！打开文件失败。\r\n");
	}
//	/* 不再读写，关闭文件 */
//	f_close(&fnew);	
  
//	/* 不再使用文件系统，取消挂载文件系统 */
//	f_mount(NULL,"0:",1);
	
}
#endif
/********************************END OF FILE****************************/

