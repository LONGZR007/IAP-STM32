
#include <stdio.h>
#include "emXGUI.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emxgui_png.h"
#include "gui_font_port.h"
#include "./pic_load/gui_pic_load.h"

/**********************分界线*********************/

HWND GUI_Boot_hwnd;
/* 各类控件ID */
enum eID
{
  ID_LOGO,
  ID_TEXT1,
  ID_TEXT2,
  ID_TEXT3,
  ID_TEXT4,
  ID_PROGBAR,
};
/* 外部图片数据 */
extern char bootlogo[];
/* 外部图片数据大小 */
extern unsigned int bootlogo_size(void);
HWND Boot_progbar = NULL;

/**
  * @brief  加载资源线程
  */
static void App_Load_Res(void )
{
  static int thread=0;

  if(thread==0)
  { 
    /* 创建线程运行自己 */
    GUI_Thread_Create((void(*)(void*))App_Load_Res,  /* 任务入口函数 */
                        "Load Res",/* 任务名字 */
                        5*1024,  /* 任务栈大小 */
                        NULL, /* 任务入口函数参数 */
                        1,    /* 任务的优先级 */
                        10); /* 任务时间片，部分任务不支持 */
    thread =1;
    return;
  }
  while(thread) //线程已创建了
  { 
    HFONT hFont;

    /* 加载字体到外部SDRAM，返回defaultFont */    
    hFont = GUI_Init_Extern_Font();
    if(hFont==NULL)
    {
      GUI_ERROR("GUI Extern Default Font Init Failed.");

      Load_state = FALSE;
    }
    else
    {   
      Load_state = TRUE;
      /* 重设默认字体 */
      GUI_SetDefFont(hFont);  
    }  
    PIC_Load_To_SDRAM();
    
    //发消息给启动窗口，关闭
    SendMessage(GUI_Boot_hwnd,WM_CLOSE,0,0);
    thread = 0;  

    /* 删除线程自己 */
    GUI_Thread_Delete(GUI_GetCurThreadHandle());
  }
  return;
}

/**
  * @brief  进度条重绘
  */
static void progbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc, hdc_mem;
	RECT rc,m_rc[2];
//	int range,val;
	WCHAR wbuf[128];
	PROGRESSBAR_CFG cfg;
	hwnd =ds->hwnd;
	hdc =ds->hDC;
   /*************第一步***************/
   //获取客户区矩形位置，大小
   GetClientRect(hwnd,&rc);

  hdc_mem = CreateMemoryDC(SURF_SCREEN, rc.w, rc.h);
  SetBrushColor(hdc_mem,MapRGB(hdc,0,0,0));
	FillRect(hdc_mem,&ds->rc); 

   //设置进度条的背景颜色
	SetBrushColor(hdc,MapRGB(hdc,250,250,250));
   //填充进度条的背景
  EnableAntiAlias(hdc, TRUE);
	FillRoundRect(hdc,&ds->rc, MIN(rc.w,rc.h)/2);   
//   //设置画笔颜色
	SetPenColor(hdc,MapRGB(hdc,100,10,10));
//   //绘制进度条的背景边框
//   DrawRect(hdc,&rc);
   /*************第二步***************/	
   cfg.cbSize =sizeof(cfg);
	cfg.fMask =PB_CFG_ALL;
	SendMessage(hwnd,PBM_GET_CFG,0,(LPARAM)&cfg);
   //生成进度条矩形
	MakeProgressRect(m_rc,&rc,cfg.Rangle,cfg.Value,PB_ORG_LEFT);
   //设置进度条的颜色
	SetBrushColor(hdc_mem,MapRGB(hdc,210,10,10));
  EnableAntiAlias(hdc, FALSE);
   //填充进度条
  // InflateRect(&m_rc[0],-1,-1);
  EnableAntiAlias(hdc_mem, TRUE);
	FillRoundRect(hdc_mem, &rc, rc.h/2);
  EnableAntiAlias(hdc_mem, FALSE);
  BitBlt(hdc, m_rc[0].x, m_rc[0].y, m_rc[0].w, m_rc[0].h, hdc_mem, 0, 0, SRCCOPY);
  
  
   //绘制进度条的边框，采用圆角边框
	//DrawRoundRect(hdc,&m_rc[0],MIN(rc.w,rc.h)/2);
   /************显示进度值****************/
	DeleteDC(hdc_mem);
	//InflateRect(&rc,40,0);
	//DrawText(hdc,L"加载中...",-1,&rc,DT_CENTER);
}

/**
  * @brief  启动界面回调函数
  */
