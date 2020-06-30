#include "emXGUI.h"
#include "./clock/GUI_CLOCK_DIALOG.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "./clock/RTC/bsp_rtc.h"
#include "stm32f4xx_rtc.h"
#include <stdlib.h>
#include "./pic_load/gui_pic_load.h"

#define ICON_BTN_NUM     2     // 按钮数量
#define ICON_TEXT_NUM   (5 + ICON_BTN_NUM)    // 文本数量

struct
{
	uint8_t page;    // 当前设置页面
	uint8_t dial;    // 选中的表盘
}Set_Start;

uint8_t clock_dial = 0;    // 表盘
static HWND clock_hwnd;

const WCHAR Week_List[][4] = {{L"星期一"}, {L"星期二"}, {L"星期三"}, {L"星期四"}, {L"星期五"}, {L"星期六"}, {L"星期日"}};
const uint8_t month_list[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    // 记录每月的最多天数

//图标管理数组
const clock_icon_t clock_icon[] = {

  /* 按钮 */
  {L"设置",           {318, 390, 166,  70},  ID_CLOCK_SET},             // 0. 设置
  {L"O",              {740,   5,  36,  72},  ID_CLOCK_EXIT},            // 1. 退出

  /* 文本 */
  {L"1",              {551, 210, 102,  93},  ID_CLOCK_DAY},             // 2. 日
  {L"星期一",         {569, 176,  77,  30},  ID_CLOCK_WEEK},            // 3. 周
  {L"1月",            {581, 303,  54,  30},  ID_CLOCK_MONTH},           // 4. 月
  {L" ",              {83,  131, 233, 243},  ID_CLOCK_TIME},            // 5. 表盘显示区域
  {L"时钟&日期",      {345,  23, 110,  33},  ID_CLOCK_TITLE},           // 6. 标题
  
  /************ 设置窗口控件 **************/
  /* 按钮 */
  {L"上一步",         {52,  390, 166,  70},  ID_CLOCK_BACK},            // 7. 上一步
  {L"下一步",         {587, 390, 166,  70},  ID_CLOCK_NEXT},            // 8. 下一步
  {L"F",              {  0,  16,  48,  48},  ID_CLOCK_WAIVE},           // 9. 放弃设置
  {L"完成",           {317, 390, 166,  70},  ID_CLOCK_OK},              // 10. 完成设置

  /* 文本 */
  {L"表盘选择",       {352,  23,  96,  33},  ID_CLOCK_SETTITLE},        // 11. 设置窗口标题
  {L"00:00",          {314, 105, 173,  30},  ID_CLOCK_SETTIME},         // 12. 设置窗口标题
  {L"2000年01月01日", {314, 105, 173,  30},  ID_CLOCK_SETDATE},         // 13. 设置窗口标题

  /* 单选按钮 */
  {L" ",              {0,   102, 266, 272},  ID_CLOCK_Background00},    // 14. 表盘 1
  {L" ",              {266, 102, 266, 272},  ID_CLOCK_Background01},    // 15. 表盘 2
  {L" ",              {532, 102, 266, 272},  ID_CLOCK_Background02},    // 16. 表盘 3

  /* 时钟&日历选择列表 */
  {L" ",              {230, 138, 170, 220},  ID_CLOCK_SetHour},         // 17. 设置小时的列表
  {L" ",              {400, 138, 169, 220},  ID_CLOCK_SetMinute},       // 18. 设置分钟的列表

  {L" ",              {230, 138, 113, 220},  ID_CLOCK_SetYear},         // 19. 设置年的列表
  {L" ",              {343, 138, 113, 220},  ID_CLOCK_SetMonth},        // 20. 设置月的列表
  {L" ",              {456, 138, 113, 220},  ID_CLOCK_SetDate},         // 21. 设置日的列表
 
};

/* 图片管理数组——HDC */
//const clock_hdc_t clock_png_info[hdc_clock_end] = 
//{
//  {GUI_CLOCK_BTN_PIC,       166, 70,     hdc_clock_btn},
//  {GUI_CLOCK_BTN_PRESS_PIC, 166, 70,     hdc_clock_btn_press},
//  {GUI_CLOCK_CALENDAR_PIC,  220, 240,    hdc_clock_calendar},
//  {GUI_CLOCK_00BACK_PIC,    233, 243,    hdc_clock_back_00},
//  {GUI_CLOCK_00H_PIC,         5, 45+39,  hdc_clock_h_00},
//  {GUI_CLOCK_00M_PIC,         5, 65+59,  hdc_clock_m_00},
//  {GUI_CLOCK_00S_PIC,         9, 103+53, hdc_clock_s_00},
//  {GUI_CLOCK_01BACK_PIC,    233, 243,    hdc_clock_back_01},
//  {GUI_CLOCK_01H_PIC,         6, 20+166, hdc_clock_h_01},
//  {GUI_CLOCK_01M_PIC,        16, 85+63,  hdc_clock_m_01},
//  {GUI_CLOCK_01S_PIC,         6, 102+72, hdc_clock_s_01},
//  {GUI_CLOCK_02BACK_PIC,    233, 243,    hdc_clock_back_02},
//  {GUI_CLOCK_02H_PIC,        15, 61+41,  hdc_clock_h_02},
//  {GUI_CLOCK_02M_PIC,        15, 77+57,  hdc_clock_m_02},
//  {GUI_CLOCK_02S_PIC,         7, 86+46,  hdc_clock_s_02},
//  {GUI_CLOCK_CHCKED_PIC,    272, 272,    hdc_clock_chcked},

//};

///* HDC */
//static HDC hdc_clock_bk;
//static HDC hdc_clock_png[hdc_clock_end];

/* 时钟位图 */
static BITMAP bm_clock_s_00;
static BITMAP bm_clock_m_00;
static BITMAP bm_clock_h_00;
static BITMAP bm_clock_s_01;
static BITMAP bm_clock_m_01;
static BITMAP bm_clock_h_01;
static BITMAP bm_clock_s_02;
static BITMAP bm_clock_m_02;
static BITMAP bm_clock_h_02;

static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
  HDC hdc;
  RECT rc, rc_tmp;
  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
  hwnd = ds->hwnd;

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 120, 120, 120));      //设置文字色
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
  
  // SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
  // FillRect(hdc, &rc);

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -23);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }

}

