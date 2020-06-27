#include "emXGUI.h"
#include "x_libc.h"

#include "./led/bsp_led.h"  
#include "GUI_AppDef.h"
#include "GUI_RGBLED_DIALOG.h"
#include "Widget.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "./pic_load/gui_pic_load.h"
/* 硬件控制接口 */
extern void TIM_GPIO_Config(void);
extern void TIM_Mode_Config(void);
extern void TIM_RGBLED_Close(void);
extern void SetRGBColor(uint32_t rgb);
extern void SetColorValue(uint8_t r,uint8_t g,uint8_t b);
extern const unsigned char RGBdesktop[];
static COLORREF color_bg;       // 透明控件的背景颜色
//static BITMAP RGBdesktop_0;
/**********************分界线*********************/
HDC hdc_bk = NULL;
HDC hdc_rgbled_checked;

uint8_t SELECT_RGB = 0;

struct leddlg
{
	char col_R;  //R分量值
	char col_G;  //G分量值
	char col_B;  //B分量值
   char led_R;//硬件RGB灯颜色分量值
   char led_G;//硬件RGB灯颜色分量值
   char led_B;//硬件RGB灯颜色分量值
   int colR_ctr;//硬件RGB灯控制位
   int colG_ctr;//硬件RGB灯控制位
   int colB_ctr;//硬件RGB灯控制位
}leddlg_S={0, 0, 0, 0, 0, 0, 1, 1, 1};

icon_S GUI_RGBLED_Icon[18] = 
{
      {"tuichu",           {740, 19, 36, 36},       FALSE},//退出按键
      {"biaotilan",        {100,0,600,70},      FALSE},//APP标题栏
      {"APPHouse",         {425,80,275,275},      FALSE},//APP房子图标
      {"hongdeng",         {105, 382, 66, 66},  FALSE},//红灯图标
      {"lvdeng",           {372, 382, 66, 66},  FALSE},//绿灯图标
      {"landeng",          {638, 382, 66, 66},  FALSE},//蓝灯图标
      {"hongdengscrollbar",{101, 238, 600, 90},  FALSE},//红色滚动条
      {"lvdengscrollbar",  {101, 238, 600, 90},  FALSE},//绿色滚动条
      {"landengscrollbar", {101, 238, 600, 90},  FALSE},//蓝色滚动条 
      
      {"100",    {307, 113, 185, 72}, FALSE},   //文字-百分比
      {"I",        {108, 129, 50, 50}, FALSE},      //文字-小灯
      {"I",        {620, 113, 72, 72}, FALSE},       //文字-大灯
};

RGBLED_DIALOG_s RGBLED_DIALOG =
{
   .RGBLED_Hwnd = NULL,
   .State = TRUE,
   .exit_bt_draw = home_owner_draw,
   .col_R = 0,
   .col_G = 0,
   .col_B = 0,
   
};

static void Delete_DlALOG()
{
  RGBLED_DIALOG.col_R = 0;
  RGBLED_DIALOG.col_G = 0;
  RGBLED_DIALOG.col_B = 0;
  RGBLED_DIALOG.State = 1;
  leddlg_S.colB_ctr = 1;
  leddlg_S.colG_ctr = 1;
  leddlg_S.colR_ctr = 1;
  leddlg_S.led_R = 0;
  leddlg_S.led_G = 0;
  leddlg_S.led_B = 0;
  leddlg_S.col_R = 0;
  leddlg_S.col_G = 0;
  leddlg_S.col_B = 0;
  DeleteDC(RGBLED_DIALOG.hdc_mem);
  DeleteDC(hdc_rgbled_checked);
  TIM_RGBLED_Close();
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
  
  BitBlt(hdc, rc.x, rc.y+45/2, rc_tmp.x, rc.h/2, hdc_adc_png[hdc_adc_slider], 0, 0, SRCCOPY);    // 与ADC使用同一个滑动条

  rc_scrollbar.x = rc.x+15;
  rc_scrollbar.y = rc.h/2-4;
  rc_scrollbar.w = rc_tmp.x;
  rc_scrollbar.h = 8;
   
	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
  FillRoundRect(hdc, &rc_scrollbar, 3);
}

