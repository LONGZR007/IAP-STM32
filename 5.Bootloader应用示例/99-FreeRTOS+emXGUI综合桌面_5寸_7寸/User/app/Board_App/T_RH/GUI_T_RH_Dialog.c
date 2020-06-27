#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"

/* 图片资源 */
#define GUI_HUMITURE_BACKGROUNG_PIC    "humiture_desktop.jpg"


/* 窗口 ID */
#define ID_TEXTBOX_Title    0x00     // 标题栏
#define ID_TEXTBOX_T        0x01     // 温度显示
#define ID_TEXTBOX_RH       0x02     // 湿度显示

#define CircleCenter_1    (98)     // 三角形旋转半径
#define CircleCenter_2    (125)    // 圆弧进度条半径（小）
#define CircleCenter_3    (CircleCenter_2 + 10)    //  不大于 CircleSize / 2

/* 移动方向标志 */
#define LeftToRight    0
#define RightToLeft    1
#define MOVE_WIN       1

#define Pointer1_W    100
#define Pointer2_H    100
#define PANEL_W       408
#define PANEL_H       408

/* 按钮 ID */
#define eID_T_RH_EXIT    0

#define CircleSize    285    // 圆形显示区域的大小
#define Circle_X      460    // 圆形显示区域的位置
#define Circle_Y      (27)   // 圆形显示区域的位置

#define TitleHeight    70    // 标题栏的高度

#define TriangleLen    20    // 三角形的边长

DHT11_Data_TypeDef DHT11_Data;

HWND T_Handle;
HWND RH_Handle;