static void waive_btn_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc, rc_tmp;
  WCHAR wbuf[128];
  HWND hwnd;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  hwnd = ds->hwnd;

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
  { //按钮是按下状态
    SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));
  }
  else
  { //按钮是弹起状态
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
  SetFont(hdc, controlFont_48);
  /* 显示文本 */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

static void btn_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc, rc_tmp;
  WCHAR wbuf[128];
  HWND hwnd;
  
  hwnd = ds->hwnd;
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
  { //按钮是按下状态
    BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_png[hdc_clock_btn_press], 0, 0, SRCCOPY);
    SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  { //按钮是弹起状态
    BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_png[hdc_clock_btn], 0, 0, SRCCOPY);
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
  
  /* 显示文本 */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

static void radiobox_owner_draw(DRAWITEM_HDR *ds, int ID) // 单选按钮外观
{
	HDC hdc;
	RECT rc, rc_tmp;
  HWND hwnd;
  
  hwnd = ds->hwnd;
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  switch(ID)
  {
    case ID_CLOCK_Background00:
          BitBlt(hdc, rc.w/2.0-233/2.0, rc.h/2.0-243/2.0+7, 233, 243, hdc_clock_png[hdc_clock_back_00], 0, 0, SRCCOPY);
    break;

    case ID_CLOCK_Background01:
          BitBlt(hdc, rc.w/2.0-233/2.0, rc.h/2.0-243/2.0+7, 233, 243, hdc_clock_png[hdc_clock_back_01], 0, 0, SRCCOPY);
    break;

    case ID_CLOCK_Background02:
          BitBlt(hdc, rc.w/2.0-233/2.0, rc.h/2.0-243/2.0+7, 233, 243, hdc_clock_png[hdc_clock_back_02], 0, 0, SRCCOPY);
    break;
  }

  if (ds->State & BN_CHECKED)
  { 
    // 按钮被选中状态
    BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_png[hdc_clock_chcked], 3, 0, SRCCOPY);
  }
}

/*
 * @brief  重绘列表
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void _draw_listbox(HDC hdc,HWND hwnd,COLOR_RGB32 text_c,COLOR_RGB32 back_c,COLOR_RGB32 sel_c, int fontsize)
{

	RECT rc,rc_cli;
	int i,count;
	WCHAR wbuf[128];


	GetClientRect(hwnd,&rc_cli);

	SetBrushColor(hdc,MapRGB888(hdc,back_c));
	FillRect(hdc,&rc_cli);

	SetTextColor(hdc,MapRGB888(hdc,text_c));

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);

  if (fontsize == 32)
  {
    SetFont(hdc,controlFont_32);
  }
  else
  {
    SetFont(hdc,defaultFont);
  }
	

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > rc_cli.h)
		{
			break;
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER);

		i++;
	}



}

static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc,hdc0,hdc1;
	RECT rc_m;
	int x,y,w,h;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

	//定义一个中间的矩形．
	rc_m.w =ds->rc.w;
	rc_m.h =44;
	rc_m.x =0;
	rc_m.y =(ds->rc.h-rc_m.h)>>1;

	//创建两个一样大小的DC,把listbox分别绘制进去，但颜色参数不同的.
	hdc0 =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);
	hdc1 =CreateMemoryDC(SURF_SCREEN,ds->rc.w,ds->rc.h);

	//一个listbox绘到hdc0中．
	_draw_listbox(hdc0,hwnd,RGB888(168,168,168),RGB888(250,250,250),RGB888(10,100,100), 24);

	//一个listbox绘到hdc1中．
	_draw_listbox(hdc1,hwnd,RGB888(236,126,65),RGB888(232,232,232),RGB888(150,200,220), 32);

	//中间框绘制到hdc1中．
	SetPenColor(hdc1,MapRGB(hdc1,212,212,212));
	// DrawRect(hdc1,&rc_m);

  HLine(hdc1, rc_m.x, rc_m.y, rc_m.x + rc_m.w);
  HLine(hdc1, rc_m.x, rc_m.y+rc_m.h-1, rc_m.x + rc_m.w);

	//上面的矩形部分丛hdc0里复制出来.
	x =0;
	y =0;
	w =rc_m.w;
	h =rc_m.y-ds->rc.y;
	BitBlt(hdc,x,y,w,h,hdc0,x,y,SRCCOPY);

	//中间矩形部分丛hdc1里复制出来.
	BitBlt(hdc,rc_m.x,rc_m.y,rc_m.w,rc_m.h,hdc1,rc_m.x,rc_m.y,SRCCOPY);

	//下面的矩形部分丛hdc0里复制出来.
	x =0;
	y =rc_m.y+rc_m.h;
	w =rc_m.w;
	h =ds->rc.h-(rc_m.y+rc_m.h);
	BitBlt(hdc,x,y,w,h,hdc0,x,y,SRCCOPY);


	DeleteDC(hdc0);
	DeleteDC(hdc1);

}

uint8_t Sec = 0;
uint8_t Min = 0;
uint8_t Hour = 0;

static void Dial_OwnerDraw(DRAWITEM_HDR *ds)  // 绘制表盘
{
  HDC hdc;
	RECT rc, rc_tmp;
  RTC_TIME rtc_time;
  HWND hwnd;
  int clock_back;
  BITMAP clock_s, clock_m, clock_h;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  hwnd = ds->hwnd;

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  
  RTC_GetTime(RTC_Format_BIN, &rtc_time.RTC_Time);
//  RTC_GetDate(RTC_Format_BIN, &rtc_time.RTC_Date);
//  drv_clock.Time_flag=1;
//  drv_clock.Date_flag=1;
  Hour=rtc_time.RTC_Time.RTC_Hours;
  Min=rtc_time.RTC_Time.RTC_Minutes;
  Sec=rtc_time.RTC_Time.RTC_Seconds;
//  drv_clock.Date_day=rtc_time.RTC_Date.RTC_Date;
//  drv_clock.Date_month=rtc_time.RTC_Date.RTC_Month;
//  drv_clock.Date_year=rtc_time.RTC_Date.RTC_Year;
  /* Unfreeze the RTC DR Register */
  (void)RTC->DR;

  if (clock_dial == 0)    
  {
    clock_back = hdc_clock_back_00;
    clock_h = bm_clock_h_00;
    clock_m = bm_clock_m_00;
    clock_s = bm_clock_s_00;
  }
  else if (clock_dial == 1)
  {
    clock_back = hdc_clock_back_01;
    clock_h = bm_clock_h_01;
    clock_m = bm_clock_m_01;
    clock_s = bm_clock_s_01;
  }
  else if (clock_dial == 2)
  {
    clock_back = hdc_clock_back_02;
    clock_h = bm_clock_h_02;
    clock_m = bm_clock_m_02;
    clock_s = bm_clock_s_02;
  }

  BitBlt(hdc, 0, 0, 233, 243, hdc_clock_png[clock_back], 0, 0, SRCCOPY);
  EnableAntiAlias(hdc, TRUE);
  Hour = Hour>12 ? Hour-12 : Hour;
  RotateBitmap(hdc, 116, 117, &clock_h, Hour / 12.0 * 360 + 360.0 / 12.0 * Min / 60.0);
  RotateBitmap(hdc, 116, 117, &clock_m, Min / 60.0 * 360 + 360.0 / 60. * Sec / 60.0);
  RotateBitmap(hdc, 116, 117, &clock_s, Sec / 60.0 * 360);
  
  EnableAntiAlias(hdc, FALSE);
}

