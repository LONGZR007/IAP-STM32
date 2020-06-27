
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
void	*hBmpArrowUp(void);     //“上箭头”的BMP数据。
void	*hBmpArrowDown(void);   //“下箭头”的BMP数据。
void	*hBmpArrowLeft(void);   //“左箭头”的BMP数据。
void	*hBmpArrowRight(void);  //“右箭头”的BMP数据。

/*=========================================================================================*/

#define	SCROLLBAR_TEXTCOLOR		XRGB8888(0,0,0)
#define	SCROLLBAR_BACKCOLOR		XRGB8888(200,205,220)
#define	SCROLLBAR_BODERCOLOR	XRGB8888(120,130,140)
#define	SCROLLBAR_FORECOLOR		XRGB8888(100,150,240)


static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{

	cr->TextColor =SCROLLBAR_TEXTCOLOR;
	cr->BackColor =SCROLLBAR_BACKCOLOR;
	cr->BorderColor =SCROLLBAR_BODERCOLOR;
	cr->ForeColor  =SCROLLBAR_FORECOLOR;


	if(style&WS_DISABLED)
	{
		cr->TextColor =RGB32_Offset(cr->TextColor,-10,-10,-10);
		cr->BackColor =RGB32_Offset(cr->BackColor,-10,-10,-10);
		cr->BorderColor =RGB32_Offset(cr->BorderColor,-10,-10,-10);
		cr->ForeColor =RGB32_Offset(cr->ForeColor,-10,-10,-10);

		cr->TextColor =RGB32_AVG(cr->TextColor,RGB888(120,120,128));
		cr->BackColor =RGB32_AVG(cr->BackColor,RGB888(120,120,128));
		cr->BorderColor =RGB32_AVG(cr->BorderColor,RGB888(120,120,128));
		cr->ForeColor =RGB32_AVG(cr->ForeColor,RGB888(120,120,128));

	}

}

static void draw_btn(HDC hdc,CONST RECT *prc,BOOL pushed,CTLCOLOR *cr)
{//绘制箭头(按钮)背景.

	RECT rc =*prc;
	COLOR_RGB32 c;

	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&rc);

	if(pushed)
	{

		c =RGB32_AVG(cr->BackColor,cr->ForeColor);
		SetBrushColor(hdc,MapRGB888(hdc,c));
		FillRoundRect(hdc,&rc,4);

		SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
		DrawRoundRect(hdc,&rc,4);
	}
	else
	{
		SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
		FillRoundRect(hdc,&rc,3);

		SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
		DrawRoundRect(hdc,&rc,3);
	}

}

static void draw_page(HDC hdc,const RECT *prc,BOOL act,CTLCOLOR *cr)
{//绘制滑杆区域.

	COLOR_RGB32 c;

	////DrawPage
	if(act)
	{
		c =RGB32_AVG(cr->BackColor,cr->ForeColor);
		SetBrushColor(hdc,MapRGB888(hdc,c));
		FillRect(hdc,prc);
	}
	else
	{
		c =cr->BackColor;
		SetBrushColor(hdc,MapRGB888(hdc,c));
		FillRect(hdc,prc);
	}

}

static void draw_track(HDC hdc,const RECT *prc,BOOL act,BOOL is_vert,CTLCOLOR *cr)
{//绘制滑块区域.

	RECT rc;
	COLOR_RGB32 c;

	rc =*prc;

	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&rc);


	if(act)
	{
		c =cr->ForeColor;
	}
	else
	{
		c =RGB32_Offset(cr->BorderColor,-20,-20,-20);

	}
	//SetBrushColor(hdc,MapRGB888(hdc,c));
	//FillRoundRect(hdc,&rc,MIN(rc.w>>1,rc.h>>1));


	EnableAntiAlias(hdc,TRUE);
	InflateRect(&rc,-1,-1);
	//c=RGB32_AVG(c,RGB888(10,20,30));
	SetPenColor(hdc,MapRGB888(hdc,c));
	DrawRoundRect(hdc,&rc,MIN(rc.w>>1,rc.h>>1));
	EnableAntiAlias(hdc,FALSE);


	if(act)
	{
		c =RGB32_AVG(cr->BorderColor,cr->ForeColor);
	}
	else
	{
		c =cr->BorderColor;
	}
	SetPenColor(hdc,MapRGB888(hdc,c));

	if(is_vert)
	{
		int x,y,end;

		if(rc.w >= 8 && rc.h >= 8)
		{
			x =rc.x+3;
			y =rc.y+(rc.h>>1);
			end =x+(rc.w-3*2)-1;

			HLine(hdc,x,y-3,end);
			HLine(hdc,x,y-0,end);
			HLine(hdc,x,y+3,end);
		}
	}
	else
	{
		int x,y,end;

		if(rc.w >= 8 && rc.h >= 8)
		{
			x =rc.x+(rc.w>>1);
			y =rc.y+3;
			end =y+(rc.h-3*2)-1;

			VLine(hdc,x-3,y,end);
			VLine(hdc,x+0,y,end);
			VLine(hdc,x+3,y,end);
		}

	}

}


////

