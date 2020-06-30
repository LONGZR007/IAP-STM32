
#include <emXGUI.h>

#ifndef	_T
#define	_T(x) L##x
#endif

/*===================================================================================*/
/*===================================================================================*/
static	LRESULT	dlg_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	switch(msg)
	{
		case	WM_CREATE:
				
				GetClientRect(hwnd,&rc);
				CreateWindow(BUTTON,_T("关闭"),WS_BORDER|WS_VISIBLE,8,rc.h-28,80,24,hwnd,0x1000,NULL,NULL);

				break;
				////
	

		case	WM_SIZE:
		{
				return	DefDesktopProc(hwnd,msg,wParam,lParam);
		}
		
		case	WM_PAINT:
				hdc  =BeginPaint(hwnd,&ps);
		
				SetTextColor(hdc,MapRGB(hdc,200,0,0));
				TextOut(hdc,8,8,_T("Hello World !"),-1);
				EndPaint(hwnd,&ps);
				return TRUE;
				////
				
	
		default:
		return	DefWindowProc(hwnd,msg,wParam,lParam);	
	}
	return	WM_NULL;
}

static	LRESULT	win0_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	//WCHAR buf[128];
	int i,x,y;
	static U32 a0;
	static int close=0;
	static int destroy=0;
	HWND wnd;
	
	switch(msg)
	{
		case	WM_CREATE:
				
			//	SetWindowLong(hwnd,GWL_BKGNDERASEPROC,(LONG)erase_proc);
				
				a0		=0;
				close	=0;
				destroy	=0;
				GetClientRect(hwnd,&rc);
				
				CreateWindow(BUTTON,_T("Button"),WS_BORDER|WS_VISIBLE,8,8,80,30,hwnd,0x8000,NULL,NULL);


				////////
				x=20;
				y=40;
				CreateWindow(BUTTON,_T("Checkbox0"),BS_CHECKBOX|WS_VISIBLE,x,y,128,24,hwnd,0x2000,NULL,NULL);
				y+=32;
				
				CreateWindow(BUTTON,_T("Checkbox1"),BS_CHECKBOX|WS_VISIBLE,x,y,128,24,hwnd,0x2001,NULL,NULL);
				y+=32;
				
				CreateWindow(BUTTON,_T("Checkbox2"),BS_CHECKBOX|WS_VISIBLE,x,y,128,24,hwnd,0x2002,NULL,NULL);
				y+=32;
				
				////////	
				x=160;
				y=20;
				
				CreateWindow(BUTTON,_T("Radiobox0-0"),BS_RADIOBOX|WS_VISIBLE,x,y,128,24,hwnd,(1<<16)|0x2010,NULL,NULL);
				y+=32;
				
				CreateWindow(BUTTON,_T("Radiobox0-1"),BS_RADIOBOX|WS_VISIBLE,x,y,128,24,hwnd,(1<<16)|0x2011,NULL,NULL);
				y+=32;
				
				CreateWindow(BUTTON,_T("Radiobox0-2"),BS_RADIOBOX|WS_VISIBLE,x,y,128,24,hwnd,(1<<16)|0x2012,NULL,NULL);
				y+=32;
				
				
				
				CreateWindow(BUTTON,_T("Radiobox1-1"),BS_RADIOBOX|WS_VISIBLE,x,y,128,24,hwnd,(2<<16)|0x2120,NULL,NULL);
				y+=32;
				
				CreateWindow(BUTTON,_T("Radiobox1-2"),BS_RADIOBOX|WS_VISIBLE,x,y,128,24,hwnd,(2<<16)|0x2121,NULL,NULL);
				y+=32;
				
				CreateWindow(BUTTON,_T("Radiobox1-3"),BS_RADIOBOX|WS_VISIBLE,x,y,128,24,hwnd,(2<<16)|0x2122,NULL,NULL);
				y+=32;

				////////	
				x=300;
				y=20;
				
				CreateWindow(GROUPBOX,_T("Groupbox0"),WS_VISIBLE,x,y,160,60,hwnd,0x3010,NULL,NULL);
				y+=90;
				
				CreateWindow(GROUPBOX,_T("Groupbox1"),WS_VISIBLE,x,y,160,60,hwnd,0x3011,NULL,NULL);
				y+=90;
				
				wnd=CreateWindow(GROUPBOX,_T("Groupbox2"),WS_VISIBLE,x,y,160,60,hwnd,0x3012,NULL,NULL);
				y+=90;
				
				//SetWindowLong(wnd,GWL_BKGNDERASEPROC,(LONG)erase_proc);
				
				CreateWindow(BUTTON,_T("Checkbox"),BS_CHECKBOX|WS_CLIPCHILDREN|WS_VISIBLE,8,20,128,24,wnd,0x4000,NULL,NULL);
#if 0
				if(1)
				{
					SCROLLINFO sif;
					sif.cbSize		=sizeof(sif);
					sif.fMask		=SIF_ALL;
					sif.nMin		=0;
					sif.nMax		=100;
					sif.nPage		=4;
					sif.nValue		=50;
					sif.nTrackPos	=50;

					wnd = CreateWindow(SCROLLBAR,_T("HScroll"),WS_VISIBLE|SBS_VERT,20,150,20,96,hwnd,0x5010,NULL,NULL);
					SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);

					
					wnd = CreateWindow(SCROLLBAR,_T("HScroll"),WS_VISIBLE|SBS_HORZ,20,250,200,28,hwnd,0x5011,NULL,NULL);
					SendMessage(wnd,SBM_SETSCROLLINFO,TRUE,(LPARAM)&sif);
				}
