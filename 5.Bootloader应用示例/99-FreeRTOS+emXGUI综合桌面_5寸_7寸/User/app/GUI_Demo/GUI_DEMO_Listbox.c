
#include <emXGUI.h>

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/
#define	ID_EXIT		0x1000
#define	ID_LISTBOX1	0x1101
#define	ID_LISTBOX2	0x1102
#define	ID_LISTBOX3	0x1103

/*===================================================================================*/
static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];

	hwnd =ds->hwnd;
	hdc =ds->hDC;

  /************************************第一部分************************************/
	SetBrushColor(hdc,MapRGB(hdc,0,30,120));
	FillRect(hdc,&ds->rc);

  /************************************第二部分************************************/
	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);

	while(i<count)
	{
    /************************************第三部分************************************/
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
    
		if(rc.y > ds->rc.h)
		{
			break;    // 当前项不在显示范围内直接退出
		}

		if(i==cursel)
		{
      /* 绘制选中项的背景和文本颜色 */
			GradientFillRect(hdc,&rc,MapRGB(hdc,220,220,220),MapRGB(hdc,220,20,20),TRUE);
			SetTextColor(hdc,MapRGB(hdc,250,250,250));
			//SetBrushColor(hdc,MapRGB(hdc,220,20,20));
			//FillRect(hdc,&rc);
		}
		else
		{
      /* 绘制未选中的文本颜色 */
			SetTextColor(hdc,MapRGB(hdc,10,255,0));
		}

    /* 绘制文本 */
		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

    /* 下一个项位置 */
		OffsetRect(&rc,0,rc.h);

		i++;     // 下一项
	}
}

static int old_sel=0;

static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
//	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
//	WCHAR wbuf[128];
	int i;