static	void	vscroll_paint(DRAWITEM_HDR *di,CTLCOLOR *cr)
{//垂直风格ScrollBar的绘制.

	RECT rc;
	SCROLLBAR_RECT rcSL;

	HWND hwnd =di->hwnd;
	HDC hdc =di->hDC;
	u32 State =di->State;


	//获取ScrollBar的相关矩形参数.
	SendMessage(hwnd,SBM_GETRECT,0,(LPARAM)&rcSL);

	//按状态绘制上部“滑杆”.
	if(State&SST_PAGEUP)
	{//该部分是被“点击”的状况.
		draw_page(hdc,&rcSL.V.TopPage,TRUE,cr);
	}
	else
	{
		draw_page(hdc,&rcSL.V.TopPage,FALSE,cr);
	}

	//按状态绘制下部“滑杆”.
	if(State&SST_PAGEDOWN)
	{//该部分是被“点击”的状况.
		draw_page(hdc,&rcSL.V.BottomPage,TRUE,cr);
	}
	else
	{
		draw_page(hdc,&rcSL.V.BottomPage,FALSE,cr);
	}

	//判断是否有“箭头”风格,有则需要绘制.
	if(!(di->Style&SBS_NOARROWS))
	{
		int x,y;
		BITMAPINFO info;

		//绘制上“箭头”背景.
		rc =rcSL.V.TopArrow;
		if(State&SST_LINEUP)
		{//该部分是被“点击”的状况.
			draw_btn(hdc,&rc,TRUE,cr);
		}
		else
		{
			draw_btn(hdc,&rc,FALSE,cr);
		}

		//绘制上“箭头”前景图像(用户可以自行用位图或直接绘制实现...)
		BMP_GetInfo(&info,hBmpArrowUp());
		x =rc.x+(int)(rc.w-info.Width)/2;
		y =rc.y+(int)(rc.h-info.Height)/2;
		BMP_Draw(hdc,x,y,(void*)hBmpArrowUp(),NULL);
		////

		//绘制下“箭头”背景.
		rc =rcSL.V.BottomArrow;
		if(State&SST_LINEDOWN)
		{//该部分是被“点击”的状况.
			draw_btn(hdc,&rc,TRUE,cr);
		}
		else
		{
			draw_btn(hdc,&rc,FALSE,cr);
		}

		//绘制下“箭头”前景图像(用户可以自行用位图或直接绘制实现...)
		BMP_GetInfo(&info,hBmpArrowDown());
		x =rc.x+(int)(rc.w-info.Width)/2;
		y =rc.y+(int)(rc.h-info.Height)/2;
		BMP_Draw(hdc,x,y,(void*)hBmpArrowDown(),NULL);
	}

	//绘制滑块.
	if(State&SST_THUMBTRACK)
	{//绘制滑动状态的滑块.
		draw_track(hdc,&rcSL.V.Track,TRUE,TRUE,cr);
	}
	else
	{//绘制非滑动状态的滑块.
		draw_track(hdc,&rcSL.V.Track,FALSE,TRUE,cr);
	}

}


static	void	hscroll_paint(DRAWITEM_HDR *di,CTLCOLOR *cr)
{//水平风格ScrollBar的绘制.

	RECT rc;
	SCROLLBAR_RECT rcSL;

	HWND hwnd =di->hwnd;
	HDC hdc =di->hDC;
	u32 State =di->State;

	SendMessage(hwnd,SBM_GETRECT,0,(LPARAM)&rcSL);

	////DrawPage
	if(State&SST_PAGELEFT)
	{
		draw_page(hdc,&rcSL.H.LeftPage,TRUE,cr);
	}
	else
	{
		draw_page(hdc,&rcSL.H.LeftPage,FALSE,cr);
	}

	////
	if(State&SST_PAGERIGHT)
	{
		draw_page(hdc,&rcSL.H.RightPage,TRUE,cr);
	}
	else
	{
		draw_page(hdc,&rcSL.H.RightPage,FALSE,cr);
	}

	////DrawArrow
	if(!(di->Style&SBS_NOARROWS))
	{
		int x,y;
		BITMAPINFO info;

		rc =rcSL.H.LeftArrow;
		if(State&SST_LINELEFT)
		{
			draw_btn(hdc,&rc,TRUE,cr);
		}
		else
		{
			draw_btn(hdc,&rc,FALSE,cr);
		}

		BMP_GetInfo(&info,hBmpArrowLeft());
		x =rc.x+(int)(rc.w-info.Width)/2;
		y =rc.y+(int)(rc.h-info.Height)/2;
		BMP_Draw(hdc,x,y,(void*)hBmpArrowLeft(),NULL);
		////

		rc =rcSL.H.RightArrow;
		if(State&SST_LINERIGHT)
		{
			draw_btn(hdc,&rc,TRUE,cr);
		}
		else
		{
			draw_btn(hdc,&rc,FALSE,cr);
		}

		BMP_GetInfo(&info,hBmpArrowRight());
		x =rc.x+(int)(rc.w-info.Width)/2;
		y =rc.y+(int)(rc.h-info.Height)/2;
		BMP_Draw(hdc,x,y,(void*)hBmpArrowRight(),NULL);
	}

	////
	if(State&SST_THUMBTRACK)
	{
		draw_track(hdc,&rcSL.H.Track,TRUE,FALSE,cr);
	}
	else
	{
		draw_track(hdc,&rcSL.H.Track,FALSE,FALSE,cr);
	}

}


static void _Paint(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{
	if(di->Style & SBS_VERT)
	{//垂直风格的.
		vscroll_paint(di,cr);
	}
	else
	{//水平风格的.
		hscroll_paint(di,cr);
	}
}

static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

win_oem_t* GetScrollBarOEM(void)
{
	return &oem;
}
