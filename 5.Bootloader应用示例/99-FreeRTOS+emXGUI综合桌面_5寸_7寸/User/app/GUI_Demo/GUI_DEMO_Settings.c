#include "emXGUI.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include <stdlib.h>
#include "./pic_load/gui_pic_load.h"
#include "CListMenu.h"
#include "./sys_update/sys_update.h" 

/* 图片资源名 */
//#define GUI_SETTINGS_BACKGROUNG_PIC      "settingsdesktop.jpg"        // 800*480

typedef enum 
{ 
  /****************** 按钮控件 ID 值 *******************/
  ID_SETTINGS_EXIT = 0x1000,      // 退出按钮
  ID_SETTINGS_DET,                // 关于开发板
	ID_SETTINGS_THEME,              // 主题选择
  ID_SETTINGS_UPDATE,             // 系统升级
  ID_DET_EXIT,                // 关于开发板
  /***************** 文本控件 ID 值 *********************/
  ID_SETTINGS_TITLE,              // 标题
  ID_SETTINGS_THEMEINFO,          // 主题提示
  /***************** 子窗口 ID 值 *********************/
  ID_DET_WIN,
}set_id_t;

typedef struct{
	WCHAR *icon_name;    // 图标名
	RECT rc;             // 位置信息
	set_id_t id;         // 按钮ID
}set_icon_t;

#define ICON_BTN_NUM     4     // 按钮数量

//图标管理数组
const set_icon_t set_icon[] = {

  /* 按钮 */
  {L"-",           {740,  22,  36,  36}, ID_SETTINGS_EXIT},      // 0. 退出按钮
  {L"关于开发板",  { 18,  93, 782,  36}, ID_SETTINGS_DET},       // 1. 关于开发板
  {L"1",           {725, 142,  65,  30}, ID_SETTINGS_THEME},     // 2. 主题选择
  {L"系统升级",    { 18, 178, 782,  36}, ID_SETTINGS_UPDATE},    // 3. 
  {L"设置",        {100,  0,  600,  80}, ID_SETTINGS_TITLE},     // 4. 
  {L"主题",        { 18, 135, 100,  42}, ID_SETTINGS_THEMEINFO}, // 5. 
};

extern uint8_t Theme_Flag;   // 主题标志
extern HWND	hwnd_home;
extern HWND	app_hwnd_desktop;

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

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -2);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }
}

static void det_exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
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
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
  {
    SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  {
    SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
  }
	
  SetFont(hdc, controlFont_48);
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
}