/*
 * @brief  重绘普通透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Common_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
  RECT rc,rc_tmp;
	WCHAR wbuf[128];
  HDC hdc_temp;

  hdc_temp = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, GUI_XSIZE, GUI_YSIZE);
  ClrDisplay(hdc_temp, NULL, 0);
  BitBlt(hdc_temp, 0, 0, GUI_XSIZE, GUI_YSIZE, hdc_clock_bk, 0, 0, SRCCOPY);
  BitBlt(hdc_temp, 497, 134, 220, 240, hdc_clock_png[hdc_clock_calendar], 0, 0, SRCCOPY);

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.

	/* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_temp, rc_tmp.x, rc_tmp.y, SRCCOPY);

	SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)

  DeleteDC(hdc_temp);
}

/*
 * @brief  重绘标题透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Title_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
  RECT rc,rc_tmp;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

  /* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

	SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	GetWindowText(hwnd, wbuf, 128);                        // 获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

/*
 * @brief  重绘白色背景文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void WhiteBK_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
  RECT rc;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

  /* 背景 */
  SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
  FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
	GetWindowText(hwnd, wbuf, 128);                        // 获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

/*
 * @brief  重绘数字透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Num_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
	HDC hdc;
  RECT rc,rc_tmp;
	WCHAR wbuf[128];

  HDC hdc_temp;

  hdc_temp = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, GUI_XSIZE, GUI_YSIZE);
  ClrDisplay(hdc_temp, NULL, 0);
  BitBlt(hdc_temp, 0, 0, GUI_XSIZE, GUI_YSIZE, hdc_clock_bk, 0, 0, SRCCOPY);
  BitBlt(hdc_temp, 497, 134, 220, 240, hdc_clock_png[hdc_clock_calendar], 0, 0, SRCCOPY);

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.

	/* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_temp, rc_tmp.x, rc_tmp.y, SRCCOPY);

  SetFont(hdc, controlFont_72);
	SetTextColor(hdc, MapRGB(hdc, 0, 0, 0));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
  SetFont(hdc, defaultFont);

  DeleteDC(hdc_temp);
}

/*
 * @brief  获得列表选中项的数值（字符串转整型）
 * @param  hwnd:	窗口句柄
 * @param  ID：   列表 ID
 * @retval NONE
*/
static uint16_t GetListCurselVal(HWND hwnd, uint32_t ID)
{
  HWND wnd;
  uint8_t csl;    // 选中项
  WCHAR wbuf[10];
  char cbuf[10];

  wnd = GetDlgItem(hwnd, ID);
  csl = SendMessage(wnd, LB_GETCURSEL, 0, 0);    // 获得选中项
  SendMessage(wnd, LB_GETTEXT, csl, (LPARAM)wbuf);       // 获得选中项的文本
  x_wcstombs_cp936(cbuf, wbuf, 9);

  return x_atoi(cbuf);                           // 返回选中项的值
}

/*
 * @brief  获得列表指定项的数值（字符串转整型）
 * @param  hwnd:	窗口句柄
 * @param  ID：   列表 ID
 * @param  csl:   列表的项
 * @retval NONE
*/
//static uint16_t GetListItemVal(HWND hwnd, uint32_t ID, uint32_t csl)
//{
//  HWND wnd;
//  WCHAR wbuf[10];
//  char cbuf[10];

//  wnd = GetDlgItem(hwnd, ID);
//  SendMessage(wnd, LB_GETTEXT, csl, (LPARAM)wbuf);       // 获得选中项的文本
//  x_wcstombs_cp936(cbuf, wbuf, 9);

//  return x_atoi(cbuf);                           // 返回选中项的值
//}

