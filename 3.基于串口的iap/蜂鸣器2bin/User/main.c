/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   蜂鸣器例程
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
#include "./beep/bsp_beep.h"



void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	SCB->VTOR = 0x8010000; /* Vector Table Relocation in Internal FLASH */
	/*蜂鸣器端口初始化 */
	BEEP_GPIO_Config(); 

	/* 控制IO */
	while (1)
	{
		BEEP_TOGGLE;
		Delay(0xFFFFFF);
	}
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/

