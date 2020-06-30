
/*
 *GUI_DEMO_Hello.c
 *2018年5月25日下午1:27:05
 *
 */

#include <emXGUI.h>
/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮(示例).
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

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...

			SetTextColor(hdc,MapRGB(hdc,250,10,10)); //设置文字颜色.
			TextOut(hdc,10,10,L"Hello! emXGUI Test",-1); //输出文字

			SetPenColor(hdc,MapRGB(hdc,0,0,250)); //设置画笔颜色(用于画线，框).

			//设置矩形参数
			rc.x =10;
			rc.y =50;
			rc.w =100;
			rc.h =60;
			DrawRect(hdc,&rc); //绘制一个空心矩形

			DrawCircle(hdc,80,120,50); //绘制一个空心圆.

			Line(hdc,10,150,120,200); //画线

			SetBrushColor(hdc,MapRGB(hdc,0,200,0)); //设置画刷颜色(用于填充).

			//设置矩形参数
			rc.x =150;
			rc.y =50;
			rc.w =120;
			rc.h =60;
			FillRect(hdc,&rc); //填充一个实心矩形

			OffsetRect(&rc,0,rc.h+4);  //移动矩形位置
			FillRoundRect(hdc,&rc,15); //填充一个实心圆角矩形.

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

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

void	GUI_DEMO_Hello(void)
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
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
//	wcex.hIconSm		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	NULL,
								&wcex,
								_T("emXGUI Window"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								10,20,320,240,    //窗口位置和大小
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
