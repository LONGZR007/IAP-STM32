/**
  ******************************************************************************
  * @file    iap_arch.c
  * @author  long
  * @version V1.0
  * @date    2019-11-23
  * @brief   内部FLASH读写测试范例
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

/**
 * @brief   Xmodem 接收数据的接口.
 * @param   *data ：接收数据
 * @param   *len ：接收数据的长度
 * @return  接收数据的状态
 */
int x_receive(volatile uint8_t *data, uint32_t len)
{
	while (data_rx_flag == 1)   // 等待数据接收完成
	{}
	
	/* 获取接收数据 */
	data = get_rx_data();
		
	if (len == get_rx_len())
	{
		return -1;    // 长度不正确返回错误
	}
		
	return 0;
}

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

