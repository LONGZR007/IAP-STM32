#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"

#define COLOR_SMS_BACK_GROUND         50,50,50
extern int SelectDialogBox(HWND hwndParent, RECT rc, const WCHAR *pText, const WCHAR *pCaption, const MSGBOX_OPTIONS *ops);
extern int number_input_box(int x, int y, int w, int h,
							              const WCHAR *pCaption,
							              WCHAR *pOut,
							              int MaxCount,
							              HWND hwndParent);
  
/* 控件 ID 定义 */
enum
{   
  /* 按钮 ID */
  eID_SMS_EXIT  = 0,
  eID_SMS_CLEAR,
  eID_SMS_SEND,
  eID_SMS_DEL,
  eID_SMS_CONTENT,
  eID_SMS_NUMBER,
  eID_SMS_LIST,

};

/* 用户自定义消息 */
enum eMSG 
{
	eMSG_READ_TEXT = WM_USER + 1,
	eMSG_INIT_ERROR,
};

typedef struct
{
	WCHAR	cTime[20];
	WCHAR cState[2];
  uint16_t	Address;
}ListEXData_t;

/*
 * @brief  读短信内容
 * @param  hListWnd: 列表框句柄
 * @retval NONE
*/
void Read_Text(HWND hListWnd)
{
	u32 i = 0;
	u32 xC = 1;
	WCHAR wbuf[40];
  char messagename[40];
//  char messagename1[20]={'收'};
  char *wNumber;
  char *wTime;
  uint16_t ListData = 0;    // 最高位 0:发，1:收，后15位为短信地址
  uint8_t Temp = 0;
  
  sim900a_tx_printf("AT+CNMI=2,1\r");
  SIM900A_DELAY(100);  
  sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
  SIM900A_DELAY(100); 
  sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");     //"GSM"字符集
  SIM900A_DELAY(100); 
  
  wNumber = (char *)GUI_VMEM_Alloc(200);
  wTime = (char *)GUI_VMEM_Alloc(3*1024);

  SIM900A_CLEAN_RX();
  Temp = ReadMessageInfo(xC, wNumber, wTime);

	while((Temp) != 0)
	{
    hexuni2gbk(wNumber, messagename);	
    strcat(messagename, wTime);                             // 拼接上时间
    
//    GUI_DEBUG("number->(%s)\n",messagename);
    
    x_mbstowcs_cp936(wbuf, messagename, sizeof(wbuf));	    // 将Ansi字符转换成GUI的unicode字符.
    
		//在Listbox中增加一个Item项，记录文件名和文件属性.
		i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
		SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
    
		/* 通过设置Item项的DATA值来记录短信的属性(用于区分是发送的还是接收的). */
    ListData = xC++;        // 保存短信地址
    if (Temp != 3)
    {
      ListData |= 1 << 15;    // 标记为收
    }
    else
    {
      ListData &= ~(1 << 15);    // 标记为发
    }
		SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)ListData);
    
    SIM900A_CLEAN_RX();
    
    Temp = ReadMessageInfo(xC, wNumber, wTime);
	}
  
  GUI_VMEM_Free(wNumber);
  GUI_VMEM_Free(wTime);
}

/*
 * @brief  退出按钮重绘
 * @param  ds: 自定义绘制结构体
 * @retval NONE
*/
static void SMS_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
#if 0
	HWND hwnd;
	HDC hdc;
	RECT rc;
	// RECT rc_top={0,0,800,70};
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
	FillRect(hdc, &rc); //用矩形填充背景

	SetBrushColor(hdc, MapRGB(hdc, COLOR_DESKTOP_BACK_GROUND));

	FillCircle(hdc, rc.x + rc.w, rc.y, rc.w);

	if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字色
	}
	else
	{ //按钮是弹起状态

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	/* 使用控制图标字体 */
	SetFont(hdc, controlFont_64);

	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	rc.y = -10;
	rc.x = 16;
	DrawText(hdc, wbuf, -1, &rc, NULL);//绘制文字(居中对齐方式)

  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);
#else
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
#endif
}

// 圆角按钮重绘制
static void SMS_FilletButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	rc = ds->rc;

	SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB(hdc, 22, 155, 213));
	EnableAntiAlias(hdc, TRUE);
	FillRoundRect(hdc, &rc, 5);
	EnableAntiAlias(hdc, FALSE);

	if (ds->State & BST_PUSHED)
	{
		/* 按钮是按下状态 */
		SetTextColor(hdc, MapRGB(hdc, 120, 120, 120));       //设置文字色
		OffsetRect(&rc, 1, 1);
	}
	else
	{
		/* 按钮是弹起状态 */
		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	}

	GetWindowText(hwnd, wbuf, 128);                          //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);    //绘制文字(居中对齐方式)
}

