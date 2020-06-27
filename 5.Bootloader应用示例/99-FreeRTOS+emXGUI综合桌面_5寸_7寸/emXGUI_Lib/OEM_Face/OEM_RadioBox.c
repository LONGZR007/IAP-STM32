
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
////本文件实现"单选框(带BS_RADIO风格的BUTTON控件)"控件的外观绘制.

#define	RADIOBOX_TEXTCOLOR_0	XRGB8888(0,0,0)
#define	RADIOBOX_TEXTCOLOR_1	XRGB8888(0,0,0)

#define	RADIOBOX_BACKCOLOR_0	XRGB8888(250,250,250)
#define	RADIOBOX_BACKCOLOR_1	XRGB8888(230,230,230)

#define	RADIOBOX_BODERCOLOR_0	XRGB8888(30,30,50)
#define	RADIOBOX_BODERCOLOR_1	XRGB8888(70,70,90)

#define	RADIOBOX_FORECOLOR_0	XRGB8888(30,30,50)
#define	RADIOBOX_FORECOLOR_1	XRGB8888(70,70,90)


static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{
	if(state&BST_CHECKED)
	{
		cr->TextColor =RADIOBOX_TEXTCOLOR_1;
		cr->BackColor =RADIOBOX_BACKCOLOR_1;
		cr->BorderColor =RADIOBOX_BODERCOLOR_1;
		cr->ForeColor  =RADIOBOX_FORECOLOR_1;
	}
	else
	{
		cr->TextColor =RADIOBOX_TEXTCOLOR_0;
		cr->BackColor =RADIOBOX_BACKCOLOR_0;
		cr->BorderColor =RADIOBOX_BODERCOLOR_0;
		cr->ForeColor  =RADIOBOX_FORECOLOR_0;
	}

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
	int	box_size,r,cx,cy;
	u32 state;
	RECT rc;

	////
	hdc =di->hDC;
	rc =di->rc;
	state =di->State;

	box_size  =MAX(8,GetFontAveHeight(GetFont(hdc))+4);
	box_size  =MIN(box_size,rc.h);

	r	=(box_size>>1)-1;
	cx	=r+2;
	cy  =r+((rc.h-box_size)>>1);

	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillCircle(hdc,cx,cy,r);


	SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
	EnableAntiAlias(hdc,TRUE);
	DrawCircle(hdc,cx,cy,r);

	EnableAntiAlias(hdc,FALSE);

	InflateRectEx(&rc,-(cx+r+2),0,0,0);

	SetTextColor(hdc,MapRGB888(hdc,cr->TextColor));
	DrawText(hdc,pText,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

	if(state&BST_CHECKED)
	{//如果是“选中”状态，则绘制RadioBox的“选中标识”.
		int i;
		i =((r*3)>>2)-2;
		i =MAX(i,2);

		SetBrushColor(hdc,MapRGB888(hdc,cr->ForeColor));
		SetPenColor(hdc,MapRGB888(hdc,RGB32_AVG(cr->ForeColor,0)));

		FillCircle(hdc,cx,cy,i);

		EnableAntiAlias(hdc,TRUE);
		DrawCircle(hdc,cx,cy,i);
		EnableAntiAlias(hdc,FALSE);

	}
}

static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

win_oem_t* GetRadioBoxOEM(void)
{
	return &oem;
}
