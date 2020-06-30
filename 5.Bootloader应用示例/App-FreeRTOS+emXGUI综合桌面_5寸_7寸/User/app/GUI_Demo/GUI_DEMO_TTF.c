
/*
 *GUI_DEMO_TTF.c
 *2018年5月31日上午11:21:47
 *
 */

#include <emXGUI.h>
#include "FontDrv_Freetype.h"
#include "x_file.h"
#include "x_commdlg.h"
#include "Kernel.h"

/*============================================================================*/
//定义控件ID
#define	ID_OK		0x1000

static u8 *font_data_buf=NULL; //ttf字体文件数据
static freetype_obj *ft_obj=NULL; //freetype

/*============================================================================*/

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			ft_obj =freetype_font_obj_new(16,16,72,72,font_data_buf); //创建一个freetype对象
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
			HFONT hFont;
			RECT rc0;

			GetClientRect(hwnd,&rc0);

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...

			hFont =freetype_CreateFont(ft_obj); //创建freetype字体

			SetFont(hdc,hFont); //设置新的字体.

			SetTextColor(hdc,MapRGB(hdc,10,10,10));

			freetype_font_set_size(ft_obj,16,16,72,72); //设置freetype字体的大小参数
			TextOut(hdc,10,10,L"矢量字体示例1234567890",-1);

			freetype_font_set_size(ft_obj,32,32,72,72);
			TextOut(hdc,10,30,L"矢量字体示例1234567890",-1);

			freetype_font_set_size(ft_obj,64,64,72,72);
			TextOut(hdc,10,80,L"矢量字体示例1234567890",-1);

			freetype_font_set_size(ft_obj,128,128,72,72);
			TextOut(hdc,10,200,L"矢量字体示例1234567890",-1);


			SetTextColor(hdc,MapRGB(hdc,200,50,50));
			freetype_font_set_size(ft_obj,40,40,72,72);

			rc.w =rc0.w;
			rc.h =48;
			rc.x =0;
			rc.y =rc0.h-48;
			DrawText(hdc,L"emXGUI,十年深度优化图像引擎",-1,&rc,DT_VCENTER|DT_RIGHT|DT_BORDER);

			DeleteFont(hFont); //释放字体
			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{
			freetype_font_obj_delete(ft_obj); //释放freetype对象
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

void	GUI_DEMO_TTF(void)
{
	HWND	hwnd;
	WNDCLASSEX	wcex;
	MSG msg;

	/////

	if(1)
	{
		X_FILE *fp;
		int fsize;
		char buf[128],f_path[128];

		CDlg_FileExpoler(NULL,20,20,400,300,L"Open TTF File","B:",f_path); //通过文件对话框选择ttf文件

		fp	=x_fopen(f_path,"rb"); //打开文件
		if(fp==NULL)
		{
			return;
		}

		fsize	=x_fsize(fp);
		font_data_buf =(u8*)vmalloc(fsize);
		if(font_data_buf!=NULL)
		{
			x_fread(font_data_buf,fsize,1,fp); //将整个ttf文件读到内存.
		}
		x_fclose(fp);

		if(font_data_buf==NULL)
		{ //内存不够
			MSGBOX_OPTIONS ops;
			const WCHAR *btn[]={L"确定"};
			int x,y,w,h;

			ops.Flag =MB_BTN_WIDTH(60)|MB_ICONERROR;
			ops.pButtonText =btn;
			ops.ButtonCount =1;

			w =300;
			h =250;
			x =(GUI_XSIZE-w)>>1;
			y =(GUI_YSIZE-h)>>1;
			MessageBox(hwnd,x,y,w,h,L"系统内存不足,\r\n请选择更小的字体文件...",L"消息",&ops);
			return;
		}
	}

	wcex.Tag 		    = WNDCLASSEX_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hIconSm		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS, //窗口不可拖动
								&wcex,
								L"emxGUI矢量字体示例" , //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
								0,0,GUI_XSIZE,GUI_YSIZE,    //窗口位置和大小
								NULL,NULL,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	vfree(font_data_buf);
}

/*============================================================================*/
