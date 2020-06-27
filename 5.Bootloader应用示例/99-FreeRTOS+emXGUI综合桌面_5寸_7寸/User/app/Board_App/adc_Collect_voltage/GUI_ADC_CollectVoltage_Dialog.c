#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include  "GUI_ADC_CollectVoltage_Dialog.h"
#include "./pic_load/gui_pic_load.h"

/* 图片资源 */
//#define BMP    1    // 1：使用png 0：使用bmp

//#if BMP
//  #define Slider_Button_Name        "slider_button.png"    // 90 * 90
//  #define Slider_Name               "slider.png"           // 600 * 45 
//  #define Adc_Circle_Name           "adc_circle.png"       // 230 * 230
//  #define F429_RP_Name              "F429_RP.png"          // 350 * 340
//#else
//  #define Slider_Button_Name        "slider_button.bmp"    // 90 * 90
//  #define Slider_Name               "slider.bmp"           // 600 * 45 
//  #define Adc_Circle_Name           "adc_circle.bmp"       // 230 * 230
//  #define F429_RP_Name              "F429_RP.bmp"          // 350 * 340
//#endif
//  #define GUI_ADC_BACKGROUNG_PIC    "adc_desktop.jpg"


/* 窗口 ID */
#define ID_ADV_WIN         0x01    // 中间显示窗口ID
#define SCROLLBAR_Brigh_ID 0x02    // 滑动条的ID
#define ID_TEXTBOX_Title   0x03    // 标题栏
#define ID_TEXTBOX_Brigh   0x04    // 亮度百分比

#define CircleCenter_1    (98)     // 三角形旋转半径
#define CircleCenter_2    (125)    // 圆弧进度条半径（小）
#define CircleCenter_3    (CircleCenter_2 + 10)    //  不大于 CircleSize / 2

/* 移动方向标志 */
#define LeftToRight    0
#define RightToLeft    1
#define MOVE_WIN       1

#define CircleSize    285    // 圆形显示区域的大小
#define Circle_X      460    // 圆形显示区域的位置
#define Circle_Y      (27)   // 圆形显示区域的位置

#define TitleHeight    70    // 标题栏的高度

#define TriangleLen    20    // 三角形的边长
 
uint8_t AovingDirection = 0;

HWND MAIN_Handle;
HWND Title_Handle;
HWND Brigh_Handle;
HWND ADC_Handle;
HWND Brigh_TEXTBOX_Handle;

HDC BacklightFont_hdc;
HDC TrianglePointer_DC;
HDC hdc_mem;

static BITMAP bm_Triangle;

static WCHAR Backlightwbuf[50] __EXRAM;

static COLORREF color_bg;       // 透明控件的背景颜色

static uint8_t res_prep = 1;    // 资源准备标志(提前加载到SDRAM这里设置为1表示加载完成)

// 局部变量，用于保存转换计算后的电压值 	 
double ADC_Vol; 

static void	X_MeterPointer(HDC hdc, int cx, int cy, int r, u32 color, double dat_val)
{
  double angle;
  int midpoint_x,midpoint_y;
  POINT pt[4];

  angle = (dat_val * 1.427 );    // 计算角度 + 0.785 = 45°
  
  /* 计算与切线平行的一边的中点坐标 */
  midpoint_x =cx - sin(angle) * (r - TriangleLen * 0.866);    // 0.866 = sqrt(3) / 2
  midpoint_y =cy + cos(angle) * (r - TriangleLen * 0.866);

  /* 最远的一个点 */
  pt[0].x = cx - r * sin(angle);
  pt[0].y = cy + r * cos(angle);

  /* 最远一点的左边一点 */
  pt[1].x = midpoint_x - (TriangleLen / 2) * sin(angle - 1.57);    // 1.57 = 3.14/2 = π/2 = 90°
  pt[1].y = midpoint_y + (TriangleLen / 2) * cos(angle - 1.57);

  /* 最远一点的右边一点 */
  pt[2].x = midpoint_x - (TriangleLen / 2) * sin(angle + 1.57);
  pt[2].y = midpoint_y + (TriangleLen / 2) * cos(angle + 1.57);

  pt[3].x = pt[0].x;
  pt[3].y = pt[0].y;


  /* 画三角形 */
  SetBrushColor(hdc,color);
  EnableAntiAlias(hdc, TRUE);
  FillPolygon(hdc,0,0,pt,4);
  EnableAntiAlias(hdc, FALSE);
}

