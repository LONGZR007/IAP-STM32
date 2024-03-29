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
  * 实验平台:野火 STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include "./usart/bsp_debug_usart.h"
#include "./xmodem/xmodem.h"
#include "./FATFS/ff.h"

FATFS fs;													/* FatFs文件系统对象 */

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	uint8_t update_flag = 0;
  FRESULT res_sd;                /* 文件操作结果 */
	
  /*初始化按键*/
  LED_GPIO_Config();
  Key_GPIO_Config();
	Debug_USART_Config();
  
  //在外部 sd 挂载文件系统
	res_sd = f_mount(&fs, "0:" ,1);
	if(res_sd!=FR_OK)
  {
    LED1_ON;
    printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
    printf("！！可能原因：SD卡初始化不成功。\r\n");
		while(1);
  }
  
	/* 轮询按键状态，若按键按下则反转LED */ 
	while(1)
	{
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			/* 获取接收数据 */
      update_flag = 1;
		}
		
		if (update_flag)
		{
      LED2_ON;
			if (xmodem_receive() != 0)
      {
        LED1_ON;
      }
      update_flag = 0;
      LED2_OFF;
		}
	}
}



/*********************************************END OF FILE**********************/