static HDC bk_hdc;
uint8_t Pointerstyle = 0;
#if 0
static void	X_MeterPointer(HDC hdc,int cx,int cy,int r,u32 color,int st_angle,int angle_size,int dat_size,int dat_val,int style)
{
	int angle; 
	POINT pt[8];

	////

	if(angle_size>360)	angle_size=360;

	angle = st_angle+(dat_val*angle_size)/dat_size;
	angle = MIN(angle,st_angle+angle_size);

	angle -= 90;

//	GUI_DEBUG("%d", angle);
  
  if(style==0)
  {
    pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
    pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

    pt[1].x = (int)(cx + sin((angle+90)*3.14/180)*(r>>5));
    pt[1].y = (int)(cy - cos((angle+90)*3.14/180)*(r>>5));

    pt[2].x = (int)(cx + sin((angle+180)*3.14/180)*(r>>5));
    pt[2].y = (int)(cy - cos((angle+180)*3.14/180)*(r>>5));

    pt[3].x = (int)(cx + sin((angle+270)*3.14/180)*(r>>5));
    pt[3].y = (int)(cy - cos((angle+270)*3.14/180)*(r>>5));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;


    SetBrushColor(hdc,color);
    EnableAntiAlias(hdc, TRUE);
    FillPolygon(hdc,0,0,pt,5);
    EnableAntiAlias(hdc, FALSE);
    //SetPenColor(hdc,color);
    //AA_DrawPolygon(hdc,0,0,pt,5); //?????????
  }


  if(style==1)
  {
    pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
    pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

    pt[1].x = (int)(cx + sin((angle+4)*3.14/180)*(r-(r>>2)));
    pt[1].y = (int)(cy - cos((angle+4)*3.14/180)*(r-(r>>2)));

    pt[2].x = cx;
    pt[2].y = cy;

    pt[3].x = (int)(cx + sin((angle-4)*3.14/180)*(r-(r>>2)));
    pt[3].y = (int)(cy - cos((angle-4)*3.14/180)*(r-(r>>2)));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;


    FillPolygon(hdc,0,0,pt,5); //?????????

  }


  if(style==2)
  {
    POINT pt[7];

    pt[0].x	 = (int)(cx + sin(angle*3.14/180)*r);
    pt[0].y	 = (int)(cy - cos(angle*3.14/180)*r);

    pt[1].x = (int)(cx + sin((angle+6)*3.14/180)*(r-(r>>2)));
    pt[1].y = (int)(cy - cos((angle+6)*3.14/180)*(r-(r>>2)));

    pt[2].x = (int)(cx + sin((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));
    pt[2].y = (int)(cy - cos((angle+1)*3.14/180)*(r-(r>>2)+(r>>4)));

    pt[3].x = (int)cx;
    pt[3].y = (int)cy;

    pt[4].x = (int)(cx + sin((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));
    pt[4].y = (int)(cy - cos((angle-1)*3.14/180)*(r-(r>>2)+(r>>4)));

    pt[5].x = (int)(cx + sin((angle-6)*3.14/180)*(r-(r>>2)));
    pt[5].y = (int)(cy - cos((angle-6)*3.14/180)*(r-(r>>2)));

    pt[6].x = pt[0].x;
    pt[6].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,7);

  }

  if(style==3) 
  {
    POINT pt[7];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

    pt[0].x	 = (int)(x1 + sin((angle-90)*3.14/180)*(2));
    pt[0].y	 = (int)(y1 - cos((angle-90)*3.14/180)*(2));

    pt[1].x	 = x1;
    pt[1].y	 = y1;

    pt[2].x	 = (int)(x1 + sin((angle+90)*3.14/180)*(2));
    pt[2].y	 = (int)(y1 - cos((angle+90)*3.14/180)*(2));


    pt[3].x	 = (int)(cx + sin((angle+90)*3.14/180)*(2));
    pt[3].y	 = (int)(cy - cos((angle+90)*3.14/180)*(2));

    pt[4].x	 = cx;
    pt[4].y	 = cy;

    pt[5].x	 = (int)(cx + sin((angle-90)*3.14/180)*(2));
    pt[5].y	 = (int)(cy - cos((angle-90)*3.14/180)*(2));

    pt[6].x = pt[0].x;
    pt[6].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,7);

  }

  if(style==4) 
  {
    POINT pt[7];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

    pt[0].x	 = (int)(cx + sin((angle-60)*3.14/180)*(5));
    pt[0].y	 = (int)(cy - cos((angle-60)*3.14/180)*(5));

    pt[1].x	 = cx;
    pt[1].y	 = cy;

    pt[2].x	 = (int)(cx + sin((angle+60)*3.14/180)*(5));
    pt[2].y	 = (int)(cy - cos((angle+60)*3.14/180)*(5));


    pt[3].x	 = (int)(pt[2].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
    pt[3].y	 = (int)(pt[2].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

    pt[4].x	 = x1;
    pt[4].y	 = y1;

    pt[5].x	 = (int)(pt[0].x + sin((angle+0)*3.14/180)*(r-(r>>2)));
    pt[5].y	 = (int)(pt[0].y - cos((angle+0)*3.14/180)*(r-(r>>2)));

    pt[6].x = pt[0].x;
    pt[6].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,7);

  }

  if(style==5) 
  {
    POINT pt[5];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*(r-(r>>5)));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*(r-(r>>5)));

    pt[0].x	 = x1;
    pt[0].y	 = y1;

    pt[1].x = (int)(cx + sin((angle+12)*3.14/180)*(r-(r>>1)));
    pt[1].y = (int)(cy - cos((angle+12)*3.14/180)*(r-(r>>1)));

    pt[2].x = (int)cx;
    pt[2].y = (int)cy;

    pt[3].x = (int)(cx + sin((angle-12)*3.14/180)*(r-(r>>1)));
    pt[3].y = (int)(cy - cos((angle-12)*3.14/180)*(r-(r>>1)));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,5);

  }

  if(style==6) 
  {
    POINT pt[5];
    int x1,y1;

    x1 = (int)(cx + sin((angle-0)*3.14/180)*((r*15)>>4));
    y1 = (int)(cy - cos((angle-0)*3.14/180)*((r*15)>>4));

    pt[0].x	 = x1;
    pt[0].y	 = y1;

    pt[1].x = (int)(cx + sin((angle+15)*3.14/180)*((r*4)>>4));
    pt[1].y = (int)(cy - cos((angle+15)*3.14/180)*((r*4)>>4));

    pt[2].x = (int)cx;
    pt[2].y = (int)cy;

    pt[3].x = (int)(cx + sin((angle-15)*3.14/180)*((r*4)>>4));
    pt[3].y = (int)(cy - cos((angle-15)*3.14/180)*((r*4)>>4));

    pt[4].x = pt[0].x;
    pt[4].y = pt[0].y;

    FillPolygon(hdc,0,0,pt,5);

  }

	////

	SetBrushColor(hdc,fr_color);
	FillCircle(hdc,cx,cy,r>>3);
}
#endif

