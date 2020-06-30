
#ifndef	__EMXGUI_PNG_H__
#define	__EMXGUI_PNG_H__

#ifdef	__cplusplus
extern	"C"{
#endif

#include "emXGUI.h"

/*===================================================================================*/

typedef struct __PNG_DEC PNG_DEC;
  
////PNG解码成位图数据(BITMAP).
PNG_DEC*    PNG_DecodeOpen(const char *png_dat);
BOOL		PNG_DecodeGetBitmap(PNG_DEC *dec,BITMAP *bm);
void		PNG_DecodeClose(PNG_DEC *dec);

////获得PNG位图信息.
BOOL	PNG_GetInfo(BITMAPINFO *info,const char *png_dat);

////PNG直接显示,当PNG为Gray/Alpha格式时,ExColor指定颜色值.
BOOL	PNG_DrawEx(HDC hdc,int x,int y,const char *png_dat,COLOR_RGB32 ExColor);


#define	PNG_Open		PNG_DecodeOpen
#define	PNG_GetBitmap	PNG_DecodeGetBitmap
#define	PNG_Close		PNG_DecodeClose

#define	PNG_Draw(hdc,x,y,png_dat)	PNG_DrawEx(hdc,x,y,png_dat,XRGB8888(0,0,0))
/*===================================================================================*/

#ifdef	__cplusplus
}
#endif
#endif
