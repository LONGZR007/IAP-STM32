/**
  ******************************************************************************
  * @file    iap_arch.c
  * @author  long
  * @version V1.0
  * @date    2019-11-23
  * @brief   ymodem 对外接口文件
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
#include "./ymodem/ymodem.h"

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

/**
 * @brief   Ymodem 擦除要保存接收数据的扇区.
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

