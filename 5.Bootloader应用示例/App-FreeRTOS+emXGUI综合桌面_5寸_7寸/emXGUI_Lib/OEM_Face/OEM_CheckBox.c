
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
////本文件实现"复选框(带BS_CHECKBOX风格的BUTTON控件)"控件的外观绘制.

/* 文本颜色 */
#define	CHECKBOX_TEXTCOLOR_0	XRGB8888(0,0,0)
#define	CHECKBOX_TEXTCOLOR_1	XRGB8888(0,0,0)

/* 背景颜色 */
#define	CHECKBOX_BACKCOLOR_0	XRGB8888(250,250,250)
#define	CHECKBOX_BACKCOLOR_1	XRGB8888(200,210,250)

/* 边框颜色 */
#define	CHECKBOX_BODERCOLOR_0	XRGB8888(10,30,50)
#define	CHECKBOX_BODERCOLOR_1	XRGB8888(20,50,90)

#define	CHECKBOX_FORECOLOR_0	XRGB8888(120,130,190)
#define	CHECKBOX_FORECOLOR_1	XRGB8888(120,130,190)


static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{
	if(state&BST_CHECKED)     // 选中状态的颜色
	{
		cr->TextColor =CHECKBOX_TEXTCOLOR_1;
		cr->BackColor =CHECKBOX_BACKCOLOR_1;
		cr->BorderColor =CHECKBOX_BODERCOLOR_1;
		cr->ForeColor  =CHECKBOX_FORECOLOR_1;
	}
	else                      // 未选中状态的颜色
	{
		cr->TextColor =CHECKBOX_TEXTCOLOR_0;
		cr->BackColor =CHECKBOX_BACKCOLOR_0;
		cr->BorderColor =CHECKBOX_BODERCOLOR_0;
		cr->ForeColor  =CHECKBOX_FORECOLOR_0;
	}

	if(style&WS_DISABLED)     // 创建但未使用的颜色
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
{//绘制CheckBox的外观.

	HDC hdc;
	int box_size;
	RECT rc,box_rc;
	COLOR_RGB32 c;

	hdc =di->hDC;
	rc =di->rc;
	box_rc =rc;

	InflateRect(&box_rc,-2,-2);
	box_rc.w =box_rc.h;
	box_size =box_rc.h;

	SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
	DrawRect(hdc,&box_rc);

	InflateRect(&box_rc,-1,-1);
	c =RGB32_Offset(cr->BackColor,+55,+55,+55);
	SetPenColor(hdc,MapRGB888(hdc,c));
	DrawRect(hdc,&box_rc);

	InflateRect(&box_rc,-1,-1);
	c=RGB32_AVG(cr->ForeColor,cr->BackColor);
	SetPenColor(hdc,MapRGB888(hdc,c));
	DrawRect(hdc,&box_rc);


	InflateRect(&box_rc,-1,-1);
	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&box_rc);

	SetTextColor(hdc,MapXRGB8888(hdc,cr->TextColor));
	InflateRectEx(&rc,-(box_rc.x+box_rc.w+2),0,0,0);
	DrawText(hdc,pText,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_LEFT);

	if(di->State&BST_CHECKED)
	{ //如果是“选中”状态，则绘制CheckBox的“选中标识”.

		InflateRect(&box_rc,-(box_size>>3),-(box_size>>3));

		SetBrushColor(hdc,MapXRGB8888(hdc,cr->ForeColor));
		FillRoundRect(hdc,&box_rc,10);

		//c=AlphaBlendColor(cr.ForeColor,RGB888(0,0,0),160);
		c=RGB32_AVG(cr->ForeColor,cr->BorderColor);
		c=RGB32_Offset(c,-20,-20,-20);
		SetPenColor(hdc,MapXRGB8888(hdc,c));
		DrawRoundRect(hdc,&box_rc,10);

	}
}

static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

win_oem_t* GetCheckBoxOEM(void)
{
	return &oem;
}

