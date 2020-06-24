/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   使用按键控制彩灯
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include "./boot_loader/boot_loader.h" 
#include "./usart/bsp_debug_usart.h"
#include "./tim/bsp_basic_tim.h"
#include "./internalFlash/bsp_internalFlash.h"  
#include "./ymodem/ymodem.h"
#include "./FATFS/ff.h"
#include "./flash/bsp_spi_flash.h"

FATFS fs;													/* FatFs文件系统对象 */

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	uint8_t update_flag = 0;
	
  /*初始化按键*/
  LED_GPIO_Config();
  Key_GPIO_Config();
	Debug_USART_Config();
  SPI_FLASH_Init();

	/* 轮询按键状态，若按键按下则反转LED */ 
	while(1)
	{
		if( Key_Scan(KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON  )
		{
			/* 获取接收数据 */
      update_flag = 1;
		}
    
    if( Key_Scan(KEY2_GPIO_PORT, KEY2_PIN) == KEY_ON  )
		{
      LED2_ON;
			SPI_FLASH_BulkErase();
      LED2_OFF;
		}
		
		if (update_flag)
		{
      LED2_ON;
			ymodem_receive();
      update_flag = 0;
      LED2_OFF;
		}
    
//    GetResOffset("GB2312_24_4BPP.xft");
	}
}



/*********************************************END OF FILE**********************/

