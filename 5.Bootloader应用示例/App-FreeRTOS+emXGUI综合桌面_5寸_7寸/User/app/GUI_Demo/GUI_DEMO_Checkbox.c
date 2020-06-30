
#include	"emXGUI.h"

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/

#define	ID_EXIT		0x1000
#define	ID_CB1		0x1101
#define	ID_CB2		0x1102
#define	ID_CB3		0x1103
#define	ID_CB4		0x1104
#define	ID_CB5		0x1105

/*===================================================================================*/

//static void checkbox_owner_draw(DRAWITEM_HDR *ds)
//{


//}


static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
//	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
//	WCHAR wbuf[128];
	//int i,x,y;
//	HWND wnd;

	switch(msg)
	{
		case	WM_CREATE:
				

				GetClientRect(hwnd,&rc);
				


//				CreateWindow(BUTTON,L"EXIT",WS_VISIBLE,rc.w-100,8,80,48,hwnd,ID_EXIT,NULL,NULL);
				rc.x =20;
				rc.y =40;
				rc.w =140;
				rc.h =30;

				CreateWindow(BUTTON,L"Checkbox1",BS_CHECKBOX|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_CB1,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10);
				CreateWindow(BUTTON,L"Checkbox2",BS_CHECKBOX|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_CB2,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10);
				CreateWindow(BUTTON,L"Checkbox3",WS_DISABLED|BS_CHECKBOX|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_CB3,NULL,NULL);

				OffsetRect(&rc,0,rc.h+10);
				CreateWindow(BUTTON,L"Checkbox4",BS_PUSHLIKE|BS_CHECKBOX|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_CB4,NULL,NULL);

				rc.x =170;
				rc.y =40;
				rc.w =140;
				rc.h =30;
				CreateWindow(BUTTON,L"Checkbox5",BS_CHECKBOX|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_CB5,NULL,NULL);

				SetTimer(hwnd,1,200,TMR_START,NULL);
				return TRUE;
				////////
				
		case	WM_TIMER:
		{

          return TRUE;
		}
		
		////

		case	WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;
			NMHDR *nr=(NMHDR*)lParam;

			id  =LOWORD(wParam);
			code=HIWORD(wParam);

			if(id >= ID_CB1 && id<= ID_CB5)
			{
				if(code == BN_CLICKED) //被点击了
				{
					if(SendMessage(nr->hwndFrom,BM_GETSTATE,0,0)&BST_CHECKED) //获取当前状态
					{ //复选框选中.
						GUI_INFO("Checkbox Checked: ID:%04XH\r\n",id);
					}
					else
					{//复选未框选中.
						GUI_INFO("Checkbox Unchecked: ID:%04XH\r\n",id);
					}
				}

				if(code == BN_CHECKED) //指定了BS_NOTIFY,才会产生该消息.
				{ //复选框选中.
					GUI_INFO("Checkbox Checked: ID:%04XH\r\n",id);
				}

			}

		}
		break;
		////
		case	WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);

			if(id== ID_CB5)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

				if(SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_CHECKED)
				{
					cr->TextColor =RGB888(250,0,0);
					cr->BackColor =RGB888(220,200,200);
					cr->BorderColor =RGB888(30,30,230);
					cr->ForeColor =RGB888(100,250,100);
				}
				else
				{
					cr->TextColor =RGB888(10,150,10);
					cr->BackColor =RGB888(200,220,200);
					cr->BorderColor =RGB888(50,50,50);
					cr->ForeColor =RGB888(180,200,230);
				}
				return TRUE;
			}
			else
			{
				return FALSE;
			}

		}
//		break;
		////

		case	WM_DRAWITEM:
		{
//			DRAWITEM_HDR *ds;

//			ds =(DRAWITEM_HDR*)lParam;
#if 1
			//if(wParam==ID_SCROLLBAR2)
			{
				//progressbar_owner_draw(ds);
				return TRUE;
			}
#endif
//			return FALSE;
		}
//		break;
						
		case	WM_PAINT:
				BeginPaint(hwnd,&ps);
				
				EndPaint(hwnd,&ps);
				return	TRUE;
				////
    
    case	WM_DESTROY:
				{
					PostQuitMessage(hwnd);
				}
				return	TRUE;
			
				
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
	return	WM_NULL;
}

void	GUI_DEMO_Checkbox(void *p)
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
		wcex.hInstance		= 0;//hInst;
		wcex.hIcon			= 0;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
		wcex.hCursor		= 0;//LoadCursor(NULL, IDC_ARROW);
		
		
		hwnd	=CreateWindowEx(	WS_EX_LOCKPOS,
									&wcex,
									_T("GUI Demo - Checkbox"),
									/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN|WS_CLOSEBOX,
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
