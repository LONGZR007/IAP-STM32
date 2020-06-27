
/*
 *GUI_DEMO_Drawbitmap.c
 *2018年3月17日下午7:41:26
 *
 */

#include <emXGUI.h>
#include "emXGUI_JPEG.h"
#include "GUI_AppDef.h"
#include "emxgui_png.h"
//#include "cpuusage.h"

/* 为0时显示GUI的广告，为1时显示开发板的广告 */
u8 slogan_flag = 1;
 /*============================================================================*/
 /*============================================================================*/
const wchar_t string_slogan_gui[] = L" ■ 型号 : 挑战者F429 V2开发板\r\n\r\n"\
L" ■ 官网 : www.embedFire.com\r\n\r\n"\
L" ■ 论坛 : www.firebbs.cn\r\n\r\n"\
L" ■ 淘宝 : firestm32.taobao.com\r\n\r\n"\
L" ■ 微信公众号 : 公众号搜索“野火电子”，\r\n\r\n"\
L"    即可关注";
 
const wchar_t header_slogan_gui[] = L"emXGUI 中国自主嵌入式图形界面";

const wchar_t string_slogan_board[] = L" ■ 型号 : 挑战者F429 V2开发板\r\n\r\n"\
L" ■ 官网 : www.embedFire.com\r\n\r\n"\
L" ■ 论坛 : www.firebbs.cn\r\n\r\n"\
L" ■ 淘宝 : firestm32.taobao.com\r\n\r\n"\
L" ■ 微信公众号 : 公众号搜索“野火电子”，\r\n\r\n"\
L"    即可关注";
const wchar_t header_slogan_board[] = L"野火 @ 嵌入式教育专家·为初学而生";
//extern const unsigned char gImage_0[];
/*============================================================================*/

/* 外部图片 */
extern const char res_slogan[];
/* 外部图片数据大小 */
extern unsigned int res_slogan_size(void);
extern uint8_t Theme_Flag;   // 主题标志

#define GUI_DEMO_PIC             "gui_demo_pic.png"
#define GUI_EXPLAINDESKTOP_PIC   "explain_desktop.jpg"

/*============================================================================*/
/**
  * @brief  emXGUI slogan界面函数，
			通过修改本函数的内容可更改slogan
  * @param  hdc 绘图上下文
  * @param  lprc 要绘制的矩形区域，为NULL时会自动绘制hwnd的客户区
  * @param  hwnd 窗口对象句柄
  * @retval 无
  */
static void CreateSlogan(HDC hdc, const RECT *lprc, HWND hwnd)
{
	RECT rc;
	JPG_DEC *dec;

	const wchar_t *p_header;
	const wchar_t *p_string;

	if (slogan_flag)
	{
		p_header = header_slogan_board;
		p_string = string_slogan_board;
	}
	else
	{
		p_header = header_slogan_gui;
		p_string = string_slogan_gui;
	}

	if (lprc == NULL)
	{
		GetClientRect(hwnd, &rc);
	}
	else
	{
		CopyRect(&rc, lprc);
	}

	/* 背景 */
	GetClientRect(hwnd, &rc);
	rc.y = 0;
	rc.h = 0 + HEAD_INFO_HEIGHT;
	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB(hdc, 82, 85, 82));
	rc.y = rc.y + rc.h;
	rc.h = GUI_YSIZE + 0 - rc.y;
	FillRect(hdc, &rc);

	/* 首栏 */

	SetFont(hdc, GB2312_32_Font);
	SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	GetClientRect(hwnd, &rc);
	rc.h = HEAD_INFO_HEIGHT;

	DrawText(hdc, p_header, -1, &rc, DT_CENTER | DT_VCENTER);	

	GetClientRect(hwnd, &rc);
	rc.y += HEAD_INFO_HEIGHT + 40;

	/* 广告语 */
	SetFont(hdc, defaultFont);
	//      DrawText(hdc, SLOGAN, -1,&rc0,DT_LEFT);       

	DrawText(hdc, p_string, -1, &rc, DT_LEFT);

	SetTextColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
  
	SetTextColor(hdc, MapRGB(hdc, 250,250,250));
	rc.y = GUI_YSIZE - 60;
  rc.x = 180;
  rc.h = 50;
	DrawText(hdc, L"copyright @ 东莞野火电子技术有限公司", -1, &rc, DT_LEFT|DT_VCENTER);

	/* 右侧图片 */
#if 1
  /* 使用资源设备中的文件 */
  {
		BOOL res;
		u8 *pic_buf;
		u32 pic_size;
		PNG_DEC *png_dec;
		BITMAP png_bm;

		//res = FS_Load_Content(GUI_DEMO_PIC, (char**)&pic_buf, &pic_size);    // 资源在 SD 卡
		res = RES_Load_Content(GUI_DEMO_PIC, (char**)&pic_buf, &pic_size);     // 资源在外部 FLASH
		if(res)
		{
			png_dec = PNG_Open(pic_buf);
			PNG_GetBitmap(png_dec, &png_bm);
			DrawBitmap(hdc, 473, 105, &png_bm, NULL);
			PNG_Close(png_dec);
		}
		/* 释放图片内容空间 */
		RES_Release_Content((char **)&pic_buf);
  }
#else
	/* 根据图片数据创建JPG_DEC句柄 */
	dec = JPG_Open(res_slogan, res_slogan_size());

	/* 绘制至内存对象 */
	JPG_Draw(hdc, 480, HEAD_INFO_HEIGHT + 40, dec);

	/* 关闭JPG_DEC句柄 */
	JPG_Close(dec);
  
#endif
}