/*
 * @brief  重绘关于开发板按钮
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void det_button_OwnerDraw(DRAWITEM_HDR *ds)
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
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  SetFont(hdc, defaultFont);
  if (ds->State & BST_PUSHED)
  {
    SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  {
    SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
  }
	
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
  rc.w -= 10;
  SetFont(hdc, controlFont_32);
  DrawText(hdc, L"C", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式)
  SetPenColor(hdc, MapRGB(hdc, 220, 220, 220));
  HLine(hdc, rc.x, rc.y+rc.h-1, rc.x+rc.w+10);
}

/*
 * @brief  重绘主题选择按钮
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void theme_button_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
	HDC hdc, hdc_temp;
  RECT rc, rc_tmp;
  RECT rc_tmp1 = {0, 0, ds->rc.h-4, ds->rc.h-4};
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

	/* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  hdc_temp = CreateMemoryDC(SURF_ARGB8888, rc_tmp1.h, rc_tmp1.h);
  ClrDisplay(hdc_temp, NULL, 0);

  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  SetTextColor(hdc_temp, MapRGB(hdc_temp, 10, 10, 10));
  SetBrushColor(hdc_temp, MapRGB(hdc_temp, 250, 250, 250));

  EnableAntiAlias(hdc_temp, ENABLE);
  FillCircle(hdc_temp, rc_tmp1.h/2, rc_tmp1.h/2, rc_tmp1.h/2);    // 绘制圆
  EnableAntiAlias(hdc_temp, DISABLE);

  DrawText(hdc_temp, wbuf, -1, &rc_tmp1, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)

  SetBrushColor(hdc, MapRGB(hdc, 1, 153, 255));

  EnableAntiAlias(hdc, ENABLE);
  FillRoundRect(hdc, &rc, MIN(rc.w/2, rc.h/2));    // 绘制圆角矩形
  
  
  if (wbuf[0] == L'1')
  {
    BitBlt(hdc, rc.x+2, rc.y+2, rc_tmp1.w, rc_tmp1.h, hdc_temp, 0, 0, SRCCOPY);
  }
  else 
  {
    BitBlt(hdc, rc.w-2 - rc_tmp1.w, rc.y+2, rc_tmp1.w, rc_tmp1.h, hdc_temp, 0, 0, SRCCOPY);
  }
  EnableAntiAlias(hdc, DISABLE);

  DeleteDC(hdc_temp);
}

/*
 * @brief  重绘文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void text_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
	HDC hdc;
  RECT rc, rc_tmp;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

	/* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));

  if (ds->ID == ID_SETTINGS_TITLE)
  {
    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
  }
  else
  {
    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
  }

}

static LRESULT DetWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ 
   switch(msg){
      case WM_CREATE:
      {
          CreateWindow(BUTTON, L"F", WS_OWNERDRAW | WS_VISIBLE,
                        18, 19, 30, 45, hwnd, ID_DET_EXIT, NULL, NULL); 

        break;
      }    
      
      case WM_NOTIFY:
      {
         u16 code,  id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型   

         //发送单击
        if(code == BN_CLICKED)
        {
          if (id == ID_DET_EXIT)
          {
            PostCloseMessage(hwnd);    // 发送关闭窗口的消息
          }
        }

        break;
      }

      //重绘制函数消息
      case WM_DRAWITEM:
      {
        DRAWITEM_HDR *ds;
        ds = (DRAWITEM_HDR*)lParam;        
        if(ds->ID == ID_DET_EXIT)
        {
          det_exit_owner_draw(ds);
          return TRUE;
        }

        return FALSE;
      }
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
        RECT rc = {18, 93, 782-10, 42};
        RECT rc1 = {100, 0, 600, 80};
        HDC hdc;

        //开始绘制
        hdc = BeginPaint(hwnd, &ps); 

        BitBlt(hdc, 0, 0, GUI_XSIZE, GUI_YSIZE, hdc_clock_bk, 0,0, SRCCOPY);

        SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
        SetPenColor(hdc, MapRGB(hdc, 220, 220, 220));

        DrawText(hdc, L"关于开发板", -1, &rc1, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)

        DrawText(hdc, L"硬件版本", -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
        DrawText(hdc, L"V2", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+782);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"开发板型号", -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
        DrawText(hdc, L"野火-挑战者", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+782);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"储存空间", -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
        DrawText(hdc, L"1MB+32MB", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+782);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"运行内存", -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
        DrawText(hdc, L"192KB+32MB", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+782);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"emXGUI版本", -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
        DrawText(hdc, L"V1.0.0", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+782);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"FreeRTOS版本", -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
        DrawText(hdc, L"V9.0.0", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+782);

        OffsetRect(&rc,0,rc.h);
        DrawText(hdc, L"处理器", -1, &rc, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
        DrawText(hdc, L"STM32F429", -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式) 
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+782);
        
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+782);
        
        EndPaint(hwnd, &ps);
        break;
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

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}
HWND hwnd_d;
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ 
   switch(msg){
      case WM_CREATE:
      {
//        u8 *jpeg_buf;
//        u32 jpeg_size;
//        JPG_DEC *dec;
//        BOOL res = NULL;

//        res = RES_Load_Content(GUI_SETTINGS_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
//        //res = FS_Load_Content(GUI_SETTINGS_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
//        hdc_am_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
//        if(res)
//        {
//          /* 根据图片数据创建JPG_DEC句柄 */
//          dec = JPG_Open(jpeg_buf, jpeg_size);

//          /* 绘制至内存对象 */
//          JPG_Draw(hdc_am_bk, 0, 0, dec);

