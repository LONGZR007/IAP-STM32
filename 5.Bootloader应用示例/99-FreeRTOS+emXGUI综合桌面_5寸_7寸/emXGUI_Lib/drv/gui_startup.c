
#include <string.h>
#include "gui_drv.h"

/*============================================================================*/
extern BOOL	GL_CursorInit(const SURFACE *pSurf,int x,int y);
extern BOOL	GUI_Arch_Init(void);

extern void GUI_DesktopStartup(void);
static BOOL GUI_LowLevelInit(void);
/***********************第1部分*************************/
/**
  * @brief  GUI低级别的初始化,这是GUI的第一个初始化函数
  * @param  无
  * @retval TRUE:成功; FALSE：失败.
  */
static BOOL GUI_LowLevelInit(void)
{

  //GUI Core内存管理初始化
  GUI_MEM_Init();

#if(GUI_VMEM_EN)    
   //vmem内存管理初始化
	GUI_VMEM_Init();
#endif  
  
  //初始化日志接口
  if(GUI_Log_Init() != TRUE)
  {
  
  }
    
  /* 直接返回TRUE，为了部分设备初始化不正常不影响后续运行 */
	return TRUE;
}
/***********************第2部分*************************/
/**
  * @brief  启动GUI，包含各种内存、液晶、输入设备的初始化
  * @param  无
  * @retval 无
  */
void	GUI_Startup(void)
{
	SURFACE *pSurf=NULL;
	HFONT hFont=NULL;
  
/***********************第3部分*************************/
	if(!GUI_LowLevelInit()) //GUI低级初始化.
	{
    GUI_ERROR("GUI_LowLevelInit Failed.");
		return;
	}
  
/***********************第4部分*************************/
	if(!GUI_Arch_Init())   //GUI架构适配层初始化.
	{
    GUI_ERROR("GUI_Arch_Init Failed.");
		return;
	}

	if(!X_GUI_Init())	   //GUI内核初始化.
	{
    GUI_ERROR("X_GUI_Init Failed.");
		return;
	}
  
/***********************第5部分*************************/
  /* 初始化液晶屏 */
	pSurf =GUI_DisplayInit(); 
	if(pSurf==NULL)
	{
    GUI_ERROR("GUI_DisplayInit Failed.");
		return;
	}
	GUI_SetScreenSurface(pSurf); //设置屏幕Surface对象
  
/***********************第6部分*************************/
  #if(GUI_RES_DEV_EN)  
   //资源设备初始化（FLASH）
  if(RES_DevInit() != TRUE)
  {
    GUI_ERROR("RES_DevInit Failed.");
  }
#endif  
  
/***********************第7部分*************************/
  /* 先初始化触摸，以确认液晶屏的类型 */ 
#if(GUI_INPUT_DEV_EN)    
  //初始化输入设备
  if(GUI_InputInit() != TRUE)
  {
    GUI_ERROR("GUI_InputInit Failed.");
  }  
#endif
  
#if (GUI_SHOW_CURSOR_EN)
	GL_CursorInit(pSurf,pSurf->Width>>1,pSurf->Height>>1); //初始化光标
#endif  
  
/***********************第8部分*************************/
  hFont =GUI_Default_FontInit(); //初始化默认的字体
	if(hFont==NULL)
	{
    GUI_ERROR("GUI_Default_FontInit Failed.");
		return;
	}
	GUI_SetDefFont(hFont);  //设置默认的字体

#if(GUI_FS_EN)  
   //文件系统初始化
  if(FileSystem_Init() != TRUE)
  {
    GUI_ERROR("File_System Failed.");
  }
#endif 
  
/***********************第9部分*************************/
   GUI_DesktopStartup();   //启动桌面窗口(该函数不会返回).
}

/********************************END OF FILE****************************/

