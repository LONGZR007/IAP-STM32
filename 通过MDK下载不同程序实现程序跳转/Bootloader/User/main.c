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

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  /*初始化按键*/
  Key_GPIO_Config();
	
	/* 轮询按键状态，若按键按下则反转LED */ 
	while(1)
	{	   
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			/*LED1反转*/
			iap_jump_app(0x8042000);
		}   
    
    if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
		{
			/*LED2反转*/
			iap_jump_app(0x08010000);
		}   
	}
}



/*********************************************END OF FILE**********************/

