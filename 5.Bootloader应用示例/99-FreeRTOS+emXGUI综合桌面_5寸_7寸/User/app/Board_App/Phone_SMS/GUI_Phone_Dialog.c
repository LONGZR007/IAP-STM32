#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"

/* 来电信号量 */
GUI_SEM *Call_Sem;
TaskHandle_t* CallCallMonitorHandle;    // 来电监测任务控制块

/* 通话信息 */
typedef struct 
{
	u8 Flag;
	WCHAR Status[128];
  WCHAR PhoneNum[128];
}CallInfo_t;

static u16 id_tbl[]=
{
	L'1', L'2', L'3',
	L'4', L'5', L'6',
	L'7', L'8', L'9',
	L'*', L'0', L'#',

};

static const WCHAR *str_tbl[]=
{
	L"1", L"2", L"3",
	L"4", L"5", L"6",
	L"7", L"8", L"9",
	L"*", L"0", L"#",

};

static WCHAR phone_num[12] = {L'1',L'1',L'2'};

#define	NUM_BTN_COUNT	(sizeof(id_tbl)/sizeof(id_tbl[0]))
  
/* 控件 ID 定义 */
enum
{   
  /* 按钮 ID */
  eID_Phone_EXIT  = 0,
  eID_Phone_CONN,
  eID_Phone_DEL,
  eID_Phone_INNUM,
  eID_Call_EXIT,
  eID_Call_EXIT1,    // 来电接电话后的按钮
  eID_Call_WIN,
  eID_Call_ANSWER,
  eID_Call_STATUS,
  eID_Call_TIME,
  eID_Call_PHONENUM,

};

uint8_t CallFlag = 0;    // 通话标志

