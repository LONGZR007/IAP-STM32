
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "res_mgr.h"
#include "GUI_AppDef.h"




/**********************分界线*********************/

/* 各类控件ID */
#define ID_EXIT            0x3000
#define ID_BURN           0x3001
#define ID_RESET           0x3002
#define ID_INFO           0x3003
#define ID_TITLE          0x3004
#define ID_EXIT_INFO       0x3005
#define ID_PROGBAR		      0x3006

//定义控件的私有消息(所有控件的私有消息要从 WM_WIDGET 开始定义)
#define MSG_MYWRITE_RESULT WM_WIDGET+1 //烧录结果

HWND wnd_info_textbox ;
HWND wnd_res_writer_dialog;
HWND wnd_progbar;

/* 用于标记是否有资源文件无法找到 */
extern BOOL res_not_found_flag;


/**
  * @brief  烧录应用线程
  */
static void App_FLASH_Writer(void )
{
  static int thread=0;
	static TaskHandle_t h_flash;
  
   //HDC hdc;
  u32 result;
   
	if(thread==0)
	{  
//      h_flash=rt_thread_create("Flash writer",(void(*)(void*))App_FLASH_Writer,NULL,5*1024,1,5);
      xTaskCreate((TaskFunction_t )(void(*)(void*))App_FLASH_Writer,  /* 任务入口函数 */
                            (const char*    )"App_FLASH_Writer",/* 任务名字 */
                            (uint16_t       )4*1024/4,  /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )NULL,/* 任务入口函数参数 */
                            (UBaseType_t    )5, /* 任务的优先级 */
                            (TaskHandle_t  )&h_flash);/* 任务控制块指针 */
      thread =1;
//      rt_thread_startup(h_flash);//启动线程
      return;
	}
	while(thread) //线程已创建了
	{     
    result = (u32)BurnFile();
    
    //发消息给wnd_res_writer_dialog,烧录结果
    SendMessage(wnd_res_writer_dialog,MSG_MYWRITE_RESULT,result,0);

    thread = 0;       

    GUI_Thread_Delete(h_flash);
//    rt_thread_delete

	}
  return;
}

