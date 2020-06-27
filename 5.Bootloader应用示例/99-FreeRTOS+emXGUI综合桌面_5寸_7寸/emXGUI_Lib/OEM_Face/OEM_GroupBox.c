
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
#define	GROUPBOX_TEXTCOLOR		XRGB8888(0,0,0)
#define	GROUPBOX_BACKCOLOR		XRGB8888(210,210,210)
#define	GROUPBOX_BODERCOLOR		XRGB8888(70,70,70)
#define	GROUPBOX_FORECOLOR		XRGB8888(150,150,150)


static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{

	cr->TextColor =GROUPBOX_TEXTCOLOR;
	cr->BackColor =GROUPBOX_BACKCOLOR;
	cr->BorderColor =GROUPBOX_BODERCOLOR;
	cr->ForeColor  =GROUPBOX_FORECOLOR;


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

static void _Paint(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{

	HWND hwnd;
	HDC hdc;
	int	i;
	RECT rc;
	POINT pt[6];
	SIZE16 text_size;
	COLORREF c;
	U8 r,g,b;
	////


	hwnd =di->hwnd;
	hdc =di->hDC;

	GetWindowRect(hwnd,&rc);
	rc.x	=0;
	rc.y	=0;
	////

	GetTextExtent(hdc,pText,-1,&text_size);

	text_size.cx =MIN(text_size.cx,rc.w);
	text_size.cy =MIN(text_size.cy,rc.h);

	TextOut(hdc,10,0,pText,-1);

	if(text_size.cx==0)
	{
		i=0;
	}
	else
	{
		i=2;
	}


	pt[0].x	=10-i;
	pt[0].y	=(text_size.cy>>1)+1;

	pt[1].x	=1;
	pt[1].y	=pt[0].y;

	pt[2].x	=pt[1].x;
	pt[2].y	=rc.h-1;

	pt[3].x	=rc.w-1;
	pt[3].y	=pt[2].y;

	pt[4].x	=pt[3].x;
	pt[4].y	=pt[0].y;

	pt[5].x	=10+text_size.cx+i;
	pt[5].y	=pt[4].y;


	c =GetBrushColor(hdc);
	GetRGB(hdc,c,&r,&g,&b);

	c =RGB32_Offset(RGB888(r,g,b),+35,+35,+35);

	SetPenColor(hdc,MapRGB888(hdc,c));
	PolyLine(hdc,0,0,pt,6);

	for(i=0;i<6;i++)
	{
		pt[i].x -=1;
		pt[i].y -=1;
	}

	c =RGB32_Offset(RGB888(r,g,b),-35,-35,-35);
	SetPenColor(hdc,MapRGB888(hdc,c));
	PolyLine(hdc,0,0,pt,6);
}

static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

win_oem_t* GetGroupBoxOEM(void)
{
	return &oem;
}
