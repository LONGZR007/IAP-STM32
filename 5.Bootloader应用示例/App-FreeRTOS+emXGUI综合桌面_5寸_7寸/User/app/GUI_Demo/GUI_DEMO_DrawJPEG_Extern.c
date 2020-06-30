/**
  *********************************************************************
  * @file    GUI_DEMO_DrawJPEG.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   显示外部BMP图像，
  * @note    使用时需要在FLASH或SD卡存放图片文件
  **********************************************************************
  */ 
#include <emXGUI.h>
#include "emXGUI_JPEG.h"

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000
/*============================================================================*/

/*============================================================================*/


/* 为1时显示RES FLASH资源文件，为0时显示SD卡的文件 */
#define RES_PIC_DEMO    1

#if(RES_PIC_DEMO)
  /* FLASH资源文件加载 */
  #define DEMO_JPEG_FILE_NAME  "tiger.jpg"

#else
  /* SD文件系统加载 */
  #define DEMO_JPEG_FILE_NAME  "0:srcdata/tiger.jpg"

#endif
/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
    /* 外部图片尺寸大小 */
  static uint16_t pic_width,pic_height;
  static HDC hdc_mem=NULL;
  static  BOOL res;

  
	RECT rc;
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口
		{
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;

			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形
      
#if(RES_PIC_DEMO)
      /* 资源设备中加载 */
      res = RES_Load_Content(DEMO_JPEG_FILE_NAME, (char **)&jpeg_buf, &jpeg_size);
#else
      /* SD文件系统加载 */
      res = FS_Load_Content(DEMO_JPEG_FILE_NAME, (char **)&jpeg_buf, &jpeg_size);
#endif

      if(res)
      {
        /* 根据图片数据创建JPG_DEC句柄 */
        dec = JPG_Open(jpeg_buf, jpeg_size);
        /* 读取图片文件信息 */
        JPG_GetImageSize(&pic_width, &pic_height,dec);
        
        /* 创建内存对象 */
        hdc_mem =CreateMemoryDC(SURF_SCREEN,pic_width,pic_height); 
        
        /* 绘制至内存对象 */
        JPG_Draw(hdc_mem, 0, 0, dec);
              
        /* 关闭JPG_DEC句柄 */
        JPG_Close(dec);
      }
      /* 释放图片内容空间 */
      RES_Release_Content((char **)&jpeg_buf);
      
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-70,rc.h-40,68,32,hwnd,ID_OK,NULL,NULL);
			//SetTimer(hwnd,0,50,TMR_START,NULL);

		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OK && code==BN_CLICKED)
			{
				PostCloseMessage(hwnd); //产生WM_CLOSE消息关闭窗口
			}
		}
		break;
		////
		case WM_TIMER:

			InvalidateRect(hwnd,&rc,FALSE);
			break;

		case WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB(hdc,0,30,130));
			FillRect(hdc,&rc);
		}
		return TRUE;
		/////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
//			WCHAR wbuf[128];
			RECT rc0;
			int x=0,y=0;


			hdc =BeginPaint(hwnd,&ps);

			////用户的绘制内容...
			GetClientRect(hwnd,&rc0);

      /* 若正常加载了图片 */
      if(res)
      {
        for(y=0; y<rc0.h; y+=pic_height)
        {
          for(x=0; x<rc0.w; x+=pic_width)
          {
            /* 把内存对象绘制至屏幕 */
            BitBlt(hdc,x,y,pic_width,pic_height,hdc_mem,0,0,SRCCOPY); //将MEMDC输出到窗口中。

            rc.x=x;
            rc.y=y;
            rc.w=pic_width;
            rc.h=pic_height;
            DrawRect(hdc,&rc);
          }
        }
      }
			EndPaint(hwnd,&ps);
			//////////
/*
			rc.x=10;
			rc.y=10;
			rc.w=100;
			rc.h=200;
			InvalidateRect(hwnd,&rc,FALSE);
*/
		}
		break;
		////

		case WM_DESTROY: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
		{
      DeleteDC(hdc_mem);

			return PostQuitMessage(hwnd); //调用PostQuitMessage，使用主窗口结束并退出消息循环.
		}
//		break;
		////

		default:
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_DrawJPEG_Extern(void)
//static void	AppMain(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////
	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
//	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	//创建主窗口
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("DrawJPEG_Extern"),
								/*WS_MEMSURFACE|*/WS_CAPTION|WS_BORDER|WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,
								NULL,NULL,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
