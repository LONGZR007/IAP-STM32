
/*
 *GUI_DEMO_Messagebox.c
 *2018年4月21日下午8:31:34
 *
 */

#include <emXGUI.h>
/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000
#define	ID_BTN1		0x1001
#define	ID_BTN2		0x1002
#define	ID_BTN3		0x1003

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形

			//CreateWindow(BUTTON,L"OK",WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL);

			rc.x =20;
			rc.y =30;
			rc.w =100;
			rc.h =40;
			CreateWindow(BUTTON,L"Msgbox-1",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN1,NULL,NULL);

			OffsetRect(&rc,rc.w+10,0);
			CreateWindow(BUTTON,L"Msgbox-2",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN2,NULL,NULL);

			OffsetRect(&rc,rc.w+10,0);
			CreateWindow(BUTTON,L"Msgbox-3",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_BTN3,NULL,NULL);


		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;
			int ret=0;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_OK && code==BN_CLICKED)
			{
				PostCloseMessage(hwnd); //产生WM_CLOSE消息关闭窗口
			}

			if(id==ID_BTN1 && code==BN_CLICKED)
			{

				ret=MessageBox(hwnd,10,20,200,120,L"Hello!",L"MsgBox-1",NULL);
			}

			if(id==ID_BTN2 && code==BN_CLICKED)
			{
				const WCHAR *btn[] ={L"YES",L"NO",};
				MSGBOX_OPTIONS mb;
				mb.Flag =MB_BTN_WIDTH(80)|MB_ICONWARNING;
				mb.pButtonText =btn;
				mb.ButtonCount=2;

				ret=MessageBox(hwnd,10,20,320,150,L"MB_YESNO + MB_ICON\r\nABCDEFG",L"MsgBox-2",&mb);
			}

			if(id==ID_BTN3 && code==BN_CLICKED)
			{

				const WCHAR *btn[] ={L"Yes to all",L"取消",L"确认>>"};

				MSGBOX_OPTIONS mb;
				mb.Flag = MB_ICONINFORMATION;
				mb.pButtonText =btn;
				mb.ButtonCount=3;

				ret=MessageBox(hwnd,10,20,360,180,L"Auto size + MB_ICON\r\nMulti-Line",L"MsgBox-3",&mb);
			}
			GUI_INFO("Messagebox Return:%d\r\n",ret);

		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
//			HDC hdc;

			BeginPaint(hwnd,&ps);

			////用户的绘制内容...
			//TextOut(hdc,10,10,L"Hello",-1);

			EndPaint(hwnd,&ps);
		}
		break;
		////

		case WM_DESTROY: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
		{
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

void	GUI_DEMO_Messagebox(void *p)
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

	//创建主窗口
	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS,
								&wcex,
								_T("GUI_DEMO-Messagebox"),
								/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN|WS_CLOSEBOX,
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