/*
 * @brief  重绘显示亮度的透明文本
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

	SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
	FillRect(hdc, &rc);

	SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
	EnableAntiAlias(hdc, TRUE);
	FillRoundRect(hdc, &rc, 8);
	EnableAntiAlias(hdc, FALSE);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)
}

#if 0
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
	rc.w -= 45;
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}
#endif
/*
 * @brief  重绘列表
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	int i,count,cursel;
	WCHAR wbuf[128];
  WCHAR Time[40];
  WCHAR *Temp;
	POINT pt;
  uint16_t ListData = 0;

	hwnd =ds->hwnd;
	hdc =ds->hDC;

//	hdc =CreateMemoryDC(SURF_ARGB4444,ds->rc.w,ds->rc.h); //创建一个内存DC来绘图.

	rc =ds->rc;

	SetBrushColor(hdc,MapRGB(hdc,220,220,240));
	FillRect(hdc,&ds->rc);
  
  if (!SendMessage(hwnd,LB_GETCOUNT,0,0))
  {
    /* 列表为空，显示提示信息然后直接返回 */
    DrawText(hdc, L"列表加载中，\r\n或还没有信息！", -1, &rc, DT_CENTER|DT_VCENTER);
    return;
  }

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);

	while(i<count)
	{
    ListData =SendMessage(hwnd,LB_GETITEMDATA,i,0);     // 获得Item项的DATA值(在增加项目是时设置的DATA值...).
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > ds->rc.h)
		{
			break;
		}
		
    SetBrushColor(hdc,MapRGB(hdc,22,155,213));
    InflateRectEx(&rc, -1, 0, -1, 0);
    FillCircle(hdc, rc.x+rc.h/2, rc.y+rc.h/2, rc.h/2);

		SetTextColor(hdc,MapRGB(hdc,50,10,10));

		if(i==cursel)
		{
			SetTextColor(hdc,MapRGB(hdc,250,10,10));

			SetBrushColor(hdc,MapRGB(hdc,250,250,250));
			FillRect(hdc,&rc);

		}
		else
		{
			SetTextColor(hdc,MapRGB(hdc,10,10,10));
		}

		SendMessage(hwnd,LB_GETTEXT,i,(LPARAM)wbuf);
    if ((ListData >> 15) & 1)
    {
      DrawText(hdc, L"收",-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);    // 显示收
    }
    else
    {
      DrawText(hdc, L"发",-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);    // 显示发
    }
    rc.x += rc.h;
    rc.w -= rc.h;
    
    Temp = x_wstrstr(wbuf, L"/");
    if (Temp != NULL)
    { 
      /* 有时间,把电话号码和时间拆开 */
      x_wstrcpy(Time, Temp+1);
      *Temp = '\0';
      DrawText(hdc,Time,-1,&rc,DT_SINGLELINE|DT_RIGHT|DT_VCENTER);     // 显示时间
    }
    
		DrawText(hdc,wbuf,-1,&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);      // 显示电话号码
    
    SetPenColor(hdc,MapRGB(hdc,10,10,10));
    HLine(hdc, rc.x, rc.y + rc.h - 1, rc.x + rc.w);                  // 画一条线

		i++;
	}

//	BitBlt(ds->hDC,0,0,ds->rc.w,ds->rc.h,hdc,0,0,SRCCOPY); //将内存DC的内容输出到窗口(DC)中.
//	DeleteDC(hdc);
}


