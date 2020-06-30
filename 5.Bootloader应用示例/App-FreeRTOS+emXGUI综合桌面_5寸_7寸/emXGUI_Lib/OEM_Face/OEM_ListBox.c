
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/
////本文件实现"列表框"控件的外观绘制.

#define	LISTBOX_TEXTCOLOR		XRGB8888(0,10,20)
#define	LISTBOX_BACKCOLOR		XRGB8888(232,235,243)
#define	LISTBOX_BODERCOLOR		XRGB8888(200,210,240)
#define	LISTBOX_FORECOLOR		XRGB8888(170,190,250)


static void _SetColor(HWND hwnd,CTLCOLOR *cr,u32 style,u32 state)
{
	cr->TextColor =LISTBOX_TEXTCOLOR;
	cr->BackColor =LISTBOX_BACKCOLOR;
	cr->BorderColor =LISTBOX_BODERCOLOR;
	cr->ForeColor  =LISTBOX_FORECOLOR;

#if 0 //Listbox是否要处理“禁止”风格的颜色?　还是由用户可自行决定...
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
#endif
}

static void _Paint(DRAWITEM_HDR *di,CTLCOLOR *cr,const WCHAR *pText)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];

	hwnd =di->hwnd;
	hdc =di->hDC;
	rc =di->rc;

	//SetBrushColor(hdc,MapRGB888(hdc,RGB32_AVG(cr->BackColor,RGB888(0,0,0)))); //用单色填充整个listbox背景.
	//FillRect(hdc,&ds->rc);

	////绘制顶部背景.
	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);    //获得第一个可见Item项索引。
	SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc); //获得指定Item项的矩形位置参数.
	if(rc.y > 0)
	{
		COLORREF c1,c2;

		rc.h = rc.y;
		rc.y =0;

		c1 =MapRGB888(hdc,RGB32_AVG(cr->BackColor,RGB888(0,0,0)));
		c2 =MapRGB888(hdc,RGB32_AVG(cr->BackColor,RGB888(255,255,255)));
		GradientFillRect(hdc,&rc,c1,c2,TRUE);
	}


	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);    //获得第一个可见Item项索引。
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);   //获得总的Item项总数。
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0); //获得当前选中的Item项索引。
	while(i < count) //循环绘制.
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc); //获得指定Item项的矩形位置参数.
		if(rc.y > di->rc.h) //如果是超出listbox客户区的Item项不绘制了，直接完成退出.
		{
			break;
		}

		if(di->Style&LBS_LINE)
		{ //需要绘制分行线.
			COLORREF c1,c2;

			c1 =RGB32_AVG(cr->ForeColor,cr->BorderColor);
			c2 =RGB32_AVG(cr->BackColor,cr->TextColor);

			SetPenColor(hdc,MapRGB888(hdc,RGB32_AVG(c1,c2)));
			HLine(hdc,rc.x,rc.y+rc.h-1,rc.x+rc.w-1);

			InflateRectEx(&rc,0,0,0,-1);
		}

		if(i==cursel)
		{ // 绘制当前选中的Item项。


			SetBrushColor(hdc,MapRGB888(hdc,RGB32_Offset(cr->ForeColor,10,10,10)));
			FillRect(hdc,&rc);

			SetPenColor(hdc,MapRGB888(hdc,RGB32_Offset(cr->ForeColor,-25,-25,-25)));

			SetTextColor(hdc,MapRGB888(hdc,cr->TextColor));

			SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
			DrawText(hdc,wbuf,-1,&rc,DT_LEFT|DT_VCENTER|DT_BORDER);

		}
		else
		{// 绘制未选中的Item项。

			SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
			FillRect(hdc,&rc);

			//SetPenSize(hdc,1);
			//SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));

			SetTextColor(hdc,MapRGB888(hdc,cr->TextColor));

			SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
			DrawText(hdc,wbuf,-1,&rc,DT_LEFT|DT_VCENTER);

		}

		i++; //下一个Item项索引值
	}


	////绘制底部背景.
	if(i==count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc); //获得指定Item项的矩形位置参数.
		OffsetRect(&rc,0,rc.h);

		if(rc.y < di->rc.h)
		{
			rc.h =di->rc.h -rc.y;
			SetBrushColor(hdc,MapRGB888(hdc,cr->BackColor));
			FillRect(hdc,&rc);
			//SetPenColor(hdc,MapRGB888(hdc,cr->BorderColor));
			//DrawRect(hdc,&rc);
		}

	}
}

static win_oem_t oem=
{
	_SetColor,
	_Paint,
};

win_oem_t* GetListBoxOEM(void)
{
	return &oem;
}