static LRESULT setting_win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg){
      case WM_CREATE:
      {
        Set_Start.dial = 0;
        Set_Start.page = 0;

        for (uint8_t xC=7; xC<11; xC++)     //  按钮
        {
          /* 循环创建按钮 */
          CreateWindow(BUTTON, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w, clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }

        ShowWindow(GetDlgItem(hwnd, ID_CLOCK_BACK), SW_HIDE);    // 隐藏上一步按钮

        for (uint8_t xC=11; xC<14; xC++)
        {
          /* 循环创建文本框 */
          CreateWindow(TEXTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w,clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL);
        }
        ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), SW_SHOW);    // 隐藏上一步按钮
         
        for (uint8_t xC=14; xC<17; xC++)
        {
          /* 循环单选按钮 */
          CreateWindow(BUTTON, clock_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE | BS_RADIOBOX,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w, clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id | (1 << 16), NULL, NULL); 
        }

        SendMessage(GetDlgItem(hwnd, ID_CLOCK_Background00), BM_SETSTATE, BST_CHECKED, 0);    // 默认选中第一个

        for (uint8_t xC=17; xC<22; xC++)
        {
          /* 创建设置列表（创建为不立即可见） */
          CreateWindow(LISTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW | LBS_NOTIFY,
                            clock_icon[xC].rc.x, clock_icon[xC].rc.y, clock_icon[xC].rc.w,
                            clock_icon[xC].rc.h, hwnd, clock_icon[xC].id, NULL, NULL);
        }

        for (uint8_t xC=0; xC<24; xC++)
        {
          WCHAR wbuf[10];
          HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetHour);

          /* 在列表中增加小时 */
          x_wsprintf(wbuf, L"%02d", xC);
          SendMessage(ListHwnd, LB_ADDSTRING, xC, (LPARAM)wbuf);
          SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC, (LPARAM)44);
        }
        
        for (uint8_t xC=0; xC<60; xC++)
        {
          WCHAR wbuf[10];
          HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetMinute);

          /* 在列表中增加分钟 */
          x_wsprintf(wbuf, L"%02d", xC);
          SendMessage(ListHwnd, LB_ADDSTRING, xC, (LPARAM)wbuf);
          SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC, (LPARAM)44);
        }

        uint8_t i=0;
        for (uint16_t xC=2000; xC<2099; xC++)
        {
          WCHAR wbuf[10];
          
          HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetYear);

          /* 在列表中增加年 */
          x_wsprintf(wbuf, L"%d", xC);
          SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
          SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
        }

        i=0;
        for (uint8_t xC=1; xC<13; xC++)
        {
          WCHAR wbuf[10];
          HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetMonth);

          /* 在列表中增加月 */
          x_wsprintf(wbuf, L"%d", xC);
          SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
          SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
        }

        i=0;
        for (uint8_t xC=1; xC<32; xC++)
        {
          WCHAR wbuf[10];
          HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetDate);

          /* 在列表中增加日期 */
          x_wsprintf(wbuf, L"%d", xC);
          SendMessage(ListHwnd, LB_ADDSTRING, i, (LPARAM)wbuf);
          SendMessage(ListHwnd, LB_SETITEMHEIGHT, i++, (LPARAM)44);
        }

        SetTimer(hwnd, 1, 50, TMR_START, NULL);
        
        break;
      }

      case WM_TIMER:
      {
        int cur_sel, i, y;
        HWND wnd;
        RECT rc0, rc;

        if(GetKeyState(VK_LBUTTON)==0) //鼠标左键／触摸屏　没有按下时.
        {
          for (uint32_t xC=ID_CLOCK_SetYear; xC<ID_CLOCK_SetMinute+1; xC++)
          {
            wnd =GetDlgItem(hwnd, xC); //获得LISTBOX 设置时的HWND.

            cur_sel=SendMessage(wnd,LB_GETCURSEL,0,0); //获得LISTBOX顶项.

            GetClientRect(wnd,&rc0);
            SendMessage(wnd,LB_GETITEMRECT,cur_sel,(LPARAM)&rc); //获得顶项的矩形位置.

            y = rc.y+(rc.h/2);

            if(y < (rc0.h/2)) //如果选中的项在Listbox　中线的上面,则往下移动.
            {
              if(abs(y-(rc0.h/2)) > (rc.h>>2))
              {//与中线隔得远，就每次多移一点距离(看上去移动速度快)．
                //i =rc.h>>2;
                i = 4;
              }
              else
              {//与中线隔得很近了，就只移1个offset(缓慢移动)．
                i = 1;
              }

              SendMessage(wnd,LB_OFFSETPOS,TRUE,i);
            }

            if(y > (rc0.h/2)) //如果选中的项在Listbox　中线的下面,则往上移动.
            {
              if(abs(y-(rc0.h/2)) > (rc.h>>2))
              {//与中线隔得远，就每次多移一点距离(看上去移动速度快)．
                //i =rc.h>>2;
                i = 4;
              }
              else
              {//与中线隔得很近了，就只移1个offset(缓慢移动)．
                i = 1;
              }

              SendMessage(wnd,LB_OFFSETPOS,TRUE,-i);
            }
          }
        }
        
      }  
			break;     
      
      case WM_NOTIFY:
      {
         u16 code, id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         
         NMHDR *nr;
         nr =(NMHDR*)lParam;      

         //发送单击
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case ID_CLOCK_WAIVE:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

            /* 完成设置按钮按下 */
            case ID_CLOCK_OK:
            {
              if (Set_Start.page == 0)    // 只设置表盘
              {
                clock_dial = Set_Start.dial;    // 设置表盘
              }
              else if (Set_Start.page == 1)    // 修改表盘和时间
              {
                RTC_TimeTypeDef RTC_Time;

                clock_dial = Set_Start.dial;    // 设置表盘
                RTC_Time.RTC_Hours = GetListCurselVal(hwnd, ID_CLOCK_SetHour);        // 读取列表的时
                RTC_Time.RTC_Minutes = GetListCurselVal(hwnd, ID_CLOCK_SetMinute);    // 读取列表的分
                RTC_Time.RTC_Seconds = 0;

                RTC_SetTime(RTC_Format_BIN, &RTC_Time);    // 设置时间
              }
              else if (Set_Start.page == 2)    // 修改表盘、时间和日历
              {
                RTC_DateTypeDef RTC_Date;
                RTC_TimeTypeDef RTC_Time;

                clock_dial = Set_Start.dial;    // 设置表盘

                RTC_Time.RTC_Hours = GetListCurselVal(hwnd, ID_CLOCK_SetHour);        // 读取列表的时
                RTC_Time.RTC_Minutes = GetListCurselVal(hwnd, ID_CLOCK_SetMinute);    // 读取列表的分
                RTC_Time.RTC_Seconds = 0;
                RTC_SetTime(RTC_Format_BIN, &RTC_Time);    // 设置时间

                RTC_Date.RTC_Year = GetListCurselVal(hwnd, ID_CLOCK_SetYear) - 2000;    // 读取列表的年
                RTC_Date.RTC_Month = GetListCurselVal(hwnd, ID_CLOCK_SetMonth);         // 读取列表的月
                RTC_Date.RTC_Date = GetListCurselVal(hwnd, ID_CLOCK_SetDate);           // 读取列表的日
                /* 基姆拉尔森周计算公式 */
                RTC_Date.RTC_WeekDay = (RTC_Date.RTC_Date + 2 * RTC_Date.RTC_Month + 3     \
                                        * (RTC_Date.RTC_Month + 1) / 5 + RTC_Date.RTC_Year \
                                        + RTC_Date.RTC_Year / 4 - RTC_Date.RTC_Year / 100  \
                                        + RTC_Date.RTC_Year / 400) % 7 + 1;
                RTC_SetDate(RTC_Format_BIN, &RTC_Date);                                 // 设置日期

                /* 设置当前显示日期 */
                WCHAR wbuf[5];
                
                x_wsprintf(wbuf, L"%d", RTC_Date.RTC_Date);
                SetWindowText(GetDlgItem(clock_hwnd, ID_CLOCK_DAY), wbuf);    // 设置日期
                
                x_wsprintf(wbuf, L"%d月", RTC_Date.RTC_Month);
                SetWindowText(GetDlgItem(clock_hwnd, ID_CLOCK_MONTH), wbuf);    // 设置月
                
                SetWindowText(GetDlgItem(clock_hwnd, ID_CLOCK_WEEK), Week_List[RTC_Date.RTC_WeekDay - 1]);    // 设置星期
              }

              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

            /* 表盘选择 */
            case ID_CLOCK_Background00:
            {
              Set_Start.dial = 0;    // 选择表盘 1
            }
            break;

            /* 表盘选择 */
            case ID_CLOCK_Background01:
            {
              Set_Start.dial = 1;    // 选择表盘 2
            }
            break;

            /* 表盘选择 */
            case ID_CLOCK_Background02:
            {
              Set_Start.dial = 2;    // 选择表盘 3
            }
            break;

            /* 下一步 */
            case ID_CLOCK_NEXT:
            {
              if (Set_Start.page == 0)
              {
                Set_Start.page = 1;    // 标记为时钟设置界面
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置时间");

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background00), SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background01), SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background02), SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),      SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute),    SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_BACK),         SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),      SW_SHOW);
              }
              else if (Set_Start.page == 1)
              {
                Set_Start.page = 2;    // 标记为日期设置界面
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置日期");

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),   SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute), SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),      SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),   SW_HIDE);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_SHOW);
              }
              
            }
            break;

            /* 上一步 */
            case ID_CLOCK_BACK:
            {
              if (Set_Start.page == 2)
              {
                Set_Start.page = 1;    // 标记为时间设置界面
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"设置时间");

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),   SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_NEXT),      SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),  SW_SHOW);

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetDate),  SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetYear),  SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMonth), SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETDATE),  SW_HIDE);
              }
              else if (Set_Start.page == 1)
              {
                Set_Start.page = 0;    // 标记为表盘选择界面
                SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTITLE), L"表盘选择");

                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background00), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background01), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_Background02), SW_SHOW);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetHour),      SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SetMinute),    SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_BACK),         SW_HIDE);
                ShowWindow(GetDlgItem(hwnd, ID_CLOCK_SETTIME),      SW_HIDE);
              }
            }
            break;
          }
        }

        /* 列表处理 */
        if (id >= ID_CLOCK_SetYear && id <= ID_CLOCK_SetMinute)
        {
          if(nr->code == LBN_SELCHANGE)    // 列表选中项被改变
          {
            
          }
          
          if(nr->code == LBN_POSCHANGE)    // 有列表位置被改变
          {
            RECT rc0,rc;
            HWND wnd;
            int i;

            wnd = GetDlgItem(hwnd, id);     // 获得被改变列表的 HWND

            GetClientRect(wnd,&rc0);
            /* 使列表循环 */
            i = 0;
            SendMessage(wnd,LB_GETITEMRECT,i,(LPARAM)&rc);
            if(rc.y > 2)
            {
              i =SendMessage(wnd,LB_GETCOUNT,0,0);
              i-=1;
              SendMessage(wnd,LB_MOVEINDEX,i,0);
            }
            else
            {
              i =SendMessage(wnd,LB_GETCOUNT,0,0);
              i-=1;
              SendMessage(wnd,LB_GETITEMRECT,i,(LPARAM)&rc);
              if((rc.y+rc.h) < (rc0.h-8))
              {
                SendMessage(wnd,LB_MOVEINDEX,0,0xFFFF);
              }
            }

            /* 设置靠近中间项为选中项 */
            int min = 0;    // 记录最近的（最后的选中项）
            int len = 0xFFFFFFF;
            int count = SendMessage(wnd,LB_GETCOUNT,0,0);
            i = SendMessage(wnd, LB_GETTOPINDEX, 0, 0);            // 首个显示的子项目
            
            while (1)
            {
              SendMessage(wnd, LB_GETITEMRECT, i, (LPARAM)&rc);    // 获得子项目的矩形参数

              if (rc.y >= rc0.h || i >= count - 1)          // 判断是不是在显示区域内
              {
                SendMessage(wnd, LB_SETCURSEL, min, 0);     // 设置当前选中的子项目
                break;                                      // 不是在显示区域，结束循环
              }

              rc.y += rc.h/2;                // 计算这项的中间位置
              rc.y = abs(rc.y - rc0.h/2);    // 计算当前项到中间的距离

              if (len > rc.y)
              {
                len = rc.y;    // 记录较小距离
                min = i;       // 记录较小项
              }
              i++;         // 查找下一项
              if (i>=count) 
              {
                i = 0;
              }
            }

            WCHAR wbuf[5];
            if (id >= ID_CLOCK_SetYear && id <= ID_CLOCK_SetDate)    // 这在设置日历
            {
              x_wsprintf(wbuf, L"%d年%02d月%02d日", GetListCurselVal(hwnd, ID_CLOCK_SetYear),
                                                    GetListCurselVal(hwnd, ID_CLOCK_SetMonth),
                                                    GetListCurselVal(hwnd, ID_CLOCK_SetDate));

              SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETDATE), wbuf);    // 设置日期
            }
            else    // 正在设置时间
            {
              x_wsprintf(wbuf, L"%02d:%02d", GetListCurselVal(hwnd, ID_CLOCK_SetHour),
                                             GetListCurselVal(hwnd, ID_CLOCK_SetMinute));

              SetWindowText(GetDlgItem(hwnd, ID_CLOCK_SETTIME), wbuf);    // 设置日期
            }
      
            uint8_t month = 0;
            month = GetListCurselVal(hwnd, ID_CLOCK_SetMonth);    // 获得选中的月
            if (id == ID_CLOCK_SetMonth || (id == ID_CLOCK_SetYear && month == 2))    // 设置月份，不同月份需调整日期
            {
              uint8_t date_max = 0;

              date_max = month_list[month - 1];

              if (month == 2)    // 调整平年 闰年
              {
                uint16_t year = GetListCurselVal(hwnd, ID_CLOCK_SetYear);    // 获得选中的年

                if( year % 400 == 0 || (year % 4 ==  0 && year % 100 != 0))
                {
                  date_max = 29;    // 闰年
                }
              }

              HWND ListHwnd = GetDlgItem(hwnd, ID_CLOCK_SetDate);

              SendMessage(ListHwnd, LB_RESETCONTENT, 0, 0);

              for (uint8_t xC=1; xC<date_max+1; xC++)
              {
                WCHAR wbuf[10];

                /* 在列表中增加日期 */
                x_wsprintf(wbuf, L"%d", xC);
                SendMessage(ListHwnd, LB_ADDSTRING, xC-1, (LPARAM)wbuf);
                SendMessage(ListHwnd, LB_SETITEMHEIGHT, xC-1, (LPARAM)44);
              }
              SendMessage(ListHwnd, LB_SETCURSEL, 2, 0);

              // uint8_t list_conut = SendMessage(ListHwnd, LB_GETCOUNT, 0, 0);
              // uint8_t DateCursel = GetListCurselVal(hwnd, ID_CLOCK_SetDate);    // 获得选中的日期
              // uint8_t max = 0;
              // uint8_t item = 0;

              // if (list_conut > date_max)
              // {
              //   /* 列表项目比实际要多需要删除 */
              //   for (uint8_t xC=0; xC<list_conut; xC++)
              //   {
              //     if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) > date_max)
              //     {
              //       SendMessage(ListHwnd, LB_DELETESTRING, xC, 0);
              //     }
              //   }
              // }
              // else if (list_conut < date_max)
              // {
              //   /* 列表项目比实际要少需要增加 */
              //   for (uint8_t xC=0; xC<list_conut; xC++)    // 找到最大项目
              //   {
              //     if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) > max)
              //     {
              //       item = xC;    // 记录较大项目
              //       max = GetListItemVal(hwnd, ID_CLOCK_SetDate, item);       // 得到最大项的内容
              //     }
              //   }

                
              //   list_conut = SendMessage(ListHwnd, LB_GETCOUNT, 0, 0);    // 得到现有的项目数
              //   uint8_t ii = max;    // 项目

              //   for (uint8_t xY=item; xY<list_conut+date_max-max+1; xY++)    // 在最大的项目后面增加需要增加的项目
              //   {
              //     x_wsprintf(wbuf, L"%d", ii+1);
              //     ii++;
              //     SendMessage(ListHwnd, LB_ADDSTRING, xY, (LPARAM)wbuf);      // 增加项目
              //     SendMessage(ListHwnd, LB_SETITEMHEIGHT, xY, (LPARAM)45);    // 设置项目高度
              //   }
              // }

              // /* 设置列表的选中项目 */
              // list_conut = SendMessage(ListHwnd, LB_GETCOUNT, 0, 0);
              // max = 0;
              // uint8_t xC=0;
              // for (xC=0; xC<list_conut; xC++)    // 找到最大项目
              // {
              //   if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) == DateCursel)
              //   {
              //     SendMessage(ListHwnd, LB_SETCURSEL, xC, 0);    // 设置选中的项目
              //     break;
              //   }

              //   if (GetListItemVal(hwnd, ID_CLOCK_SetDate, xC) > max)
              //   {
              //     item = xC;    // 记录较大项目
              //     max = GetListItemVal(hwnd, ID_CLOCK_SetDate, xC);
              //   }
              // }

              // if (xC >= list_conut)    // 没找到原来的项，设置最大项为选中项
              // {
              //   SendMessage(ListHwnd, LB_SETCURSEL, item, 0);    // 设置选中的项目
              // }
              // printf("xC=%d,item=%d\n", xC, item);
            }
          }
        }

      break;
    }

      // 重绘制函数消息
      case WM_DRAWITEM:
      {
        DRAWITEM_HDR *ds;
        ds = (DRAWITEM_HDR*)lParam;        
        if (ds->ID >= ID_CLOCK_BACK && ds->ID <= ID_CLOCK_OK)
        {
          btn_owner_draw(ds);             // 重绘按钮
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_SETTITLE)
        {
          Title_Textbox_OwnerDraw(ds);
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_SETTIME || ds->ID == ID_CLOCK_SETDATE)
        {
          WhiteBK_Textbox_OwnerDraw(ds);
          return TRUE;
        }
        else if (ds->ID == ID_CLOCK_WAIVE)
        {
          waive_btn_owner_draw(ds);
          return TRUE;
        }
        else if (ds->ID >= ID_CLOCK_Background00 && ds->ID <= ID_CLOCK_Background02)
        {
          radiobox_owner_draw(ds, ds->ID);
          return TRUE;
        }
        else if (ds->ID >= ID_CLOCK_SetYear && ds->ID <= ID_CLOCK_SetMinute)
        {
          listbox_owner_draw(ds);
          return TRUE;
        }

        return FALSE;
      }     
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
//        HDC hdc;//屏幕hdc

        //开始绘制
        BeginPaint(hwnd, &ps); 
        
        EndPaint(hwnd, &ps);
      }
      break;

      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        RECT rc =*(RECT*)lParam;
        RECT rc1 = {230, 102, 339, 272};

        BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc.x, rc.y, SRCCOPY);

        if (Set_Start.page != 0)     // 不是表盘选择
        {
          SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
          EnableAntiAlias(hdc, TRUE);
          FillRoundRect(hdc, &rc1, 10);    // 画列表背景
          EnableAntiAlias(hdc, FALSE);
        }

        return TRUE;
      }

      //关闭窗口消息处理case
      case WM_CLOSE:
      {   
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {
        SetTimer(clock_hwnd, 1, 400, TMR_START, NULL);

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ 
  static uint8_t Load_Flag = 1;
   switch(msg){
      case WM_CREATE:
      {
        for (uint8_t xC=0; xC<ICON_BTN_NUM; xC++)     //  按钮
        {
          /* 循环创建按钮 */
          CreateWindow(BUTTON, clock_icon[xC].icon_name, WS_OWNERDRAW|WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w, clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }

        for (uint8_t xC=ICON_BTN_NUM; xC<ICON_TEXT_NUM; xC++)    // 文本框
        {
          /* 循环创建文本框 */
          CreateWindow(TEXTBOX, clock_icon[xC].icon_name, WS_OWNERDRAW|WS_VISIBLE,
                        clock_icon[xC].rc.x, clock_icon[xC].rc.y,
                        clock_icon[xC].rc.w,clock_icon[xC].rc.h,
                        hwnd, clock_icon[xC].id, NULL, NULL); 
        }

        /* 初始化日期 */
        WCHAR wbuf[5];
        RTC_TIME rtc_time;
        RTC_GetDate(RTC_Format_BIN, &rtc_time.RTC_Date);
        
        x_wsprintf(wbuf, L"%d", rtc_time.RTC_Date.RTC_Date);
        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_DAY), wbuf);    // 设置日期
        
        x_wsprintf(wbuf, L"%d月", rtc_time.RTC_Date.RTC_Month);
        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_MONTH), wbuf);    // 设置月
        
        SetWindowText(GetDlgItem(hwnd, ID_CLOCK_WEEK), Week_List[rtc_time.RTC_Date.RTC_WeekDay - 1]);    // 设置星期

        SetTimer(hwnd, 1, 400, TMR_START, NULL);
        
        /* 转换成bitmap */
        DCtoBitmap(hdc_clock_png[hdc_clock_s_00], &bm_clock_s_00);
        DCtoBitmap(hdc_clock_png[hdc_clock_h_00], &bm_clock_h_00);
        DCtoBitmap(hdc_clock_png[hdc_clock_m_00], &bm_clock_m_00);
        DCtoBitmap(hdc_clock_png[hdc_clock_s_01], &bm_clock_s_01);
        DCtoBitmap(hdc_clock_png[hdc_clock_h_01], &bm_clock_h_01);
        DCtoBitmap(hdc_clock_png[hdc_clock_m_01], &bm_clock_m_01);
        DCtoBitmap(hdc_clock_png[hdc_clock_s_02], &bm_clock_s_02);
        DCtoBitmap(hdc_clock_png[hdc_clock_h_02], &bm_clock_h_02);
        DCtoBitmap(hdc_clock_png[hdc_clock_m_02], &bm_clock_m_02);
        
        // SetTimer(hwnd, 2, 10, TMR_START|TMR_SINGLE, NULL);    // 资源加载定时器

        break;
      }

      case WM_TIMER:
      {
        int tmr_id;

				tmr_id = wParam;    // 定时器 ID

				if (tmr_id == 1)
        {
          WCHAR wbuf[5];
          
          RTC_TIME rtc_time;
          RTC_GetDate(RTC_Format_BIN, &rtc_time.RTC_Date);
          
          x_wsprintf(wbuf, L"%d", rtc_time.RTC_Date.RTC_Date);
          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_DAY), wbuf);    // 设置日期
          
          x_wsprintf(wbuf, L"%d月", rtc_time.RTC_Date.RTC_Month);
          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_MONTH), wbuf);    // 设置月
          
          SetWindowText(GetDlgItem(hwnd, ID_CLOCK_WEEK), Week_List[rtc_time.RTC_Date.RTC_WeekDay - 1]);    // 设置星期
          /* Unfreeze the RTC DR Register */
          (void)RTC->DR;
          
          InvalidateRect(GetDlgItem(hwnd, ID_CLOCK_TIME), NULL, TRUE);
        }
