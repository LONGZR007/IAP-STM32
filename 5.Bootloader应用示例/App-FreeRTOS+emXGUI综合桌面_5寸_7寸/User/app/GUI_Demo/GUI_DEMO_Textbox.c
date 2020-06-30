
#include <emXGUI.h>
#include <x_libc.h>

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/

#define	ID_EXIT		0x1000
#define	ID_TB1		0x1101
#define	ID_TB2		0x1102
#define	ID_TB3		0x1103
#define	ID_TB4		0x1104
#define	ID_TB5		0x1105
#define	ID_TB6		0x1106
#define	ID_TB7		0x1107

#define	ID_TMR1		0x0001

/*===================================================================================*/


static u16 count=0;

static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
//	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	WCHAR wbuf[128];
//	int i;
	HWND wnd;

	switch(msg)
	{
		case	WM_CREATE:
				

				GetClientRect(hwnd,&rc);
				
				//CreateWindow(BUTTON,L"EXIT",WS_VISIBLE,rc.w-100,8,80,48,hwnd,ID_EXIT,NULL,NULL);
				rc.x =20;
				rc.y =30;
				rc.w =100;
				rc.h =40;

				wnd=CreateWindow(TEXTBOX,L"Text1",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TB1,NULL,NULL);
				SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND|DT_BORDER);

				OffsetRect(&rc,0,rc.h+10);
				wnd=CreateWindow(TEXTBOX,L"Text2",TBS_SUNK|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TB2,NULL,NULL);
				SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_BKGND);

				OffsetRect(&rc,0,rc.h+10);
				wnd=CreateWindow(TEXTBOX,L"Text3",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TB3,NULL,NULL);
				SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_RIGHT|DT_VCENTER|DT_BKGND|DT_BORDER);

				OffsetRect(&rc,0,rc.h+10);
				wnd=CreateWindow(TEXTBOX,L"Text4",WS_DISABLED|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TB4,NULL,NULL);
				SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_SINGLELINE|DT_VCENTER|DT_BOTTOM|DT_BKGND|DT_BORDER);

				rc.x =150;
				rc.y =30;
				rc.w =100;
				rc.h =40;
				CreateWindow(TEXTBOX,L"Text5",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TB5,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10);
				rc.w =180;
				rc.h =80;
				wnd=CreateWindow(TEXTBOX,L"Text6\r\nMulti-Line\r\nVCENTER+CENTER",WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TB6,NULL,NULL);
				SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND|DT_BORDER);

				OffsetRect(&rc,0,rc.h+10);

				wnd=CreateWindow(TEXTBOX,L"Text7\r\nMulti-Line\r\nBOTTOM+RIGHT",TBS_SUNK|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TB7,NULL,NULL);
				SendMessage(wnd,TBM_SET_TEXTFLAG,0,DT_BOTTOM|DT_RIGHT|DT_BKGND|DT_BORDER);

				SetTimer(hwnd,ID_TMR1,200,TMR_START,NULL); //创建一个200ms定时器
				return TRUE;
				////////
				
		case	WM_TIMER:
		{
			u16 id;

			id =LOWORD(wParam);

			if(id==ID_TMR1)
			{
				count++;

				x_wsprintf(wbuf,L"%04d",count);
				wnd =GetDlgItem(hwnd,ID_TB5);
				SetWindowText(wnd,wbuf);
			}
		}
		return TRUE;
		////

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

			if(id== ID_TB5 || id== ID_TB6)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;


				cr->TextColor =RGB888(255,180,180);
				cr->BackColor =RGB888(100,0,0);
				cr->BorderColor =RGB888(255,10,10);

				return TRUE;
			}
			else
			{
				return FALSE;
			}

		}
//		break;
		////

    case	WM_DESTROY:
    {
      PostQuitMessage(hwnd);
    }
    return	TRUE;
						
		case	WM_PAINT:
				BeginPaint(hwnd,&ps);
				
				EndPaint(hwnd,&ps);
				return	TRUE;
				////
			
				
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
//	return	WM_NULL;
}

void	GUI_DEMO_Textbox(void *p)
{
		HWND	hwnd;
		WNDCLASS	wcex;
		MSG msg;

		/////
		wcex.Tag 		    = WNDCLASS_TAG;
		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= win_proc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= NULL;//hInst;
		wcex.hIcon			= NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
		wcex.hCursor		= NULL;//LoadCursor(NULL, IDC_ARROW);
		
		
		hwnd	=CreateWindowEx(	WS_EX_LOCKPOS,
									&wcex,
									L"GUI Demo - Textbox",
									WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN|WS_CLOSEBOX,
									0,0,GUI_XSIZE,GUI_YSIZE,
									NULL,NULL,NULL,NULL);
		
			
		ShowWindow(hwnd,SW_SHOW);	
		while(GetMessage(&msg,hwnd))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	
}
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