/*
 * @brief  自定义回调函数
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void GUI_RGBLED_ScrollbarOwnerDraw(DRAWITEM_HDR *ds)
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
  BitBlt(hdc_mem, rc.x, rc.y, rc.w, rc.h, RGBLED_DIALOG.hdc_mem, rc_tmp.x, rc_tmp.y, SRCCOPY);

	//绘制白色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem, color_bg, RGB888(50, 50, 50), RGB888(255, 255, 255));
	//绘制渐变类型的滚动条
	draw_gradient_scrollbar(hwnd, hdc_mem, color_bg, RGB888(50, 50, 50), RGB888(201, 220, 236));
  EnableAntiAlias(hdc, FALSE);
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   
  
	//绘制滑块
  BitBlt(hdc_mem, rc.x, rc.y, rc.w, rc.h, hdc_adc_png[hdc_adc_slider_btn], 0, 0, SRCCOPY);

  BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);

	//释放内存MemoryDC
	DeleteDC(hdc_mem);
}

void GUI_RGBLED_HomeOwnerDraw(DRAWITEM_HDR *ds) 
{
  HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));      //设置画笔色
	}

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -1);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }
}
/**
  * @brief  两个灯重绘
  * @param  ds:DRAWITEM_HDR结构体
  * @retval NULL
  */
static void GUI_TEXTLED_OwnerDraw(DRAWITEM_HDR *ds)
{
   HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 
  
   /* 显示亮度图标 */
   if (ds->ID == ID_TEXTBOX_SMALL)
   {
      SetFont(hdc, controlFont_48);
   }
   else if (ds->ID == ID_TEXTBOX_BIG)
   {
      SetFont(hdc, controlFont_72);
   }

   SetTextColor(hdc, MapRGB(hdc, leddlg_S.col_R, leddlg_S.col_G, leddlg_S.col_B));

   DrawText(hdc, L"I", -1, &rc, NULL);//绘制文字(居中对齐方式)
}
/**
  * @brief  百分比文本重绘
  * @param  ds:DRAWITEM_HDR结构体
  * @retval NULL
  */
static void GUI_RGBPCTTEXT_OwnerDraw(DRAWITEM_HDR *ds)
{
   HDC hdc; //控件窗口HDC
   HWND hwnd; //控件句柄 
   RECT rc_cli, rc_tmp;//控件的位置大小矩形
   WCHAR wbuf[128];
   hwnd = ds->hwnd;
	 hdc = ds->hDC; 
//   if(ds->ID ==  ID_BUTTON_START && show_lrc == 1)
//      return;
   //获取控件的位置大小信息
   GetClientRect(hwnd, &rc_cli);
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   //GetClientRect(hwnd, &rc_cli);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, RGBLED_DIALOG.hdc_mem, rc_tmp.x, rc_tmp.y, SRCCOPY);  

	GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字  

   //设置文本的颜色
   SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
   SetFont(hdc, controlFont_48);
   DrawText(hdc, L"H",-1,&rc_cli,DT_BOTTOM|DT_RIGHT);//绘制文字(居中对齐方式)

   SetFont(hdc, controlFont_72);
   rc_cli.w -= 45;
   DrawText(hdc, wbuf,-1,&rc_cli,DT_BOTTOM|DT_RIGHT);//绘制文字(居中对齐方式)
  
}

static void radiobox_owner_draw(DRAWITEM_HDR *ds) // 单选按钮外观
{
	HDC hdc;
	RECT rc, rc_tmp;
  HWND hwnd;
  
  hwnd = ds->hwnd;
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, RGBLED_DIALOG.hdc_mem, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BN_CHECKED)
  { 
    // 按钮被选中状态
    BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_rgbled_checked, 0, 0, SRCCOPY);
  }
}

