#ifndef __GUI_RESOURCE_PORT_H
#define	__GUI_RESOURCE_PORT_H

#if defined(STM32F429_439xx)
#include "stm32f4xx.h"
#elif defined(STM32H743xx)
#include "stm32h7xx.h"
  #if GUI_RES_DEV_EN
    #include "./flash/bsp_qspi_flash.h"
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif  

typedef int         BOOL;

/* 资源所在的基地址 */
#define GUI_RES_BASE             4096

/* 存储在FLASH中的资源目录大小 */
#define GUI_CATALOG_SIZE         (8*1024)
  
/* 目录信息类型 */
typedef struct 
{
	char 	name[40];  /* 资源的名字 */
	u32  	size;      /* 资源的大小 */ 
	u32 	offset;    /* 资源相对于基地址的偏移 */
}CatalogTypeDef;


/* 用于标记是否有资源文件无法找到 */
extern BOOL res_not_found_flag;

BOOL RES_DevRead(u8 *buf,u32 addr,u32 size);
int RES_DevEraseSector(u32 addr);
void RES_DevTest(void);

s32 RES_GetOffset(const char *res_name);
s32 RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir);

#ifdef	__cplusplus
}
#endif
#endif /* __GUI_RESOURCE_PORT_H */