//退出按钮重绘制
static void T_RH_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
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

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
  rc.w -= 45;
  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  SetFont(hdc, controlFont_32);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}


static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 

      /* DHT11初始化 */
	    DHT11_GPIO_Config();
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  740, 25, 36, 36, hwnd, eID_T_RH_EXIT, NULL, NULL); 

      rc.w = GUI_XSIZE / 2;
      rc.h = TitleHeight-2;
      rc.x = GUI_XSIZE / 2 - rc.w / 2;
      rc.y = 0; 

      BOOL res;
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
      res = RES_Load_Content(GUI_HUMITURE_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      //res = FS_Load_Content(GUI_HUMITURE_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
      bk_hdc = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
      if(res)
      {
        /* 根据图片数据创建JPG_DEC句柄 */
        dec = JPG_Open(jpeg_buf, jpeg_size);

        /* 绘制至内存对象 */
        JPG_Draw(bk_hdc, 0, 0, dec);

        /* 关闭JPG_DEC句柄 */
        JPG_Close(dec);
      }
      /* 释放图片内容空间 */
      RES_Release_Content((char **)&jpeg_buf);

      SetTimer(hwnd, 0, 2000, TMR_START, NULL);

      break;
    } 
    case WM_TIMER:
    {
      if(Read_DHT11(&DHT11_Data) == SUCCESS)
      {
        InvalidateRect(hwnd, NULL, FALSE);    // 重绘窗口
      }
      else
      {
        /* 出错处理 */
      }

      break;
    }

    case WM_PAINT:
    {
      HDC hdc;
//      HDC hdc_pointer;
      PAINTSTRUCT ps;
      WCHAR wbuf[128];
      RECT rc;
//      hdc_pointer = CreateMemoryDC(SURF_SCREEN, PANEL_W, PANEL_H);
      hdc = BeginPaint(hwnd, &ps);
      
      BitBlt(hdc, 0, 0, GUI_XSIZE, GUI_YSIZE, bk_hdc, 0, 0, SRCCOPY);

//      BitBlt(hdc_pointer, 0, 0, PANEL_W, PANEL_H, bk_hdc, 369, 64, SRCCOPY);
      
//      EnableAntiAlias(hdc, TRUE);
//      X_MeterPointer(hdc_pointer, PANEL_W/2, PANEL_H/2, 191, MapRGB(hdc_pointer,250,20,20), 30, 298, 100, DHT11_Data.temp_int+DHT11_Data.temp_deci*0.1, Pointerstyle);
//      X_MeterPointer(hdc_pointer, PANEL_W/2, PANEL_H/2, 138, MapRGB(hdc_pointer,20,250,20), -58, 298, 100, DHT11_Data.humi_int, Pointerstyle);
//      EnableAntiAlias(hdc, FALSE);
      
      /* 温度数值显示 */
      rc.w = 141;
      rc.h = 82;
      rc.x = 529;
      rc.y = 131;
      
      x_wsprintf(wbuf, L"%d.%d", DHT11_Data.temp_int,DHT11_Data.temp_deci);
      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
      SetFont(hdc, controlFont_72);
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式)

      /* 显示湿度数值 *///defaultFont
      rc.y = 259;
      rc.w = 81;
      x_wsprintf(wbuf, L"%d", DHT11_Data.humi_int);//.%d//,DHT11_Data.humi_deci
      DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式)

//      BitBlt(hdc, 369, 64, PANEL_W, PANEL_H, hdc_pointer, 0, 0, SRCCOPY);
      
      EndPaint(hwnd, &ps);
//      DeleteDC(hdc_pointer);
      break;
    }
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_T_RH_EXIT:
          {
            T_RH_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  

          case ID_TEXTBOX_T:
          case ID_TEXTBOX_RH:
          {
            Brigh_Textbox_OwnerDraw(ds);
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
      if(code == BN_CLICKED && id == eID_T_RH_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }

      break;
    } 

    case WM_DESTROY:
    {
      memset(&DHT11_Data, 0, sizeof(DHT11_Data));
      DeleteDC(bk_hdc);
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_T_RH_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;
  HWND MAIN_Handle;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = win_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//创建主窗口
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI_T_RH_Dialog",
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