//退出按钮重绘制
static void CollectVoltage_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;
//  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
//  hwnd = ds->hwnd;

//  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //设置画笔色
	}

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -1);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }

}

/*
 * @brief  绘制滚动条
 * @param  hwnd:   滚动条的句柄值
 * @param  hdc:    绘图上下文
 * @param  back_c：背景颜色
 * @param  Page_c: 滚动条Page处的颜色
 * @param  fore_c：滚动条滑块的颜色
 * @retval NONE
*/
static void draw_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc,rc_tmp;
  RECT rc_scrollbar;

	/* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc_tmp);    // 得到按钮的位置

  BitBlt(hdc, rc_tmp.x, rc.y+45/2, rc.w - rc_tmp.x, rc.h/2, hdc_adc_png[hdc_adc_slider], rc_tmp.x, 0, SRCCOPY);

  rc_scrollbar.x = rc_tmp.x;
  rc_scrollbar.y = rc.h/2-4;
  rc_scrollbar.w = rc.w - rc_tmp.x - 15;
  rc_scrollbar.h = 10;
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
  FillRoundRect(hdc, &rc_scrollbar, 4);
}

/*
 * @brief  绘制滚动条
 * @param  hwnd:   滚动条的句柄值
 * @param  hdc:    绘图上下文
 * @param  back_c：背景颜色
 * @param  Page_c: 滚动条Page处的颜色
 * @param  fore_c：滚动条滑块的颜色
 * @retval NONE
*/
static void draw_gradient_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc,rc_tmp;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc_tmp);    // 得到按钮的位置
  
  BitBlt(hdc, rc.x, rc.y+45/2, rc_tmp.x, rc.h/2, hdc_adc_png[hdc_adc_slider], 0, 0, SRCCOPY);

  rc_scrollbar.x = rc.x+15;
  rc_scrollbar.y = rc.h/2-4;
  rc_scrollbar.w = rc_tmp.x;
  rc_scrollbar.h = 8;
   
	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
  FillRoundRect(hdc, &rc_scrollbar, 3);
}

/*
 * @brief  自定义滑动条绘制函数
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void scrollbar_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	HDC hdc_mem;
	RECT rc;
	RECT rc_cli,rc_tmp;;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);   
         
  EnableAntiAlias(hdc, TRUE);

  /* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
  BitBlt(hdc_mem, rc.x, rc.y, rc.w, rc.h, hdc_adc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

	//绘制白色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem, color_bg, RGB888(50, 50, 50), RGB888(255, 255, 255));
	//绘制渐变类型的滚动条
	draw_gradient_scrollbar(hwnd, hdc_mem, color_bg, RGB888(50, 50, 50), RGB888(2, 100, 253));
  EnableAntiAlias(hdc, FALSE);
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   
  
	//绘制滑块
  BitBlt(hdc_mem, rc.x, rc.y, rc.w, rc.h, hdc_adc_png[hdc_adc_slider_btn], 0, 0, SRCCOPY);

  BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);

	//释放内存MemoryDC
	DeleteDC(hdc_mem);
}

/*
 * @brief  重绘透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  if (res_prep == 0)
  {
    SetBrushColor(hdc, MapRGB(hdc, 5, 5, 5));
    FillRect(hdc, &rc);
  }
  else
    BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_adc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));

  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

/*
 * @brief  重绘显示亮度的透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Brigh_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_adc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
  rc.w -= 45;
  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  SetFont(hdc, controlFont_100);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式)
  rc.w += 45;
  rc.y += 17;
  SetFont(hdc, controlFont_48);
  DrawText(hdc, L"H", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式)
  SetFont(hdc, defaultFont);
}

/* 重绘圆形显示区域 */
void Circle_Paint(HWND hwnd, HDC hdc)
{
  char  cbuf[128];
  WCHAR wbuf[128];
  int vertex_x,vertex_y;
  RECT rc = {0, 0, CircleSize, CircleSize};
  int angle = ((225 - (-45))) * ADC_Vol / 3.3;

  EnableAntiAlias(hdc, TRUE);

  BitBlt(hdc, CircleSize/2-270/2, CircleSize/2-270/2, 270, 270, hdc_adc_png[hdc_adc_circle], 0, 0, SRCCOPY);     // 270 是图片的大小
  RotateBitmap(hdc, CircleSize/2, CircleSize/2, &bm_Triangle, angle + 45);
  
  if (ADC_Vol > 0.03)    // 电压太小不画白色小圆圈
  {
    SetBrushColor(hdc, MapARGB(hdc, 255, 200, 200, 200));
    FillArc(hdc, CircleSize/2, CircleSize/2, CircleCenter_2+1, CircleCenter_3-1, -45, angle  - 45);
    /* 计算左边小圆圈的坐标 */
    vertex_x =  CircleSize / 2 - ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * sin(3.14/4); 
    vertex_y =  CircleSize / 2 + ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * cos(3.14/4); 
    FillCircle(hdc, vertex_x, vertex_y, (CircleCenter_3 - CircleCenter_2) / 2 - 1);     // 左边小圆圈
  }

  EnableAntiAlias(hdc, FALSE);

  /* 使用默认字体 */
	SetFont(hdc, defaultFont);

  rc.w = 24*4;
  rc.h = 48;
  rc.x = CircleSize/2 - rc.w/2;
  rc.y = CircleSize/2 - rc.h/2;

  /* 显示电压百分比 */
  SetTextColor(hdc, MapARGB(hdc, 255, 0, 0, 0));
  SetFont(hdc, controlFont_48);
  x_sprintf(cbuf, "%d", (int)(ADC_Vol/3.3*100));    // H -> % x_wsprintf(Backlightwbuf, L"%d", i);
  x_mbstowcs_cp936(wbuf, cbuf, 128);
  rc.w -= 25;
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);    // 绘制文字(居中对齐方式)DT_CENTER
  rc.w += 25;
  rc.y += 4;
  SetFont(hdc, controlFont_32);
  DrawText(hdc, L"H", -1, &rc, DT_VCENTER|DT_RIGHT);    // 绘制文字(居中对齐方式)DT_CENTER

  /* 显示采集到的电压值 */
  SetFont(hdc, defaultFont);
  rc.w = 24*4;
  rc.h = 30;
  rc.y = CircleSize/2 - rc.h/2 + CircleCenter_3;
  x_sprintf(cbuf, "%.2fV", ADC_Vol);
  x_mbstowcs_cp936(wbuf, cbuf, 128);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