#endif
				SetTimer(hwnd,1,1000,TMR_START,NULL);
				SetTimer(hwnd,2,1500,TMR_START,NULL);
				SetTimer(hwnd,3,2500,TMR_START|TMR_SINGLE,NULL);
				return TRUE;
				////////

		case	WM_HSCROLL:
		case	WM_VSCROLL:
				{
					U16 val,event;
					
					val		=(U16)HIWORD(wParam);
					event	=(U16)LOWORD(wParam);
					
					GUI_Printf("WM_SCROLL: event:%d,val%d,id:%04XH.\r\n",event,val,GetWindowLong((HWND)lParam,GWL_ID));
					switch(event)
					{
#if 0
						case	SB_LINEUP:
								if(val>0)
								{
									val--;
									SendMessage((HWND)lParam,SBM_SETPOS,val,0);
									//InvalidateRect(hwnd,NULL,FALSE);
								}
								break;
								////
								
						case	SB_LINEDOWN:
								if(val<100)
								{
									val++;
									SendMessage((HWND)lParam,SBM_SETPOS,val,0);
									//InvalidateRect(hwnd,NULL,FALSE);
									
								}
								break;
								/////
#endif
						case	SB_THUMBTRACK:
								{
									SendMessage((HWND)lParam,SBM_SETVALUE,val,0);
									//InvalidateRect(hwnd,NULL,FALSE);
								}
								break;
								////
								
					}
				}
				break;
				////////
#if 0
		case	WM_COMMAND:
				{
					U16 event,id;
					
					event =HIWORD(wParam);
					id	=LOWORD(wParam);
					GUI_Printf("WM_COMMAND: event: %04XH,id:%04XH.\r\n",event,id);
					
					switch(id)
					{
						
						case	0x8000:
								{
									int ret;
									WNDCLASSEX wcex;		
									
									wcex.Tag 			= WNDCLASSEX_TAG;

									wcex.Style			= CS_HREDRAW | CS_VREDRAW;
									wcex.lpfnWndProc	= dlg_proc;
									wcex.cbClsExtra		= 0;
									wcex.cbWndExtra		= 0;
									wcex.hInstance		= 0;//hInst;
									wcex.hIcon			= 0;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
									wcex.hCursor		= 0;//LoadCursor(NULL, IDC_ARROW);
									wcex.hIconSm		= 0;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
		
#if 1
									ret	=DialogWindowEx(	NULL,
															&wcex,
															_T("对话框窗口"),
															WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
															20,30,200,128,
															hwnd,NULL,NULL,NULL);
#endif

#if 0
									CreateWindowEx(	NULL,
															&wcex,
															_T("对话框窗口2"),
															/*WS_MEMSURFACE|*/WS_VISIBLE|WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
															20,30,160,120,
															hwnd,NULL,NULL,NULL);
#endif
								
									GUI_Printf("dlg_ret=%d\r\n",ret);
	
								}
								break;
								////
						default:
								return	DefWindowProc(hwnd,msg,wParam,lParam);
								////
					}
				}
				break;
				////
#endif
		case	WM_TIMER:
				
				GUI_Printf("WM_TIMER: id:%d\r\n",wParam);
				break;
				
		
		case	WM_LBUTTONDOWN:
		/*
				i=MessageBox(hwnd,_T("Hello"),_T("MessageBox"),MB_YESNO|MB_ICONQUESTION);
				
				if(i==IDYES)
				{
					SendMessage(hwnd,WM_CLOSE,0,0);
				}  */
				
				//SendMessage(hwnd,WM_CLOSE,0,0);
				GUI_Printf("window0:WM_LBUTTONDOWN:%08XH,%d,%d\r\n",wParam,HIWORD(lParam),LOWORD(lParam));
				break;
				////
				
		case	WM_LBUTTONUP:
				GUI_Printf("window0:WM_LBUTTONUP:%08XH,%d,%d\r\n",wParam,HIWORD(lParam),LOWORD(lParam));
				//SendMessage(hwnd,WM_CLOSE,0,0);
				break;
				////
		case	WM_MOUSEMOVE:
				GUI_Printf("window0:WM_MOUSEMOVE:%08XH,%d,%d\r\n",wParam,HIWORD(lParam),LOWORD(lParam));
				break;
				////
				
						
		case	WM_PAINT:
				hdc	=BeginPaint(hwnd,&ps);
				
				EndPaint(hwnd,&ps);
				return	TRUE;
				////
			
		case	WM_CLOSE:
				{
					
					GUI_Printf("win0_close\r\n");					
					DestroyWindow(hwnd);
				}
				return	TRUE;
				////
				
		case	WM_DESTROY:
				{
					GUI_Printf("win0_destroy:\r\n");
					PostQuitMessage(hwnd);
				}
				return	TRUE;
				////
				
		default:
				return	DefWindowProc(hwnd,msg,wParam,lParam);
	}
	
	return	WM_NULL;
}

void	GUI_DEMO_Window(void)
{
		HWND	hwnd;
		WNDCLASSEX	wcex;
		MSG msg;
	
		//(��һ����.)
		InvalidateRect(GetDesktopWindow(),NULL,TRUE);
		GUI_msleep(100);
		
		/////
		wcex.Tag 		    = WNDCLASSEX_TAG;

		wcex.Style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= win0_proc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= 0;//hInst;
		wcex.hIcon			= 0;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
		wcex.hCursor		= 0;//LoadCursor(NULL, IDC_ARROW);
		wcex.hIconSm		= 0;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
		
		
		hwnd	=CreateWindowEx(	NULL,
									&wcex,
									_T("Window0(主窗口)"),
									/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
									20,30,480,320,
									NULL,NULL,NULL,NULL);
		
			
		ShowWindow(hwnd,SW_SHOW);	
		while(GetMessage(&msg,hwnd))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		GUI_Printf("window exit=%08XH\r\n",msg.lParam);
		
	
}
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
/*===================================================================================*/
