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


#include "GUI_AppDef.h"
#include "emXGUI.h"
#include "./pic_load/gui_pic_load.h"

/* 外部资源加载完成标志 */
BOOL Load_state = FALSE;
extern uint8_t Theme_Flag;   // 主题标志
/* 录音文件存放路径 */
#define SCREENSHOTDIR	"0:/screenshot"

/*===================================================================================*/
extern void	GUI_Boot_Interface_Dialog(void *param);
extern void GUI_AppMain(void);


void	gui_app_thread(void *p)
{
    #if(GUI_TOUCHSCREEN_EN & GUI_TOUCHSCREEN_CALIBRATE)
    {
        int i=0;
        while(TouchPanel_IsPenDown())
    	{
    		GUI_msleep(100);
    		if(i++>10)
    		{
    			ShowCursor(FALSE);
    			TouchScreenCalibrate(NULL);
    			ShowCursor(TRUE);
    			break;
    		}
    	}
    }
    #endif
		
  /* 调用APP函数 */  
//	GUI_AppMain();
 //   GUI_UserAppStart();
//   	ShellWindowStartup();
    while(1)
    {
      int i = 0;
      DIR dir;
      FRESULT result; 
      FIL *file;
      BOOL res;
      char recfilename[50];
      
      if (Key_Scan (KEY1_GPIO_PORT, KEY1_PIN) == KEY_ON) 
      {
        /* 文件句柄空间 */
        file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));
        
        /*  如果路径不存在，创建文件夹  */
        result = f_opendir(&dir,SCREENSHOTDIR);
        while(result != FR_OK)
        {
          i++;
          if (i > 5)
          {
            GUI_DEBUG("打开文件失败，请检查SD卡！");
            break;
          }
          f_mkdir(SCREENSHOTDIR);
          result = f_opendir(&dir,SCREENSHOTDIR);
        }
        
        /* 寻找合适文件名 */
        for(i=1;i<0xff;++i)
        {
          sprintf(recfilename,"0:/screenshot/screenshot%03d.bmp",i);
          result=f_open(file,(const TCHAR *)recfilename,FA_READ);
          if(result==FR_NO_FILE)break;					
        }
        f_close(file);
        
        if(i==0xff)
        {
          GUI_VMEM_Free(file);
          GUI_DEBUG("没有找到可用文件名");
          continue;
        }
        /* 寻找合适文件名 */
        for(i=1;i<0xff;++i)
        {
          sprintf(recfilename,"0:/screenshot/screenshot%03d.bmp",i);
          result=f_open(file,(const TCHAR *)recfilename,FA_READ);
          if(result==FR_NO_FILE)break;					
        }
        f_close(file);
        GUI_VMEM_Free(file);
        
        if (PIC_Capture_Screen_To_BMP(recfilename) == TRUE)
        {
          GUI_DEBUG("截图成功, %s", recfilename);
        }
        else
        {
          GUI_DEBUG("截图失败");
        }
      }
//      GUI_DEBUG("gui_app_thread");
      GUI_msleep(40);
    }
}

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
  if (Theme_Flag == 1)
  {
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
    rc.x = 370;
    rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT+15;
    rc.h = HEAD_INFO_HEIGHT;
    rc.w = 80;    
    /* 控制图标字体 */
    SetFont(hdc, controlFont_72);

    /* 向上图标 */
    SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
  //  DrawText(hdc,L"D",-1,&rc,DT_TOP|DT_CENTER);
    DrawText(hdc,L"f",-1,&rc,DT_TOP);

  // /* 恢复默认字体 */
    SetFont(hdc, defaultFont);
    OffsetRect(&rc,20,-5);
    DrawText(hdc,L"说明",-1,&rc,DT_LEFT|DT_VCENTER);
    rc.x = 360;
    rc.w = 100;
    rc.h = 40;
    rc.y = 480-45-10;
    SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
    DrawRoundRect(hdc, &rc, MIN(rc.w, rc.h)>>1);
  //  rc.y -= 20;
  //  DrawText(hdc,L"\r\n\r\n详细",-1,&rc,DT_BOTTOM|DT_CENTER);
    GetClientRect(hwnd,&rc);
    rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
    rc.h = HEAD_INFO_HEIGHT;

    DrawText(hdc,L"www.firebbs.cn  ",-1,&rc,DT_RIGHT|DT_VCENTER); 
  }  
  else
  {
    /* 使用图片 */
    rc.x = 0;
    rc.y = GUI_YSIZE - HEAD_INFO_HEIGHT;
    rc.w = GUI_XSIZE;
    rc.h = HEAD_INFO_HEIGHT;
    
    BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_home_bk, rc.x, rc.y, SRCCOPY);

  }
}
extern GUI_SEM *Input_Sem;
/* 使用专用的线程来处理输入 */
#if 0
static	void	gui_input_thread(void *p)
{
	while(1)
	{
    GUI_SemWait(Input_Sem, 0xFFFFFFFF);
		GUI_InputHandler(); //处理输入设备
		GUI_msleep(20);
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
			   //创建1个20ms定时器，处理循环事件.
				 SetTimer(hwnd,1,20,TMR_START,NULL);

				//创建App线程						
				{
#if (GUI_APP_BOOT_INTERFACE_EN )  
          
         GUI_Thread_Create(GUI_Boot_Interface_Dialog,  /* 任务入口函数 */
                              "Boot_Interface",/* 任务名字 */
                              2*1024,  /* 任务栈大小 */
                              NULL, /* 任务入口函数参数 */
                              5,    /* 任务的优先级 */
                              10); /* 任务时间片，部分任务不支持 */
//          GUI_Thread_Create(gui_input_thread,  /* 任务入口函数 */
//                              "gui_input_thread",/* 任务名字 */
//                              1*1024,  /* 任务栈大小 */
//                              NULL, /* 任务入口函数参数 */
//                              11,    /* 任务的优先级 */
//                              10); /* 任务时间片，部分任务不支持 */

          
//        GUI_Thread_Create(gui_app_thread,  /* 任务入口函数 */
//                            "GUI_APP",/* 任务名字 */
//                            1*1024,  /* 任务栈大小 */
//                            NULL, /* 任务入口函数参数 */
//                            5,    /* 任务的优先级 */
//                            10); /* 任务时间片，部分任务不支持 */    
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
      RECT rc = *(RECT*)lParam;
			HDC hdc =(HDC)wParam; 
      
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

//  GUI_DEBUG("Create desktop");
	//创建桌面窗口.
	hwnd = GUI_CreateDesktop(	WS_EX_LOCKPOS,
                              &wcex,
                              L"DESKTOP",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);

//	GUI_DEBUG("HWND_Desktop=%08XH\r\n",	hwnd);

	//显示桌面窗口.
	ShowWindow(hwnd,SW_SHOW);

#if (GUI_SHOW_CURSOR_EN)
	//设置系统打开光标显示(可以按实际情况看是否需要).
	ShowCursor(TRUE);
#endif

	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/********************************END OF FILE****************************/

