/**
  *********************************************************************
  * @file    desktop.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI 桌面
  *********************************************************************
  * 实验平台:野火 F429-挑战者 STM32 开发板
  * 官网    :www.embedfire.com
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */ 


#include	"rtthread.h"
#include "board.h"
#include "emXGUI.h"
#include "GUI_AppDef.h"


/*===================================================================================*/
extern void 	GUI_Boot_Interface_DIALOG(void);
extern BOOL Load_state;



/*===================================================================================*/

/**
  * @brief  桌面背景绘制函数，触发背景绘制时会调用本函数，
            通过修改本函数的内容可更改桌面的背景
  * @param  hdc 绘图上下文
  * @param  lprc 要绘制的矩形区域，为NULL时会自动绘制hwnd的客户区
  * @param  hwnd 窗口对象句柄
  * @retval 无
  */
static	void	_EraseBackgnd(HDC hdc,const RECT *lprc,HWND hwnd)
{
	RECT rc;

	if(lprc==NULL)
	{
		GetClientRect(hwnd,&rc);
	}
	else
	{
		CopyRect(&rc,lprc);
	}

	SetBrushColor(hdc,MapRGB(hdc,COLOR_DESKTOP_BACK_GROUND));
	FillRect(hdc,&rc);
  	
  SetTextColor(hdc,MapRGB(hdc,255,255,255));
  
//  SetFont(hdc, iconFont_100);
//	DrawText(hdc,L" A B C D E \r\n F G H I J",-1,&rc,DT_LEFT|DT_VCENTER);
  SetFont(hdc, GB2312_32_Font);
  
	SetTextColor(hdc,MapRGB(hdc,255,255,255));
  rc.y +=20;
  DrawText(hdc,L"emXGUI@Embedfire STM32F429 ",-1,&rc,DT_CENTER);
    
  /* 背景 */
  GetClientRect(hwnd,&rc);
  SetBrushColor(hdc,MapRGB(hdc,82,85,82));
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
  rc.h = HEAD_INFO_HEIGHT;
  FillRect(hdc,&rc);
  
    /* 首栏 */ 
  SetFont(hdc, logoFont);
  /* 显示logo */
  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT-10;
  rc.h = HEAD_INFO_HEIGHT;
  
  SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
  DrawText(hdc,L" B",-1,&rc,DT_LEFT|DT_VCENTER);
  
  
  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
  rc.h = HEAD_INFO_HEIGHT;

  /* 恢复默认字体 */
  SetFont(hdc, defaultFont);
  rc.x +=50;
  DrawText(hdc,L" 野火@emXGUI",-1,&rc,DT_LEFT|DT_VCENTER);

  GetClientRect(hwnd,&rc);
  rc.x = 350;
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT-25;
  rc.h = HEAD_INFO_HEIGHT;
  rc.w = 80;    
  /* 控制图标字体 */
  SetFont(hdc, controlFont_100);

  /* 向上图标 */
  SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
//  DrawText(hdc,L"D",-1,&rc,DT_TOP|DT_CENTER);
  DrawText(hdc,L"f",-1,&rc,DT_TOP);

// /* 恢复默认字体 */
  SetFont(hdc, defaultFont);
  OffsetRect(&rc,20,-5);
//  DrawText(hdc,L"广告",-1,&rc,DT_LEFT|DT_VCENTER);
//  rc.x = 360;
//  rc.w = 100;
//  rc.h = 40;
//  rc.y = 480-45-10;
//  SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
//  DrawRoundRect(hdc, &rc, MIN(rc.w, rc.h)>>1);
//  rc.y -= 20;
//  DrawText(hdc,L"\r\n\r\n详细",-1,&rc,DT_BOTTOM|DT_CENTER);
  GetClientRect(hwnd,&rc);
  rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
  rc.h = HEAD_INFO_HEIGHT;

  DrawText(hdc,L"www.embedFire.com  ",-1,&rc,DT_RIGHT|DT_VCENTER);  


}

#if 0
/* 使用独立的线程来处理触摸 */
static	int	gui_input_thread(void *p)
{
	SYS_thread_set_priority(NULL,+4);
	while(1)
	{
		GUI_InputHandler(); //处理输入设备
		GUI_msleep(20);
	}
}
#endif