static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static  BITMAP png_bm;
  static  PNG_DEC *png_dec; 
  PROGRESSBAR_CFG cfg;
  switch (msg)
  {  
    case WM_CREATE:
    {
      RECT rc,rc0;

      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形
      CopyRect(&rc0,&rc);
      
      /* 本窗口垂直分为2份 */

      /* 根据图片数据创建PNG_DEC句柄 */
      png_dec = PNG_Open((u8 *)bootlogo);
      /* 把图片转换成bitmap */
      PNG_GetBitmap(png_dec, &png_bm);
      
      rc0.x = (rc.w - png_bm.Width)/2;
      rc0.y = rc.h/2 - png_bm.Height - 10;
      rc0.w = rc.w;
//      DrawBitmap(hdc, 250, 80, &png_bm, NULL); 
      
      OffsetRect(&rc0,0,png_bm.Height);
      rc0.x = 0;
      rc0.y = rc.h/2;
      rc0.h = 30;      
      rc0.w = rc.w;

      CreateWindow(TEXTBOX, L"system booting", WS_VISIBLE, 
                    rc0.x,rc0.y,rc0.w,rc0.h,
                    hwnd, ID_TEXT1, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXT1),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 

      OffsetRect(&rc0,0,rc0.h);

      CreateWindow(TEXTBOX, L"Copying FontLIB from SPIFLASH to SDRAM", WS_VISIBLE, 
                    rc0.x,rc0.y,rc0.w,rc0.h,
                    hwnd, ID_TEXT2, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXT2),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND); 

      OffsetRect(&rc0,0,rc0.h+10);

      rc0.x = 100;
      rc0.h = 30;
      rc0.w = rc.w - 200;

      //PROGRESSBAR_CFG结构体的大小
      cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
      //开启所有的功能
      cfg.fMask    = PB_CFG_ALL;
      //文字格式水平，垂直居中
      cfg.TextFlag = DT_VCENTER|DT_CENTER;  

      Boot_progbar = CreateWindow(PROGRESSBAR,L"Loading",
                                     PBS_TEXT|PBS_ALIGN_LEFT|WS_VISIBLE|WS_OWNERDRAW|WS_TRANSPARENT,
                                    rc0.x,rc0.y,rc0.w,rc0.h,hwnd,ID_PROGBAR,NULL,NULL);

      SendMessage(Boot_progbar,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
      SendMessage(Boot_progbar,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
      SendMessage(Boot_progbar,PBM_SET_RANGLE,TRUE, FONT_NUM);
      SendMessage(Boot_progbar,PBM_SET_VALUE,TRUE,0); 
      SetTimer(hwnd, 1, 20, TMR_SINGLE|TMR_START, NULL);
      
      rc0.x = 305;
      rc0.y = 410;
      rc0.w = 200;
      rc0.h = 30;
      
      CreateWindow(TEXTBOX, L"powered by", WS_VISIBLE, 
                    rc0.x,rc0.y,rc0.w,rc0.h,
                    hwnd, ID_TEXT3, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXT3),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND); 
      rc0.y = 441;          
      CreateWindow(TEXTBOX, L"emXGUI+FreeRTOS", WS_VISIBLE, 
                    rc0.x,rc0.y,rc0.w,rc0.h,
                    hwnd, ID_TEXT4, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, ID_TEXT4),TBM_SET_TEXTFLAG,0,
                    DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND); 
      
      break;
    }
    
    case WM_TIMER:
    {
      /* 启动界面创建后timer时间后才开始加载 */
      App_Load_Res();
      break;         
    }
    
    case WM_PAINT:
    {
      PAINTSTRUCT ps;

      BeginPaint(hwnd, &ps);

      EndPaint(hwnd, &ps);

      break;
    }
    
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;

      GetClientRect(hwnd,&rc); //获得窗口的客户区矩形

      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);    
      
      rc.x = (rc.w - png_bm.Width)/2;
      rc.y = rc.h/2 - png_bm.Height - 40;;
      /* 显示图片 */
      DrawBitmap(hdc, rc.x, rc.y, &png_bm, NULL);  