//  int x,y;
	HWND wnd;
	
	switch(msg)
	{
		case	WM_CREATE:
				

				GetClientRect(hwnd,&rc);
				
				//CreateWindow(BUTTON,_T("Exit"),WS_BORDER|WS_VISIBLE|WS_CHILDWINDOW,8,8,80,30,hwnd,ID_EXIT,NULL,NULL);
				//wnd=CreateWindow(LISTBOX,_T("Listbox1"),WS_BORDER|WS_VISIBLE|WS_CHILDWINDOW,8,8,160,80,hwnd,ID_LISTBOX1,NULL,NULL);
				wnd=CreateWindow(LISTBOX,_T("Listbox"),WS_OWNERDRAW|LBS_LINE|LBS_NOTIFY|WS_BORDER|WS_VISIBLE,8,8,160,100,hwnd,ID_LISTBOX1,NULL,NULL);
				SendMessage(wnd,LB_ADDSTRING,0,(LPARAM)L"Item-1-0");
				SendMessage(wnd,LB_ADDSTRING,1,(LPARAM)L"Item-1-1");
				SendMessage(wnd,LB_ADDSTRING,2,(LPARAM)L"Item-1-2");
				SendMessage(wnd,LB_ADDSTRING,3,(LPARAM)L"Item-1-3");
				SendMessage(wnd,LB_ADDSTRING,4,(LPARAM)L"Item-1-4");
				SendMessage(wnd,LB_ADDSTRING,5,(LPARAM)L"Item-1-5");
				SendMessage(wnd,LB_ADDSTRING,6,(LPARAM)L"Item-1-6");
				SendMessage(wnd,LB_ADDSTRING,7,(LPARAM)L"Item-1-7");
				SendMessage(wnd,LB_ADDSTRING,8,(LPARAM)L"Item-1-8");
				SendMessage(wnd,LB_ADDSTRING,9,(LPARAM)L"Item-1-9");
				SendMessage(wnd,LB_ADDSTRING,10,(LPARAM)L"Item-1-10");

				SendMessage(wnd,LB_SETITEMHEIGHT,1,20);
				SendMessage(wnd,LB_SETITEMHEIGHT,3,50);

				////
				wnd=CreateWindow(LISTBOX,_T("Listbox2"),LBS_LINE|WS_BORDER|WS_VISIBLE,200,8,100,160,hwnd,ID_LISTBOX2,NULL,NULL);
				SendMessage(wnd,LB_ADDSTRING,11,(LPARAM)L"Item-2-0");
				SendMessage(wnd,LB_ADDSTRING,12,(LPARAM)L"Item-2-1");
				SendMessage(wnd,LB_ADDSTRING,13,(LPARAM)L"Item-2-2");
				SendMessage(wnd,LB_ADDSTRING,14,(LPARAM)L"Item-2-3");
				SendMessage(wnd,LB_ADDSTRING,15,(LPARAM)L"Item-2-4");
				SendMessage(wnd,LB_ADDSTRING,16,(LPARAM)L"Item-2-5");
				SendMessage(wnd,LB_ADDSTRING,17,(LPARAM)L"Item-2-6");
				SendMessage(wnd,LB_ADDSTRING,18,(LPARAM)L"Item-2-7");
				SendMessage(wnd,LB_ADDSTRING,19,(LPARAM)L"Item-2-8");
				SendMessage(wnd,LB_ADDSTRING,20,(LPARAM)L"Item-2-9");
				SendMessage(wnd,LB_ADDSTRING,21,(LPARAM)L"Item-2-10");

				wnd=CreateWindow(LISTBOX,_T("Listbox3"),LBS_LINE|WS_BORDER|WS_VISIBLE,8,130,160,100,hwnd,ID_LISTBOX3,NULL,NULL);
				SendMessage(wnd,LB_ADDSTRING,20,(LPARAM)L"Item-3-0");
				SendMessage(wnd,LB_ADDSTRING,21,(LPARAM)L"Item-3-1");
				SendMessage(wnd,LB_ADDSTRING,22,(LPARAM)L"Item-3-2");
				SendMessage(wnd,LB_ADDSTRING,23,(LPARAM)L"Item-3-3");
				SendMessage(wnd,LB_ADDSTRING,24,(LPARAM)L"Item-3-4");
				SendMessage(wnd,LB_ADDSTRING,25,(LPARAM)L"Item-3-5");
				SendMessage(wnd,LB_ADDSTRING,26,(LPARAM)L"Item-3-6");
				SendMessage(wnd,LB_ADDSTRING,27,(LPARAM)L"Item-3-7");
				SendMessage(wnd,LB_ADDSTRING,28,(LPARAM)L"Item-3-8");
				SendMessage(wnd,LB_ADDSTRING,29,(LPARAM)L"Item-3-9");
				SendMessage(wnd,LB_ADDSTRING,30,(LPARAM)L"Item-3-10");


				old_sel =0;
				SetTimer(hwnd,1,1000,TMR_START,NULL);
				return TRUE;
				////////
				


				
		case	WM_TIMER:
				
				GUI_INFO("WM_TIMER: id:%d\r\n",wParam);
				break;
				
		
		case	WM_LBUTTONDOWN:
		/*
				i=MessageBox(hwnd,_T("Hello"),_T("MessageBox"),MB_YESNO|MB_ICONQUESTION);
				
				if(i==IDYES)
				{
					SendMessage(hwnd,WM_CLOSE,0,0);
				}  */
				
				//SendMessage(hwnd,WM_CLOSE,0,0);
				GUI_INFO("window0:WM_LBUTTONDOWN:%08XH,%d,%d\r\n",wParam,HIWORD(lParam),LOWORD(lParam));
				break;
				////
				
		case	WM_LBUTTONUP:
				GUI_INFO("window0:WM_LBUTTONUP:%08XH,%d,%d\r\n",wParam,HIWORD(lParam),LOWORD(lParam));
				//SendMessage(hwnd,WM_CLOSE,0,0);
				break;
				////
		case	WM_MOUSEMOVE:
				GUI_INFO("window0:WM_MOUSEMOVE:%08XH,%d,%d\r\n",wParam,HIWORD(lParam),LOWORD(lParam));
				break;
				////

		case	WM_NOTIFY:
		{
			NMHDR *nr;
			nr =(NMHDR*)lParam;

			if(nr->idFrom == ID_LISTBOX1)
			{
				if(nr->code == LBN_SELCHANGE)
				{
					int i;

					i =SendMessage(nr->hwndFrom,LB_GETCURSEL,0,0);

					SendMessage(nr->hwndFrom,LB_SETITEMHEIGHT,old_sel,20);

					SendMessage(nr->hwndFrom,LB_SETITEMHEIGHT,i,30);

					old_sel =i;

					GUI_INFO("Listbox SEL_CHANGE:%d.\r\n",i);
				}

				if(nr->code==LBN_CLICKED)
				{
					i =SendMessage(nr->hwndFrom,LB_GETCURSEL,0,0);
					i =SendMessage(nr->hwndFrom,LB_GETITEMOFFSET,i,0);

					//SendMessage(nr->hwndFrom,LB_SETPOS,0,i);
				}
			}

		}
		break;
		////
		case	WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);

			if(id== ID_LISTBOX3)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

					cr->TextColor =RGB888(255,255,255);
					cr->BackColor =RGB888(0,130,0);
					cr->BorderColor =RGB888(50,150,50);
					cr->ForeColor =RGB888(0,50,0);

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
			DRAWITEM_HDR *ds;

			ds =(DRAWITEM_HDR*)lParam;

			if(wParam==ID_LISTBOX1)
			{
				listbox_owner_draw(ds);
				return TRUE;
			}
			return FALSE;
		}
//		break;
		////
						
		case	WM_PAINT:
				BeginPaint(hwnd,&ps);
				
				EndPaint(hwnd,&ps);
				return	TRUE;
				////
			
		case	WM_CLOSE:
				{
					
					GUI_INFO("win_close\r\n");
					DestroyWindow(hwnd);
				}
				return	TRUE;
				////
				
		case	WM_DESTROY:
				{
					PostQuitMessage(hwnd);
				}
				return	TRUE;
				////
				
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
	return	WM_NULL;
}

void	GUI_DEMO_Listbox(void *p)
{
		HWND	hwnd;
		WNDCLASS	wcex;
		MSG msg;
	
		//(???????.)
		InvalidateRect(GetDesktopWindow(),NULL,TRUE);
		
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
									_T("GUI Demo - Listbox"),
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
