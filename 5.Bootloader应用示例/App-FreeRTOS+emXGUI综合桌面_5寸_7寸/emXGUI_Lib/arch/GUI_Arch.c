
#include <string.h>
#include "GUI_Drv.h"

/*===================================================================================*/

BOOL	GUI_Arch_Init(void)
{

	return TRUE;
}

/*============================================================================*/

BOOL GUI_LowLevelInit(void);
SURFACE* GUI_DisplayInit(void);
HFONT GUI_FontInit(void);
void GUI_DesktopStartup(void);

//void	GUI_Startup(void)
//{
//	SURFACE *pSurf=NULL;
//	HFONT hFont=NULL;

//	if(!GUI_LowLevelInit()) //GUI低级初始化.
//	{
//		return;
//	}

//	if(!GUI_Arch_Init())   //GUI架构适配层初始化.
//	{
//		return;
//	}

//	if(!X_GUI_Init())	   //GUI内核初始化.
//	{
//		return;
//	}

//	pSurf =GUI_DisplayInit(); //显示设备初始化.
//	if(pSurf==NULL)
//	{
//		return;
//	}
//	GUI_SetScreenSurface(pSurf); //设置屏幕Surface对象.

//	GUI_CursorInit(pSurf,pSurf->Width>>1,pSurf->Height>>1); //初始化光标.

//	hFont =GUI_FontInit(); //初始化默认的字体.
//	if(hFont==NULL)
//	{
//		return;
//	}
//	GUI_SetDefFont(hFont);  //设置默认的字体.

//	GUI_DesktopStartup();	//启动桌面窗口(该函数不会返回).

//}


/*============================================================================*/

