










#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ff.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include "./led/bsp_led.h"  
#include "./key/bsp_key.h" 
#include "./pic_load/gui_pic_load.h"
/* 图片资源 */
#define GUI_LED_KEY_PIC    "LED_KEY.jpg"


/* 窗口 ID */


/* 按钮 ID */
enum
{
  eID_LED_KEY_EXIT  = 0x1001,
  eID_LED_USER,
  eID_LED_ONOFF,
  eID_LED_KEY
};

static uint8_t LED_ENTER_TOG=0;//打开RBG灯自动闪烁标志
static HDC bk_hdc;
static uint8_t LED1_ON_FLAG=0,LED2_ON_FLAG=0,LED3_ON_FLAG=0;//不同LED灯刷图标志

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

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, bk_hdc, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
  { //按钮是按下状态
    BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_png[1], 0, 0, SRCCOPY);
    SetTextColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  { //按钮是弹起状态
    BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_clock_png[0], 0, 0, SRCCOPY);
    SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
  }
  
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
  
  /* 显示文本 */
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

static void LED_KEY_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
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

  if ( ds->State & BST_PUSHED )
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

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      Key_GPIO_Config();//初始化按键
      RECT rc;
      GetClientRect(hwnd, &rc); 

			CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
									740, 25, 36, 36, hwnd , eID_LED_KEY_EXIT, NULL, NULL); 
			CreateWindow(BUTTON, L"USER", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
									46,  393, 166, 70, hwnd, eID_LED_USER, NULL, NULL); 
			CreateWindow(BUTTON, L"ON/OFF", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
									317, 393, 166, 70, hwnd, eID_LED_ONOFF, NULL, NULL); 
			CreateWindow(BUTTON, L"KEY", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
									588, 393, 166, 70, hwnd, eID_LED_KEY, NULL, NULL); 

      BOOL res;
      u8 *jpeg_buf;
      u32 jpeg_size;
      JPG_DEC *dec;
      res = RES_Load_Content(GUI_LED_KEY_PIC, (char**)&jpeg_buf, &jpeg_size);
//      res = FS_Load_Content(GUI_LED_KEY_PIC, (char**)&jpeg_buf, &jpeg_size);
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

      SetTimer(hwnd, 0, 20, TMR_START, NULL);
      SetTimer(hwnd, 1, 300, TMR_START, NULL);

      break;
    } 
    case WM_TIMER:
    {
      uint16_t timer_id;
      timer_id = wParam;
			
      if(timer_id == 0)
      {
        if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN))
        {
					LED2_ON_FLAG++;
					LED2_TOGGLE;
					InvalidateRect(hwnd,NULL,FALSE);
        }
				if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN))
        {
					LED3_ON_FLAG++;
          LED3_TOGGLE;
					InvalidateRect(hwnd,NULL,FALSE);
        }
      }
      else if (timer_id == 1)
      {
        LED_ENTER_TOG++;
        switch (LED_ENTER_TOG)
        {
        case 1:
        {
					LED1_ON_FLAG++;
					InvalidateRect(hwnd,NULL,FALSE);
          LED1_TOGGLE;
        }break;

        case 2:
        {
					LED1_ON_FLAG++;
					LED2_ON_FLAG++;
					InvalidateRect(hwnd,NULL,FALSE);
          LED1_TOGGLE;
          LED2_TOGGLE;
        }break;

        case 3:
        {
					LED2_ON_FLAG++;
					LED3_ON_FLAG++;
					InvalidateRect(hwnd,NULL,FALSE);
          LED2_TOGGLE;
          LED3_TOGGLE;
        }break;

        case 4:
        {
					LED3_ON_FLAG++;
					InvalidateRect(hwnd,NULL,FALSE);
          LED3_TOGGLE;
        }break;
        
        default:
          KillTimer(hwnd,timer_id);
          LED_ENTER_TOG = 0;
        }break;
        
      }
      

      break;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      hdc = BeginPaint(hwnd, &ps);
      
      BitBlt(hdc, 0, 0, GUI_XSIZE, GUI_YSIZE, bk_hdc, 0, 0, SRCCOPY);
			
			if(LED1_ON_FLAG >= 2)
			{
				LED1_ON_FLAG =0;
			}
			if(LED2_ON_FLAG >= 2)
			{
				LED2_ON_FLAG =0;
			}
			if(LED3_ON_FLAG >= 2)
			{
				LED3_ON_FLAG =0;
			}
			
			if(LED1_ON_FLAG ==1)
			{
				BitBlt(hdc,  81, 122, 98, 181, hdc_led_key_png[hdc_led_key_1_btn], 0, 0, SRCCOPY);
			}else
			{
				BitBlt(hdc,  81, 122, 98, 181, hdc_led_key_png[hdc_led_key_off_btn], 0, 0, SRCCOPY);
			}
			
			if(LED2_ON_FLAG ==1)
			{
				BitBlt(hdc,  351, 122, 98, 181, hdc_led_key_png[hdc_led_key_2_btn], 0, 0, SRCCOPY);
			}else
			{
				BitBlt(hdc, 351, 122, 98, 181, hdc_led_key_png[hdc_led_key_off_btn], 0, 0, SRCCOPY);
			}
			
			if(LED3_ON_FLAG ==1)
			{
				BitBlt(hdc,  622, 122, 98, 181, hdc_led_key_png[hdc_led_key_3_btn], 0, 0, SRCCOPY);
			}else
			{
				BitBlt(hdc, 622, 122, 98, 181, hdc_led_key_png[hdc_led_key_off_btn], 0, 0, SRCCOPY);
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
          case eID_LED_KEY_EXIT:
          {
            LED_KEY_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }  

          case eID_LED_USER:
          {
            btn_owner_draw(ds);
            return TRUE;   
          }
          case eID_LED_KEY:
          {
            btn_owner_draw(ds);
            return TRUE;   
          }
          case eID_LED_ONOFF:
          {
            btn_owner_draw(ds);
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
      if(code == BN_CLICKED && id == eID_LED_KEY_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }
     if(code == BN_CLICKED && id == eID_LED_USER)
      {
				LED1_ON_FLAG++;
        LED1_TOGGLE;//红色灯翻转
				InvalidateRect(hwnd,NULL,FALSE);
        break;
      }
      if(code == BN_CLICKED && id == eID_LED_ONOFF)
      {
				LED2_ON_FLAG++;
        LED2_TOGGLE;//绿色灯翻转
				InvalidateRect(hwnd,NULL,FALSE);
        break;
      }
			
			if(code == BN_CLICKED && id == eID_LED_KEY)
      {
				LED3_ON_FLAG++;
				LED3_TOGGLE;
				InvalidateRect(hwnd,NULL,FALSE);
        break;
      }
      break;
    } 

    case WM_DESTROY:
    {
      DeleteDC(bk_hdc);
			LED_RGBOFF;
			LED1_ON_FLAG=0;
			LED2_ON_FLAG=0;
			LED3_ON_FLAG=0;
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_LED_KEY_Dialog(void)
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
                              L"GUI_LED_KEY_Dialog",
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

