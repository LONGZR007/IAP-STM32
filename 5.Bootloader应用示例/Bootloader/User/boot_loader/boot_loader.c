/**
  ******************************************************************************
  * @file    boot_loader.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   跳转实现
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./boot_loader/boot_loader.h" 
#include "./flash/bsp_spi_flash.h"
#include "./internalFlash/bsp_internalFlash.h"
#include "./lcd/bsp_lcd.h"

pFunction JumpToApplication; 
uint8_t buff[1024*10];

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_jump_app(u32 appaddr)
{ 
	if(((*(vu32*)appaddr)&0x2FF00000)==0x20000000)	                        // 检查栈顶地址是否合法.
	{
    /* Initialize user application's Stack Pointer & Jump to user application */
		JumpToApplication = (pFunction) (*(__IO uint32_t*) (appaddr + 4));		// 用户代码区第二个字为程序开始地址(复位地址)		
		__set_MSP(*(__IO uint32_t*) appaddr);				                         	// 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		JumpToApplication();									                                // 跳转到APP.
	}
}

/**
 * @brief   Xmodem 擦除要保存接收数据的扇区.
 * @param   address ：根据地址来擦除扇区
 * @return  返回当前扇区剩余的大小
 */
uint32_t i_flash_erasure(uint32_t address)
{
  sector_t sector_info;
  if (erasure_sector(address, 1))    // 擦除当前地址所在扇区
  {
    return 0;    // 擦除失败
  }

  sector_info = GetSector(address);    // 得到当前扇区的信息

  return sector_info.size + sector_info.start_addr - address;     // 返回当前扇区剩余大小
}

/**
 * @brief   文件数据接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int w_app_to_flash(uint32_t addr, char *file_data, uint32_t w_size)
{
  static uint32_t sector_size = 0;    /* 扇区剩余大小. */
  
  /* 当前扇区不够了擦除下一个. */
  if (sector_size <= w_size)
  {
    sector_size += i_flash_erasure(addr + sector_size);

    if (0 == sector_size)
    {
      return -1;
    }
  }
  
  if (flash_write_data(addr, (uint8_t *)file_data, w_size) == 0)    // 写入数据
  {
    sector_size -= w_size;
    return 0;
  }
  else 
  {
    return -1;
  }
}

/**
 * @brief   把外部flash数据拷贝到内部flash.
 * @param   des:内部FLASH地址
 * @param   src:外部FLASH地址
 * @param   size:要拷贝的文件大小
 * @return  0:文件接收成功 -1:文件接收失败
 */
int sflash_to_iflash(uint32_t des_addr, uint32_t src_addr, uint32_t size)
{
  uint32_t flash_address = des_addr;
  uint32_t w_size = 1024;
  uint32_t s_size = size;
  char cbuff[128];
  
  /* 显示文字提示 */
  sprintf(cbuff, "                  正在升级应用程序！");
  LCD_SetTextColor(LCD_COLOR565_WHITE);
  LCD_DisplayStringLine_EN_CH(LINE(3),(uint8_t* )cbuff);
  
  /* 绘制进度条背景 */
  LCD_SetTextColor(LCD_COLOR565_WHITE);
  L_DrawRect(100, 147, 600, 10);
  
  for (uint32_t i=0; i<size; i+=w_size)
  {
    if (size - i < w_size)
    {
      w_size = size - i;
    }
    /* 读 SPI FLASH 里面的数据 */
    SPI_FLASH_BufferRead(buff, src_addr + i, w_size);
    
    /* 写入内部 FLASH */
    if (w_app_to_flash(flash_address + i, (char *)buff, w_size) == -1)
    {
      return -1;
    }
    
    /* 绘制进度条 */
    LCD_SetTextColor(LCD_COLOR565_RED);
    L_DrawRect(101, 148, 598 * i / s_size, 8);
  }

  return 0;
}