static BOOL Update_Circle_Flag = TRUE;

static LRESULT	ADCWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      HWND hwnd_scrolbar;
      SCROLLINFO sif;/*设置滑动条的参数*/
      GetClientRect(hwnd, &rc);
      Rheostat_Init();    // 初始化 ADC

      /*********************亮度调节滑动条******************/
      sif.cbSize = sizeof(sif);
      sif.fMask = SIF_ALL;
      sif.nMin = 0;
      sif.nMax = 100;
      sif.nValue = 50;//初始值
      sif.TrackSize = 90;//滑块值
      sif.ArrowSize = 0;//两端宽度为0（水平滑动条）          
      hwnd_scrolbar = CreateWindow(SCROLLBAR, L"SCROLLBAR_Brigh", WS_OWNERDRAW | WS_VISIBLE,//  
                      GUI_XSIZE + 100, (GUI_YSIZE - TitleHeight * 2) / 2, 600, 90, hwnd, SCROLLBAR_Brigh_ID, NULL, NULL);
      SendMessage(hwnd_scrolbar, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif); 

      // rc.w = 50*3+35;
      // rc.h = 100;
      // rc.x = GUI_XSIZE + GUI_XSIZE / 2 - rc.w / 2;
      // rc.y = TitleHeight-20;

      // Brigh_TEXTBOX_Handle = CreateWindow(TEXTBOX, L"50", WS_OWNERDRAW | WS_VISIBLE | WS_DISABLED, 
      //                           rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Brigh, NULL, NULL);  
      
      // SendMessage(Brigh_TEXTBOX_Handle,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_RIGHT|DT_BKGND);  


//      BOOL res;
//      u8 *pic_buf;
//      u32 pic_size;
//      
//      #if BMP
//      PNG_DEC *png_dec;
//      BITMAP png_bm;
//      #endif 
//      
//      /* 创建电位器提示 HDC */
//      hdc_adc_png[hdc_adc_F429_RP] = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, 350, 340);
//      ClrDisplay(hdc_adc_png[hdc_adc_F429_RP], NULL, 0);
//      res = RES_Load_Content(F429_RP_Name, (char**)&pic_buf, &pic_size);
//      if(res)
//      {
//        #if BMP
//          png_dec = PNG_Open(pic_buf);
//          PNG_GetBitmap(png_dec, &png_bm);
//          DrawBitmap(hdc_adc_png[hdc_adc_F429_RP], 0,0, &png_bm, NULL);
//          PNG_Close(png_dec);
//        #else
//          BMP_Draw(hdc_adc_png[hdc_adc_F429_RP], 0, 0, pic_buf, NULL);
//        #endif     
//      }
//      /* 释放图片内容空间 */
//      RES_Release_Content((char **)&pic_buf);


      /* 创建圆形区域的 HDC */
