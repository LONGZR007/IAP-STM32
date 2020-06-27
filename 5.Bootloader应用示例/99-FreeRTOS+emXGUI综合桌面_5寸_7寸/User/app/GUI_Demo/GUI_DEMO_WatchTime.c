
/*
 *GUI_DEMO_WatchTime.c
 *2018年6月1日上午11:38:53
 *
 */


#include <math.h>

#include "emXGUI.h"
#include "x_libc.h"
#include "XFT_res.h"

#include "Kernel.h"

/*============================================================================*/
//定义控件ID

#define	ID_OK		0x1000
#define	ID_PT_TYPE	0x1001
#define	ID_TMR		0x1002

/*============================================================================*/

static local_time_t tm;
static HFONT hFont_SEG_NUM=NULL;
static int pt_type=0;

HFONT	XFT_CreateFont(const void *xft_dat);

/*============================================================================*/
//画表盘的背景
static void	X_MeterBackgnd(HDC hdc,int cx,int cy,int r,u32 color)
{
	int x,y,a;

	SetBrushColor(hdc,color);

	for(a=0;a<60;a++)
	{

		x	= (int)(cx + sin(a*6*3.14/180)*r);
		y	= (int)(cy - cos(a*6*3.14/180)*r);

		if((a%15)==0)
		{
			FillCircle(hdc,x,y,2);
		}
		else if((a%5)==0)
		{
			FillCircle(hdc,x,y,1);
		}
		else
		{
			SetPixel(hdc,x,y,color);
		}

	}

}

