
/*
 *GUI_DEMO_Drawbitmap.c
 *2018年3月17日下午7:41:26
 *
 */

#include <emXGUI.h>

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000
/*============================================================================*/
extern "C" int	gdev_16bpp_rotate_bitmap(const SURFACE *pSurf,int cx,int cy,const BITMAP *bm,int angle);

extern const unsigned char gImage_0[];
/*============================================================================*/
static BITMAP bm_0;

static SURFACE *pSurf;
static HDC hdc_mem=NULL;

#define WIDTHBYTES(bits) (((bits) + 31) / 32 *4)
#define	RADIAN(angle) ((2*3.1416*angle)/360)

#include <math.h>

/*============================================================================*/

/*============================================================================*/

//static BITMAP bm_dst;
static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形

			//设置位图结构参数
			bm_0.Format	= BM_ARGB8888;     //位图格式
			bm_0.Width  = 92;              //宽度
			bm_0.Height = 184;             //高度
			bm_0.WidthBytes =bm_0.Width*4; //每行字节数
			bm_0.LUT =NULL;                //查找表(RGB/ARGB格式不使用该参数)
			bm_0.Bits =(void*)gImage_0;    //位图数据

			pSurf =CreateSurface(SURF_RGB565,100,200,-1,NULL);


			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-70,rc.h-40,68,32,hwnd,ID_OK,NULL,NULL);
			SetTimer(hwnd,0,1,TMR_START,NULL);

			rc.x =0;
			rc.y =0;
			rc.w =100;
			rc.h =200;
			hdc_mem =CreateDC(pSurf,&rc);

			//bm_dst.Bits=GUI_MEM_Alloc(200*200*4);
			//bm_dst.Format =BM_ARGB8888;
			//RotateDIB2(&bm_dst,&bm_0,45);
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
			if(1)
			{
				static int a=0;
				a+=5;
				a%=360;
				ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,100,0,200));
				RotateBitmap(hdc_mem,50,100,&bm_0,a);
			}
			rc.x=10;
			rc.y=10;
			rc.w=100;
			rc.h=200;

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
			int x,y;


			hdc =BeginPaint(hwnd,&ps);

			////用户的绘制内容...
			GetClientRect(hwnd,&rc0);

			x=150;
			y=10;
			DrawBitmap(hdc,x,y,&bm_0,NULL);
			rc.x=x;
			rc.y=y;
			rc.w=bm_0.Width;
			rc.h=bm_0.Height;
			DrawRect(hdc,&rc);

			rc.x=10;
			rc.y=10;
			rc.w=100;
			rc.h=200;
			BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_mem,0,0,SRCCOPY);


			//DrawBitmap(hdc,10,200,&bm_dst,NULL);

			EndPaint(hwnd,&ps);
			//////////

		}
		break;
		////

		case WM_DESTROY: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
		{
			DeleteSurface(pSurf);
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

extern "C" void	GUI_DEMO_Rotatebitmap(void)
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
	wcex.hInstance		= NULL;//hInst;
	wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	//创建主窗口
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("RotateBitmap(ARGB8888 Format)"),
								/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
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