//      hdc_adc_png[hdc_adc_circle] = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, CircleSize, CircleSize);
//      ClrDisplay(hdc_adc_png[hdc_adc_circle],NULL,0);
//      res = RES_Load_Content(Adc_Circle_Name, (char**)&pic_buf, &pic_size);
//      // res = FS_Load_Content("0:/adc_circle.png", (char**)&pic_buf, &pic_size);
//      if(res)
//      {
//        #if BMP 
//          png_dec = PNG_Open(pic_buf);
//          PNG_GetBitmap(png_dec, &png_bm);
//          DrawBitmap(hdc_adc_png[hdc_adc_circle], CircleSize/2-270/2, CircleSize/2-270/2, &png_bm, NULL);
//          PNG_Close(png_dec);
//        #else
//          BMP_Draw(hdc_adc_png[hdc_adc_circle], CircleSize/2-270/2, CircleSize/2-270/2, pic_buf, NULL);
//        #endif
//      }
//      RES_Release_Content((char **)&pic_buf);     // 释放图片内容空间

      // EnableAntiAlias(hdc_adc_png[hdc_adc_circle], TRUE);

      // SetBrushColor(hdc_adc_png[hdc_adc_circle], MapARGB(hdc_adc_png[hdc_adc_circle], 255, 65, 65, 65));
      
      // /* 计算右边小圆圈的坐标 */
      // vertex_x =  CircleSize / 2 - ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * sin(3.14*7/4); 
      // vertex_y =  CircleSize / 2 + ((CircleCenter_3 - CircleCenter_2) / 2 + CircleCenter_2) * cos(3.14*7/4); 
      // FillCircle(hdc_adc_png[hdc_adc_circle], vertex_x, vertex_y, (CircleCenter_3 - CircleCenter_2) / 2);     // 右边小圆圈
      // FillArc(hdc_adc_png[hdc_adc_circle], CircleSize/2, CircleSize/2, CircleCenter_2, CircleCenter_3, -45, 225);    // 画进度条背景
      // EnableAntiAlias(hdc_adc_png[hdc_adc_circle], FALSE);

      /* 画三角形指针 */
      TrianglePointer_DC = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, TriangleLen, CircleCenter_1 * 2);    // 创建三角形指针内存 DC
      ClrDisplay(TrianglePointer_DC, NULL, 0);
      X_MeterPointer(TrianglePointer_DC, TriangleLen/2, CircleCenter_1, CircleCenter_1-2, MapARGB(TrianglePointer_DC, 255, 250, 20, 20), 0);
      /* 转换成bitmap */
      DCtoBitmap(TrianglePointer_DC,&bm_Triangle);

//      /* 创建滑动条按钮的 HDC */
//      hdc_adc_png[hdc_adc_slider_btn] = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, 90, 90);
//      ClrDisplay(hdc_adc_png[hdc_adc_slider_btn],NULL,0);
//      res = RES_Load_Content(Slider_Button_Name, (char**)&pic_buf, &pic_size);
//      if(res)
//      {
//        #if BMP
//          png_dec = PNG_Open(pic_buf);
//          PNG_GetBitmap(png_dec, &png_bm);
//          DrawBitmap(hdc_adc_png[hdc_adc_slider_btn], 0,0, &png_bm, NULL);
//          PNG_Close(png_dec);
//        #else
//          BMP_Draw(hdc_adc_png[hdc_adc_slider_btn], 0, 0, pic_buf, NULL);
//        #endif 
//      }
//      /* 释放图片内容空间 */
//      RES_Release_Content((char **)&pic_buf);