//        else if (tmr_id == 2)
//        {
//          u8 *jpeg_buf;
//          u32 jpeg_size;
//          JPG_DEC *dec;
//          BOOL res = NULL;

//          res = RES_Load_Content(GUI_CLOCK_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
//          // res = FS_Load_Content(GUI_CLOCK_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
//          hdc_clock_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
//          if(res)
//          {
//            /* 根据图片数据创建JPG_DEC句柄 */
//            dec = JPG_Open(jpeg_buf, jpeg_size);

//            /* 绘制至内存对象 */
//            JPG_Draw(hdc_clock_bk, 0, 0, dec);

//            /* 关闭JPG_DEC句柄 */
//            JPG_Close(dec);
//          }
//          /* 释放图片内容空间 */
//          RES_Release_Content((char **)&jpeg_buf);

//          u8 *pic_buf;
//          u32 pic_size;
//          PNG_DEC *png_dec;
//          BITMAP png_bm;
//          
//          for (uint8_t xC=0; xC<hdc_clock_end; xC++)
//          {
//            /* 创建 HDC */
//            hdc_clock_png[clock_png_info[xC].id] = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, clock_png_info[xC].w, clock_png_info[xC].h);
//            ClrDisplay(hdc_clock_png[clock_png_info[xC].id], NULL, 0);
//            res = RES_Load_Content(clock_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);
//            // res = FS_Load_Content(clock_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);
//            if(res)
//            {
//              png_dec = PNG_Open(pic_buf);
//              PNG_GetBitmap(png_dec, &png_bm);
//              DrawBitmap(hdc_clock_png[clock_png_info[xC].id], 0, 0, &png_bm, NULL);
//              PNG_Close(png_dec);
//            }
//            /* 释放图片内容空间 */
//            RES_Release_Content((char **)&pic_buf);
//          }

