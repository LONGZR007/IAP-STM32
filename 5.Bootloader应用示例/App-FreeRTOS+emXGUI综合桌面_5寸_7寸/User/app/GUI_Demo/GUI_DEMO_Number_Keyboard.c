/*============================================================================*/


#include <emXGUI.h>
#include "x_libc.h"
#include "stdio.h"

/*============================================================================*/
enum	eID
{

	eID_EXIT=0x1000,
	eID_OK,

	eID_TEXTBOX,
	eID_TIMER_100ms

};

////焦点切换顺序表(活动对象)
//static s16 act_obj=0;
//static u16 act_obj_tbl[]=
//{
//	eID_EXIT,
//	eID_OK,

//	L'7', L'8', L'9',
//	L'4', L'5', L'6',
//	L'1', L'2', L'3',
//	L'0', L'.', L'\b',


//};
//#define	ACT_OBJ_NUM	(sizeof(act_obj_tbl)/sizeof(act_obj_tbl[0]))


struct	__num_input_tbl
{
	u32 id;
	const WCHAR *pName;
};

static u16 id_tbl[]=
{
	L'7', L'8', L'9',
	L'4', L'5', L'6',
	L'1', L'2', L'3',
	L'0', L'.', L'\b',

};

static const WCHAR *str_tbl[]=
{
	L"7", L"8", L"9",
	L"4", L"5", L"6",
	L"1", L"2", L"3",
	L"0", L".", L"<-",

};

#define	NUM_BTN_COUNT	(sizeof(id_tbl)/sizeof(id_tbl[0]))



static HANDLE hInst=NULL;

/*============================================================================*/
/*============================================================================*/
#define	MAX_INPUT	40

typedef	struct {
	const WCHAR *pCaption;
	WCHAR str_buf[MAX_INPUT+2];
	u16 cur_count;
	u16 max_count;


}num_input_box_data_t;

