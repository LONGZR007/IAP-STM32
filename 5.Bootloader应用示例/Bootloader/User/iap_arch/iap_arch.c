/**
  ******************************************************************************
  * @file    iap_arch.c
  * @author  long
  * @version V1.0
  * @date    2019-11-23
  * @brief   xmodem 对外接口文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./iap_arch/iap_arch.h"   
#include <stdio.h>
#include "./usart/bsp_debug_usart.h"
#include "./internalFlash/bsp_internalFlash.h" 
#include "./boot_loader/boot_loader.h"
#include "./lcd/bsp_lcd.h"

static uint32_t xmodem_actual_flash_address = FLASH_APP_ADDR;       /* 写入的地址. */

/**
 * @brief   Xmodem 发送一个字符的接口.
 * @param   ch ：发送的数据
 * @return  返回发送状态
 */
int x_transmit_ch(uint8_t ch)
{
	Usart_SendByte(DEBUG_USART, ch);
	
	return 0;
}

/**
 * @brief   Xmodem 擦除要保存接收数据的扇区.
 * @param   address ：根据地址来擦除扇区
 * @return  返回当前扇区剩余的大小
 */
uint32_t x_receive_flash_erasure(uint32_t address)
{
  sector_t sector_info;
  if (erasure_sector(address, 1))    // 擦除当前地址所在扇区
  {
    return 0;    // 擦除失败
  }

  sector_info = GetSector(address);    // 得到当前扇区的信息

  return (sector_info.size + sector_info.start_addr - address);     // 返回当前扇区剩余大小
}

/**
  * @brief   Xmodem 将接受到的数据保存到flash.
  * @param  start_address ：要写入的起始地址
  * @param  *data : 需要保存的数据
	* @param  len ：长度
  * @return 写入状态
 */
int x_receive_flash_writea(uint32_t start_address, const void *data, uint32_t len)
{
  if (flash_write_data(start_address, (uint8_t *)data, len) == 0)    // 擦除当前地址所在扇区
  {
    return 0;    // 写入成功
  }
  else
  {
    return -1;    // 写入失败
  }
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
  static uint32_t sector_size = 0;    /* 扇区剩余大小. */
  static uint32_t recv_size = 0;    /* 扇区剩余大小. */
  uint8_t buff[128];
  
  /* 当前扇区不够了擦除下一个. */
  if (sector_size <= w_size)
  {
    sector_size += x_receive_flash_erasure(xmodem_actual_flash_address + sector_size);

    if (0 == sector_size)
    {
      return -1;
    }
  }
  
  if (flash_write_data(xmodem_actual_flash_address, (uint8_t *)file_data, w_size) == 0)    // 写入数据
  {
    xmodem_actual_flash_address += w_size;
    recv_size += w_size;
    sector_size -= w_size;
    sprintf((char*)buff, "                 已接收：%d字节！", recv_size);
    LCD_DisplayStringLine_EN_CH(LINE(3), buff);
    return 0;
  }
  else 
  {
    return -1;
  }
}

/**
 * @brief   文件接收完成回调.
 * @param   *ptr: 控制句柄.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_callback(void *ptr)
{
//  printf("开始运行 App！\r\n");
//	iap_jump_app(FLASH_APP_ADDR);
  LCD_DisplayStringLine_EN_CH(LINE(5), "                 应用程序接收完成！");
  
  return 0;
}