/*
 * @brief  退出按钮重绘制
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Phone_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
	RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

//	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 120, 120, 120));      //设置文字色
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 26, 197, 93));
	}
  
  SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
  FillRect(hdc, &rc);

  SetPenSize(hdc, 2);

  InflateRect(&rc, -11, -23);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, 63);
    rc.y += 9;
  }
}

/*
 * @brief  圆形按钮重绘制
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Phone_CircleButton_OwnerDraw(DRAWITEM_HDR *ds, COLOR_RGB32 bk_colous)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	// RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	//	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));

	if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));      //设置文字色
	}
	else
	{ //按钮是弹起状态

		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	}

	SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB888(hdc, bk_colous));
	FillCircle(hdc, rc.w / 2, rc.h / 2, rc.h / 2);
  
  SetFont(hdc, controlFont_64);
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  rc.x -= 2;
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)

  SetFont(hdc, defaultFont);    // 恢复默认字体
}

/*
 * @brief  圆形按钮重绘制
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Phone_DelButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	// RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	//	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));

	if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));      //设置文字色
	}
	else
	{ //按钮是弹起状态

		SetTextColor(hdc, MapRGB(hdc, 20, 20, 20));
	}

	SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
	FillRect(hdc, &rc);
  
  SetFont(hdc, controlFont_64);
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  rc.y += 4;
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)

  SetFont(hdc, defaultFont);    // 恢复默认字体
}

/*
 * @brief  重绘电话号码的文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Fillet_Textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	GetClientRect(hwnd, &rc);//得到控件的位置

	SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
	FillRoundRect(hdc, &rc, 10);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)
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
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	GetClientRect(hwnd, &rc);//得到控件的位置

	SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
	FillRect(hdc, &rc);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
//  rc.w -= 45;
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

static LRESULT	DialWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static uint32_t TimeCount = 0;       // 通话时长计时
	switch (msg)
	{
    case WM_CREATE:
    {
      RECT rc;
      CallInfo_t	*CallInfo;

      CallInfo = (CallInfo_t *)lParam;

      GetClientRect(hwnd, &rc);

      if (CallInfo->Flag == 1)    // 拨号
      {
        CreateWindow(BUTTON, L"N", WS_VISIBLE | WS_OWNERDRAW, 370, 304, 75, 75, hwnd, eID_Call_EXIT,   NULL, NULL);
      }
      else if (CallInfo->Flag == 2)    // 来电
      {
        CreateWindow(BUTTON, L"N", WS_OWNERDRAW, 370, 304, 75, 75, hwnd, eID_Call_EXIT1, NULL, NULL);
        CreateWindow(BUTTON, L"N", WS_VISIBLE | WS_OWNERDRAW, 268, 304, 75, 75, hwnd, eID_Call_EXIT,   NULL, NULL);
        CreateWindow(BUTTON, L"N", WS_VISIBLE | WS_OWNERDRAW, 456, 304, 75, 75, hwnd, eID_Call_ANSWER, NULL, NULL);
        SetTimer(hwnd, 1, 300, TMR_START, NULL);    // 启动铃声定时器
      }
      
      CreateWindow(TEXTBOX, CallInfo->Status, WS_VISIBLE | WS_OWNERDRAW, 350, 54, 100, 30, hwnd, eID_Call_STATUS, NULL, NULL);
      CreateWindow(TEXTBOX, CallInfo->PhoneNum, WS_VISIBLE | WS_OWNERDRAW, 300, 132, 201, 30, hwnd, eID_Call_PHONENUM, NULL, NULL);
      CreateWindow(TEXTBOX, L"00:00", WS_VISIBLE | WS_OWNERDRAW, 346, 160, 108, 30, hwnd, eID_Call_TIME, NULL, NULL);

      SetTimer(hwnd, 0, 1000, TMR_START, NULL);

      break;
    }
    
    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // 定时器 ID

      if (tmr_id == 0)    // 每秒定时器
      {
        if(IsNOCARRIER() == SIM900A_TRUE)    // 对方挂断了电话
        {
          CallFlag = 0;
          SIM900A_HANGOFF();         // 挂电话
          SIM900A_CLEAN_RX();        // 清除接收缓存
          BEEP_OFF;                  // 关闭蜂鸣器
          PostCloseMessage(hwnd);    // 发送关闭窗口的消息
        }
        
        if(sim900a_cmd_check("\r\nNO ANSWER") == SIM900A_TRUE)    // 对方超时没接电话
        {
          CallFlag = 0;
          SIM900A_HANGOFF();         // 挂电话
          SIM900A_CLEAN_RX();        // 清除接收缓存
          BEEP_OFF;                  // 关闭蜂鸣器
          PostCloseMessage(hwnd);    // 发送关闭窗口的消息
        }
        
        if(sim900a_cmd_check("\r\n+COLP:") == SIM900A_TRUE)    // 对方接听了电话(这里只使用了部分参数)
        {
          SIM900A_CLEAN_RX();    // 清除接收缓存
          TimeCount = 0;         // 对方接听了电话，重新开始计时
        }
        
        TimeCount++;
        
        WCHAR wbuf[20];
        
        x_wsprintf(wbuf, L"%02d:%02d", TimeCount/60, TimeCount%60);
        SetWindowText(GetDlgItem(hwnd, eID_Call_TIME), wbuf);
      }
      else if (tmr_id == 1)    // 铃声定时器
      {
        BEEP_TOGGLE;    // 反正蜂鸣器状态
      }
      break;
    }

    case WM_ERASEBKGND:
    {
      HDC hdc = (HDC)wParam;
      RECT rc;

      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
      FillRect(hdc, &rc);
      return FALSE;
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);

      EndPaint(hwnd, &ps);
      break;
    }

    case WM_DRAWITEM:
    {
      DRAWITEM_HDR *ds;
      ds = (DRAWITEM_HDR*)lParam;
      switch (ds->ID)
      {
        case eID_Call_EXIT1:
        case eID_Call_EXIT:
        {
          Phone_CircleButton_OwnerDraw(ds, RGB888(240, 0, 0));
          return TRUE;
        }

        case eID_Call_ANSWER:
        {
          Phone_CircleButton_OwnerDraw(ds, RGB888(26, 197, 93));
          return TRUE;
        }

        case eID_Call_STATUS:
        case eID_Call_TIME:
        case eID_Call_PHONENUM:
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
      id = LOWORD(wParam);//获取消息的ID码
      code = HIWORD(wParam);//获取消息的类型    

      switch (code)
      {
        case BN_CLICKED:
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case eID_Call_EXIT:
            case eID_Call_EXIT1:
            {
              CallFlag = 0;
              SIM900A_HANGOFF();         // 挂电话
              SIM900A_CLEAN_RX();        // 清除接收缓存
              BEEP_OFF;                  // 关闭蜂鸣器
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;
            
            /* 接听按钮按下 */
            case eID_Call_ANSWER:
            {
              TimeCount = 0;    // 接电话
              SIM900A_HANGON();      // 接听来电
              SIM900A_CLEAN_RX();    // 清除接收缓存
              KillTimer(hwnd, 1);    // 删除来电铃声定时器
              BEEP_OFF;              // 关闭蜂鸣器

              ShowWindow(GetDlgItem(hwnd, eID_Call_ANSWER), SW_HIDE);    // 隐藏接听按钮
              ShowWindow(GetDlgItem(hwnd, eID_Call_EXIT),  SW_HIDE);     // 隐藏靠左退出按钮
              ShowWindow(GetDlgItem(hwnd, eID_Call_EXIT1), SW_SHOW);     // 显示中间退出按钮
            }
            break;
          }
        }
        break;
      }

      break;
    }

    case WM_DESTROY:
    {
      TimeCount = 0;    // 清零通话时长
      BEEP_OFF;         // 关闭蜂鸣器
      return PostQuitMessage(hwnd);
    }

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return WM_NULL;

}