//      /* 创建滑动条的 HDC */
//      hdc_adc_png[hdc_adc_slider] = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, 600, 45);
//      ClrDisplay(hdc_adc_png[hdc_adc_slider],NULL,0);
//      res = RES_Load_Content(Slider_Name, (char**)&pic_buf, &pic_size);
//      if(res)
//      {
//        #if BMP
//          png_dec = PNG_Open(pic_buf);
//          PNG_GetBitmap(png_dec, &png_bm);
//          DrawBitmap(hdc_adc_png[hdc_adc_slider], 0,0, &png_bm, NULL);
//          PNG_Close(png_dec);
//        #else
//          BMP_Draw(hdc_adc_png[hdc_adc_slider], 0, 0, pic_buf, NULL);
//        #endif 
//      }
//      /* 释放图片内容空间 */
//      RES_Release_Content((char **)&pic_buf);
      
      /* 创建圆形区域的最终 DC */
      hdc_mem = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, CircleSize, CircleSize);
      
      ClrDisplay(hdc_mem, NULL, 0);

      SetTimer(hwnd, 2, 50, TMR_START, NULL);
      
      x_wsprintf(Backlightwbuf, L"%d", 50);

      res_prep = 1;    // 标记资源加载完成
      SetTimer(hwnd, 4, 10, TMR_START | TMR_SINGLE, NULL);

      break;
    }

    case WM_TIMER:
    {
      RECT rc;
      int tmr_id;
       
      tmr_id = wParam;
      GetWindowRect(hwnd, &rc);
      SetForegroundWindow(hwnd);

      if (tmr_id == MOVE_WIN)
      {
        if (AovingDirection == LeftToRight)
        {
          if (rc.x < 0)
          {
            OffsetRect(&rc, (rc.w >> 3), 0);
            rc.x = MIN(rc.x, 0);
            MoveWindow(hwnd, rc.x, rc.y, rc.w, rc.h, TRUE);
          }
          else
          {
            InvalidateRect(MAIN_Handle, NULL, TRUE);
            SetWindowText(Title_Handle,L"ADC一电位器电压显示");
            KillTimer(hwnd, 1);
          }
        }
        else if (AovingDirection == RightToLeft)
        {
          if (rc.x > -800)
          {
            OffsetRect(&rc, -(rc.w >> 3), 0);
            rc.x = MAX(rc.x, -800);
            MoveWindow(hwnd, rc.x, rc.y, rc.w, rc.h, TRUE);
          }
          else
          {
            // RECT indicate_rc;
            // indicate_rc.x =
            InvalidateRect(MAIN_Handle, NULL, TRUE);
            SetWindowText(Title_Handle,L"滑动控件演示");
            KillTimer(hwnd, 1);
          }
        }
      }
      else if (tmr_id == 2)
      {
        RECT rc;
        static double ADC_buff = 0.0;
        double vol_buff = 0.0;
        static uint8_t xC = 0;
        static double ADC_Vol_Old;

        vol_buff =(double) ADC_ConvertedValue/4096*(double)3.3; // 读取转换的AD值
//        GUI_DEBUG("电压值前为：%f", ADC_Vol);
        #if 1

          if (xC++ < 3)
          {
            ADC_buff += vol_buff;
            break;
          }
          else
          {
            ADC_Vol = ADC_buff / ((double)(xC-1));
            ADC_buff = 0;
            xC = 0;
          }

        #else
          
        ADC_Vol = vol_buff;//(double)(((int)( * 10)) / 10.0);

        #endif
      //  GUI_DEBUG("电压值后为：%f", ADC_Vol);
      //  ADC_Vol = (double)(((int)(vol_buff * 100)) / 100.0);
      //  GUI_DEBUG("电压值后为：%f", ADC_Vol);
        if (fabs(ADC_Vol - ADC_Vol_Old) <= 0.01)
        {
//          ADC_Vol_Old = ADC_Vol;    // 更新旧的值
          break;                    // 两次采集数据一样不重绘直接返回
        }
        // GUI_DEBUG(" %fV", ADC_Vol - ADC_Vol_Old);
        ADC_Vol_Old = ADC_Vol;    // 更新旧的值

        rc.x = Circle_X;
        rc.y = Circle_Y;
        rc.w = CircleSize;
        rc.h = CircleSize;
        Update_Circle_Flag = TRUE;

        InvalidateRect(hwnd, &rc, TRUE);
      }
      else if (tmr_id == 4)
      {
        // 重绘父窗口
        InvalidateRect(MAIN_Handle, NULL, TRUE);
        InvalidateRect(GetDlgItem(MAIN_Handle, ID_TEXTBOX_Title), NULL, TRUE);     // 重绘窗口标题
        InvalidateRect(hwnd, NULL, TRUE);    // 重绘子窗口
      }
      
      break;
    }

    case WM_ERASEBKGND:
    {
      
      HDC hdc =(HDC)wParam;
      RECT rc = {0, TitleHeight, GUI_XSIZE, GUI_YSIZE - TitleHeight};

      ScreenToClient(hwnd, (POINT *)&rc, 1);
      BitBlt(hdc, rc.x, rc.y, GUI_XSIZE, rc.h, hdc_adc_bk, 0, TitleHeight, SRCCOPY);

      BitBlt(hdc, 0, 0, 350, 340, hdc_adc_png[hdc_adc_F429_RP], 0, 0, SRCCOPY);

      /* 显示亮度图标 */
      SetFont(hdc, controlFont_48);
      SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
      rc.y = TitleHeight;
      rc.x = GUI_XSIZE + 100;
      rc.w = 70;
      rc.h = TitleHeight;
      DrawText(hdc, L"I", -1, &rc, NULL);//绘制文字(居中对齐方式)

      SetFont(hdc, controlFont_72);
      rc.x = GUI_XSIZE + 630;
      DrawText(hdc, L"I", -1, &rc, NULL);//绘制文字(居中对齐方式)

      SetFont(hdc, defaultFont);   // 恢复默认字体

      return FALSE;
    }
  
    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      RECT rc = {Circle_X, Circle_Y, 2, 2};

      ClientToScreen(hwnd, (POINT *)&rc, 1);

      hdc = BeginPaint(hwnd, &ps);

      if (Update_Circle_Flag == TRUE)    /* 需要更新 */
      {
        Update_Circle_Flag = FALSE;
        ClrDisplay(hdc_mem, NULL, 0);
        Circle_Paint(hwnd, hdc_mem);    /* 绘制圆形显示区域 */
      }

      BitBlt(hdc, Circle_X, Circle_Y, CircleSize, CircleSize, hdc_mem, 0, 0, SRCCOPY);

      rc.w = 50*3+35;
      rc.h = 100;
      rc.x = GUI_XSIZE + GUI_XSIZE / 2 - rc.w / 2;
      rc.y = TitleHeight-20;

      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
      rc.w -= 45;
      SetFont(hdc, controlFont_100);
      DrawText(hdc, Backlightwbuf, -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式)
      rc.w += 45;
      rc.y += 17;
      SetFont(hdc, controlFont_48);
      DrawText(hdc, L"H", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式)
      SetFont(hdc, defaultFont);

      EndPaint(hwnd, &ps);

      break;
    }

    case WM_NOTIFY:
    {
      u16 ctr_id;
      NMHDR *nr;
//      code=HIWORD(wParam);//获取消息的类型
      ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
      nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.      

      if (ctr_id == SCROLLBAR_Brigh_ID)
      {
        NM_SCROLLBAR *sb_nr;
        int i = 0;
        sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
        switch (nr->code)
        {
          case SBN_THUMBTRACK: //R滑块移动
          {
            i = sb_nr->nTrackValue; //获得滑块当前位置值                
            SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, i); //设置进度值

            x_wsprintf(Backlightwbuf, L"%d", i);

            RECT rc;
            rc.w = 35*7;
            rc.h = 100;
            rc.x = GUI_XSIZE + GUI_XSIZE / 2 - rc.w / 2;
            rc.y = TitleHeight-20;
            InvalidateRect(hwnd, &rc, TRUE);//FALSE
            // SetWindowText(Brigh_TEXTBOX_Handle, Backlightwbuf);
          }
          break;
        }
      }
      break;
    } 
 
    
    static int x_move;

    case WM_LBUTTONDOWN:
    {
      int x;
      x = LOWORD(lParam);
      RECT rc;
	  	GetWindowRect(hwnd, &rc);
      rc.x = x;
      ClientToScreen(hwnd, (POINT *)&rc, 1);
      x_move = rc.x;
//      GUI_DEBUG("按下：x = %d",x);
      break;
    }

    static int x_old;
    case WM_MOUSEMOVE:
    {
      int x;
      RECT rc;
      RECT Client_rc;
	  	GetWindowRect(hwnd, &rc);
      GetWindowRect(hwnd, &Client_rc);
      x = LOWORD(lParam);
      
      rc.x = x;
      ClientToScreen(hwnd, (POINT *)&rc, 1);
      OffsetRect(&Client_rc, rc.x - x_move, 0);

      Client_rc.x = MIN(Client_rc.x, 30);
      Client_rc.x = MAX(Client_rc.x, -800 - 30);
      
      MoveWindow(hwnd, Client_rc.x, Client_rc.y, Client_rc.w, Client_rc.h, TRUE);

      x_old = x_move;
      x_move = rc.x;

//      GUI_DEBUG("移动：x = %d", x_old);
      break;
    }

    case WM_LBUTTONUP:
    { 
      int x;
      RECT Client_rc;
      RECT rc;
      
      x = LOWORD(lParam);
      rc.x = x;
      GetWindowRect(hwnd, &Client_rc);
      ClientToScreen(hwnd, (POINT *)&rc, 1);
//      GUI_DEBUG("抬起：x = %d",rc.x);

      if (rc.x - x_old > 0)    // 从左往右滑
      {
        if (Client_rc.x > 0)
        {
          MoveWindow(hwnd, 0, Client_rc.y, Client_rc.w, Client_rc.h, TRUE);
        }
        else 
        {
          SetTimer(hwnd, 1, 5, TMR_START, NULL);
          AovingDirection = LeftToRight;
        }
      }
      else    // 从右往左滑
      {
        if (Client_rc.x < -800)
        {
          MoveWindow(hwnd, -800, Client_rc.y, Client_rc.w, Client_rc.h, TRUE);
        }
        else 
        {
          SetTimer(hwnd, 1, 5, TMR_START, NULL);
          AovingDirection = RightToLeft;
        }
      }
      
      break;
    } 

    case WM_DRAWITEM:    // 按钮重绘
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_ADC_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }   

          case SCROLLBAR_Brigh_ID:
          {
            scrollbar_owner_draw(ds);
            return TRUE;             
          }

          case ID_TEXTBOX_Brigh:
          {
            Brigh_Textbox_OwnerDraw(ds);
            return TRUE;             
          } 
       }
       break;
    }

    case WM_DESTROY:
    {
      

      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
}
 
