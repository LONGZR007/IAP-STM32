#ifndef __GUI_LCD_PORT_H
#define	__GUI_LCD_PORT_H


#if defined(STM32F429_439xx)
#include "stm32f4xx.h"
#elif defined(STM32H743xx)
#include "stm32h7xx.h"
#endif

#include	"emXGUI.h"
#include	"gui_drv_cfg.h"


/* 屏幕旋转类型定义 */
#define	ROTATE_0	  0
#define	ROTATE_90	1
#define	ROTATE_180	2
#define	ROTATE_270	3

/*============================================================================*/


SURFACE* GUI_DisplayInit(void);
SURFACE*	GUI_CreateSurface(SURF_FORMAT Format,int Width,int Height,int LineBytes,void *bits);
void	GUI_DeleteSurface(const SURFACE *pSurf);



#endif /* __GUI_LCD_PORT_H */