//画表盘的指针
static void	X_MeterPointer(HDC hdc,int cx,int cy,int r,u32 color,int st_angle,int angle_size,int dat_size,int dat_val,int style)
{
	int angle,i,j;
	POINT pt[8];

	////

	if(angle_size>360)	angle_size=360;

	angle = st_angle+(dat_val*angle_size)/dat_size;
	angle = MIN(angle,st_angle+angle_size);

	angle -= 90;


	////
#if 0	//画表盘背景
	SetBrushColor(hdc,bk_color);
	FillRect(hdc,&rc);

	SetPenColor(hdc,bd_color);
	DrawCircle(hdc,cx,cy,r);
	r-=8;
	DrawArc(hdc,cx,cy,r,st_angle,st_angle+angle_size);

	j=angle_size/10;
	for(i=(st_angle-90);i<=(st_angle-90)+angle_size;i+=j)
	{
		int x0,y0,x1,y1;

		x0	= (int)(cx + sin(i*3.14/180)*r);
		y0	= (int)(cy - cos(i*3.14/180)*r);

		x1	= (int)(cx + sin(i*3.14/180)*(r-8));
		y1	= (int)(cy - cos(i*3.14/180)*(r-8));

		DrawLine(hdc,x0,y0,x1,y1);
	}
#endif
	////
	//????r =r-(r>>5);

if(style==0)//表针风格-1
{
	pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
	pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

	pt[1].x = (int)(cx + sin((angle+90)*3.14/180)*(r>>5));
	pt[1].y = (int)(cy - cos((angle+90)*3.14/180)*(r>>5));

	pt[2].x = (int)(cx + sin((angle+180)*3.14/180)*(r>>5));
	pt[2].y = (int)(cy - cos((angle+180)*3.14/180)*(r>>5));

	pt[3].x = (int)(cx + sin((angle+270)*3.14/180)*(r>>5));
	pt[3].y = (int)(cy - cos((angle+270)*3.14/180)*(r>>5));

	pt[4].x = pt[0].x;
	pt[4].y = pt[0].y;


	SetBrushColor(hdc,color);
	AA_FillPolygon(hdc,0,0,pt,5);

	//SetPenColor(hdc,color);
	//AA_DrawPolygon(hdc,0,0,pt,5); //抗锯齿的多边形绘制
}


if(style==1) //表针风格-2
{
	pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
	pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

	pt[1].x = (int)(cx + sin((angle+4)*3.14/180)*(r-(r>>2)));
	pt[1].y = (int)(cy - cos((angle+4)*3.14/180)*(r-(r>>2)));

	pt[2].x = cx;
	pt[2].y = cy;

	pt[3].x = (int)(cx + sin((angle-4)*3.14/180)*(r-(r>>2)));
	pt[3].y = (int)(cy - cos((angle-4)*3.14/180)*(r-(r>>2)));

	pt[4].x = pt[0].x;
	pt[4].y = pt[0].y;


	AA_FillPolygon(hdc,0,0,pt,5); //抗锯齿的多边形填充

}


if(style==2) //表针风格-3:箭头样式
{
	POINT pt[7];

	pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
	pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

	pt[1].x = (int)(cx + sin((angle+6)*3.14/180)*(r-(r>>2)));
	pt[1].y = (int)(cy - cos((angle+6)*3.14/180)*(r-(r>>2)));

	pt[2].x = (int)(cx + sin((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));
	pt[2].y = (int)(cy - cos((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));

	pt[3].x = (int)cx;
	pt[3].y = (int)cy;

	pt[4].x = (int)(cx + sin((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));
	pt[4].y = (int)(cy - cos((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));

	pt[5].x = (int)(cx + sin((angle-6)*3.14/180)*(r-(r>>2)));
	pt[5].y = (int)(cy - cos((angle-6)*3.14/180)*(r-(r>>2)));

	pt[6].x = pt[0].x;
	pt[6].y = pt[0].y;

	AA_FillPolygon(hdc,0,0,pt,7);

}

if(style==3) //表针风格-4:矩形样式
{
	POINT pt[7];
	int x1,y1;

	x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
	y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

	pt[0].x	 = (int)(x1 + sin((angle-90)*3.14/180)*(2));
	pt[0].y	 = (int)(y1 - cos((angle-90)*3.14/180)*(2));

	pt[1].x	 = x1;
	pt[1].y	 = y1;

	pt[2].x	 = (int)(x1 + sin((angle+90)*3.14/180)*(2));
	pt[2].y	 = (int)(y1 - cos((angle+90)*3.14/180)*(2));


	pt[3].x	 = (int)(cx + sin((angle+90)*3.14/180)*(2));
	pt[3].y	 = (int)(cy - cos((angle+90)*3.14/180)*(2));

	pt[4].x	 = cx;
	pt[4].y	 = cy;

	pt[5].x	 = (int)(cx + sin((angle-90)*3.14/180)*(2));
	pt[5].y	 = (int)(cy - cos((angle-90)*3.14/180)*(2));

	pt[6].x = pt[0].x;
	pt[6].y = pt[0].y;

	AA_FillPolygon(hdc,0,0,pt,7);

}

if(style==4) //表针风格-5:条形+尖针样式
{
	POINT pt[7];
	int x1,y1;

	x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
	y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

	pt[0].x	 = (int)(cx + sin((angle-60)*3.14/180)*(5));
	pt[0].y	 = (int)(cy - cos((angle-60)*3.14/180)*(5));

	pt[1].x	 = cx;
	pt[1].y	 = cy;

	pt[2].x	 = (int)(cx + sin((angle+60)*3.14/180)*(5));
	pt[2].y	 = (int)(cy - cos((angle+60)*3.14/180)*(5));


	pt[3].x	 = (int)(pt[2].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
	pt[3].y	 = (int)(pt[2].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

	pt[4].x	 = x1;
	pt[4].y	 = y1;

	pt[5].x	 = (int)(pt[0].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
	pt[5].y	 = (int)(pt[0].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

	pt[6].x = pt[0].x;
	pt[6].y = pt[0].y;

	AA_FillPolygon(hdc,0,0,pt,7);

}

if(style==5) //表针风格-6:菱形箭头样式1
{
	POINT pt[5];
	int x1,y1;

	x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
	y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

	pt[0].x	 = x1;
	pt[0].y	 = y1;

	pt[1].x = (int)(cx + sin((angle+12)*3.14/180)*(r-(r>>1)));
	pt[1].y = (int)(cy - cos((angle+12)*3.14/180)*(r-(r>>1)));

	pt[2].x = (int)cx;
	pt[2].y = (int)cy;

	pt[3].x = (int)(cx + sin((angle-12)*3.14/180)*(r-(r>>1)));
	pt[3].y = (int)(cy - cos((angle-12)*3.14/180)*(r-(r>>1)));

	pt[4].x = pt[0].x;
	pt[4].y = pt[0].y;

	AA_FillPolygon(hdc,0,0,pt,5);

}

if(style==6) //表针风格-7:菱形箭头样式2
{
	POINT pt[5];
	int x1,y1;

	x1 = (int)(cx + sin((angle-0)*3.14/180)*((r*15)>>4));
	y1 = (int)(cy - cos((angle-0)*3.14/180)*((r*15)>>4));

	pt[0].x	 = x1;
	pt[0].y	 = y1;

	pt[1].x = (int)(cx + sin((angle+15)*3.14/180)*((r*4)>>4));
	pt[1].y = (int)(cy - cos((angle+15)*3.14/180)*((r*4)>>4));

	pt[2].x = (int)cx;
	pt[2].y = (int)cy;

	pt[3].x = (int)(cx + sin((angle-15)*3.14/180)*((r*4)>>4));
	pt[3].y = (int)(cy - cos((angle-15)*3.14/180)*((r*4)>>4));

	pt[4].x = pt[0].x;
	pt[4].y = pt[0].y;

	AA_FillPolygon(hdc,0,0,pt,5);

}

	////
#if 0	//画表针中心装饰
	SetBrushColor(hdc,fr_color);
	FillCircle(hdc,cx,cy,r>>3);
#endif

}

static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc0,rc;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			GetClientRect(hwnd,&rc); //获得窗口的客户区矩形.

			hFont_SEG_NUM =XFT_CreateFont(SEG_NUM_64); //创建64x64的数码管外观的字体(xft字体)


			CreateWindow(BUTTON,L"OK",  WS_VISIBLE,rc.w-80,8,68,32,hwnd,ID_OK,NULL,NULL); //创建一个按钮.
			CreateWindow(BUTTON,L">>", WS_VISIBLE,rc.w-80,60,68,32,hwnd,ID_PT_TYPE,NULL,NULL); //创建一个按钮.

			SetTimer(hwnd,ID_TMR,100,TMR_START,NULL); //创建一个100ms的定时器.

			SYS_get_local_time(&tm); //获得当前时间
		}
		return TRUE;
		////

		case	WM_TIMER: //定时器消息,当窗口定时器定时时间到来时，产生该消息,wParam参数指明当前产生该消息的定时器ID.
		{

			if(wParam == ID_TMR)
			{
				local_time_t tm0;
				SYS_get_local_time(&tm0);
				if(tm.sec != tm0.sec)
				{
					tm =tm0; //RTC秒钟变化了
					InvalidateRect(hwnd,NULL,FALSE);
				}
			}
		}
		break;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id==ID_PT_TYPE && code==BN_CLICKED) // 按钮“单击”了.
			{
				pt_type++;
				if(pt_type > 6)
				{
					pt_type=0;
				}
				InvalidateRect(hwnd,NULL,FALSE);
			}

			if(id==ID_OK && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			HDC hdc,hdc_mem;
			PAINTSTRUCT ps;
			int r;
			RECT rc0;
			WCHAR wbuf[64];


			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			////
			rc.x =8;
			rc.y =32;
			rc.w =320;
			rc.h =320;

			r =MIN(rc.w,rc.h)>>1;

			hdc_mem =CreateMemoryDC(BM_DEVICE,rc.w,rc.h); //创建一个与屏幕颜色兼容的"内存型绘图设备",用于绘制时钟内容,可以提升绘图性能并且可以避免画面闪烁.

			/////绘制时钟
			ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,230,230,230));


			X_MeterBackgnd(hdc_mem,rc.w>>1,rc.h>>1,r-5,MapRGB(hdc_mem,10,20,30));

			X_MeterPointer(hdc_mem,rc.w>>1,rc.h>>1,r-((r*5)>>4),MapRGB(hdc_mem,20,20,20),90,360,360,(tm.hour%12)*30+(tm.min*30)/60,pt_type);
			X_MeterPointer(hdc_mem,rc.w>>1,rc.h>>1,r-((r*3)>>4),MapRGB(hdc_mem,20,20,250),90,360,60,tm.min,pt_type);
			X_MeterPointer(hdc_mem,rc.w>>1,rc.h>>1,r-((r*1)>>4),MapRGB(hdc_mem,250,20,20),90,360,60,tm.sec,pt_type);

			BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_mem,0,0,SRCCOPY); //将内存绘制设备的内存输出到窗口.
			DeleteDC(hdc_mem); //删除绘图设备.

			////
			OffsetRect(&rc,0,rc.h+8);
			rc.w =320;
			rc.h =72;

			r =MIN(rc.w,rc.h)>>1;

			hdc_mem =CreateMemoryDC(BM_DEVICE,rc.w,rc.h);

			/////显示时间字符
			ClrDisplay(hdc_mem,NULL,MapRGB(hdc_mem,10,40,80));

			SetTextColor(hdc_mem,MapRGB(hdc_mem,10,200,250));
			//SetTextColor(hdc_mem,MapRGB888(hdc_mem,time_c[time_c_idx]));
			SetFont(hdc_mem,hFont_SEG_NUM);
			x_wsprintf(wbuf,L"%d:%02d:%02d",tm.hour,tm.min,tm.sec);
			rc0.x=0;
			rc0.y=0;
			rc0.w=rc.w;
			rc0.h=rc.h;
			DrawText(hdc_mem,wbuf,-1,&rc0,DT_SINGLELINE|DT_VCENTER|DT_CENTER);

			BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_mem,0,0,SRCCOPY);
			DeleteDC(hdc_mem);

			EndPaint(hwnd,&ps); //结束绘图
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			DeleteFont(hFont_SEG_NUM); //删除字体.
			return DestroyWindow(hwnd);   //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
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

void	GUI_DEMO_WatchTime(void)
{
	HWND	hwnd;
	WNDCLASSEX	wcex;
	MSG msg;

	/////

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
								_T("emXGUI Demo - WatchTime"), //窗口名称
								WS_CAPTION|WS_DLGFRAME|WS_BORDER|WS_CLIPCHILDREN,
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