static LRESULT	CollectVoltage_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
      
      /********************* 创建中间滑动窗口 ***************************/
      WNDCLASS wcex;

      wcex.Tag	 		    = WNDCLASS_TAG;
      wcex.Style			  = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc	= (WNDPROC)ADCWinProc;
      wcex.cbClsExtra		= 0;
      wcex.cbWndExtra		= 0;
      wcex.hInstance		= NULL;
      wcex.hIcon			  = NULL;
      wcex.hCursor		  = NULL;
      
      rc.x = 0;
      rc.y = TitleHeight;
      rc.w = GUI_XSIZE*2;
      rc.h = GUI_YSIZE - TitleHeight * 2;
      // 创建" ADC 采集窗口"的控件.
      ADC_Handle = CreateWindowEx(WS_EX_NOFOCUS, &wcex,L"---",WS_CLIPCHILDREN|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_ADV_WIN,NULL,NULL);
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  740, 17, 36, 36, hwnd, eID_ADC_EXIT, NULL, NULL); 

      rc.w = GUI_XSIZE / 2;
      rc.h = TitleHeight-2;
      rc.x = GUI_XSIZE / 2 - rc.w / 2;
      rc.y = 0;

      Title_Handle = CreateWindow(TEXTBOX, L"ADC一电位器电压显示", WS_VISIBLE | WS_OWNERDRAW, rc.x, rc.y, rc.w, rc.h, hwnd, ID_TEXTBOX_Title, NULL, NULL);//
      SendMessage(Title_Handle, TBM_SET_TEXTFLAG, 0, DT_VCENTER | DT_CENTER);   

      /************************ 加载背景图片 ************************/