//      TextOut(hdc, 337, 410, L"powered by",128);
//      TextOut(hdc, 337, 441, L"emXGUI",128);
     
      return TRUE;

    }


    case	WM_CTLCOLOR:
    {
      u16 id;
      CTLCOLOR *cr;
      id =LOWORD(wParam);				
      cr =(CTLCOLOR*)lParam;
      if(id >= ID_TEXT1 || id <= ID_TEXT4)
      {
        cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
        cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
        //cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
        return TRUE;
      }

      break;
    }
    
    case	WM_DRAWITEM:
    {
      /*　当控件指定了WS_OWNERDRAW风格，则每次在绘制前都会给父窗口发送WM_DRAWITEM消息。
       *  wParam参数指明了发送该消息的控件ID;lParam参数指向一个DRAWITEM_HDR的结构体指针，
       *  该指针成员包含了一些控件绘制相关的参数.
       */

      DRAWITEM_HDR *ds;

      ds = (DRAWITEM_HDR*)lParam;

      if(ds->ID == ID_PROGBAR)
      {
        progbar_owner_draw(ds); //执行自绘制按钮
      }

       /* 返回TRUE表明使用重绘操作 */
      return TRUE;
    }
    
    case WM_CLOSE: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
    {
      /* 关闭PNG_DEC句柄 */
      PNG_Close(png_dec);
      DestroyWindow(hwnd); 
      Boot_progbar = NULL;
      return TRUE; //调用PostQuitMessage，使用主窗口结束并退出消息循环.
    }      
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return	WM_NULL;                                     
}

extern void	GUI_RES_Writer_Dialog(void);
extern void	GUI_DEMO_SlideWindow(void *P);
void	GUI_Board_App_Desktop(void *p);
extern void PhoneCallMonitorTask(void *p);
extern TaskHandle_t* CallCallMonitorHandle;    // 来电监测任务控制块

void	GUI_Boot_Interface_Dialog(void *param)
{

  WNDCLASS	wcex;
  MSG msg;
  
  /* 设默认字体为ASCII 内部flash字库，防止出错 */
  GUI_SetDefFont(defaultFontEn);  

  wcex.Tag 		    = WNDCLASS_TAG;

  wcex.Style			= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= win_proc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= NULL;//hInst;
  wcex.hIcon			= NULL;
  wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);

  //创建启动提示
  GUI_Boot_hwnd = CreateWindowEx(	WS_EX_LOCKPOS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"Booting",
                              WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED|WS_DISABLED,
                              0,0,GUI_XSIZE,GUI_YSIZE,
                              NULL,0,NULL,NULL);


  //显示窗口.
  ShowWindow(GUI_Boot_hwnd,SW_SHOW);

  while(GetMessage(&msg,GUI_Boot_hwnd))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  /* 启动界面在加载完资源后会关闭，执行以下代码，创建应用线程 */
//  {  
#if (GUI_APP_RES_WRITER_EN )  
    /* 人为设置为TRUE，测试用 */
    //res_not_found_flag = TRUE; 
  
     if(res_not_found_flag)
     {
        GUI_INFO("外部SPI FLASH缺少资源，即将开始烧录资源内容...");

        /* 若找不到资源，进入资源烧录应用 */      
        GUI_Thread_Create((void (*) (void *))GUI_RES_Writer_Dialog,  /* 任务入口函数 */
                              "GUI_FLASH_WRITER",/* 任务名字 */
                              8*1024,  /* 任务栈大小 */
                              NULL, /* 任务入口函数参数 */
                              5,    /* 任务的优先级 */
                              10); /* 任务时间片，部分任务不支持 */


     }
#endif     
     else
     {	
        /* 找到资源，正常跑应用*/ 
     
    
        GUI_Thread_Create(GUI_Board_App_Desktop,     /* 任务入口函数 */
                              "GUI_Board_App_Desktop",/* 任务名字 */
                              10*1024,               /* 任务栈大小 */
                              NULL,                  /* 任务入口函数参数 */
                              8,                     /* 任务的优先级 */
                              10);                   /* 任务时间片，部分任务不支持 */
       
       GUI_Thread_Create(GUI_DEMO_SlideWindow,       /* 任务入口函数 */
                              "GUI_SLIDE_WIN",       /* 任务名字 */
                              2*1024,                /* 任务栈大小 */
                              NULL,                  /* 任务入口函数参数 */
                              7,                     /* 任务的优先级 */
                              10);                   /* 任务时间片，部分任务不支持 */
       
       xTaskCreate(PhoneCallMonitorTask,       /* 任务入口函数 */
                              "Phone_Call_Monitor",  /* 任务名字 */
                              3*1024/4,                /* 任务栈大小 */
                              NULL,                  /* 任务入口函数参数 */
                              6,                     /* 任务的优先级 */
                              CallCallMonitorHandle);                   /* 任务时间片，部分任务不支持 */
       
     }
//  } 

    /* 部分操作系统在退出任务函数时，必须删除线程自己 */
    GUI_Thread_Delete(GUI_GetCurThreadHandle());

}