static	LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
//   static SURFACE *pSurfTop = NULL;
   
   RECT rc;
	switch (msg)
	{
      case WM_CREATE: 
      {
         WCHAR wbuf[128];
         GetClientRect(hwnd, &rc);
         //pSurfTop = CreateSurface(SURF_ARGB4444, rc.w, rc.h, NULL, 0);

          CreateWindow(BUTTON, L"O",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                      GUI_RGBLED_Icon[0].rc.x, GUI_RGBLED_Icon[0].rc.y, 
                      GUI_RGBLED_Icon[0].rc.w, GUI_RGBLED_Icon[0].rc.h, 
                      hwnd, ID_EXIT, NULL, NULL); 

         CreateWindow(TEXTBOX, L"RGB灯控制", WS_TRANSPARENT|WS_VISIBLE, 
                      GUI_RGBLED_Icon[1].rc.x, GUI_RGBLED_Icon[1].rc.y, 
                      GUI_RGBLED_Icon[1].rc.w, GUI_RGBLED_Icon[1].rc.h, 
                      hwnd, ID_TEXTBOX_Title, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_TEXTBOX_Title),TBM_SET_TEXTFLAG,0,
                        DT_SINGLELINE|DT_CENTER|DT_VCENTER); 

         RGBLED_DIALOG.sif_R.cbSize = sizeof(RGBLED_DIALOG.sif_R);
         RGBLED_DIALOG.sif_R.fMask = SIF_ALL;
         RGBLED_DIALOG.sif_R.nMin = 0;
         RGBLED_DIALOG.sif_R.nMax = 255;
         RGBLED_DIALOG.sif_R.nValue = RGBLED_DIALOG.col_R;
         RGBLED_DIALOG.sif_R.TrackSize = 90;
         RGBLED_DIALOG.sif_R.ArrowSize = 0;

         /*创建滑动条--R*/
         CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW |WS_TRANSPARENT| WS_VISIBLE, 
                      GUI_RGBLED_Icon[6].rc.x, GUI_RGBLED_Icon[6].rc.y, 
                      GUI_RGBLED_Icon[6].rc.w, GUI_RGBLED_Icon[6].rc.h, 
                      hwnd, ID_SCROLLBAR_R, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_R), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_DIALOG.sif_R);

         RGBLED_DIALOG.sif_G.cbSize = sizeof(RGBLED_DIALOG.sif_G);
         RGBLED_DIALOG.sif_G.fMask = SIF_ALL;
         RGBLED_DIALOG.sif_G.nMin = 0;
         RGBLED_DIALOG.sif_G.nMax = 255;
         RGBLED_DIALOG.sif_G.nValue = RGBLED_DIALOG.col_G;
         RGBLED_DIALOG.sif_G.TrackSize = 90;
         RGBLED_DIALOG.sif_G.ArrowSize = 0;
         /*创建滑动条--G*/
         CreateWindow(SCROLLBAR, L"SCROLLBAR_G", WS_OWNERDRAW |WS_TRANSPARENT, 
                      GUI_RGBLED_Icon[7].rc.x, GUI_RGBLED_Icon[7].rc.y, 
                      GUI_RGBLED_Icon[7].rc.w, GUI_RGBLED_Icon[7].rc.h, 
                      hwnd, ID_SCROLLBAR_G, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_G), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_DIALOG.sif_G);
         RGBLED_DIALOG.sif_B.cbSize = sizeof(RGBLED_DIALOG.sif_B);
         RGBLED_DIALOG.sif_B.fMask = SIF_ALL;
         RGBLED_DIALOG.sif_B.nMin = 0;
         RGBLED_DIALOG.sif_B.nMax = 255;
         RGBLED_DIALOG.sif_B.nValue = RGBLED_DIALOG.col_B;
         RGBLED_DIALOG.sif_B.TrackSize = 90;
         RGBLED_DIALOG.sif_B.ArrowSize = 0;
         /*创建滑动条--B*/
         CreateWindow(SCROLLBAR, L"SCROLLBAR_B", WS_OWNERDRAW | WS_TRANSPARENT, 
                      GUI_RGBLED_Icon[8].rc.x, GUI_RGBLED_Icon[8].rc.y, 
                      GUI_RGBLED_Icon[8].rc.w, GUI_RGBLED_Icon[8].rc.h,
                      hwnd, ID_SCROLLBAR_B, NULL, NULL);
         SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_B), SBM_SETSCROLLINFO, TRUE, (LPARAM)&RGBLED_DIALOG.sif_B);

         SetColorValue(0, 0, 0);

         /* 单选按钮 */
         CreateWindow(BUTTON, L"-", WS_OWNERDRAW | WS_VISIBLE | BS_RADIOBOX,
                        GUI_RGBLED_Icon[3].rc.x, GUI_RGBLED_Icon[3].rc.y,
                        GUI_RGBLED_Icon[3].rc.w, GUI_RGBLED_Icon[3].rc.h,
                        hwnd, ID_RGB_SELECT_R | (1 << 16), NULL, NULL); 

         /* 单选按钮 */
         CreateWindow(BUTTON, L"-", WS_OWNERDRAW | WS_VISIBLE | BS_RADIOBOX,
                        GUI_RGBLED_Icon[4].rc.x, GUI_RGBLED_Icon[4].rc.y,
                        GUI_RGBLED_Icon[4].rc.w, GUI_RGBLED_Icon[4].rc.h,
                        hwnd, ID_RGB_SELECT_G | (1 << 16), NULL, NULL); 
         
         /* 单选按钮 */
         CreateWindow(BUTTON, L"-", WS_OWNERDRAW | WS_VISIBLE | BS_RADIOBOX,
                        GUI_RGBLED_Icon[5].rc.x, GUI_RGBLED_Icon[5].rc.y,
                        GUI_RGBLED_Icon[5].rc.w, GUI_RGBLED_Icon[5].rc.h,
                        hwnd, ID_RGB_SELECT_B | (1 << 16), NULL, NULL); 

        SendMessage(GetDlgItem(hwnd, ID_RGB_SELECT_R), BM_SETSTATE, BST_CHECKED, 0);    // 默认选中第一个

         // CreateWindow(BUTTON,L"Checkbox5",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
         //              GUI_RGBLED_Icon[9].rc.x, GUI_RGBLED_Icon[9].rc.y, 
         //              GUI_RGBLED_Icon[9].rc.w, GUI_RGBLED_Icon[9].rc.h,
         //              hwnd,ID_CHECKBOX_SW,NULL,NULL);         
         /*创建文本框--红灯*/
         CreateWindow(BUTTON, L"0", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, 
                      GUI_RGBLED_Icon[9].rc.x, GUI_RGBLED_Icon[9].rc.y, 
                      GUI_RGBLED_Icon[9].rc.w, GUI_RGBLED_Icon[9].rc.h,
                      hwnd, ID_TEXTBOX_PCT, NULL, NULL);

         CreateWindow(BUTTON, L"I", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, 
                      GUI_RGBLED_Icon[10].rc.x, GUI_RGBLED_Icon[10].rc.y, 
                      GUI_RGBLED_Icon[10].rc.w, GUI_RGBLED_Icon[10].rc.h,
                      hwnd, ID_TEXTBOX_SMALL, NULL, NULL);

         CreateWindow(BUTTON, L"I", WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, 
                      GUI_RGBLED_Icon[11].rc.x, GUI_RGBLED_Icon[11].rc.y, 
                      GUI_RGBLED_Icon[11].rc.w, GUI_RGBLED_Icon[11].rc.h,
                      hwnd, ID_TEXTBOX_BIG, NULL, NULL);
                    

         BOOL res;
         u8 *jpeg_buf;
         u32 jpeg_size;
         JPG_DEC *dec;
         res = RES_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
         //res = FS_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);    // 资源在 SD 卡
         RGBLED_DIALOG.hdc_mem = CreateMemoryDC(SURF_SCREEN, 800, 480);
         if(res)
         {
            /* 根据图片数据创建JPG_DEC句柄 */
            dec = JPG_Open(jpeg_buf, jpeg_size);

            /* 绘制至内存对象 */
            JPG_Draw(RGBLED_DIALOG.hdc_mem, 0, 0, dec);

            /* 关闭JPG_DEC句柄 */
            JPG_Close(dec);
         }
         /* 释放图片内容空间 */
         RES_Release_Content((char **)&jpeg_buf);

         u8 *pic_buf;
         u32 pic_size;
         PNG_DEC *png_dec;
         BITMAP png_bm;
         HDC hdc;
         /* 创建 HDC */
         hdc_rgbled_checked = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, 66, 66);
         ClrDisplay(hdc_rgbled_checked, NULL, 0);
         //res = FS_Load_Content(GUI_RGBLED_CHECKED_PIC, (char**)&pic_buf, &pic_size);    // 资源在 SD 卡
         res = RES_Load_Content(GUI_RGBLED_CHECKED_PIC, (char**)&pic_buf, &pic_size);     // 资源在外部 FLASH
         if(res)
         {
            png_dec = PNG_Open(pic_buf);
            PNG_GetBitmap(png_dec, &png_bm);
            DrawBitmap(hdc_rgbled_checked, 0, 0, &png_bm, NULL);
            PNG_Close(png_dec);
         }
         /* 释放图片内容空间 */
         RES_Release_Content((char **)&pic_buf);
         
         //DrawBitmap(hdc_bk,0, 0,&RGBdesktop_0,NULL);
         //StretchBlt(RGBLED_DIALOG.hdc_mem, rc.x, rc.y, rc.w, rc.h, hdc_bk, 0, 0,RGBdesktop_0.Width, RGBdesktop_0.Height, SRCCOPY);               
         SetColorValue(leddlg_S.led_R, leddlg_S.led_G, leddlg_S.led_B);
         //GUI_DEBUG("%x%x", leddlg_S.led_R/16, leddlg_S.led_R%16);
         break;
      }

      case WM_NOTIFY: 
      {
         NMHDR *nr;
         WCHAR wbuf[128];
         u16 ctr_id; 
         u16 code,  id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID.
         nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
         if(id == ID_EXIT && code == BN_CLICKED)
         {
            PostCloseMessage(hwnd);
         }

         if (ctr_id == ID_SCROLLBAR_R)
         {
            NM_SCROLLBAR *sb_nr;		
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R滑块移动
               {
                  leddlg_S.col_R = sb_nr->nTrackValue; //获得滑块当前位置值
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_R); //设置位置值
                  x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_R/255.0*100));
                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_SMALL), NULL, TRUE);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_BIG), NULL, TRUE);
                  //RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_PCT), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
               }
               break;
            }
         }
         
         if (ctr_id == ID_SCROLLBAR_G)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //G滑块移动
               {
                  leddlg_S.col_G = sb_nr->nTrackValue; //获得滑块当前位置值
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_G); //设置位置值
                  x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_G/255.0*100));
                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_SMALL), NULL, TRUE);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_BIG), NULL, TRUE);
                  //RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_PCT), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
               }
               break;
            }
         }

         if(ctr_id == ID_SCROLLBAR_B)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //B滑块移动
               {
                  leddlg_S.col_B = sb_nr->nTrackValue; //获得B滑块当前位置值
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, leddlg_S.col_B); //设置B滑块的位置
                  x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_B/255.0*100));
                  SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_PCT), NULL, TRUE);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_SMALL), NULL, TRUE);
                  InvalidateRect(GetDlgItem(hwnd, ID_TEXTBOX_BIG), NULL, TRUE);
                  
                  //RedrawWindow(GetDlgItem(hwnd, ID_TEXTBOX_B_LED), NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
               }
               break;
            }
         }

         /* RGB调节 */
         if (id >= ID_RGB_SELECT_R && id <= ID_RGB_SELECT_B && code ==  BN_CLICKED)
         {
            WCHAR wbuf[128];
            SELECT_RGB = id - ID_RGB_SELECT_R;    // 选择要调节的RGB
            if (SELECT_RGB == 0)
            {
               x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_R/255.0*100));
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_R), SW_SHOW);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_G), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_B), SW_HIDE);
            }
            else if (SELECT_RGB == 1)
            {
               x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_G/255.0*100));
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_R), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_G), SW_SHOW);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_B), SW_HIDE);
            }
            else if (SELECT_RGB == 2)
            {
               x_wsprintf(wbuf, L"%d", (int)(leddlg_S.col_B/255.0*100));
               SetWindowText(GetDlgItem(hwnd, ID_TEXTBOX_PCT), wbuf);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_R), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_G), SW_HIDE);
               ShowWindow(GetDlgItem(hwnd, ID_SCROLLBAR_B), SW_SHOW);
            }
         }

         if(RGBLED_DIALOG.State == TRUE)
         {
            leddlg_S.led_R=leddlg_S.col_R;
            leddlg_S.led_G=leddlg_S.col_G;
            leddlg_S.led_B=leddlg_S.col_B;
            SetColorValue(leddlg_S.led_R, leddlg_S.led_G, leddlg_S.led_B);
         }
         else
         {
            SetColorValue(0, 0, 0);
         }
         break;
      }
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;

         rc = ds->rc;

         switch(ds->ID)
         {
            case ID_EXIT:
            {              
               GUI_RGBLED_HomeOwnerDraw(ds);

               return TRUE;              
            }
            case ID_SCROLLBAR_R:
            case ID_SCROLLBAR_G:
            case ID_SCROLLBAR_B:
            {
               GUI_RGBLED_ScrollbarOwnerDraw(ds);
               return TRUE;
            }
            case ID_RGB_SELECT_R:
            case ID_RGB_SELECT_G:
            case ID_RGB_SELECT_B:
            {
               radiobox_owner_draw(ds);
               return TRUE;
            }
            
            case ID_TEXTBOX_PCT:
            {
               GUI_RGBPCTTEXT_OwnerDraw(ds);
               return TRUE;
            }
            case ID_TEXTBOX_SMALL:
            case ID_TEXTBOX_BIG:
            {
               GUI_TEXTLED_OwnerDraw(ds);
               return TRUE;
            }      
         }
         break;

      }
     case WM_PAINT:
     {
        HDC hdc;
        PAINTSTRUCT ps;
        RECT rc;

        hdc = BeginPaint(hwnd, &ps);

        EndPaint(hwnd, &ps);
        break;
     }      
