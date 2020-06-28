#ifndef __BOOT_LOADER_H__
#define __BOOT_LOADER_H__

#include "stm32f4xx.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);

typedef struct
{
	uint8_t  update_flag;    // 更新标志
	uint32_t size;       // app 大小
} app_info_t;

#define SPI_FLASH_APP_ADDR     22020096    // app 在外部的偏移地址从第 21MB 开始的
#define FLASH_APP_ADDR		     0x8004000   // 应用程序起始地址(存放在FLASH)
											                     // 保留 0X08000000~0X0800FFFF 的空间为 Bootloader 使用(共64KB)	  

void iap_jump_app(u32 appxaddr);			     // 跳转到 APP 程序执行
int sflash_to_iflash(uint32_t des_addr, uint32_t src_addr, uint32_t size);

#endif /* __USART1_H */