static LRESULT	number_input_box_win_proc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int i;
	RECT rc,rc0,m_rc[12];
	WCHAR wbuf[128];

	num_input_box_data_t *pMsgData =(num_input_box_data_t*)GetWindowLong(hwnd,GWL_USERDATA);

	switch(msg)
	{
		case WM_CREATE: //窗口被创建时产生该消息，可以在这里做一些初始化的操作


			SetWindowLong(hwnd,GWL_USERDATA,lParam);

			pMsgData =(num_input_box_data_t *)lParam;
			pMsgData->str_buf[0] =L'\0';
			pMsgData->cur_count  =0;

			GetClientRect(hwnd,&rc0);

			rc =rc0;
			InflateRectEx(&rc,-2,-80,-56,-2);
			MakeMatrixRect(m_rc,&rc,1,1,3,4);
			for(i=0;i<NUM_BTN_COUNT;i++)
			{
				CreateWindowEx(0,BUTTON, str_tbl[i],	WS_VISIBLE|WS_BORDER,m_rc[i].x,m_rc[i].y,m_rc[i].w,m_rc[i].h,hwnd,id_tbl[i],hInst,NULL);
			}

			OffsetRect(&rc,rc.w+2,0);
			rc.w =rc0.w-rc.x-2;
			MakeMatrixRect(m_rc,&rc,1,1,1,2);

			i=0;
			CreateWindowEx(0,
                      BUTTON, 
                      L"Exit", 
                      BS_3D|WS_VISIBLE|WS_BORDER,
                      m_rc[i].x,
                      m_rc[i].y,
                      m_rc[i].w,
                      m_rc[i].h,
                      hwnd,
                      eID_EXIT,
                      hInst,
                      NULL);
      
			i=1;
			CreateWindowEx(0,BUTTON,	L"OK",	BS_3D|WS_VISIBLE|WS_BORDER,m_rc[i].x,m_rc[i].y,m_rc[i].w,m_rc[i].h,hwnd,eID_OK,hInst,NULL);

			rc =rc0;
			InflateRect(&rc,-4,0);
			rc.y =40;
			rc.h =32;

			CreateWindowEx(0,TEXTBOX,	pMsgData->str_buf,	TBS_RIGHT|TBS_VCENTER|WS_VISIBLE|WS_BORDER,rc.x,rc.y,rc.w,rc.h,hwnd,eID_TEXTBOX,hInst,NULL);

			//SetWindowFont(GetDlgItem(hwnd,eID_TEXTBOX),hFont_ASCII_24);
			SetFocus(GetDlgItem(hwnd,eID_EXIT));

			break;
			////

		case	WM_NOTIFY: //公共消息(按钮控件有被按下后会产生该消息)
		{
			u16 id;
			id =LOWORD(wParam); //获得产生该消息的控件ID.


			switch(id)
			{
				case	L'0':
				case	L'1':
				case	L'2':
				case	L'3':
				case	L'4':
				case	L'5':
				case	L'6':
				case	L'7':
				case	L'8':
				case	L'9':
				case	L'.':

					if(pMsgData->cur_count < pMsgData->max_count)
					{
						pMsgData->str_buf[pMsgData->cur_count++] =id;
						pMsgData->str_buf[pMsgData->cur_count] =L'\0';
					}
					break;
					////

				case	L'\b':
					if(pMsgData->cur_count > 0)
					{
						pMsgData->cur_count--;
						pMsgData->str_buf[pMsgData->cur_count] =L'\0';
					}
					break;
					////

				default:
					break;

			}
			SetWindowText(GetDlgItem(hwnd,eID_TEXTBOX),pMsgData->str_buf);

			if(id==eID_EXIT)
			{
				pMsgData->cur_count =0;
				SendMessage(hwnd,WM_CLOSE,0,0); //产生关闭窗口的消息.
			}

			if(id==eID_OK)
			{
				SendMessage(hwnd,WM_CLOSE,0,0); //产生关闭窗口的消息.
			}

			InvalidateRect(hwnd,NULL,TRUE);
			return	DefWindowProc(hwnd,msg,wParam,lParam);
		}

		case	WM_ERASEBKGND:
		{
			HDC hdc =(HDC)wParam;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB(hdc,0,30,30));
			FillRect(hdc,&rc);
		}
		return TRUE;
		////

		case	WM_CTLCOLOR:
		{
			u16 id =LOWORD(wParam);
			if(id==eID_TEXTBOX)
			{
				CTLCOLOR *cr=(CTLCOLOR*)lParam;

				cr->TextColor =RGB888(0,0,250);
				cr->BackColor =RGB888(250,250,250);
				cr->BorderColor =RGB888(0,100,250);
				return TRUE;
			}
			return FALSE;
		}

		case	WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;

			hdc =BeginPaint(hwnd,&ps);
			if(hdc!=NULL)
			{
				GetClientRect(hwnd,&rc);

				//SetFont(hdc,hFont_20);

				rc.w =rc.w;
				rc.h =28;
				rc.x =0;
				rc.y =0;
				SetTextColor(hdc,MapRGB(hdc,255,255,255));
				SetBrushColor(hdc,MapRGB(hdc,250,100,0));
//				x_wsprintf(wbuf,L"%s(%d/%d)",pMsgData->pCaption,pMsgData->cur_count,pMsgData->max_count);
				x_wsprintf(wbuf,L"(%d/%d)",pMsgData->cur_count,pMsgData->max_count);

				DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_BKGND);
			}

			EndPaint(hwnd,&ps);
		}
		break;
		////

		case	WM_CLOSE: //响应关闭窗口消息
		{
			DestroyWindow(hwnd); //调用DestroyWindow函数时，才会真正关闭并销毁窗口.
		}
		break;
		/////

		default:
			return	DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;
}


int number_input_box(int x,int y,int w,int h,const WCHAR *pCaption,WCHAR *pOut,int MaxCount,HWND hwndParent)
{
	WNDCLASS wcex;
	num_input_box_data_t mb_data;
  
  if(hwndParent==NULL)
	{
		hwndParent =GetDesktopWindow();
	}


	mb_data.pCaption =pCaption;
	mb_data.cur_count =0;
	mb_data.max_count =MIN(MaxCount,MAX_INPUT);

	wcex.Tag	 		= WNDCLASS_TAG;
	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)number_input_box_win_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	DialogWindow(	&wcex,	pCaption,
//							WS_OVERLAPPED|WS_BORDER|WS_DLGFRAME|WS_CLIPCHILDREN,
                WS_BORDER|WS_CLIPCHILDREN,//

							x,y,w,h,
							hwndParent,0x0000,hInst,(LPVOID)&mb_data);

	if(mb_data.cur_count > 0)
	{
		memcpy(pOut,mb_data.str_buf,sizeof(WCHAR)*(mb_data.cur_count+1));
	}

	return mb_data.cur_count;

}

/*============================================================================*/


