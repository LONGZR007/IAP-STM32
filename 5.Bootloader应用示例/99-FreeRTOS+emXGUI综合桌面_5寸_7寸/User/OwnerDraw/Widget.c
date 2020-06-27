#include <emXGUI.h>
#include "GUI_AppDef.h" 
#include "Widget.h"
/*
 * @brief  绘制主界面按钮
 * @param  hwnd:   滚动条的句柄值
 * @retval NONE
*/
void home_owner_draw(DRAWITEM_HDR *ds) 
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  EnableAntiAlias(hdc, TRUE);
	

  EnableAntiAlias(hdc, FALSE);   
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
   //按钮按下状态
   if (ds->State & BST_PUSHED)
	{ 
		SetBrushColor(hdc, MapRGB(hdc, 105,105,105));
	}
	else//按钮弹起状态
	{ 
		SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));
	}
  FillCircle(hdc, rc.x+rc.w, rc.y, rc.w);
   /* 使用控制图标字体 */
	SetFont(hdc, controlFont_64);
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
   rc.y = -10;
   rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//绘制文字(居中对齐方式)


  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);
}

/*
 * @brief  绘制滚动条
 * @param  hwnd:   滚动条的句柄值
 * @param  ScrollBar_parm 滑动条的参数
 * @retval NONE
*/
void draw_scrollbar(HWND hwnd, HDC hdc, ScrollBar_S ScrollBar_parm)
{
	RECT rc;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* 背景 */
	SetBrushColor(hdc, ScrollBar_parm.back_c);
	FillRect(hdc, &rc);

   rc_scrollbar.x = rc.x;
   rc_scrollbar.y = rc.h/2;
   rc_scrollbar.w = rc.w;
   rc_scrollbar.h = 2;
   
	SetBrushColor(hdc, MapRGB888(hdc, ScrollBar_parm.page_c));
	FillRect(hdc, &rc_scrollbar);

	/* 滑块 */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));

	/* 边框 */

	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.w / 2, rc.w / 2 - 1);
   InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, ScrollBar_parm.fore_c));
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.w / 2, rc.w / 2 - 1);
}
/*
 * @brief  自定义滑动条绘制函数
 * @param  ds:	自定义绘制结构体
 * @param  ScrollBar_parm1,ScrollBar_parm2:滑动条的参数
 * @retval NONE
*/
void scrollbar_owner_draw(DRAWITEM_HDR *ds, ScrollBar_S ScrollBar_parm1, ScrollBar_S ScrollBar_parm2)
{
	HWND hwnd;
	HDC hdc;
	HDC hdc_mem;
	HDC hdc_mem1;
	RECT rc;
	RECT rc_cli;
	//	int i;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
	hdc_mem1 = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);   
         
   	
	//绘制白色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem1, ScrollBar_parm1);
	//绘制绿色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem, ScrollBar_parm2);
   SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   

	//左
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc.x, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
	//右
	BitBlt(hdc, rc.x + rc.w, 0, rc_cli.w - (rc.x + rc.w) , rc_cli.h, hdc_mem1, rc.x + rc.w, 0, SRCCOPY);

	//绘制滑块
	if (ds->State & SST_THUMBTRACK)//按下
	{
      BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
	}
	else//未选中
	{
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

