
/*
 *GUI_DEMO_ScaleBitmap.c
 *2018年6月30日下午3:58:32
 *
 */

#include <emXGUI.h>
#include "x_libc.h"
/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000


/*============================================================================*/

static HDC hdc_mem=NULL;
static int time=0;
static int x,y,w,h;
/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			time=0;
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			hdc_mem =CreateMemoryDC(SURF_XRGB8888,128,80);

			x=10;
			y=50;
			w=200;
			h=100;
			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
			SetTimer(hwnd,1,1000,TMR_START,NULL);
		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////
		case WM_TIMER:
		{
			time++;
			InvalidateRect(hwnd,NULL,FALSE);
		}
		break;

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			WCHAR wbuf[48];
			BITMAP bm;

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);
			ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,210,210,210));
			rc.x=0;
			rc.y=0;
			rc.w=128;
			rc.h=80;

			SetPenColor(hdc_mem,MapRGB(hdc_mem,10,10,100));
			SetBrushColor(hdc_mem,MapRGB(hdc_mem,100,100,200));
			SetTextColor(hdc_mem,MapRGB(hdc_mem,250,0,0));
			x_wsprintf(wbuf,L"TIME\r\n[%04d]",time);
			DrawText(hdc_mem,wbuf,-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

			DCtoBitmap(hdc_mem,&bm);
			ScaleBitmap(hdc,x,y,w,h,&bm);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
			DeleteDC(hdc_mem);
			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
//		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_ScaleBitmap(void)
//static void	AppMain(void)
{
	HWND	hwnd;
	WNDCLASSEX	wcex;
	MSG msg;

	/////

	wcex.Tag 		    = WNDCLASSEX_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hIconSm		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("GUI_DEMO: ScaleBitmap"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								10,20,400,300,    //窗口位置和大小
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