//      case WM_CTLCOLOR:
//      {
//         u16 id;
//         id =LOWORD(wParam);         
//         CTLCOLOR *cr;
//         cr =(CTLCOLOR*)lParam;
//         
//         switch(id)
//         {
           
//            case ID_TEXTBOX_R_LED:
//            {
//               cr->TextColor = RGB888(leddlg_S.col_R, 0, 0);
//               cr->BackColor = ID_TEXTBOX_R_LED_BackColor;     
//               break;
//            }
//            case ID_TEXTBOX_G_LED:
//            {
//               cr->TextColor = RGB888(0, leddlg_S.col_G, 0);
//               cr->BackColor = ID_TEXTBOX_G_LED_BackColor;     
//               break;
//            }
//            case ID_TEXTBOX_B_LED:
//            {
//               cr->TextColor = RGB888(0, 0, leddlg_S.col_B);
//               cr->BackColor = ID_TEXTBOX_B_LED_BackColor;     
//               break;
//            }              
//            default:
//               return FALSE;
//         }
         
         
//         
//         return TRUE;
//         
//      } 
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         RECT rc =*(RECT*)lParam;
         
         if(RGBLED_DIALOG.State!=FALSE)
            BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, RGBLED_DIALOG.hdc_mem, rc.x, rc.y, SRCCOPY);
         else
         {
            SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
            FillRect(hdc, &rc);             
         }
    


         return TRUE;

      } 
      case WM_DESTROY:
      {        
         Delete_DlALOG();
         // DeleteDC(hdc_bk);
         // DeleteSurface(pSurfTop);
         return PostQuitMessage(hwnd);	
      }          
      default:
         return	DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return	WM_NULL;
}


void	GUI_LED_DIALOG(void)
{
	HWND	hwnd;
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
   //初始化定时器
   //LED_GPIO_Config();
   TIM_GPIO_Config();
   TIM_Mode_Config();
   //SetColorValue(leddlg_S.col_R, leddlg_S.col_R, leddlg_S.col_R);
   
	//创建主窗口
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                        &wcex,
                        L"GUI_LED_DIALOG",
                        WS_CLIPCHILDREN,
                        0, 0, GUI_XSIZE, GUI_YSIZE,
                        NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(hwnd, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}