static LRESULT	win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc, m_rc[12];
      
      GetClientRect(hwnd, &rc);

      /* 初始化 GSM 模块 */
      if (sim900a_init() != SIM900A_TRUE)//
      {
        SetTimer(hwnd, 2, 1, TMR_START|TMR_SINGLE, NULL);    // 初始化失败开启错误提示定时器
      }

      //InflateRectEx(&rc, -3, -112, -3, -101);
      //MakeMatrixRect(m_rc, &rc, 0, 0, 3, 4);
	    
      CreateWindow(BUTTON, L"O",	WS_VISIBLE|WS_OWNERDRAW|WS_TRANSPARENT, 740, 22, 36, 36, hwnd, eID_SMS_EXIT, NULL, NULL);
      CreateWindow(BUTTON, L"清除",	WS_VISIBLE|WS_OWNERDRAW, 590, 433, 84, 40, hwnd, eID_SMS_CLEAR, NULL, NULL);
      CreateWindow(BUTTON, L"删除全部短信",	WS_VISIBLE|WS_OWNERDRAW, 395, 433, 160, 40, hwnd, eID_SMS_DEL,  NULL, NULL);
      CreateWindow(BUTTON, L"发送", WS_VISIBLE|WS_OWNERDRAW, 710, 433, 84, 40, hwnd, eID_SMS_SEND, NULL, NULL);
      CreateWindow(TEXTBOX, L"这里显示内容", WS_VISIBLE, 390, 70, 410, 354, hwnd, eID_SMS_CONTENT, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SMS_CONTENT), TBM_SET_TEXTFLAG, 0, DT_TOP | DT_LEFT | DT_BKGND | DT_WORDBREAK);
      CreateWindow(TEXTBOX, L"", WS_VISIBLE | WS_OWNERDRAW, 485, 18, 221, 40, hwnd, eID_SMS_NUMBER, NULL, NULL);
      CreateWindow(LISTBOX, L"SMS LIST", WS_VISIBLE | WS_BORDER | WS_OWNERDRAW, 6, 70, 379, 402, hwnd, eID_SMS_LIST, NULL, NULL);
//      SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_LOCKCURSEL, TRUE, 0);
      SetTimer(hwnd, 0, 1, TMR_START|TMR_SINGLE, NULL);
      SetTimer(hwnd, 1, 1000, TMR_START, NULL);

