#ifndef __DMA2D_EMXGUI_H__
#define __DMA2D_EMXGUI_H__

#ifdef	__cplusplus
extern "C"{
#endif

#include "def.h"

#include "stm32f4xx_dma2d.h"

#include "emXGUI.h"

/*=========================================================================================*/

void DMA2D_DrvInit(void);
BOOL DMA2D_DrawBitmap_RGB565(const SURFACE *pSurf,int x,int y,U16 w,U16 h,int width_bytes,const U8 *bits);
BOOL DMA2D_DrawBitmap_ARGB(const SURFACE *pSurf,int x,int y,U16 w,U16 h,int width_bytes,const U8 *bits,U32 color_format);
BOOL DMA2D_FillRect(const SURFACE *pSurf,int x,int y,U16 w,U16 h,U32 color);

/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif /*__DMA2D_EMXGUI_H__*/














