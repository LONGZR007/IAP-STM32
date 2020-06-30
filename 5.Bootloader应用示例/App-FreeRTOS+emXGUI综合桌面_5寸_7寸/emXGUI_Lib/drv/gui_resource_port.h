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
#include	"emXGUI.h"
#include	"gui_drv_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif  
  
/* 目录信息类型 */
typedef struct 
{
	char 	name[40];  /* 资源的名字 */
	u32  	size;      /* 资源的大小 */ 
	u32 	offset;    /* 资源相对于基地址的偏移 */
}CatalogTypeDef;


/* 用于标记是否有资源文件无法找到 */
extern BOOL res_not_found_flag;

BOOL RES_DevInit(void);
U32 RES_DevGetID(void);
BOOL RES_DevWrite(u8 *buf,u32 addr,u32 size);
BOOL RES_DevRead(u8 *buf,u32 addr,u32 size);
int RES_DevEraseSector(u32 addr);
void RES_DevTest(void);

s32 RES_GetOffset(const char *res_name);
s32 RES_GetInfo_AbsAddr(const char *res_name, CatalogTypeDef *dir);

BOOL FS_Load_Content(char *file_name, char** buf, u32* size); 
BOOL RES_Load_Content(char *file_name, char** buf,u32* size);
BOOL PIC_JPEG_Draw_FS(HDC hdc, int x, int y, char *file_name);
 /**
  * @brief  释放动态申请的buf空间
  * @retval 无
  */
static __inline void RES_Release_Content(char **buf)
{
    /* 释放空间 */
    GUI_VMEM_Free(*buf);
}

#ifdef	__cplusplus
}
#endif
#endif /* __GUI_RESOURCE_PORT_H */
