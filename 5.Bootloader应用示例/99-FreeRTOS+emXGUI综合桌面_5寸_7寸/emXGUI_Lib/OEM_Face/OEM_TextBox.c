
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
#define	TEXTBOX_TEXTCOLOR		XRGB8888(0,0,0)
#define	TEXTBOX_BACKCOLOR		XRGB8888(210,210,210)
#define	TEXTBOX_BODERCOLOR		XRGB8888(130,150,170)
#define	TEXTBOX_FORECOLOR		XRGB8888(150,170,210)


static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{

	cr->TextColor =TEXTBOX_TEXTCOLOR;
	cr->BackColor =TEXTBOX_BACKCOLOR;
	cr->BorderColor =TEXTBOX_BODERCOLOR;
	cr->ForeColor  =TEXTBOX_FORECOLOR;


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
	HDC hdc;
	RECT rc;
	u32 flag;

	////
	hdc =di->hDC;
	rc =di->rc;

	flag =SendMessage(di->hwnd,TBM_GET_TEXTFLAG,0,0);

	if(di->Style & TBS_SUNK)
	{
		COLORREF c1,c2;

		flag &= ~(DT_BORDER);

		c1 =RGB32_AVG(cr->BackColor,RGB888(250,250,250));
		c2 =RGB32_AVG(cr->BackColor,RGB888(20,20,20));

		Draw3DRect(hdc,&rc,MapRGB888(hdc,c2),MapRGB888(hdc,c1));
		InflateRect(&rc,-1,-1);
	}

	SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));

	SetTextColor(hdc,MapRGB888(hdc,cr->TextColor));
	DrawText(hdc,pText,-1,&rc,flag);

}

static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

win_oem_t* GetTextBoxOEM(void)
{
	return &oem;
}