extern int SelectDialogBox(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops);

static LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc, m_rc[12];
      
      GetClientRect(hwnd, &rc);
      
      /* 初始化 GSM 模块 */
      if(sim900a_init()!= SIM900A_TRUE)
      {  
        SetTimer(hwnd, 0, 0, TMR_START|TMR_SINGLE, NULL);        
      }
      // else
      // {
      //   SetTimer(hwnd, 1, 500, TMR_START, NULL); 
      // }
      SIM900A_CLEAN_RX();//清除接收缓存

      InflateRectEx(&rc, -3, -112, -3, -101);
      MakeMatrixRect(m_rc, &rc, 0, 0, 3, 4);

      for(int i=0; i<NUM_BTN_COUNT; i++)
      {
        CreateWindow(BUTTON, str_tbl[i], WS_VISIBLE,m_rc[i].x,m_rc[i].y,m_rc[i].w,m_rc[i].h,hwnd,id_tbl[i],NULL,NULL);
      }
	    
      CreateWindow(BUTTON, L"返回",	WS_VISIBLE|WS_OWNERDRAW, 76,  391, 75, 75, hwnd, eID_Phone_EXIT, NULL, NULL);
      CreateWindow(BUTTON, L"N",	WS_VISIBLE|WS_OWNERDRAW, 363, 391, 75, 75, hwnd, eID_Phone_CONN, NULL, NULL);
      CreateWindow(BUTTON, L"M",	WS_VISIBLE|WS_OWNERDRAW, 651, 391, 75, 75, hwnd, eID_Phone_DEL,  NULL, NULL);
      CreateWindow(TEXTBOX, L"112", WS_VISIBLE|WS_OWNERDRAW, 5, 18, 793, 75, hwnd, eID_Phone_INNUM, NULL, NULL);
      
      break;
    } 
    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // 定时器 ID

      if (tmr_id == 0)    // GSM 模块初始化错误
      {
        RECT RC;
        MSGBOX_OPTIONS ops;
        const WCHAR *btn[] ={L"确认",L"取消"};      //对话框内按钮的文字

        ops.Flag =MB_ICONERROR;
        ops.pButtonText =btn;
        ops.ButtonCount =2;
        RC.w = 300;
        RC.h = 200;
        RC.x = (GUI_XSIZE - RC.w) >> 1;
        RC.y = (GUI_YSIZE - RC.h) >> 1;
        SelectDialogBox(hwnd, RC, L"没有检测到GSM模块\n请重新检查连接。", L"错误", &ops);    // 显示错误提示框
        PostCloseMessage(hwnd);                                                              // 发送关闭窗口的消息
      }
      else if(0)    // 放在任务里跑了
      {
        char num[20]={0};
        
        if(IsRing(num)== SIM900A_TRUE && CallFlag == 0)
        {
          CallFlag = 1;     // 标记来电
          WNDCLASS wcex;
          RECT rc;
          CallInfo_t CallInfo;
          WCHAR buf[128];

          /* 拨打电话号码 */
          x_mbstowcs_cp936(buf, num, 40);
          x_wstrcpy(CallInfo.PhoneNum, buf);

          CallInfo.Flag = 2;                            // 来电提醒
          x_wstrcpy(CallInfo.Status, L"来电提醒");

          wcex.Tag = WNDCLASS_TAG;
          wcex.Style = CS_HREDRAW | CS_VREDRAW;
          wcex.lpfnWndProc = (WNDPROC)DialWinProc;
          wcex.cbClsExtra = 0;
          wcex.cbWndExtra = 0;
          wcex.hInstance = NULL;
          wcex.hIcon = NULL;
          wcex.hCursor = NULL;

          rc.x = 0;
          rc.y = 0;
          rc.w = GUI_XSIZE;
          rc.h = GUI_YSIZE;

          CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE, rc.x, rc.y, rc.w, rc.h, hwnd, eID_Call_WIN, NULL, &CallInfo);
        }
      }

      break;
    }
    
    case WM_ERASEBKGND:
    {
      HDC hdc = (HDC)wParam;
      RECT rc;

      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc, MapRGB(hdc, 242, 242, 242));
      FillRect(hdc, &rc);
      return FALSE;
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      BeginPaint(hwnd, &ps);
      
      EndPaint(hwnd, &ps);
      break;
    }
    
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Phone_EXIT:
          {
            Phone_ExitButton_OwnerDraw(ds);
            return TRUE;             
          } 
          
          case eID_Phone_DEL:
          {
            Phone_DelButton_OwnerDraw(ds);    // 重绘退格按钮
            return TRUE;  
          }
          
          case eID_Phone_CONN:
          {
            Phone_CircleButton_OwnerDraw(ds, RGB888(26, 197, 93));   // 重绘拨号按钮
            return TRUE;             
          }    

          case eID_Phone_INNUM:
          {
            Fillet_Textbox_OwnerDraw(ds);
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

		switch (code)
		{
			case BN_CLICKED:
			{
				switch (id)
				{
					/* 退出按钮按下 */
					case eID_Phone_EXIT:
					{
						PostCloseMessage(hwnd);    // 发送关闭窗口的消息
					}
					break;

					/* 接通按钮按下 */
					case eID_Phone_CONN:
					{
						WNDCLASS wcex;
						RECT rc;
						CallInfo_t CallInfo;
            char TextBuf[12];
            
            CallFlag = 1;     // 标志通话中
            
            /* 拨打电话号码 */
            GetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num, 12);
            x_wcstombs_cp936(TextBuf ,phone_num, 12);
            
            if (*phone_num == NULL)    // 第一个号码为空，则说明没有号码
            {
              break;
            }
//            sim900a_tx_printf("AT+COLP=1\r");    // 这条语句放在了初始化中
            sim900a_call(TextBuf);

						CallInfo.Flag = 1;    // 拨号
						x_wstrcpy(CallInfo.Status, L"正在拨号");
            x_wstrcpy(CallInfo.PhoneNum, phone_num);

						wcex.Tag = WNDCLASS_TAG;
						wcex.Style = CS_HREDRAW | CS_VREDRAW;
						wcex.lpfnWndProc = (WNDPROC)DialWinProc;
						wcex.cbClsExtra = 0;
						wcex.cbWndExtra = 0;
						wcex.hInstance = NULL;
						wcex.hIcon = NULL;
						wcex.hCursor = NULL;

						rc.x = 0;
						rc.y = 0;
						rc.w = GUI_XSIZE;
						rc.h = GUI_YSIZE;

						CreateWindow(&wcex, L"---", WS_CLIPCHILDREN | WS_VISIBLE, 
                         rc.x, rc.y, rc.w, rc.h, hwnd, eID_Call_WIN, NULL, &CallInfo);
					}
					break;

					/* 退格按钮按下 */
					case eID_Phone_DEL:
					{
						u8 Len = 0;
						Len = GetWindowTextLength(GetDlgItem(hwnd, eID_Phone_INNUM));
						if (Len >= 1)
						{
							GetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num, 11);
							*(phone_num + Len - 1) = L'\0';
							SetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num);
						}
						
					}
					break;

					/* 退格按钮按下 */
					case	L'0':
					case	L'1':
					case	L'2':
					case	L'3':
					case	L'4':
					case	L'5':
					case	L'6':
					case	L'7':
					case	L'8':
					case	L'9':
					case	L'*':
					case	L'#':
					{
						WCHAR wbuf[2] = {id, '\0'};

						if (GetWindowTextLength(GetDlgItem(hwnd, eID_Phone_INNUM)) < 11)
						{
							GetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num, 11);        // 得到原文本
							x_wstrcat(phone_num, wbuf);                                             // 追加新文本
							SetWindowText(GetDlgItem(hwnd, eID_Phone_INNUM), phone_num);            // 设置窗口的文本
						}
					}
					break;
				}
			}
			break;
		}
		break;

    } 

    case WM_DESTROY:
    {
      // USART_ITConfig(GSM_USART, USART_IT_RXNE, DISABLE);	
      // USART_Cmd(GSM_USART, DISABLE);
      // GSM_USART_CLKCMD(GSM_USART_CLK, DISABLE);
      SIM900A_CLEAN_RX();
      CallFlag = 0;
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

/*
 * @brief  电话 APP 窗口
 * @param  NONE
 * @retval NONE
*/
void GUI_Phone_Dialog(void)
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
                              L"GUI Phone Dialog",
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

/*
 * @brief  来电显示窗口
 * @param  num[]:来电电话号码
 * @retval NONE
*/
void GUI_PhoneCall_Dialog(char num[])
{

  CallFlag = 1;     // 标记来电
  WNDCLASS wcex;
//  RECT rc;
  CallInfo_t CallInfo;
  WCHAR buf[128];

  /* 拨打电话号码 */
  x_mbstowcs_cp936(buf, num, 40);
  x_wstrcpy(CallInfo.PhoneNum, buf);

  CallInfo.Flag = 2;                            // 来电提醒
  x_wstrcpy(CallInfo.Status, L"来电提醒");
	
	MSG msg;
  HWND MAIN_Handle;
	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)DialWinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//创建主窗口
	MAIN_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                              &wcex,
                              L"GUI PhoneCall Dialog",
                              WS_VISIBLE|WS_CLIPCHILDREN,
                              0, 0, GUI_XSIZE, GUI_YSIZE,
                              NULL, NULL, NULL, &CallInfo);
   //显示主窗口
	ShowWindow(MAIN_Handle, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, MAIN_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}

