/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   控制多彩流水灯
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

void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	SCB->VTOR = 0x8010000 | 0; /* Vector Table Relocation in Internal FLASH */

	/* LED 端口初始化 */
	LED_GPIO_Config();

	/* 控制LED灯 */
	while (1)
	{
		LED1( ON );			 // 亮 
		Delay(0xFFFFFF);
		LED1( OFF );		  // 灭

		LED2( ON );			// 亮 
		Delay(0xFFFFFF);
		LED2( OFF );		  // 灭

		LED3( ON );			 // 亮 
		Delay(0xFFFFFF);
		LED3( OFF );		  // 灭	

		/*轮流显示 红绿蓝黄紫青白 颜色*/
		LED_RED;
		Delay(0xFFFFFF);
		
		LED_GREEN;
		Delay(0xFFFFFF);
		
		LED_BLUE;
		Delay(0xFFFFFF);
		
		LED_YELLOW;
		Delay(0xFFFFFF);
		
		LED_PURPLE;
		Delay(0xFFFFFF);
				
		LED_CYAN;
		Delay(0xFFFFFF);
		
		LED_WHITE;
		Delay(0xFFFFFF);
		
		LED_RGBOFF;
		Delay(0xFFFFFF);
	}
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