//          /* 关闭JPG_DEC句柄 */
//          JPG_Close(dec);
//        }
//        /* 释放图片内容空间 */
//        RES_Release_Content((char **)&jpeg_buf);

        for (uint8_t xC=0; xC<ICON_BTN_NUM; xC++)     //  按钮
        {
          /* 循环创建按钮 */
          CreateWindow(BUTTON, set_icon[xC].icon_name,  WS_OWNERDRAW | WS_VISIBLE,
                        set_icon[xC].rc.x, set_icon[xC].rc.y,
                        set_icon[xC].rc.w, set_icon[xC].rc.h,
                        hwnd, set_icon[xC].id, NULL, NULL); 
        }

        if (Theme_Flag == 0)
        {
          SetWindowText(GetDlgItem(hwnd, ID_SETTINGS_THEME), L"1");
        }
        else if  (Theme_Flag == 1)
        {
          SetWindowText(GetDlgItem(hwnd, ID_SETTINGS_THEME), L"2");
        }
        
        for (uint8_t xC=ICON_BTN_NUM; xC<ICON_BTN_NUM+2; xC++)     //  按钮
        {
          /* 循环创建文本框 */
          CreateWindow(TEXTBOX, set_icon[xC].icon_name,  WS_OWNERDRAW | WS_VISIBLE,
                        set_icon[xC].rc.x, set_icon[xC].rc.y,
                        set_icon[xC].rc.w, set_icon[xC].rc.h,
                        hwnd, set_icon[xC].id, NULL, NULL); 
        }

        break;
      }

      case WM_TIMER:
      {


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
          if (id == ID_SETTINGS_EXIT)
          {
            PostCloseMessage(hwnd);    // 发送关闭窗口的消息
          }
          else if (id == ID_SETTINGS_DET)
          {
            WNDCLASS wcex;
						RECT rc;

						wcex.Tag = WNDCLASS_TAG;
						wcex.Style = CS_HREDRAW | CS_VREDRAW;
						wcex.lpfnWndProc = (WNDPROC)DetWinProc;
						wcex.cbClsExtra = 0;
						wcex.cbWndExtra = 0;
						wcex.hInstance = NULL;
						wcex.hIcon = NULL;
						wcex.hCursor = NULL;

						rc.x = 0;
						rc.y = 0;
						rc.w = GUI_XSIZE;
						rc.h = GUI_YSIZE;

						CreateWindow(&wcex, L"---", WS_VISIBLE, 
                         rc.x, rc.y, rc.w, rc.h, hwnd, ID_DET_WIN, NULL, NULL);
          }
          else if (id == ID_SETTINGS_UPDATE)
          {
            gui_sys_update_dialog();
          }
          else if (id == ID_SETTINGS_THEME)
          {
            WCHAR wbuf[3];
            HWND  wnd = GetDlgItem(hwnd, ID_SETTINGS_THEME);

            GetWindowText(wnd, wbuf, 3);
            if (wbuf[0] == L'1')
            {
              Theme_Flag = 1;
              SendMessage(GetDlgItem(hwnd_home, 0x1000), MSG_SET_BGCOLOR, COLOR_DESKTOP_BACK_GROUND_HEX, NULL);    // 0x1000 是home 的列表ID
              SetWindowText(wnd, L"2");
            }
            else
            {
              SendMessage(GetDlgItem(hwnd_home, 0x1000), MSG_SET_BGCOLOR, 1, NULL);           // 设置为 1 时 使用图片作为背景  0x1000 是home 的列表ID
              Theme_Flag = 0;
              SetWindowText(wnd, L"1");
            }
            
          }
        }

        break;
      }

      //重绘制函数消息
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_SETTINGS_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if(ds->ID == ID_SETTINGS_DET || ds->ID == ID_SETTINGS_UPDATE)
         {
            det_button_OwnerDraw(ds);
            return TRUE;
         }
         else if(ds->ID == ID_SETTINGS_THEME)
         {
            theme_button_OwnerDraw(ds);
            return TRUE;
         }
         else if(ds->ID == ID_SETTINGS_TITLE || ds->ID == ID_SETTINGS_THEMEINFO)
         {
            text_OwnerDraw(ds);
            return TRUE;
         }
         

        return FALSE;
      }
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
        RECT rc = {18, 135, 782, 42};
        HDC hdc;

        //开始绘制
        hdc = BeginPaint(hwnd, &ps); 
        SetPenColor(hdc, MapRGB(hdc, 220, 220, 220));
        HLine(hdc, rc.x, rc.y+rc.h, rc.x+rc.w);
        
        EndPaint(hwnd, &ps);
        break;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        RECT rc =*(RECT*)lParam;

        BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_bk, rc.x, rc.y, SRCCOPY);

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

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_Settings_DIALOG(void)
{ 	
	WNDCLASS	wcex;
  HWND hwnd;
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
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,//
                                    &wcex,
                                    L"GUI Settings DIALOG",
                                    WS_VISIBLE,
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