//          /* 转换成bitmap */
//          DCtoBitmap(hdc_clock_png[hdc_clock_s_00], &bm_clock_s_00);
//          DCtoBitmap(hdc_clock_png[hdc_clock_h_00], &bm_clock_h_00);
//          DCtoBitmap(hdc_clock_png[hdc_clock_m_00], &bm_clock_m_00);
//          DCtoBitmap(hdc_clock_png[hdc_clock_s_01], &bm_clock_s_01);
//          DCtoBitmap(hdc_clock_png[hdc_clock_h_01], &bm_clock_h_01);
//          DCtoBitmap(hdc_clock_png[hdc_clock_m_01], &bm_clock_m_01);
//          DCtoBitmap(hdc_clock_png[hdc_clock_s_02], &bm_clock_s_02);
//          DCtoBitmap(hdc_clock_png[hdc_clock_h_02], &bm_clock_h_02);
//          DCtoBitmap(hdc_clock_png[hdc_clock_m_02], &bm_clock_m_02);

//          Load_Flag = 1;    // 标志资源加载完成
//          for (uint32_t xC=0; xC<7; xC++)
//          {
//            ShowWindow(GetDlgItem(hwnd, clock_icon[xC].id), SW_SHOW);    // 资源加载完成，显示主页的全部控件
//          }
//          InvalidateRect(hwnd, NULL, TRUE);    // 重绘窗口
//        }
      }  
			break;     
      
      case WM_NOTIFY:
      {
         u16 code,  id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型   

         //发送单击
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case ID_CLOCK_EXIT:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

            /* 设置按钮按下 */
            case ID_CLOCK_SET:
            {
              KillTimer(hwnd, 1);
              /* 创建设置窗口 */
              WNDCLASS wcex;
              RECT rc;

              wcex.Tag	 		= WNDCLASS_TAG;

              wcex.Style			= CS_HREDRAW | CS_VREDRAW;
              wcex.lpfnWndProc	= (WNDPROC)setting_win_proc;
              wcex.cbClsExtra		= 0;
              wcex.cbWndExtra		= 0;
              wcex.hInstance		= NULL;
              wcex.hIcon			= NULL;
              wcex.hCursor		= NULL;
              
              rc.x = 0;
              rc.y = 0;
              rc.w = GUI_XSIZE;
              rc.h = GUI_YSIZE;
              
              // 创建"设置"窗口.
              CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS,
                           rc.x, rc.y, rc.w, rc.h, hwnd, ID_CLOCK_SetWin, NULL, NULL);
            }
            break;
          }
        }
        break;
      }

      //重绘制函数消息
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_CLOCK_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_SET)
         {
            btn_owner_draw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_MONTH || ds->ID == ID_CLOCK_WEEK)
         {
           Common_Textbox_OwnerDraw(ds);
           return TRUE;
         }
         else if (ds->ID == ID_CLOCK_DAY)
         {
            Num_Textbox_OwnerDraw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_TITLE)
         {
            Title_Textbox_OwnerDraw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_CLOCK_TIME)
         {
            Dial_OwnerDraw(ds);
            return TRUE;
         }

         return FALSE;
      }     
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;

        //开始绘制
        BeginPaint(hwnd, &ps); 
        
        EndPaint(hwnd, &ps);
        break;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        RECT rc =*(RECT*)lParam;
        RECT rc2 = {0, 0, GUI_XSIZE, GUI_YSIZE};
        
        if (Load_Flag)     // 资源加载完成
        {
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc.x, rc.y, SRCCOPY);
          BitBlt(hdc, 497, 134, 220, 240, hdc_clock_png[hdc_clock_calendar], 0, 0, SRCCOPY);
        }
        else
        {    /* 资源加载未完成 */ 
          SetBrushColor(hdc, MapRGB(hdc, 10, 10, 10));
          FillRect(hdc, &rc2);
          SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
          DrawText(hdc, L"资源加载中，请稍等...", -1, &rc2, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
        }

        return TRUE;
      }

      //关闭窗口消息处理case
      case WM_CLOSE:
      {   
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {        
//        Load_Flag = 0;
//        DeleteDC(hdc_clock_bk);
//        for (int i=0; i<hdc_clock_end; i++)
//        {
//          DeleteDC(hdc_clock_png[i]);
//        }
        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_CLOCK_DIALOG(void)
{ 	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	clock_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,//
                                    &wcex,
                                    L"GUI CLOCK DIALOG",
                                    WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(clock_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, clock_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


