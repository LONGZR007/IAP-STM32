#ifndef __BOOT_LOADER_H__
#define __BOOT_LOADER_H__

#include "stm32f4xx.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);

#define FLASH_APP_ADDR		0x08010000  	// 应用程序起始地址(存放在FLASH)
											                  // 保留 0X08000000~0X0800FFFF 的空间为 Bootloader 使用(共64KB)	  

void iap_jump_app(u32 appxaddr);			  // 跳转到 APP 程序执行

#endif /* __USART1_H */
