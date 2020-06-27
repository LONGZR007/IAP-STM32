
#include <emXGUI.h>
#include <x_libc.h>

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/
#define	ID_EXIT			0x1000
#define	ID_SCROLLBAR1	0x1101
#define	ID_SCROLLBAR2	0x1102
#define	ID_SCROLLBAR3	0x1103

/*===================================================================================*/

static void scrollbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
//	int i;
	WCHAR wbuf[128];
	SCROLLINFO sif;



	hwnd =ds->hwnd;
	hdc =ds->hDC;

	GetClientRect(hwnd,&rc);

	SetBrushColor(hdc,MapRGB(hdc,150,200,250));
	FillRect(hdc,&ds->rc);

	SendMessage(hwnd,SBM_GETTRACKRECT,0,(LPARAM)&rc);

	sif.fMask =SIF_ALL;

	SendMessage(hwnd,SBM_GETSCROLLINFO,0,(LPARAM)&sif);

	SetBrushColor(hdc,MapRGB(hdc,250,10,10));

	if(ds->State & SST_THUMBTRACK)
	{
		SetPenColor(hdc,MapRGB(hdc,250,50,50));

	}
	else
	{
		SetPenColor(hdc,MapRGB(hdc,50,50,50));
	}

	//FillCircle(hdc,rc.x+(rc.w>>1),rc.y+(rc.h>>1),MIN((rc.w>>1),(rc.h>>1))-1);

	DrawRect(hdc,&rc);
	x_wsprintf(wbuf,L"%d",sif.nValue);

	InflateRect(&rc,40,0);
	DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);

}

//static int old_sel=0;

static	LRESULT	win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
//	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
//	WCHAR wbuf[128];
//	int i,x,y;
	HWND wnd;

	switch(msg)
	{
		case	WM_CREATE:
				
				GetClientRect(hwnd,&rc);
				
				if(1)
				{
					SCROLLINFO sif;
					sif.cbSize		=sizeof(sif);
					sif.fMask		=SIF_ALL;
					sif.nMin		=-50;
					sif.nMax		=+50;
					//sif.nMin		=-(1020<<20);
					//sif.nMax		=+(1020<<20);
					sif.nValue		=0;
					//sif.nTrackValue	=0;
					sif.TrackSize		=30;
					sif.ArrowSize		=20;//20;


					wnd = CreateWindow(SCROLLBAR,L"VScroll",SBS_VERT|WS_VISIBLE,8,8,20,160,hwnd,ID_SCROLLBAR1,NULL,NULL);
					SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);


					wnd = CreateWindow(SCROLLBAR,L"HScroll",WS_OWNERDRAW|WS_VISIBLE,50,100,200,20,hwnd,ID_SCROLLBAR2,NULL,NULL);
					SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);

					wnd = CreateWindow(SCROLLBAR,L"HScroll",WS_VISIBLE,50,150,200,20,hwnd,ID_SCROLLBAR3,NULL,NULL);
					SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);
				}


//				old_sel =0;

				return TRUE;
				////////
				

		case	WM_NOTIFY: //通知消息
		{
			NMHDR *nr;
			u16 ctr_id;

			ctr_id =LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
			nr =(NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.

			if(ctr_id == ID_SCROLLBAR1 || ctr_id == ID_SCROLLBAR2 )
			{
				NM_SCROLLBAR *sb_nr;
				int i;

				sb_nr =(NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
				switch(nr->code)
				{
					case SBN_CLICKED: //单击
					{

						if(sb_nr->cmd==SB_TRACK) //NM_SCROLLBAR结构体成员cmd指明了单击发生的位置
						{//在滑块内单击.
							GUI_INFO("SCROLLBAR CLICK In Track.\r\n");
						}
						else
						{
							GUI_INFO("SCROLLBAR CLICK :%d.\r\n",sb_nr->cmd);
						}
					}
					break;
					////

					case SBN_THUMBTRACK: //滑块移动
					{

						i =sb_nr->nTrackValue; //获得滑块当前位置值

						SendMessage(nr->hwndFrom,SBM_SETVALUE,TRUE,i); //设置位置值
						GUI_INFO("SCROLLBAR TRACK :%d.\r\n",i);
					}
					break;
					////

					default:
						break;

				}

			}

		}
		break;
		////
		case	WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);

			if(id== ID_SCROLLBAR3)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;


					cr->TextColor =RGB888(100,255,255);
					cr->BackColor =RGB888(100,100,150);
					cr->BorderColor =RGB888(50,50,150);
					cr->ForeColor =RGB888(50,150,250);

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
#if 1
			//if(wParam==ID_SCROLLBAR2)
			{
				scrollbar_owner_draw(ds);
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

void	GUI_DEMO_Scrollbar(void *p)
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
									_T("GUI Demo - Scrollbar"),
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
