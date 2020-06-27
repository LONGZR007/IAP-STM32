/**
  ******************************************************************************
  * @file    iap_arch.c
  * @author  long
  * @version V1.0
  * @date    2019-11-23
  * @brief   xmodem 对外接口文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
 
#include "emXGUI.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "./sys_update/sys_update.h"   
#include <stdio.h>
#include "./usart/bsp_debug_usart.h"
#include "./lcd/bsp_lcd.h"

/* 图片资源名 */
//#define GUI_SETTINGS_BACKGROUNG_PIC      "settingsdesktop.jpg"        // 800*480

typedef enum 
{ 
  /****************** 按钮控件 ID 值 *******************/
  ID_SYS_UPDATE_EXIT = 0x1000,      // 退出按钮
  ID_SYS_UPDATE_DET,                // 关于开发板
	ID_SYS_UPDATE_THEME,              // 主题选择
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
  {L"系统升级",    {18, 178, 782,  36},   ID_SETTINGS_UPDATE},   // 3. 
  {L"设置",        {100, 0,  600,  80},  ID_SETTINGS_TITLE},      // 4. 
  {L"主题",        {18, 135, 100, 42},   ID_SETTINGS_THEMEINFO},   // 5. 
};

static uint32_t xmodem_actual_flash_address = FLASH_APP_ADDR;       /* 写入的地址. */

/**
 * @brief   Xmodem 发送一个字符的接口.
 * @param   ch ：发送的数据
 * @return  返回发送状态
 */
int x_transmit_ch(uint8_t ch)
{
	Usart_SendByte(DEBUG_USART, ch);
	
	return 0;
}

/**
 * @brief   Xmodem 擦除要保存接收数据的扇区.
 * @param   address ：根据地址来擦除扇区
 * @return  返回当前扇区剩余的大小
 */
uint32_t x_receive_flash_erasure(uint32_t address)
{
  sector_t sector_info;
  if (erasure_sector(address, 1))    // 擦除当前地址所在扇区
  {
    return 0;    // 擦除失败
  }

  sector_info = GetSector(address);    // 得到当前扇区的信息

  return (sector_info.size + sector_info.start_addr - address);     // 返回当前扇区剩余大小
}

/**
  * @brief   Xmodem 将接受到的数据保存到flash.
  * @param  start_address ：要写入的起始地址
  * @param  *data : 需要保存的数据
	* @param  len ：长度
  * @return 写入状态
 */
int x_receive_flash_writea(uint32_t start_address, const void *data, uint32_t len)
{
  if (flash_write_data(start_address, (uint8_t *)data, len) == 0)    // 擦除当前地址所在扇区
  {
    return 0;    // 写入成功
  }
  else
  {
    return -1;    // 写入失败
  }
}

/**
 * @brief   文件数据接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_data_callback(void *ptr, char *file_data, uint32_t w_size)
{
  static uint32_t sector_size = 0;    /* 扇区剩余大小. */
  static uint32_t recv_size = 0;    /* 扇区剩余大小. */
  uint8_t buff[128];
  
  /* 当前扇区不够了擦除下一个. */
  if (sector_size <= w_size)
  {
    sector_size += x_receive_flash_erasure(xmodem_actual_flash_address + sector_size);

    if (0 == sector_size)
    {
      return -1;
    }
  }
  
  if (flash_write_data(xmodem_actual_flash_address, (uint8_t *)file_data, w_size) == 0)    // 写入数据
  {
    xmodem_actual_flash_address += w_size;
    recv_size += w_size;
    sprintf((char*)buff, "                 已接收：%d字节！", recv_size);
    LCD_DisplayStringLine_EN_CH(LINE(3), buff);
    return 0;
  }
  else 
  {
    return -1;
  }
}

/**
 * @brief   文件接收完成回调.
 * @param   *ptr: 控制句柄.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_callback(void *ptr)
{
//  printf("开始运行 App！\r\n");
//	iap_jump_app(FLASH_APP_ADDR);
  LCD_DisplayStringLine_EN_CH(LINE(5), "                 应用程序接收完成！");
  
  return 0;
}

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

void gui_sys_update_dialog(void)
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
                                    L"GUI sys update DIALOG",
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
