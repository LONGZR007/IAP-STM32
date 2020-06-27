
/*
 *GUI_DEMO_WatchTime.c
 *2018年6月1日上午11:38:53
 *
 */

#include <emXGUI.h>
#include <math.h>

#include "x_libc.h"
#include "XFT_res.h"

#include "Kernel.h"

/*============================================================================*/
//定义控件ID

#define	ID_OK		0x1000
#define	ID_PT_TYPE	0x1001
#define	ID_TMR		0x1002

/*============================================================================*/

static local_time_t tm;
static HFONT hFont_SEG_NUM=NULL;
static int pt_type=0;

HFONT	XFT_CreateFont(const void *xft_dat);


BITMAP* png_decode_to_bitmap(const char *fpath);
void png_decode_free(BITMAP *de_bm);

static BITMAP *bm_back=NULL;
static BITMAP *bm_pt_hour=NULL;
static BITMAP *bm_pt_min=NULL;
static BITMAP *bm_pt_sec=NULL;

#define	BK_COLOR	10,40,50

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc0,rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			bm_back =png_decode_to_bitmap("B:sim_clock/back.png");
			bm_pt_sec =png_decode_to_bitmap("B:sim_clock/pt_sec.png");
			bm_pt_min =png_decode_to_bitmap("B:sim_clock/pt_min.png");
			bm_pt_hour =png_decode_to_bitmap("B:sim_clock/pt_hour.png");

			hFont_SEG_NUM =XFT_CreateFont(SEG_NUM_64); //创建64x64的数码管外观的字体(xft字体)


			CreateWindow(BUTTON,L"OK",  WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮.
			CreateWindow(BUTTON,L">>", WS_VISIBLE,rc.w-80,60,68,32,hwnd,ID_PT_TYPE,NULL,NULL); //创建一个按钮.

			SetTimer(hwnd,ID_TMR,100,TMR_START,NULL); //创建一个100ms的定时器.

			SYS_get_local_time(&tm); //获得当前时间
		}
		return TRUE;
		////

		case	WM_TIMER: //定时器消息,当窗口定时器定时时间到来时，产生该消息,wParam参数指明当前产生该消息的定时器ID.
		{

			if(wParam == ID_TMR)
			{
				local_time_t tm0;
				SYS_get_local_time(&tm0);
				if(tm.sec != tm0.sec)
				{
					tm =tm0; //RTC秒钟变化了
					InvalidateRect(hwnd,NULL,FALSE);
				}
			}
		}
		break;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_PT_TYPE && code==BN_CLICKED) // 按钮“单击”了.
			{
				pt_type++;
				if(pt_type > 6)
				{
					pt_type=0;
				}
				InvalidateRect(hwnd,NULL,FALSE);
			}

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{

			HDC hdc,hdc_mem;
			PAINTSTRUCT ps;
			int x,y,r;
			RECT rc0;
			WCHAR wbuf[64];


			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			////
			rc.x =8;
			rc.y =32;
			rc.w =400;
			rc.h =400;

			r =MIN(rc.w,rc.h)>>1;

			hdc_mem =CreateMemoryDC(BM_DEVICE,rc.w,rc.h); //创建一个与屏幕颜色兼容的"内存型绘图设备",用于绘制时钟内容,可以提升绘图性能并且可以避免画面闪烁.

			/////绘制时钟
			ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,BK_COLOR));

			x =(rc.w-bm_back->Width)>>1;
			y =(rc.h-bm_back->Height)>>1;
			DrawBitmap(hdc_mem,x,y,bm_back,NULL);

			x =rc.w>>1;
			y =rc.h>>1;

			RotateBitmap(hdc_mem,x,y,bm_pt_hour,((tm.hour%12)*30+(tm.min*30)/60));
			RotateBitmap(hdc_mem,x,y,bm_pt_min,tm.min*6);
			RotateBitmap(hdc_mem,x,y,bm_pt_sec,tm.sec*6);


			BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_mem,0,0,SRCCOPY); //将内存绘制设备的内存输出到窗口.

			DeleteDC(hdc_mem); //删除绘图设备.

#if 1
			////
			OffsetRect(&rc,rc.w+4,rc.h-80);
			rc.w =320;
			rc.h =72;

			r =MIN(rc.w,rc.h)>>1;

			hdc_mem =CreateMemoryDC(BM_DEVICE,rc.w,rc.h);

			/////显示时间字符
			ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,10,40,80));

			SetTextColor(hdc_mem,MapRGB(hdc_mem,10,200,250));
			//SetTextColor(hdc_mem,MapRGB888(hdc_mem,time_c[time_c_idx]));
			SetFont(hdc_mem,hFont_SEG_NUM);
			x_wsprintf(wbuf,L"%d:%02d:%02d",tm.hour,tm.min,tm.sec);
			rc0.x=0;
			rc0.y=0;
			rc0.w=rc.w;
			rc0.h=rc.h;
			DrawText(hdc_mem,wbuf,-1,&rc0,DT_SINGLELINE|DT_VCENTER|DT_CENTER);

			BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_mem,0,0,SRCCOPY);
			DeleteDC(hdc_mem);
#endif
			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_ERASEBKGND:
		{
			HDC hdc;
			RECT rc;
			hdc =(HDC)wParam;
			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB(hdc,BK_COLOR));
			FillRect(hdc,&rc);
			return TRUE;
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			if(bm_back!=NULL)
			{
				png_decode_free(bm_back);
			}

			if(bm_pt_sec!=NULL)
			{
				png_decode_free(bm_pt_sec);
			}

			if(bm_pt_min!=NULL)
			{
				png_decode_free(bm_pt_min);
			}

			if(bm_pt_hour!=NULL)
			{
				png_decode_free(bm_pt_hour);
			}

			DeleteFont(hFont_SEG_NUM); //删除字体.
			return DestroyWindow(hwnd);   //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_WatchTime_Bitmap(void)
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
	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS, //窗口不可拖动
								&wcex,
								_T("emXGUI Demo - WatchTime"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,    //窗口位置和大小
								NULL,0,NULL,NULL);

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
