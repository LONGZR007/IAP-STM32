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

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	uint8_t *app_data = NULL;
  uint32_t app_len = 0;
	
  /*初始化按键*/
  Key_GPIO_Config();
	Debug_USART_Config();
	TIMx_Configuration();
	
	printf(" IAP 演示 DEMO！\r\n");

	/* 轮询按键状态，若按键按下则反转LED */ 
	while(1)
	{
    
    if (data_rx_flag == 1 > 0 && get_sec_timestamp() > 2)    // 接收到数据并且已经超过2s以上没有接收到新的数据
    {
      data_rx_flag = 2;
      printf("新的 App 已经接收完成！\r\n");
    }
    
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			/* 获取接收数据 */
      app_data = get_rx_data();
			app_len  = get_rx_len();
			
			printf("开始升级 App ！共 %d 个字节\r\n", app_len);
			
			for (uint32_t i=0; i<app_len; i++)
			{
				printf("%02X ", app_data[i]);
			}
      
      /* 写 App 到 FLASH */
			if (flash_write_data(FLASH_APP_ADDR, app_data, app_len) == 0)
			{
				printf("升级 App 完成！\r\n");
			}
			else
			{
				printf("升级 App 失败！\r\n");
			}
			
			reset_rx_data();
		}
    
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
			/*LED2反转*/
			printf("开始运行 App！\r\n");
			iap_jump_app(FLASH_APP_ADDR);
		}
	}
}



/*********************************************END OF FILE**********************/

