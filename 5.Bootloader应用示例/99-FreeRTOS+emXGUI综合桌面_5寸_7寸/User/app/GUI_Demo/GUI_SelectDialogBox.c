
#include "emXGUI.h"
#include "emXGUI_Arch.h"

/*=========================================================================================*/


static uint8_t ButtonNum = 0;    // 返回用户按下的按键
static const WCHAR *pTextInt;
static const WCHAR *pCaptionInt;
static const MSGBOX_OPTIONS *opsInt;

static void DialogBoxButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  
  if(ds->State&BST_PUSHED)
	{
    /* 画按键背景区 */
    SetBrushColor(hdc,MapRGB(hdc, 200, 200, 200));
	}
	else
	{
    /* 画按键背景区 */
    SetBrushColor(hdc,MapRGB(hdc, 250, 250, 250));
	}
  
  FillRoundRect(hdc, &rc, 7);
    
  SetPenColor(hdc,MapRGB(hdc, 200, 200, 200));    /* 画灰色的线 */
  HLine(hdc, rc.x, rc.y, rc.x+rc.w);
  if (ds->ID < opsInt->ButtonCount - 1)    /* 最后一个按钮不画右边的一条线 */
  {
    VLine(hdc, rc.x+rc.w-1, rc.y, rc.y+rc.h);
  }
  
  SetPenColor(hdc,MapRGB(hdc, 105, 105, 105));           // 设置画笔的颜色
  GetWindowText(ds->hwnd, wbuf, 128);                    // 获得按钮控件的文字
//  OffsetRect(&rc, 0, -3);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    /* 窗口创建 */
    case WM_CREATE:
    {
      RECT rc;
      RECT rc_button;
//      int FontH;
//      FontH = GetFontAveHeight(defaultFont);
      GetClientRect(hwnd, &rc);

      for(uint8_t xC=0; xC<opsInt->ButtonCount; xC++)
      {
        /* 计算按键位置 （高度为窗口的四分之一） */
        rc_button.w = rc.w / opsInt->ButtonCount;    // 按钮的宽度
        rc_button.h = rc.h / 4;                      // 窗口高度的 1/4
        rc_button.x = rc_button.w * xC;
        rc_button.y = rc.h - rc_button.h;
        
        /* 创建一个按钮 */
        CreateWindow(BUTTON, opsInt->pButtonText[xC], BS_FLAT|WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, //
                     rc_button.x, rc_button.y, rc_button.w, rc_button.h, hwnd, xC, NULL, NULL);
      }
      
      break; 
    }
    
    case WM_ERASEBKGND:
    {
      return TRUE;    // 直接返回 TRUE 则库不会画默认背景
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc;
      RECT rc;
      RECT rc_Text;

      GetClientRect(hwnd,&rc);
      hdc = BeginPaint(hwnd,&ps);
      
//      SetBrushColor(hdc,MapRGB(hdc,50,0,0));
//      FillRect(hdc, &rc);
      SetBrushColor(hdc, MapRGB(hdc,250,250,250));
      FillRoundRect(hdc, &rc, 7);
      
      /* 计算消息类型的提示位置 */
      rc_Text.w = rc.w;
      rc_Text.h = rc.h / 4;
      rc_Text.x = rc.x;
      rc_Text.y = rc.y;
      
      /* 显示消息类型 */
      DrawText(hdc, pCaptionInt, -1, &rc_Text, DT_VCENTER|DT_CENTER);
      
      /* 计算消息类型的提示位置 */
      rc_Text.w = rc.w;
      rc_Text.y = rc.y + rc_Text.h;
      rc_Text.h = rc.h / 4 * 2;
      rc_Text.x = rc.x;
      
      /* 显示消息类型 */
      DrawText(hdc, pTextInt, -1, &rc_Text, DT_VCENTER|DT_CENTER);
      
      EndPaint(hwnd,&ps);
      break;
    }
    
    case WM_NOTIFY: 
    {
      u16 code,  id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型

      if(code == BN_CLICKED)
      {
        ButtonNum =  id;
        
        PostCloseMessage(hwnd);
      }
      break;
     }
    
     case WM_DRAWITEM:
     {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       
       DialogBoxButton_OwnerDraw(ds);
       
       return TRUE;
     }
    
    case WM_DESTROY:
    {
      return PostQuitMessage(hwnd);
    }
    
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  
  return WM_NULL;
}

int SelectDialogBox(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops)
{
  WNDCLASS	wcex;

  if(hwndParent==NULL)
	{
		hwndParent =GetDesktopWindow();
	}
  
  /* 初始化内部参数 */
  ButtonNum = 0;
  pTextInt = pText;
  pCaptionInt = pCaption;
  opsInt = ops;
  
	wcex.Tag = WNDCLASS_TAG;  
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	DialogWindow(	&wcex,	pCaption,
//							WS_OVERLAPPED|WS_BORDER|WS_DLGFRAME|WS_CLIPCHILDREN,
                WS_CLIPCHILDREN,//WS_CAPTIONWS_BORDER|
							rc.x, rc.y, rc.w, rc.h,
							hwndParent, 0x0000, NULL, NULL);//hInst

// 	//显示主窗口
// 	ShowWindow(hwnd, SW_SHOW);
  
  return ButtonNum;
}

