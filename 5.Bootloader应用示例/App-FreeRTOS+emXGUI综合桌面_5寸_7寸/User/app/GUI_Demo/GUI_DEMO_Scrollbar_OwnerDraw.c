


#include "emXGUI.h"

#include "emxgui_png.h"

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

/*============================================================================*/
static char file_path[256];

static void back_init(HDC hdc,int w,int h,int cell_size)
{
	RECT rc;
	int x,y,i,yy;

	rc.w =cell_size;
	rc.h =cell_size;

	yy=0;
//	for(y=0;y<h;y+=rc.h)
//	{
//		i=yy&1;
//		for(x=0;x<w;x+=rc.w)
//		{
//			if(i&1)
//			{
//				SetBrushColor(hdc,MapRGB(hdc,150,150,180));
//			}
//			else
//			{
//				SetBrushColor(hdc,MapRGB(hdc,100,100,150));
//			}

//			rc.x =x;
//			rc.y =y;
//			FillRect(hdc,&rc);
//			i++;
//		}
//		yy++;
//	}

}

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
			RECT rc;
			WCHAR wbuf[128];

			GetClientRect(hwnd,&rc);
			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			back_init(hdc,rc.w,rc.h,60);

			if(1)
			{

				BOOL res;
      u8 *pic_buf;
      u32 pic_size;
      PNG_DEC *png_dec;
      BITMAP png_bm;
        int t1,t0;

      /* 创建电位器提示 HDC */

      res = RES_Load_Content("F429_RP.png", (char**)&pic_buf, &pic_size);

				if(pic_buf!=NULL)
				{
					PNG_DEC *dec;
					U16 w,h;

					t0 =GUI_GetTickCount();

					dec =PNG_Open(pic_buf);
					if(dec)
					{
						BITMAP bm;
						PNG_GetBitmap(dec,&bm);
						DrawBitmap(hdc,(rc.w-(int)bm.Width)/2,(rc.h-(int)bm.Height)/2,&bm,NULL);
						PNG_Close(dec);
					}

					t1 =GUI_GetTickCount();

					/* 释放图片内容空间 */
      RES_Release_Content((char **)&pic_buf);
					////
					x_wsprintf(wbuf,L"PNG Time: %dms",t1-t0);
					SetTextColor(hdc,MapRGB(hdc,200,10,10));
					TextOut(hdc,10,20,wbuf,-1);
				}
			}

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_PNG(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////
//	if(!CDlg_FileExpoler(NULL,20,30,300,300,L"Open PNG File","B:",file_path))
//	{
//		return;
//	}

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	WS_EX_FRAMEBUFFER,
								&wcex,
								_T("emXGUI DEMO: PNG Decode"), //窗口名称
								WS_OVERLAPPEDWINDOW,
								0,0,GUI_XSIZE,GUI_YSIZE,    //窗口位置和大小
								NULL,0,NULL,NULL);

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
