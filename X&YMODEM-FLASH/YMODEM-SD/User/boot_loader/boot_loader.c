/**
  ******************************************************************************
  * @file    boot_loader.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   跳转实现
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./boot_loader/boot_loader.h" 


pFunction JumpToApplication; 


//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_jump_app(u32 appaddr)
{ 
	if(((*(vu32*)appaddr)&0x2FF00000)==0x20000000)	                        // 检查栈顶地址是否合法.
	{
    /* Initialize user application's Stack Pointer & Jump to user application */
		JumpToApplication = (pFunction) (*(__IO uint32_t*) (appaddr + 4));		// 用户代码区第二个字为程序开始地址(复位地址)		
		__set_MSP(*(__IO uint32_t*) appaddr);				                         	// 初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		JumpToApplication();									                                // 跳转到APP.
	}
}		 














