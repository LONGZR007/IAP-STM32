/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   向STM32的内部FLASH写入数据
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
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"   
#include "./internalFlash/bsp_internalFlash.h"   



/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  /*初始化USART，配置模式为 115200 8-N-1*/
  Debug_USART_Config();
	LED_GPIO_Config();
 
	LED_BLUE;
	/*调用printf函数，因为重定向了fputc，printf的内容会输出到串口*/
  printf("this is a usart printf demo. \r\n");
	printf("\r\n 欢迎使用野火  STM32 F429 开发板。\r\n");	
	printf("正在进行读写内部FLASH实验，请耐心等待\r\n");
	
//	if(InternalFlash_Test()==0)
//	{
//		LED_GREEN;
//		printf("读写内部FLASH测试成功\r\n");

//	}
//	else
//	{
//		printf("读写内部FLASH测试失败\r\n");
//		LED_RED;
//	}
	
	
  while(1)
	{	} 

}



/*********************************************END OF FILE**********************/