static LRESULT	WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	static int win_pos = 0;
	static HDC hdc_mem = NULL;
	static HDC hdc_mem_pic = NULL;

	switch (msg)
	{
	case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口
	{
		GetClientRect(hwnd, &rc); //获得窗口的客户区矩形

  	/* 创建内存对象 */
		hdc_mem = CreateMemoryDC(SURF_SCREEN, rc.w, rc.h);
		hdc_mem_pic = CreateMemoryDC(SURF_SCREEN, rc.w, rc.h);

		BOOL res;
    u8 *jpeg_buf;
    u32 jpeg_size;
		JPG_DEC *dec;

    /* 资源设备中加载 */
    res = RES_Load_Content(GUI_EXPLAINDESKTOP_PIC, (char **)&jpeg_buf, &jpeg_size);    /* 使用图片 */
    //res = FS_Load_Content(GUI_EXPLAINDESKTOP_PIC, (char **)&jpeg_buf, &jpeg_size);
    if(res)
    {
      /* 根据图片数据创建JPG_DEC句柄 */
      dec = JPG_Open(jpeg_buf, jpeg_size);

      /* 绘制至内存对象 */
      JPG_Draw(hdc_mem_pic, 0, 0, dec);

      /* 关闭JPG_DEC句柄 */
      JPG_Close(dec);
    }
    
    /* 释放图片内容空间 */
    RES_Release_Content((char **)&jpeg_buf);

		/* 绘制slogan到内存设备 */
		CreateSlogan(hdc_mem, NULL, hwnd);

		SetTimer(hwnd, 1, 5, TMR_START, NULL);

	}
	return TRUE;
	////

	case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
	{
		u16 code;
		//      ,id;

		code = HIWORD(wParam); //获得通知码类型.
//			id   =LOWORD(wParam); //获得产生该消息的控件ID.

		if (code == BN_CLICKED)
		{
			PostCloseMessage(hwnd); //产生WM_CLOSE消息关闭窗口
		}
	}
	break;
	////
	case WM_TIMER:
	{
		/* 定时器消息，上拉或下拉窗口 */
		RECT rc;
		GetWindowRect(hwnd, &rc);
		SetForegroundWindow(hwnd);

		if (win_pos == 1)
		{
			if (rc.y > 0)
			{
				OffsetRect(&rc, 0, -(rc.h >> 3));
				rc.y = MAX(rc.y, 0);
				MoveWindow(hwnd, rc.x, rc.y, rc.w, rc.h, TRUE);
			}
			else
				KillTimer(hwnd, 1);
		}
		else
		{
			if (rc.y < (GUI_YSIZE))
			{
				OffsetRect(&rc, 0, (rc.h >> 3));
				rc.y = MIN(rc.y, (GUI_YSIZE));//
				MoveWindow(hwnd, rc.x, rc.y, rc.w, rc.h, TRUE);
			}
			else
				KillTimer(hwnd, 1);
		}
	}
	//			InvalidateRect(hwnd,&rc,FALSE);
	break;

	case WM_LBUTTONDOWN:
	{
		/* 检测是否触摸到窗口，若是的话使能定时器 下拉窗口*/
		POINT pt;
		RECT rc;

		pt.x = GET_LPARAM_X(lParam);
		pt.y = GET_LPARAM_Y(lParam);

		GetClientRect(hwnd, &rc);

		if (PtInRect(&rc, &pt))
		{
			KillTimer(hwnd, 1);
			SetTimer(hwnd, 1, 2, TMR_START, NULL);
			win_pos = 0;
		}
	}
	break;

	case WM_ERASEBKGND:
	{
		//			HDC hdc=(HDC)wParam;

		//			GetClientRect(hwnd,&rc);
		//      SetBrushColor(hdc,MapRGB(hdc,0,99,166));
		//			FillRect(hdc,&rc);      
	}
	return TRUE;
	/////

	case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
	{
		PAINTSTRUCT ps;
		HDC hdc;
		//			WCHAR wbuf[128];
		RECT rc0;

		hdc = BeginPaint(hwnd, &ps);

		////用户的绘制内容...
		GetClientRect(hwnd, &rc0);
		/* 把内存对象绘制至屏幕 */
		
		if (Theme_Flag == 0)
		{
			BitBlt(hdc, 0, 0, rc0.w, rc0.h, hdc_mem_pic, 0, 0, SRCCOPY);
		}
		else
		{
			BitBlt(hdc, 0, 0, rc0.w, rc0.h, hdc_mem, 0, 0, SRCCOPY);
		}
		EndPaint(hwnd, &ps);
		//////////
	}
	break;
	////

	case WM_DESTROY: //窗口销毁时，会自动产生该消息，在这里做一些资源释放的操作.
	{
		return PostQuitMessage(hwnd); //调用PostQuitMessage，使用主窗口结束并退出消息循环.
	}
	//		break;
			////

		/* 接收到桌面窗口传来的触摸消息*/
	case WM_MSG_FRAME_DOWN:
	{
		/* 重新开启定时器，上拉窗口 */
		KillTimer(hwnd, 1);
		SetTimer(hwnd, 1, 2, TMR_START, NULL);

		win_pos = 1;
	}
	break;

	default:
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_SlideWindow(void *p)
//static void	AppMain(void)
{
	WNDCLASS	wcex;
	MSG msg;
	HWND	hwnd;

	/////
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	hwnd = CreateWindowEx(NULL,
		&wcex,
		L"SlideWindow",
		//								/*WS_MEMSURFACE|*/WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
		/*WS_MEMSURFACE|*/WS_CLIPCHILDREN|WS_OVERLAPPED,

		0, GUI_YSIZE, GUI_XSIZE, GUI_YSIZE,
		GetDesktopWindow(), ID_SLIDE_WINDOW, NULL, NULL);

	//显示主窗口
	ShowWindow(hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