#if(GUI_PIC_CAPTURE_SCREEN_EN)
/**
  * @brief  截图线程
  * @param  无
  * @retval 无
  */
static void capture_screen_thread(void *argv)
{  
  while (1)
  {
    if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON )/* K1 被按下 */
    {
      /*LED1反转*/
      LED1_TOGGLE;
      GUI_INFO("Capture screen start..");
      
      if (PIC_Capture_Screen_To_BMP("0:screenShot.bmp"))          
        GUI_INFO("Capture screen success.");
      else
        GUI_ERROR("Capture screen failed");

      LED1_TOGGLE;
    }     
    GUI_msleep(5);
  }
}
#endif

/**
  * @brief  桌面回调函数
  * @param  hwnd 当前处理该消息的窗口对象句柄
  * @param  msg 消息类型值，用以标识和区分当前产生的消息
  * @param  wParam 消息参数值，根据msg消息代码值不同
  * @param  lParam 消息参数值，根据msg消息代码值不同
  * @retval 返回给SendMessage的值
  */

static 	 LRESULT  	desktop_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
    /* 桌面创建时,会产生该消息,可以在这里做一些初始化工作. */
		case	WM_CREATE:	

			   ////创建1个20ms定时器，处理循环事件.
				 SetTimer(hwnd,1,20,TMR_START,NULL);

				//创建App线程						
				if(1)
				{
            /* 启动界面 */
            rt_thread_t h;          
            h=rt_thread_create("Boot_Interface",GUI_Boot_Interface_DIALOG,NULL,8*1024,5,5);
            rt_thread_startup(h);         
        
#if(GUI_PIC_CAPTURE_SCREEN_EN)
						h=rt_thread_create("CAPTURE_SCREEN_APP",capture_screen_thread,NULL,2048,2,5);
						rt_thread_startup(h);		
#endif              
				}

				break;

		/* 定时处理输入设备的信息 */
		case	WM_TIMER:
      #if(GUI_INPUT_DEV_EN)
        {
          u16 id;
          
          id =LOWORD(wParam);
          if(id==1)
          {
            GUI_InputHandler(); //处理输入设备
          }
        }
      #endif
		break;        
     
    /* 检测是否触摸到“详细”一栏 */    
      case WM_LBUTTONDOWN:
		{

			POINT pt;
			RECT rc;

			pt.x =GET_LPARAM_X(lParam);
			pt.y =GET_LPARAM_Y(lParam);

			GetClientRect(hwnd,&rc);
      
      rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
      rc.h = HEAD_INFO_HEIGHT;          

      /* 若触摸到，则发送消息到slide window */
			if(PtInRect(&rc,&pt))
			{
        PostMessage(GetDlgItem(hwnd,ID_SLIDE_WINDOW), WM_MSG_FRAME_DOWN,0,0);
			}
		}
		break;

    /* 客户区背景需要被擦除 */
		case	WM_ERASEBKGND:
		{
        HDC hdc =(HDC)wParam;
         RECT rc = *(RECT*)lParam;
          /* 字体资源加载完成后才显示正常界面，刚开始时只显示纯色 */
         if(Load_state == TRUE)
         {
            _EraseBackgnd(hdc,NULL,hwnd);
         }
         else
         {
            SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
            FillRect(hdc, &rc);
         }
		}
		return TRUE;

    /* 用户不关心的信息，由系统处理 */
		default:
				return	DefDesktopProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;

}

/**
  * @brief  桌面启动函数
  * @param  无
  * @retval 无
  */
void GUI_DesktopStartup(void)
{
	WNDCLASS	wcex;
	HWND hwnd;
	MSG msg;

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= desktop_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建桌面窗口.
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
                              &wcex,
                              L"DESKTOP",
                              WS_CLIPCHILDREN,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);

	GUI_Printf("HWND_Desktop=%08XH\r\n",	hwnd);
 
   
	//显示桌面窗口.
	ShowWindow(hwnd,SW_SHOW);

	//设置系统打开光标显示(可以按实际情况看是否需要).
//	ShowCursor(TRUE);

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/********************************END OF FILE****************************/

