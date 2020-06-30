
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
#define	PROGBAR_TEXTCOLOR		  XRGB8888(0,0,0)
#define	PROGBAR_BACKCOLOR		  XRGB8888(220,220,220)
#define	PROGBAR_BODERCOLOR		XRGB8888(30,30,50)
#define	PROGBAR_FORECOLOR		  XRGB8888(0,210,0)

/* 设置颜色 */
static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{

  /* 正在使用的进度条颜色 */
	cr->TextColor   = PROGBAR_TEXTCOLOR;
	cr->BackColor   = PROGBAR_BACKCOLOR;
	cr->BorderColor = PROGBAR_BODERCOLOR;
	cr->ForeColor   = PROGBAR_FORECOLOR;

	if(style&WS_DISABLED)    // 未使用的进度条颜色
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

static void fill_pb_rect(HDC hdc,RECT *prc,COLOR_RGB32 c)
{
	RECT rc;
	COLOR_RGB32 c1,c2;

	rc = *prc;

  /* 一半使用渐变色 */
	rc.h >>= 1;
	c1 =RGB32_Offset(c,+35,+35,+35);
	c2 =RGB32_Offset(c,+10,+10,+10);
	GradientFillRect(hdc,&rc,MapRGB888(hdc,c1),MapRGB888(hdc,c2),TRUE);

  /* 另一半使用一种色 */
	OffsetRect(&rc,0,rc.h);
	rc.h =(prc->y+prc->h) - rc.y;
	c1 =RGB32_Offset(c,-5,-5,-5);
	SetBrushColor(hdc,MapRGB888(hdc,c1));
	FillRect(hdc,&rc);

}

/*============================================================================*/
/* 默认的平面风格 */
static void draw_flat_prog_bar(HDC hdc,HWND hwnd,PROGRESSBAR_CFG *pPB,ePB_ORG align,CTLCOLOR *cr)
{
	RECT m_rc[2],rc;

	GetClientRect(hwnd,&rc);

  /* 画进度条边框 */
	SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
	DrawRect(hdc,&rc);

  /* 计算进度条框边界 */
	InflateRect(&rc,-1,-1);
	MakeProgressRect(m_rc,&rc,pPB->Rangle,pPB->Value,align);

  /* 填充已完成部分 */
	SetBrushColor(hdc,MapRGB888(hdc,cr->ForeColor));
	FillRect(hdc,&m_rc[0]);
  
  /* 填充未完成部分 */
	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&m_rc[1]);

}

/* 默认的3D风格 */
static void draw_3D_prog_bar(HDC hdc,HWND hwnd,PROGRESSBAR_CFG *pPB,ePB_ORG align,CTLCOLOR *cr)
{
	RECT m_rc[2],rc;
	COLORREF c1,c2;

	GetClientRect(hwnd,&rc);

  /* 画进度条边框 */
	SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
	DrawRect(hdc,&rc);

  /* 计算进度条框边界 */
	InflateRect(&rc,-1,-1);
	MakeProgressRect(m_rc,&rc,pPB->Rangle,pPB->Value,align);

	c1 =RGB32_AVG(cr->ForeColor,RGB888(240,240,240));
	c2 =RGB32_AVG(cr->ForeColor,RGB888(20,20,20));

  /* 已完成画具有3D效果的矩形 */
	Fill3DRect(hdc,&m_rc[0],MapRGB888(hdc,c1),MapRGB888(hdc,c2));
  
  /* 填充未完成部分矩形 */
	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&m_rc[1]);

}

/* 默认风格 */
static void draw_def_prog_bar(HDC hdc,HWND hwnd,PROGRESSBAR_CFG *pPB,ePB_ORG align,CTLCOLOR *cr)
{
	RECT m_rc[2],rc;
	COLORREF c1;


	GetClientRect(hwnd,&rc);

  /* 画进度条边框 */
	c1 =RGB32_AVG(cr->BorderColor,cr->BackColor);
	SetPenColor(hdc,MapRGB888(hdc,c1));
	DrawRect(hdc,&rc);

  /* 在画一条边框 */
	InflateRect(&rc,-1,-1);
	c1 =RGB32_Offset(cr->BackColor,+25,+25,+25);
	SetPenColor(hdc,MapRGB888(hdc,c1));
	DrawRect(hdc,&rc);

	//SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
	//DrawRect(hdc,&rc);

  /* 计算进度条框边界 */
	InflateRect(&rc,-1,-1);
	MakeProgressRect(m_rc,&rc,pPB->Rangle,pPB->Value,align);
  
  /* 填充已完成部分 */
	fill_pb_rect(hdc,&m_rc[0],cr->ForeColor);
  
  /* 填充未完成部分 */
	fill_pb_rect(hdc,&m_rc[1],cr->BackColor);
/*
	c1 =RGB32_AVG(cr->ForeColor,RGB888(240,240,240));
	c2 =RGB32_AVG(cr->ForeColor,RGB888(20,20,20));


	GradientFillRect(hdc,&m_rc[0],MapRGB888(hdc,c1),MapRGB888(hdc,c2),TRUE);
	SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
	FillRect(hdc,&m_rc[1]);
*/
}

static void _Paint(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	ePB_ORG align;
	PROGRESSBAR_CFG cfg;
	////

	hwnd =di->hwnd;
	hdc =di->hDC;
	rc =di->rc;

	cfg.cbSize =sizeof(PROGRESSBAR_CFG);
	cfg.fMask  =PB_CFG_ALL;

	SendMessage(hwnd,PBM_GET_CFG,0,(LPARAM)&cfg);
  
   /* 选择进度条方向 */
	switch(di->Style&PBS_ALIGN_MASK)
	{
		case PBS_ALIGN_LEFT:
			align =PB_ORG_LEFT;
			break;
		case PBS_ALIGN_RIGHT:
			align =PB_ORG_RIGHT;
			break;
		case PBS_ALIGN_TOP:
			align =PB_ORG_TOP;
			break;
		case PBS_ALIGN_BOTTOM:
			align =PB_ORG_BOTTOM;
			break;

		default:
			return;
	}

  /* 根据不同风格绘制季度条 */
	switch(di->Style & PBS_FACE_MASK)
	{
		case PBS_FLAT:
			draw_flat_prog_bar(hdc,hwnd,&cfg,align,cr);
			break;

		case PBS_3D:
			draw_3D_prog_bar(hdc,hwnd,&cfg,align,cr);
			break;

		default:
			draw_def_prog_bar(hdc,hwnd,&cfg,align,cr);
			break;
	}

  /* 带文字显示时写文字 */
	if(di->Style & PBS_TEXT)
	{
		GetClientRect(hwnd,&rc);
		SetTextColor(hdc,MapRGB888(hdc,cr->TextColor));
		DrawText(hdc,pText,-1,&rc,cfg.TextFlag);
	}

}


static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

win_oem_t* GetProgressBarOEM(void)
{
	return &oem;
}