//	  PostAsyncMessage(hwnd, eMSG_READ_TEXT, 0, 0);     // 执行一次读短信

      break;
    } 

    case WM_TIMER:
    {
      int tmr_id;

      tmr_id = wParam;    // 定时器 ID
      
      if (tmr_id == 0)
      {
        /* 读短信到列表 */
        HWND wnd;
        wnd = GetDlgItem(hwnd, eID_SMS_LIST);

        SendMessage(wnd, LB_RESETCONTENT, 0, 0);

        Read_Text(wnd);
        InvalidateRect(wnd, NULL, TRUE);
      }
      else if (tmr_id == 1)    // 每秒定时器
      {
        /* 检查是不是有新短信 */
        uint8_t newmessadd=0;
        newmessadd=IsReceiveMS();      
        if(newmessadd)
        {	
          SIM900A_DELAY(500); 
          sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");     //"GSM"字符集        
          SIM900A_DELAY(500);        
          BEEP_ON;
          SIM900A_CLEAN_RX();//清除接收缓存
          
          u32 i = 0;
          WCHAR wbuf[40];
          char messagename[40];
          char *wNumber;
          char *wTime;
          uint16_t ListData = 0;    // 最高位 0:发，1:收，后15位为短信地址
          uint8_t Temp = 0;
                  
          HWND hListWnd = GetDlgItem(hwnd, eID_SMS_LIST);
          
          wNumber = (char *)GUI_VMEM_Alloc(200);
          wTime = (char *)GUI_VMEM_Alloc(3*1024);

          SIM900A_CLEAN_RX();
          Temp = ReadMessageInfo(newmessadd, wNumber, wTime);
          BEEP_OFF;
          if (Temp != 0)
          {
            hexuni2gbk(wNumber, messagename);	
            strcat(messagename, wTime);                             // 拼接上时间
            // GUI_DEBUG("number->(%s)\n",messagename);
            x_mbstowcs_cp936(wbuf, messagename, sizeof(wbuf));	    // 将Ansi字符转换成GUI的unicode字符.
            
            //在Listbox中增加一个Item项，记录文件名和文件属性.
            i = SendMessage(hListWnd, LB_GETCOUNT, 0, 0);
            SendMessage(hListWnd, LB_ADDSTRING, i, (LPARAM)wbuf);
            
            /* 通过设置Item项的DATA值来记录短信的属性(用于区分是发送的还是接收的). */
            ListData = newmessadd;
            ListData |= 1 << 15;    // 标记为收

            SendMessage(hListWnd, LB_SETITEMDATA, i, (LPARAM)ListData);
            
            SIM900A_CLEAN_RX();
            InvalidateRect(hListWnd, NULL, TRUE);
          }
          
          GUI_VMEM_Free(wNumber);
          GUI_VMEM_Free(wTime);
        }
      }
      else if (tmr_id == 2)
      {
        /* 初始化失败 */
        RECT RC;
        MSGBOX_OPTIONS ops;
        const WCHAR *btn[] = { L"确认",L"取消" };      //对话框内按钮的文字

        ops.Flag = MB_ICONERROR;
        ops.pButtonText = btn;
        ops.ButtonCount = 2;
        RC.w = 300;
        RC.h = 200;
        RC.x = (GUI_XSIZE - RC.w) >> 1;
        RC.y = (GUI_YSIZE - RC.h) >> 1;
        SelectDialogBox(hwnd, RC, L"没有检测到GSM模块\n请重新检查连接。", L"错误", &ops);    // 显示错误提示框
        PostCloseMessage(hwnd);
      }

      break;
    }
	

	case eMSG_INIT_ERROR:
	{
		// 在定时器处理出错
		break;
	}
  
	case eMSG_READ_TEXT:
	{
		HWND wnd;
		int i;

		wnd = GetDlgItem(hwnd, eID_SMS_LIST);

		SendMessage(wnd, LB_RESETCONTENT, 0, 0);

		Read_Text(wnd);
		InvalidateRect(wnd, NULL, TRUE);

		break;
	}
	
    
    case WM_ERASEBKGND:
    {
      HDC hdc = (HDC)wParam;
      RECT rc;

      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc, MapRGB(hdc, COLOR_SMS_BACK_GROUND));
      FillRect(hdc, &rc);
      return TRUE;
    }

    case WM_PAINT:
    {
      HDC hdc;
      PAINTSTRUCT ps;
      RECT rc = {393, 21, 79, 34};
      hdc = BeginPaint(hwnd, &ps);
      
      SetBrushColor(hdc, MapRGB(hdc, 22, 155, 213));
      EnableAntiAlias(hdc, TRUE);
      FillRoundRect(hdc, &rc, 5);
      EnableAntiAlias(hdc, FALSE);

      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
      DrawText(hdc, L"联系人", -1, &rc, DT_VCENTER | DT_CENTER);    //绘制文字(居中对齐方式)

      rc.x = 143;
      rc.y = 17;
      rc.w = 92;
      rc.h = 38;
      DrawText(hdc, L"短信", -1, &rc, DT_VCENTER | DT_CENTER);    //绘制文字(居中对齐方式)
      
      EndPaint(hwnd, &ps);
      break;
    }
    
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_SMS_EXIT:
          {
            SMS_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }    

          case eID_SMS_CLEAR:
          case eID_SMS_DEL:
          case eID_SMS_SEND:
          {
            SMS_FilletButton_OwnerDraw(ds);
            return TRUE;
          }

          case eID_SMS_NUMBER:
          {
            Fillet_Textbox_OwnerDraw(ds);
            return TRUE;
          }
          
          case eID_SMS_LIST:
          {
            listbox_owner_draw(ds);
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
            case eID_SMS_EXIT:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;

            /* 删除全部短信按钮按下 */
            case eID_SMS_DEL:
            {
              RECT RC;
              MSGBOX_OPTIONS ops;
              const WCHAR *btn[] = { L"确认",L"取消" };      //对话框内按钮的文字

              ops.Flag = MB_ICONERROR;
              ops.pButtonText = btn;
              ops.ButtonCount = 2;
              RC.w = 300;
              RC.h = 200;
              RC.x = (GUI_XSIZE - RC.w) >> 1;
              RC.y = (GUI_YSIZE - RC.h) >> 1;
              
              if (SelectDialogBox(hwnd, RC, L"将会删除全部已收发短信。", L"删除", &ops) == 0)    // 显示确认提示框
              {
                sim900a_tx_printf("AT+CMGDA=\"DEL ALL\"\r");                           // 删除全部短信
                SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_RESETCONTENT, 0, 0);    // 清空列表
                InvalidateRect(GetDlgItem(hwnd, eID_SMS_LIST), NULL, TRUE);            // 重绘列表
              }
            }
            break;

            /* 清除按钮按下 */
            case eID_SMS_CLEAR:
            {
              SetWindowText(GetDlgItem(hwnd, eID_SMS_CONTENT), L"");
              SetWindowText(GetDlgItem(hwnd, eID_SMS_NUMBER), L"");
            }
            break;

            /* 发送按钮按下 */
            case eID_SMS_SEND:
            {
              char *cNumber;
              char *cContent;
              WCHAR *wNumber;
              WCHAR *wContent;
              uint16_t Add;
              int i = 0;
              uint16_t ListData = 0;
              
              cNumber  = (char  *)GUI_VMEM_Alloc(100);
              cContent = (char  *)GUI_VMEM_Alloc(2*1024);
              wNumber  = (WCHAR *)GUI_VMEM_Alloc(100*sizeof(WCHAR));
              wContent = (WCHAR *)GUI_VMEM_Alloc(2*1024*sizeof(WCHAR));
              
              GetWindowText(GetDlgItem(hwnd, eID_SMS_NUMBER), wNumber, 99);
              GetWindowText(GetDlgItem(hwnd, eID_SMS_CONTENT), wContent, 2000);
              x_wcstombs_cp936(cNumber, wNumber, 99);
              x_wcstombs_cp936(cContent, wContent, 2000);
              
              Add = sim900a_save_sms(cNumber, cContent);
              if (Add)
              {
                //在Listbox中增加一个Item项，记录文件名和文件属性.
                i = SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_GETCOUNT, 0, 0);
                SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_ADDSTRING, i, (LPARAM)wNumber);
                
                /* 通过设置Item项的DATA值来记录短信的属性(用于区分是发送的还是接收的). */
                ListData = Add;
                ListData &= ~(1 << 15);    // 标记为发
                SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_SETITEMDATA, i, (LPARAM)ListData);
                
                InvalidateRect(GetDlgItem(hwnd, eID_SMS_LIST), NULL, TRUE);
              }
              
              GUI_VMEM_Free(cNumber);
              GUI_VMEM_Free(cContent);
              GUI_VMEM_Free(wNumber);
              GUI_VMEM_Free(wContent);
            }
            break;

            /* 电话号码文本框 */
            case eID_SMS_NUMBER:
            {
              WCHAR I[21] = {0};
              number_input_box(0, 0, GUI_XSIZE, GUI_YSIZE, L"NUMBRE", I, 20, hwnd);
              SetWindowText(GetDlgItem(hwnd, eID_SMS_NUMBER), I);
            }
            break;

            case eID_SMS_LIST:
            {
              uint16_t ListData = 0;
              uint8_t i;
              i = SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_GETCURSEL,0,0);               // 获得当前选中行
              ListData = SendMessage(GetDlgItem(hwnd, eID_SMS_LIST), LB_GETITEMDATA,i,0);      // 获得Item项的DATA值(在增加项目是时设置的DATA值...).

              char *cNumber;
              char *cContent;
              WCHAR *wNumber;
              WCHAR *wContent;
              cNumber  = (char  *)GUI_VMEM_Alloc(200);
              cContent = (char  *)GUI_VMEM_Alloc(4*1024);
              wNumber  = (WCHAR *)GUI_VMEM_Alloc(200*sizeof(WCHAR));
              wContent = (WCHAR *)GUI_VMEM_Alloc(2*1024*sizeof(WCHAR));
              
              /* 读短信内容 */
              if (readmessage(ListData & 0x7FFF, cNumber, cContent))
              {
                hexuni2gbk((char *)cNumber, (char *)&cNumber[100]);	
                hexuni2gbk((char *)cContent, (char *)&cContent[2000]);
                
//                GUI_DEBUG("cNumber:(%s)", (char *)&cNumber[100]);
//                GUI_DEBUG("cContent:(%s)", (char *)&cContent[2000]);
                
                x_mbstowcs_cp936(wNumber, (char *)&cNumber[100], 100);	    // 将Ansi字符转换成GUI的unicode字符.
                x_mbstowcs_cp936(wContent, (char *)&cContent[2000], 2000);	// 将Ansi字符转换成GUI的unicode字符.

                SetWindowText(GetDlgItem(hwnd, eID_SMS_NUMBER), wNumber);       // 设置电话号码
                SetWindowText(GetDlgItem(hwnd, eID_SMS_CONTENT), wContent);     // 设置短信内容
              }

              GUI_VMEM_Free(cNumber);
              GUI_VMEM_Free(cContent);
              GUI_VMEM_Free(wNumber);
              GUI_VMEM_Free(wContent);

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
      
      // DeleteDC(bk_hdc);
      return PostQuitMessage(hwnd);	
    } 

    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_SMS_Dialog(void)
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
                              L"Short Messaging Service",
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