/*
 * @brief  后台来电监测任务
 * @param  num[]:来电电话号码
 * @retval NONE
*/
void PhoneCallMonitorTask(void *p)
{
  char num[20]={0};

  Call_Sem = GUI_SemCreate(0, 1);    // 创建一个二值信号量
  BEEP_GPIO_Config();                // 初始化蜂鸣器

  /* 初始化 GSM 模块 */
  if(sim900a_init() != SIM900A_TRUE)           // 初始化串口并检测 GSM 模块
  {  
    GUI_ERROR("没有检测到 GSM 模块");
//     vTaskSuspend(GUI_GetCurThreadHandle());     // GSM 初始化失败挂起当前任务
  }

  while(TRUE)
  {
    GUI_SemWait(Call_Sem, 0xFFFFFFFF);    // 等待信号量

    if(IsRing(num)== SIM900A_TRUE && CallFlag == 0)    // 判断是否是来电
    {
      CallFlag = 1;
      GUI_DEBUG("主人来电话了");
      GUI_PhoneCall_Dialog(num);    // 创建来电显示窗口
      SIM900A_CLEAN_RX();           // 清除接收缓存
    }
    else
    {
      GUI_msleep(1200);             // 不是来电，留些时间给其他任务处理接收
      SIM900A_CLEAN_RX();           // 清除接收缓存
    }
  }
}