//        BOOL res;
//        u8 *jpeg_buf;
//        u32 jpeg_size;
//        JPG_DEC *dec;
//        res = RES_Load_Content(GUI_ADC_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
//        hdc_adc_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
//        if(res)
//        {
//          /* 根据图片数据创建JPG_DEC句柄 */
//          dec = JPG_Open(jpeg_buf, jpeg_size);

//          /* 绘制至内存对象 */
//          JPG_Draw(hdc_adc_bk, 0, 0, dec);

//          /* 关闭JPG_DEC句柄 */
//          JPG_Close(dec);
//        }
//        /* 释放图片内容空间 */
//        RES_Release_Content((char **)&jpeg_buf);

//      SetTimer(hwnd, 3, 10, TMR_SINGLE | TMR_START, NULL);    // 启动一次定时器进行资源加载

      break;
    } 
    case WM_TIMER:
    {

//      RECT rc;
//      int tmr_id;
//       
//      tmr_id = wParam;

//      if (tmr_id == 3)    // 资源加载定时器
//      {
//        /********************* 创建中间滑动窗口 ***************************/
//        WNDCLASS wcex;

//        wcex.Tag	 		    = WNDCLASS_TAG;
//        wcex.Style			  = CS_HREDRAW | CS_VREDRAW;
//        wcex.lpfnWndProc	= (WNDPROC)ADCWinProc;
//        wcex.cbClsExtra		= 0;
//        wcex.cbWndExtra		= 0;
//        wcex.hInstance		= NULL;
//        wcex.hIcon			  = NULL;
//        wcex.hCursor		  = NULL;
//        
//        rc.x = 0;
//        rc.y = TitleHeight;
//        rc.w = GUI_XSIZE*2;
//        rc.h = GUI_YSIZE - TitleHeight * 2;
//        // 创建" ADC 采集窗口"的控件.
//        ADC_Handle = CreateWindowEx(WS_EX_NOFOCUS, &wcex,L"---",WS_CLIPCHILDREN|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_ADV_WIN,NULL,NULL);
//      }
      
      break;
    }

    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc;
      
      GetClientRect(hwnd, &rc);

      if (res_prep == 0)    // 资源加载还没有完成
      {
        SetBrushColor(hdc, MapRGB(hdc, 5, 5, 5));
        FillRect(hdc, &rc);
      }
      else
      {
        BitBlt(hdc, 0, 0, GUI_XSIZE, GUI_YSIZE, hdc_adc_bk, 0, 0, SRCCOPY);                                                   // 绘制标题栏部分背景
        //BitBlt(hdc, 0, GUI_YSIZE - TitleHeight, GUI_XSIZE, TitleHeight, hdc_adc_bk, 0, GUI_YSIZE - TitleHeight, SRCCOPY);       // 绘制下半部分背景
      }

      return FALSE;
    }

    case WM_PAINT:
    {
      HDC hdc;
      RECT rc;
      PAINTSTRUCT ps;
      //  RECT rc = {0,0,800,70};
      //  hdc_mem = CreateMemoryDC(SURF_ARGB4444, 800,70);
       
      hdc = BeginPaint(hwnd, &ps);
      GetClientRect(hwnd, &rc);
      
      if (res_prep == 0)    // 资源加载还没有完成
      {
        SetTextColor(hdc, MapRGB(hdc, 225, 225, 225));
        DrawText(hdc, L"资源加载中请稍等...", -1, &rc, DT_VCENTER | DT_CENTER);    // 绘制文字(居中对齐方式)
      }
      else
      {
        /* 绘制两条直线 */
        // HLine(hdc, 0, TitleHeight-1, GUI_XSIZE);
        // HLine(hdc, 0, GUI_YSIZE - TitleHeight, GUI_XSIZE);

        /* 绘制屏幕底下的指示框 */

        EnableAntiAlias(hdc, TRUE);
        if(AovingDirection == RightToLeft) 
        {
          /* 绘制右边一点 */
          SetBrushColor(hdc, MapRGB(hdc, 220, 220, 220));
          FillCircle(hdc, 415+5, 415+5, 5);
        }
        else
        {
          /* 绘制左边一点 */
          SetBrushColor(hdc, MapRGB(hdc, 220, 220, 220));
          FillCircle(hdc, 376+5, 415+5, 5);
        }
        EnableAntiAlias(hdc, FALSE);
      }

      EndPaint(hwnd, &ps);

      break;
    } 
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_ADC_EXIT:
          {
            CollectVoltage_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }    

          case ID_TEXTBOX_Title:
          {
            Textbox_OwnerDraw(ds);
            return TRUE;             
          } 
       }

       break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型    
      if(code == BN_CLICKED && id == eID_ADC_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }

      break;
    } 

    case WM_LBUTTONUP:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
    }

    case WM_MOUSEMOVE:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);
    }

    case WM_LBUTTONDOWN:
    {
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
    }

    case WM_DESTROY:
    {
      Rheostat_DISABLE();    // 停止ADC的采集
//      DeleteDC(hdc_adc_bk);
//      DeleteDC(hdc_adc_png[hdc_adc_slider_btn]);
//      DeleteDC(hdc_adc_png[hdc_adc_slider]);
//      DeleteDC(hdc_adc_png[hdc_adc_circle]);
//      DeleteDC(hdc_adc_png[hdc_adc_F429_RP]);
      DeleteDC(TrianglePointer_DC);
      DeleteDC(hdc_mem);
      Update_Circle_Flag = TRUE;
//      res_prep = 0;     // 复位资源加载完成标志

      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_ADC_CollectVoltage_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;

	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = CollectVoltage_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//创建主窗口
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER|WS_OVERLAPPED|WS_CLIPCHILDREN,
                              &wcex,
                              L"GUI_ADC_CollectVoltage_Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(MAIN_Handle, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}


