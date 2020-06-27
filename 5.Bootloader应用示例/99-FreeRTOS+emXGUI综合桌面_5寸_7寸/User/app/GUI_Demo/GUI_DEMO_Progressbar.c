
#include <emXGUI.h>
#include <x_libc.h>

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/
#define	ID_EXIT			0x1000
#define	ID_PROGBAR1		0x1100
#define	ID_PROGBAR2		0x1101
#define	ID_PROGBAR3		0x1102
#define	ID_PROGBAR4		0x1103

/*===================================================================================*/

static void progressbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc,m_rc[2];
//	int range,val;
	WCHAR wbuf[128];
	PROGRESSBAR_CFG cfg;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	GetClientRect(hwnd,&rc);

	SetBrushColor(hdc,MapRGB(hdc,150,200,250));
	FillRect(hdc,&ds->rc);

	cfg.cbSize =sizeof(cfg);
	cfg.fMask =PB_CFG_ALL;

	SendMessage(hwnd,PBM_GET_CFG,0,(LPARAM)&cfg);

	MakeProgressRect(m_rc,&rc,cfg.Rangle,cfg.Value,PB_ORG_BOTTOM);

	SetBrushColor(hdc,MapRGB(hdc,250,10,10));
	FillRoundRect(hdc,&m_rc[0],2);

	SetPenColor(hdc,MapRGB(hdc,100,10,10));
	DrawRoundRect(hdc,&m_rc[0],2);

	//FillCircle(hdc,rc.x+(rc.w>>1),rc.y+(rc.h>>1),MIN((rc.w>>1),(rc.h>>1))-1);

	DrawRect(hdc,&rc);
	x_wsprintf(wbuf,L"%d",cfg.Value);

	//InflateRect(&rc,40,0);
	DrawText(hdc,wbuf,-1,&rc,DT_VCENTER|DT_CENTER);
}

static u8 pb1_val=0;
static u8 pb2_val=0;

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
					PROGRESSBAR_CFG cfg;

					cfg.cbSize	 = sizeof(PROGRESSBAR_CFG);
					cfg.fMask    = PB_CFG_ALL;
					cfg.TextFlag = DT_VCENTER|DT_CENTER;

					wnd = CreateWindow(PROGRESSBAR,L"VProgressbar",WS_OWNERDRAW|PBS_TEXT|PBS_ALIGN_BOTTOM|WS_VISIBLE,8,8,24,160,hwnd,ID_PROGBAR1,NULL,NULL);
					SendMessage(wnd,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
					SendMessage(wnd,PBM_SET_CFG,TRUE,(LPARAM)&cfg);

					wnd = CreateWindow(PROGRESSBAR,L"HProgbar & Right align",PBS_TEXT|PBS_ALIGN_RIGHT|WS_VISIBLE,50,100,280,24,hwnd,ID_PROGBAR2,NULL,NULL);
					SendMessage(wnd,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
					SendMessage(wnd,PBM_SET_CFG,TRUE,(LPARAM)&cfg);

					wnd = CreateWindow(PROGRESSBAR,L"HProgbar & Left align",PBS_TEXT|PBS_ALIGN_LEFT|WS_VISIBLE|PBS_3D,50,200,280,24,hwnd,ID_PROGBAR3,NULL,NULL);
					SendMessage(wnd,PBM_GET_CFG,TRUE,(LPARAM)&cfg);
					SendMessage(wnd,PBM_SET_CFG,TRUE,(LPARAM)&cfg);
				}


				//CreateWindow(BUTTON,L"EXIT",WS_VISIBLE,rc.w-100,8,80,48,hwnd,ID_EXIT,NULL,NULL);

				SetTimer(hwnd,1,200,TMR_START,NULL);
				return TRUE;
				////////
				
		case	WM_TIMER:
		{

			pb1_val +=1;
			if(pb1_val > 100)
			{
				pb1_val =0;
			}
			wnd =GetDlgItem(hwnd,ID_PROGBAR1);
			SendMessage(wnd,PBM_SET_VALUE,TRUE,pb1_val);

			pb2_val +=2;
			if(pb2_val > 100)
			{
				pb2_val =0;
			}
			wnd =GetDlgItem(hwnd,ID_PROGBAR2);
			SendMessage(wnd,PBM_SET_VALUE,TRUE,pb2_val);
      
      wnd =GetDlgItem(hwnd,ID_PROGBAR3);
			SendMessage(wnd,PBM_SET_VALUE,TRUE,pb2_val);

		}
		return TRUE;
		////

		case	WM_NOTIFY:
		{
			NMHDR *nr;
			nr =(NMHDR*)lParam;

			if(nr->idFrom == ID_PROGBAR1 || nr->idFrom == ID_PROGBAR2 )
			{

			}

		}
		break;
		////
		case	WM_CTLCOLOR:
		{
			u16 id;
			CTLCOLOR *cr;

			id =wParam;
			cr =(CTLCOLOR*)lParam;


			if(id==ID_PROGBAR3)
			{
				cr->TextColor =RGB888(250,250,250);
				cr->BackColor =RGB888(0,0,0);
				cr->BorderColor =RGB888(130,30,130);
				cr->ForeColor  =RGB888(200,0,200);
				return TRUE;
			}

			return FALSE;

		}
//		break;
		////

		case	WM_DRAWITEM:
		{
			DRAWITEM_HDR *ds;

			ds =(DRAWITEM_HDR*)lParam;
#if 1
			if(wParam==ID_PROGBAR1)
			{
				progressbar_owner_draw(ds);
				return TRUE;
			}
#endif
//			return FALSE;
		}
//		break;
    
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
	
	return	WM_NULL;
}

void	GUI_DEMO_Progressbar(void *p)
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
									_T("GUI Demo - Progressbar"),
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