/**
  * @brief  烧录应用回调函数
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
//	HDC hdc;
	RECT rc,rc0;
	HWND wnd;
  wnd_res_writer_dialog = hwnd;

	//static RECT rc_R, rc_G, rc_B;//RGB分量指示框
  
  const WCHAR no_res_info[] = L"FLASH中找不到某个资源文件.\r\n\
请按照以下说明进行操作:\r\n\r\n\
1.插入带有srcdata文件夹的SD卡.\r\n\
2.重新给板子上电.\r\n\
3.单击下面的按钮以加载资源.";
  
  const WCHAR normal_res_info[] = L"如果您不知道该怎么办,请按退出!!!\r\n\
此应用程序用于重新加载资源\r\n\
如果您想重新加载资源:\r\n\
1.插入带有srcdata文件夹的SD卡.\r\n\
2.重新给板子上电.\r\n\
3.单击下面的按钮以加载资源.";
  
  /* 默认显示信息 */
  const WCHAR *pStr = normal_res_info;

  PROGRESSBAR_CFG cfg;	

   //HDC hdc_mem2pic;
	switch (msg)
	{
    case WM_CREATE: {  
      
          /* 找不到资源时显示信息 */
          if(res_not_found_flag)
            pStr = no_res_info;
          
          GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.
      
          /* 标题 */
          rc0.x = 0;
          rc0.y = 0;
          rc0.w = rc.w;
          rc0.h = rc.h/5;
          
          wnd = CreateWindow(TEXTBOX,L"FLASH烧录程序" ,WS_VISIBLE,
                                rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_TITLE, NULL, NULL); 

          SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);


          /* 有退出提示 */
          if(!res_not_found_flag)
          {  
            /* 退出按钮 */
            rc0.w = 60;
            rc0.x = rc.w - rc0.w - 5*2;
            rc0.h = 40;
            CreateWindow(BUTTON,L"退出",WS_VISIBLE,rc.w-100,8,80,48,hwnd,ID_EXIT,NULL,NULL);
//            CreateWindow(BUTTON, L"退出",BS_FLAT | WS_VISIBLE,
//                          /*rc0.x, rc0.y, rc0.w, rc0.h,*/rc.w-100,8,80,48, hwnd, ID_EXIT, NULL, NULL); 
          }
          
          /* 提示信息 */
          rc0.x = 5;
          rc0.y = 1*rc.h/5;
          rc0.w = rc.w - rc0.x*2;
          rc0.h = 2*rc.h/5;
      
          CreateWindow(TEXTBOX,pStr ,WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_INFO, NULL, NULL); 

          /* 进度条 */
          rc0.x = 5;
          rc0.w = rc.w - rc0.x*2;
          rc0.h = 36;
          rc0.y = 4*rc.h/5 - rc0.h-10;
          
          //PROGRESSBAR_CFG结构体的大小
					cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
          //开启所有的功能
					cfg.fMask    = PB_CFG_ALL;
          //文字格式水平，垂直居中
					cfg.TextFlag = DT_VCENTER|DT_CENTER;  

					wnd_progbar = CreateWindow(PROGRESSBAR,L"",
                                  PBS_TEXT|PBS_ALIGN_LEFT,
                                  rc0.x, rc0.y, rc0.w, rc0.h,hwnd,ID_PROGBAR,NULL,NULL);

          SendMessage(wnd_progbar,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
			 SendMessage(wnd_progbar,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
          SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,0);

          /* 烧录按钮 */
          rc0.x = 5;
          rc0.w = rc.w/2- rc0.x*2;
          rc0.h = 45;
          rc0.y = rc.h - rc0.h;       
          CreateWindow(BUTTON, L"单击此处进行烧录资源",BS_FLAT | WS_VISIBLE,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_BURN, NULL, NULL); 

          /* 复位按钮 */
          rc0.x = rc.w/2 +5;
          rc0.w = rc.w/2 - 5*2;
          rc0.h = 45;
          rc0.y = rc.h - rc0.h;
          CreateWindow(BUTTON, L"单击此处复位系统",BS_FLAT ,
                        rc0.x, rc0.y, rc0.w, rc0.h, hwnd, ID_RESET, NULL, NULL); 
          break;
	}
    
   case MSG_MYWRITE_RESULT:
    {
      u32 result = wParam;

      /* 烧录失败 */
      if(result)
      {
        EnableWindow(GetDlgItem(hwnd,ID_BURN),ENABLE);

      }
      else
      {
        /* 烧录成功 */
        ShowWindow(GetDlgItem(hwnd,ID_RESET),SW_SHOW);
        ShowWindow(GetDlgItem(hwnd,ID_BURN),SW_HIDE);
        ShowWindow(GetDlgItem(hwnd,ID_PROGBAR),SW_HIDE);

        SetWindowText(wnd_info_textbox,L"烧录资源成功!\r\n\r\n单击下面的按钮以复位系统!");

      }  
      

      break;
    }
 
	case WM_NOTIFY: {
      u16 code,  id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型
		
      if(id == ID_EXIT && code == BN_CLICKED)
      {
         PostCloseMessage(hwnd);
      }
      
      if(id == ID_BURN && code == BN_CLICKED)
      {
//         rt_enter_critical();
         
         wnd_info_textbox = GetDlgItem(hwnd,ID_INFO);
         App_FLASH_Writer();
        
         EnableWindow(GetDlgItem(hwnd,ID_BURN),DISABLE);
         ShowWindow(GetDlgItem(hwnd,ID_EXIT),SW_HIDE);
         ShowWindow(GetDlgItem(hwnd,ID_EXIT_INFO),SW_HIDE);
         ShowWindow(GetDlgItem(hwnd,ID_PROGBAR),SW_SHOW);

//         rt_exit_critical();

      }
      
      if(id == ID_RESET && code == BN_CLICKED)
      {
         NVIC_SystemReset();
      }      
      
      
		break;
	}

   case	WM_CTLCOLOR:
   {
      /* 控件在绘制前，会发送 WM_CTLCOLOR到父窗口.
       * wParam参数指明了发送该消息的控件ID;lParam参数指向一个CTLCOLOR的结构体指针.
       * 用户可以通过这个结构体改变控件的颜色值.用户修改颜色参数后，需返回TRUE，否则，系统
       * 将忽略本次操作，继续使用默认的颜色进行绘制.
       *
       */
			u16 id;
			id =LOWORD(wParam);
			if(id== ID_INFO )
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(0,255,0);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;
			}
      else if(id == ID_TITLE)
      {
      	CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,0);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;

      }
      else if(id == ID_EXIT_INFO)
      {
      	CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,0,0);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;

      }
//      else if(id == ID_BURN || id == ID_EXIT)
//      {
//        CTLCOLOR *cr;
//				cr =(CTLCOLOR*)lParam;
//				cr->TextColor =RGB888(255,0,0);//文字颜色（RGB888颜色格式)
//				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
//				cr->BorderColor =RGB888(0,0,0);//边框颜色（RGB888颜色格式)
//				return TRUE;
//      }

			else
			{
				return FALSE;
			}
      
   }   
   case WM_ERASEBKGND:
   {
      HDC hdc =(HDC)wParam;
      RECT rc;
      GetClientRect(hwnd, &rc);
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);
      
      
      return TRUE;
      
   }

	case	WM_PAINT: //窗口需要重绘制时，会自动收到该消息.
	{	
      PAINTSTRUCT ps;
//      HDC hdc;//屏幕hdc
//      hdc = BeginPaint(hwnd, &ps); 
    BeginPaint(hwnd, &ps); 

		EndPaint(hwnd, &ps);
		return	TRUE;
	}
	default:
		return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_RES_WRITER_DIALOG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

  /* 重设默认字体为ASCII 内部flash字库，防止擦除时出错 */
  GUI_SetDefFont(defaultFontEn);  //设置默认的字体

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
	//创建主窗口
	hwnd = CreateWindowEx(WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI FLASH Writer",
                        WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(hwnd, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

  /* 恢复中文默认字体 */
  GUI_SetDefFont(defaultFont);  

}
