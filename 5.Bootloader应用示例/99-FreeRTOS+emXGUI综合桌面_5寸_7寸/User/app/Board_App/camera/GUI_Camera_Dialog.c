#include <emXGUI.h>
#include <string.h>
#include "Widget.h"
#include "./camera/bsp_ov5640.h"
#include "x_libc.h"
#include "./camera/ov5640_AF.h"


static HDC hdc_bk = NULL;//背景图层，绘制透明控件
extern BOOL g_dma2d_en;//DMA2D使能标志位，摄像头DMEO必须禁止
TaskHandle_t h_autofocus;//自动对焦线程
BOOL update_flag = 0;//帧率更新标志
static RECT win_rc;//二级菜单位置信息
static int b_close=FALSE;//窗口关闭标志位
uint8_t fps=0;//帧率
OV5640_IDTypeDef OV5640_Camera_ID;
RECT rc_fps = {0,400,800,72};//帧率显示子窗口
HWND Cam_hwnd;//主窗口句柄
HWND SetWIN=NULL;//参数设置窗口
int state = 0;//初始化摄像头状态机
//uint16_t *cam_buff0;
//uint16_t *cam_buff1;
GUI_SEM *cam_sem = NULL;//更新图像同步信号量（二值型）
GUI_SEM *set_sem = NULL;//等待对焦同步信号量（二值型）
int focus_status = 1;//自动对焦，默认开启
//定义控件ID
enum eID
{
	eID_OK =0x1000,
	eID_SET,             //设置键（摄像头窗口）
	eID_SET1,            //自动对焦
	eID_SET2,            //亮度
	eID_SET3,            //饱和度
	eID_SET4,            //对比度
  eID_SET5,            //分辨率
  eID_SET6,            //光线模式
  eID_SET7,            //特殊效果
  eID_SCROLLBAR,       //亮度滑动条
  eID_SCROLLBAR1,      //饱和度滑动条
  eID_SCROLLBAR2,      //对比度滑动条
  //单选框---分辨率
  eID_RB1,             //320*240
  eID_RB2,             //480*272
  eID_RB3,             //800*480（默认）
  //单选框---光线模式
  eID_RB4,             //自动
  eID_RB5,             //光照
  eID_RB6,             //阴天
  eID_RB7,             //办公室
  eID_RB8,             //家里
  //单选框---特殊效果
   eID_RB9,              //冷色
  eID_RB10,             //暖色
  eID_RB11,             //黑白
  eID_RB12,             //泛黄
  eID_RB13,             //反色   
  eID_RB14,             //偏绿
  eID_RB15,             //过曝
  eID_RB16,             //正常  
  eID_TB1,             //当前分辨率显示
  eID_TB2,             //当前光线模式显示   
  eID_TB3,             //当前特殊效果显示
   
  eID_switch,          //自动对焦开关
  eID_Setting1,        //设置分辨率按钮
  eID_Setting2,        //设置光线模式按钮
  eID_Setting3,        //设置特殊效果按钮
   
  eID_BT1,             //分辨率界面返回按键
  eID_BT2,             //光线模式界面返回按键
  eID_BT3,             //特殊效果界面返回按键
  ID_EXIT,
  ID_FPS,
};

/*
 * @brief  清空背景函数
 * @param  hdc:    绘图上下文
 * @param  lprc：  绘制区域
 * @param  hwnd: 绘制窗口句柄
 * @retval TRUE
*/
static BOOL cbErase(HDC hdc, const RECT* lprc,HWND hwnd)
{
  SetBrushColor(hdc, MapRGB(hdc,0,0,0));
  FillRect(hdc, lprc);
  return TRUE;
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
  /***************绘制透明控件背景*************************/
  
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  ClientToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
  ScreenToClient(SetWIN, (POINT *)&rc_tmp, 1);
  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  
  rc_scrollbar.x = rc.x;
  rc_scrollbar.y = rc.h/2;
  rc_scrollbar.w = rc.w;
  rc_scrollbar.h = 2;
  EnableAntiAlias(hdc, TRUE);
  SetBrushColor(hdc, MapRGB888(hdc, Page_c));
  FillRect(hdc, &rc_scrollbar);

  /* 滑块 */
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

  SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2);
  InflateRect(&rc, -2, -2);

  SetBrushColor(hdc, MapRGB888(hdc, fore_c));
  FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2);
  EnableAntiAlias(hdc, FALSE);
}

/*
 * @brief  自定义滑动条绘制函数
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Cam_scrollbar_ownerdraw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	HDC hdc_mem;
	HDC hdc_mem1;
	RECT rc;
	RECT rc_cli;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
	hdc_mem1 = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);   	
	//绘制白色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem1, RGB888( 250, 250, 250), RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//绘制绿色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem,RGB888( 250, 250, 250), RGB888( 0, 250, 0), RGB888( 0, 250, 0));
  SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   

	//左
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc.x, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
	//右
	BitBlt(hdc, rc.x + rc.w, 0, rc_cli.w - (rc.x + rc.w) , rc_cli.h, hdc_mem1, rc.x + rc.w, 0, SRCCOPY);

	//绘制滑块
	if (ds->State & SST_THUMBTRACK)//按下
	{
      BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem1, rc.x, 0, SRCCOPY);
		
	}
	else//未选中
	{
		BitBlt(hdc, rc.x, 0, rc.w+1, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}
/*
 * @brief  自定义自动对焦开关
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Checkbox_owner_draw(DRAWITEM_HDR *ds) 
{
	HDC hdc;
	RECT rc;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  EnableAntiAlias(hdc, TRUE);
	if (focus_status==1)//按钮是按下状态
	{ 
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); 
		FillRoundRect(hdc, &rc, rc.h / 2);
		InflateRect(&rc, -3, -3);

		SetBrushColor(hdc, MapRGB(hdc, 0, 250, 0)); 
		FillRoundRect(hdc, &rc, rc.h / 2);

		GetClientRect(ds->hwnd, &rc);
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); 
		FillCircle(hdc, rc.w - 15, 15, 15);


		SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250)); 
		FillCircle(hdc, rc.w - 15, 15, 12);
	}
	else//按钮是弹起状态
	{ 
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); 
		FillRoundRect(hdc, &rc, rc.h/2);
		InflateRect(&rc, -3,  -3);

		SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250)); 
		FillRoundRect(hdc, &rc, rc.h / 2); 

		GetClientRect(ds->hwnd, &rc);
		SetBrushColor(hdc, MapRGB(hdc, 119, 136, 153)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillCircle(hdc, rc.x + 15, 15, 15);//用矩形填充背景
		
		SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		FillCircle(hdc, rc.x + 15, 15, 12);

	}
  EnableAntiAlias(hdc, FALSE);
}
/*
 * @brief  自定义按钮（分辨率，光线模式，特殊效果）
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Button_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];
  HFONT font_old;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字

  if(ds->ID >= eID_TB1 && ds->ID <= eID_TB3)
  {
    font_old = SetFont(hdc, controlFont_32);
    rc.x = 160;
    rc.w = 40;
    if(ds->State & BST_PUSHED)//按键被按下变颜色
      SetTextColor(hdc, MapRGB(hdc, 192,192,192));
    else
      SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
    DrawText(hdc, L"C", -1, &rc, DT_VCENTER);
    SetFont(hdc, font_old);
    rc.x = 5;
    rc.w = 160;
    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_RIGHT);

  }
  else
  {
    SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_LEFT);
  }

}
/*
 * @brief  自定义普通透明按钮
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void button_owner_draw(DRAWITEM_HDR *ds) 
{
	//	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	//	hwnd =ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
	SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)

}
/*
 * @brief  自定义参数设置按钮
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/

static void BtCam_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  GetWindowText(ds->hwnd, wbuf, 128); //获得按钮控件的文字

	if(ds->State & BST_PUSHED)
	{ //按钮是按下状态
//    GUI_DEBUG("ds->ID=%d,BST_PUSHED",ds->ID);
		SetBrushColor(hdc,MapRGB(hdc,105,105,105)); //设置填充色(BrushColor用于所有Fill类型的绘图函数)
		SetPenColor(hdc,MapRGB(hdc,105,105,105));        //设置绘制色(PenColor用于所有Draw类型的绘图函数)
		SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));      //设置文字色
    
    DrawRoundRect(hdc, &rc, MIN(rc.w,rc.h)>>1);
    InflateRect(&rc, -1, -1);
    DrawRoundRect(hdc, &rc, MIN(rc.w,rc.h)>>1); 
    
    FillRoundRect(hdc, &rc, MIN(rc.w,rc.h)>>1);  
    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式) 
    
	}
	else//按钮是弹起状态
	{ 

		SetPenColor(hdc,MapRGB(hdc,250,250,250));
		SetTextColor(hdc, MapRGB(hdc, 250,250,250)); 
    DrawRoundRect(hdc, &rc, MIN(rc.w,rc.h)>>1);
    InflateRect(&rc, -1, -1);
    DrawRoundRect(hdc, &rc, MIN(rc.w,rc.h)>>1);

    DrawText(hdc, wbuf, -1, &rc, DT_VCENTER | DT_CENTER);//绘制文字(居中对齐方式)    
	}
}
/*
 * @brief  自定义返回按钮
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  if(ds->State & BST_PUSHED) //按钮是按下状态
	{

		SetTextColor(hdc, MapRGB(hdc, 105, 105, 105));      //设置文字色
	}
	else//按钮是弹起状态
	{ 

		SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));
	}

	/* 使用控制图标字体 */
	SetFont(hdc, controlFont_32);

	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER);//绘制文字(居中对齐方式)
   rc.x = 25; 
  /* 恢复默认字体 */
	SetFont(hdc, defaultFont);
  DrawText(hdc, L"返回", -1, &rc, DT_VCENTER);
}
/*
 * @brief  更新屏幕
 * @param  NONE
 * @retval NONE
*/
//  rt_tick_t tick1 = 0;
//  rt_tick_t tick2 = 0;
//static void Update_Dialog()
//{


////  GUI_DEBUG("%d", tick);
//	while(1) //线程已创建了
//	{

//		{
// 
////      tick1 = rt_tick_get();
//			GUI_SemWait(cam_sem, 0xFFFFFFFF);
////      tick2 = rt_tick_get();
////      GUI_DEBUG("%d",tick2-tick1);
//      InvalidateRect(Cam_hwnd,NULL,FALSE);
//	
//		}
//	}
////  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
//}
/**
  * @brief  创建自动对焦进程
  * @param  无
  * @retval 无
  * @notes  
  */
static int thread=0;
TaskHandle_t h;
static void Set_AutoFocus()
{
	if(thread==0)
	{  
//      h=rt_thread_create("Set_AutoFocus",(void(*)(void*))Set_AutoFocus,NULL,1024*2,5,5);
    xTaskCreate((TaskFunction_t )(void(*)(void*))Set_AutoFocus,  /* 任务入口函数 */
                            (const char*    )"Set_AutoFocus",/* 任务名字 */
                            (uint16_t       )8*1024/4,  /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )NULL,/* 任务入口函数参数 */
                            (UBaseType_t    )5, /* 任务的优先级 */
                            (TaskHandle_t  )&h);/* 任务控制块指针 */
//      rt_thread_startup(h);				
      thread =1;
      return;
	}
	while(thread==1) //线程已创建了
	{
    GUI_SemWait(set_sem, 0xFFFFFFFF);
    if(focus_status != 1)
    {
      //暂停对焦
      OV5640_FOCUS_AD5820_Pause_Focus();

    }
    else
    {
      //自动对焦
      OV5640_FOCUS_AD5820_Constant_Focus();

    } 

    GUI_Yield();
	}
  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}
/*
 *参数：y0--以y0为纵坐标对齐，h---要对齐的控件高度
 *发挥：控件的纵坐标
 */
int Set_VCENTER(int y0, int h)
{
  return y0-h/2;
}
/*============================================================================*/
/*
 * @brief  初始化参数配置
 * @param  NONE
 * @retval NONE
*/
static void Camera_ReConfig(void)
{
  cam_mode.frame_rate = FRAME_RATE_15FPS;	
	
	//ISP窗口
	cam_mode.cam_isp_sx = 0;
	cam_mode.cam_isp_sy = 0;	
	
	cam_mode.cam_isp_width = 1920;
	cam_mode.cam_isp_height = 1080;
	
	//输出窗口
	cam_mode.scaling = 1;     //使能自动缩放
	cam_mode.cam_out_sx = 16;	//使能自动缩放后，一般配置成16即可
	cam_mode.cam_out_sy = 4;	  //使能自动缩放后，一般配置成4即可
	cam_mode.cam_out_width = 800;
	cam_mode.cam_out_height = 480;
	
	//LCD位置
	cam_mode.lcd_sx = 0;
	cam_mode.lcd_sy = 0;
	cam_mode.lcd_scan = 5; //LCD扫描模式，本横屏配置可用1、3、5、7模式
	
	//以下可根据自己的需要调整，参数范围见结构体类型定义	
	cam_mode.light_mode = 0;//自动光照模式
	cam_mode.saturation = 0;	
	cam_mode.brightness = 0;
	cam_mode.contrast = 0;
	cam_mode.effect = 0;		//正常模式
	cam_mode.exposure = 0;		

	cam_mode.auto_focus = 1;
}
/*
 * @brief  设置分辨率子窗口
*/
int cur_Resolution = eID_RB3;//当前分辨率
static LRESULT	dlg_set_Resolution_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
    {
      RECT rc;
      GetClientRect(hwnd, &rc);
      rc.x =5;
      rc.y =55;
      rc.w =200;
      rc.h =24;
      //多选一按键--设置分辨率
      CreateWindow(BUTTON,L"320*240",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB1,NULL,NULL);
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"480*272",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB2,NULL,NULL);         
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"800*480(默认)",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB3,NULL,NULL);   

      switch(cur_Resolution)
      {
        case eID_RB1:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB1),BM_SETSTATE,BST_CHECKED,0);
          break;
        }
        case eID_RB2:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB2),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_RB3:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB3),BM_SETSTATE,BST_CHECKED,0);
          break;
        }              
      }   
      //返回按键
      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_TRANSPARENT|WS_OWNERDRAW |WS_VISIBLE,
      0, 0, 240, 50, hwnd, eID_BT1, NULL, NULL); 

      //擦除背景
      SetWindowEraseEx(hwnd, cbErase, TRUE);
      break;
    }
    case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
    {
      PAINTSTRUCT ps;
      HDC hdc;
      RECT rc;
      hdc =BeginPaint(hwnd,&ps); //开始绘图

      ////用户的绘制内容...
      GetClientRect(hwnd, &rc);
      //上边栏目
      rc.h = 50;
      SetBrushColor(hdc,MapRGB(hdc,0,0,0));
      FillRect(hdc, &rc);
      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc,MapRGB(hdc,105,105,105));
      
      rc.y = 50;
      rc.h = rc.h-50;
      FillRect(hdc, &rc);         
      SetTextColor(hdc, MapRGB(hdc,250,250,250));

      rc.x =100;
      rc.y =0;
      rc.w =200; 
      rc.h =50;

      DrawText(hdc,L"分辨率",-1,&rc,DT_CENTER|DT_VCENTER); 


      //TextOut(hdc,10,10,L"Hello",-1);

      EndPaint(hwnd,&ps); //结束绘图
      break;
    }
   
    case WM_DRAWITEM:
    {
      DRAWITEM_HDR *ds;
      ds = (DRAWITEM_HDR*)lParam;
      if (ds->ID == eID_SCROLLBAR)
      {
        Cam_scrollbar_ownerdraw(ds);
        return TRUE;
      }
      if (ds->ID == eID_BT1)
      {
        exit_owner_draw(ds);
        return TRUE;
      }
    } 
    case WM_NOTIFY:
    {
      u16 code,id;
      code =HIWORD(wParam); //获得通知码类型.
      id   =LOWORD(wParam); //获得产生该消息的控件ID.
      if(id==eID_BT1 && code==BN_CLICKED)
      {
        PostCloseMessage(hwnd);
      }

      if(id >= eID_RB1 && id<= eID_RB3)
      {
        if(code == BN_CLICKED)
        { 
          cur_Resolution = id;
          switch(cur_Resolution)
          {
            case eID_RB1:
            {
              OV5640_Capture_Control(DISABLE);
              //输出窗口
              cam_mode.scaling = 1;      //使能自动缩放
              cam_mode.cam_out_sx = 16;	//使能自动缩放后，一般配置成16即可
              cam_mode.cam_out_sy = 4;	  //使能自动缩放后，一般配置成4即可
              cam_mode.cam_out_width = 320;
              cam_mode.cam_out_height = 240;

              //LCD位置
              cam_mode.lcd_sx = 270;
              cam_mode.lcd_sy = 120;
              OV5640_OutSize_Set(cam_mode.scaling,
                       cam_mode.cam_out_sx,
                       cam_mode.cam_out_sy,
                       cam_mode.cam_out_width,
                       cam_mode.cam_out_height);

              OV5640_Capture_Control(ENABLE);
             
              state = 3;
              break;  
            }          
            case eID_RB2:
            {
              OV5640_Capture_Control(DISABLE);
              //输出窗口
              cam_mode.scaling = 1;      //使能自动缩放
              cam_mode.cam_out_sx = 16;	//使能自动缩放后，一般配置成16即可
              cam_mode.cam_out_sy = 4;	  //使能自动缩放后，一般配置成4即可
              cam_mode.cam_out_width = 480;
              cam_mode.cam_out_height = 272;

              //LCD位置
              cam_mode.lcd_sx = 160;
              cam_mode.lcd_sy = 104;
              OV5640_OutSize_Set(cam_mode.scaling,
                       cam_mode.cam_out_sx,
                       cam_mode.cam_out_sy,
                       cam_mode.cam_out_width,
                       cam_mode.cam_out_height);

              OV5640_Capture_Control(ENABLE);

              state = 3;
              break;
            }
            case eID_RB3:
            {
              OV5640_Capture_Control(DISABLE);
              //输出窗口
              cam_mode.scaling = 1;      //使能自动缩放
              cam_mode.cam_out_sx = 16;	//使能自动缩放后，一般配置成16即可
              cam_mode.cam_out_sy = 4;	  //使能自动缩放后，一般配置成4即可
              cam_mode.cam_out_width = 800;
              cam_mode.cam_out_height = 480;

              //LCD位置
              cam_mode.lcd_sx = 0;
              cam_mode.lcd_sy = 0;
              OV5640_OutSize_Set(cam_mode.scaling,
                       cam_mode.cam_out_sx,
                       cam_mode.cam_out_sy,
                       cam_mode.cam_out_width,
                       cam_mode.cam_out_height);


              OV5640_Capture_Control(ENABLE);
              
              state = 3;
              break;
            }
          }
        
        }
      }
      break;
    }
    case	WM_CTLCOLOR:
    {
      u16 id;
      id =LOWORD(wParam);
      CTLCOLOR *cr;
      cr =(CTLCOLOR*)lParam;
      if(id >=eID_RB1 && id <= eID_RB3)
      {
        cr->TextColor =RGB888(250,250,250);
        cr->BackColor =RGB888(105,105,105);
        cr->BorderColor =RGB888(50,50,50);
        cr->ForeColor =RGB888(105,105,105);
        return TRUE;            
      }

      return FALSE;

    }     
    case WM_CLOSE:
    {
      HWND wnd =GetDlgItem(SetWIN, eID_TB1);

      switch(cur_Resolution)
      {
        case eID_RB1:
          SetWindowText(wnd, L"320*240");
          break;
        case eID_RB2:
          SetWindowText(wnd, L"480*272");
          break;
        case eID_RB3:
          SetWindowText(wnd, L"800*480(默认)");
          break;
      }         

      RedrawWindow(SetWIN,NULL,RDW_INVALIDATE|RDW_ALLCHILDREN);

      DestroyWindow(hwnd);
      ShowWindow(SetWIN, SW_SHOW);
      return TRUE;  
    }

    default: //用户不关心的消息,由系统处理.
    {
      return DefWindowProc(hwnd,msg,wParam,lParam);
    }

  }
  return WM_NULL;

}

/*
 * @brief  设置光线模式
*/
int cur_LightMode = eID_RB4;//当前的光线模式
static LRESULT	dlg_set_LightMode_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
      RECT rc;
      GetClientRect(hwnd, &rc);
      rc.x =5;
      rc.y =55;
      rc.w =200;
      rc.h =24;
      CreateWindow(BUTTON,L"自动(默认)",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
                  rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB4,NULL,NULL);
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"光照",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
                  rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB5,NULL,NULL);         
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"阴天",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
                  rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB6,NULL,NULL);          
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"办公室",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
                  rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB7,NULL,NULL);         
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"家里",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
                  rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB8,NULL,NULL);     

      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_TRANSPARENT|WS_OWNERDRAW |WS_VISIBLE,
                  0, 0, 240, 50, hwnd, eID_BT2, NULL, NULL); 
                  
      SetWindowEraseEx(hwnd, cbErase, TRUE);

      switch(cur_LightMode)
      {
        case eID_RB4:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB4),BM_SETSTATE,BST_CHECKED,0);
          break;
        }
        case eID_RB5:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB5),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_RB6:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB6),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_RB7:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB7),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_RB8:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB8),BM_SETSTATE,BST_CHECKED,0);
          break;
        }             
      }          
      break;
		}

		
 		case	WM_CTLCOLOR:
		{
      u16 id;
      id =LOWORD(wParam);
      CTLCOLOR *cr;
      cr =(CTLCOLOR*)lParam;
      if(id >=eID_RB4 && id <= eID_RB8)
      {
        cr->TextColor =RGB888(250,250,250);
        cr->BackColor =RGB888(200,220,200);
        cr->BorderColor =RGB888(50,50,50);
        cr->ForeColor =RGB888(105,105,105);
        return TRUE;            
      }

      return FALSE;
			
		}  
    case WM_NOTIFY:
    {
      u16 code,id;
      code =HIWORD(wParam); //获得通知码类型.
      id   =LOWORD(wParam); //获得产生该消息的控件ID.
      if(id==eID_BT2 && code==BN_CLICKED)
      {
        PostCloseMessage(hwnd);
      }
      if(id >= eID_RB4 && id<= eID_RB8)
      {
        if(code == BN_CLICKED)
        { 
          cur_LightMode = id;
          switch(cur_LightMode)
          {
            case eID_RB4:
            {
              cam_mode.light_mode = 0;
              OV5640_LightMode(cam_mode.light_mode);
              break;  
            }            
            case eID_RB5:
            {
              cam_mode.light_mode = 1;
              OV5640_LightMode(cam_mode.light_mode);
              break;
            }
            case eID_RB6:
            {
              cam_mode.light_mode = 2;
              OV5640_LightMode(cam_mode.light_mode);
              break;
            }
            case eID_RB7:
            {
              cam_mode.light_mode = 3;
              OV5640_LightMode(cam_mode.light_mode);
              break;
            }
            case eID_RB8:
            {
              cam_mode.light_mode = 4;
              OV5640_LightMode(cam_mode.light_mode);
              break;
            }
          }   
        }
      }
      break;
    }     
    case WM_DRAWITEM:
    { 
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       if (ds->ID == eID_BT2)
       {
          exit_owner_draw(ds);
          return TRUE;
       }
    }  
    case WM_CLOSE:
    {
      switch(cur_LightMode)
      {
        case eID_RB4:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"自动(默认)");
          break;
        }          
        case eID_RB5:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"光照");
          break;
        }
        case eID_RB6:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"阴天");
          break;
        }
        case eID_RB7:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"办公室");
          break;
        }
        case eID_RB8:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB2), L"家里");
          break;
        }
      }         

      DestroyWindow(hwnd);
      ShowWindow(SetWIN, SW_SHOW);
      return TRUE; 
    } 
		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
      PAINTSTRUCT ps;
      HDC hdc;
      RECT rc;
      hdc =BeginPaint(hwnd,&ps); //开始绘图

      GetClientRect(hwnd, &rc);

      rc.h = 50;
      SetBrushColor(hdc,MapRGB(hdc,0,0,0));
      FillRect(hdc, &rc);
      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc,MapRGB(hdc,105,105,105));
      rc.y = 50;
      rc.h = rc.h-50;
      FillRect(hdc, &rc);         
      SetTextColor(hdc, MapRGB(hdc,250,250,250));

      rc.x =100;
      rc.y =0;
      rc.w =200; 
      rc.h =50;

      DrawText(hdc,L"光线模式",-1,&rc,DT_CENTER|DT_VCENTER); 

      EndPaint(hwnd,&ps); //结束绘图
      break; 
		}
		     
		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;

}

/*
 * @brief  设置特殊效果
*/
int cur_SpecialEffects = eID_RB16;//当前特殊效果
static LRESULT	dlg_set_SpecialEffects_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
      RECT rc;
      GetClientRect(hwnd, &rc);
      rc.x =5;
      rc.y =55;
      rc.w =200;
      rc.h =24;
      CreateWindow(BUTTON,L"冷色",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB9,NULL,NULL);
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"暖色",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB10,NULL,NULL);         
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"黑白",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB11,NULL,NULL);          
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"泛黄",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB12,NULL,NULL);         
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"反色",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB13,NULL,NULL); 
      OffsetRect(&rc,0,rc.h+10);         
      CreateWindow(BUTTON,L"偏绿",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB14,NULL,NULL);         
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"过曝",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB15,NULL,NULL);   
      OffsetRect(&rc,0,rc.h+10);
      CreateWindow(BUTTON,L"正常(默认)",BS_RADIOBOX|WS_VISIBLE|WS_TRANSPARENT,
      rc.x,rc.y,rc.w,rc.h,hwnd,(1<<16)|eID_RB16,NULL,NULL); 
      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_TRANSPARENT|WS_OWNERDRAW |WS_VISIBLE,
      0, 0, 240, 50, hwnd, eID_BT3, NULL, NULL); 

      SetWindowEraseEx(hwnd, cbErase, TRUE);

      switch(cur_SpecialEffects)
      {
        case eID_RB9:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB9),BM_SETSTATE,BST_CHECKED,0);
          break;
        }
        case eID_RB10:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB10),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_RB11:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB11),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_RB12:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB12),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_RB13:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB13),BM_SETSTATE,BST_CHECKED,0);
          break;
        }     
        case eID_RB14:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB14),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_RB15:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB15),BM_SETSTATE,BST_CHECKED,0);
          break;
        }    
        case eID_RB16:
        {
          SendMessage(GetDlgItem(hwnd, eID_RB16),BM_SETSTATE,BST_CHECKED,0);
          break;
        }              
      }          
      break;
		}
		
    case WM_ERASEBKGND:
    {
      HDC hdc =(HDC)wParam;
      RECT rc =*(RECT*)lParam;
      
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);
      return TRUE;
    }
    case	WM_CTLCOLOR:
    {
      u16 id;
      id =LOWORD(wParam);
      CTLCOLOR *cr;
      cr =(CTLCOLOR*)lParam;
      if(id >=eID_RB9 && id <= eID_RB16)
      {
        cr->TextColor =RGB888(250,250,250);
        cr->BackColor =RGB888(200,220,200);
        cr->BorderColor =RGB888(50,50,50);
        cr->ForeColor =RGB888(105,105,105);
        return TRUE;            
      }
      return FALSE;			
		}  
    case WM_NOTIFY:
    {
      u16 code,id;
      code =HIWORD(wParam); //获得通知码类型.
      id   =LOWORD(wParam); //获得产生该消息的控件ID.
      if(id==eID_BT3 && code==BN_CLICKED)
      {
        PostCloseMessage(hwnd);
      }
      if(id >= eID_RB9 && id<= eID_RB16)
      {
        if(code == BN_CLICKED)
        { 
          cur_SpecialEffects = id;
          switch(cur_SpecialEffects)
          {
            case eID_RB9:
            {
              cam_mode.effect = 1;
              OV5640_SpecialEffects(cam_mode.effect);
              break;    
            }            
            case eID_RB10:
            {
              cam_mode.effect = 2;
              OV5640_SpecialEffects(cam_mode.effect);
              break;
            }
            case eID_RB11:
            {
              cam_mode.effect = 3;
              OV5640_SpecialEffects(cam_mode.effect);               
              break;
            }
            case eID_RB12:
            {
              cam_mode.effect = 4;
              OV5640_SpecialEffects(cam_mode.effect);                
              break;
            }
            case eID_RB13:
            {
              cam_mode.effect = 5;
              OV5640_SpecialEffects(cam_mode.effect);                
              break;
            }
            case eID_RB14:
            {
              cam_mode.effect = 6;
              OV5640_SpecialEffects(cam_mode.effect);                
              break;
            }
            case eID_RB15:
            {
              cam_mode.effect = 7;
              OV5640_SpecialEffects(cam_mode.effect);                
              break;
            }
            case eID_RB16:
            {
              cam_mode.effect = 0;
              OV5640_SpecialEffects(cam_mode.effect);
              break;  
            }            
          } 
        }
      }
      break;
    }     
    case WM_DRAWITEM:
    { 
      DRAWITEM_HDR *ds;
      ds = (DRAWITEM_HDR*)lParam;
      if (ds->ID == eID_BT3)
      {
        exit_owner_draw(ds);
        return TRUE;
      }
    }  
    case WM_CLOSE:
    {
      switch(cur_SpecialEffects)
      {
        case eID_RB9:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"冷色");
          break;                 
        }
        case eID_RB10:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"暖色");
          break;
        }
        case eID_RB11:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"黑白");
          break;
        }
        case eID_RB12:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"泛黄");break;
        }
        case eID_RB13:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"反色");
          break;
        }
        case eID_RB14:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"偏绿");
          break;
        }
        case eID_RB15:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"过曝");
          break;
        }
        case eID_RB16:
        {
          SetWindowText(GetDlgItem(SetWIN, eID_TB3), L"正常(默认)");
          break;   
        }        
      }         
      ShowWindow(SetWIN, SW_SHOW);
      DestroyWindow(hwnd);
      return TRUE; 
    }      

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //开始绘图
      GetClientRect(hwnd, &rc);

      rc.h = 50;
      SetBrushColor(hdc,MapRGB(hdc,0,0,0));
      FillRect(hdc, &rc);
      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc,MapRGB(hdc,105,105,105));
      rc.y = 50;
      rc.h = rc.h-50;
      FillRect(hdc, &rc);         
      SetTextColor(hdc, MapRGB(hdc,250,250,250));

      rc.x =100;
      rc.y =0;
      rc.w =200; 
      rc.h =50;

      DrawText(hdc,L"特殊效果",-1,&rc,DT_CENTER|DT_VCENTER);

			EndPaint(hwnd,&ps); //结束绘图
      break;
		}
  
    default: //用户不关心的消息,由系统处理.
      return DefWindowProc(hwnd,msg,wParam,lParam);
  }
	return WM_NULL;

}
/*
 * @brief  参数设置窗口回调函数
*/
extern int updata;
static LRESULT	dlg_set_WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc;
  static SCROLLINFO sif, sif1, sif2;
  RECT rc_first[2];
  RECT rc_second[2];
  RECT rc_third[2];
  //static int state = 0;
	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
			b_close =FALSE;
			rc.x =5;
			rc.y =50;
			rc.w =400;
			rc.h =50;
      //把rc等间距分成两份，存在rc_first
      MakeMatrixRect(rc_first, &rc, 5, 0, 2, 1);
      CreateWindow(BUTTON,L"自动对焦",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,
                   rc_first[0].x,rc_first[0].y,rc_first[0].w,rc_first[0].h,hwnd,eID_SET1,NULL,NULL); 
      rc_first[1].y = Set_VCENTER(rc_first[0].y+rc_first[0].h/2,30);
      CreateWindow(BUTTON,L" ",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,
                   rc.w-70,rc_first[1].y,60,30,hwnd,eID_switch,NULL,NULL);   
            
			OffsetRect(&rc,0,rc.h);
      MakeMatrixRect(rc_second, &rc, 5, 0, 2, 1);
      CreateWindow(BUTTON,L"亮度",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,
                   rc_second[0].x,rc_second[0].y,rc_second[0].w,rc_second[0].h,hwnd,eID_SET2,NULL,NULL);
      sif.cbSize = sizeof(sif);
      sif.fMask = SIF_ALL;
      sif.nMin = -2;
      sif.nMax = 2;
      sif.nValue = cam_mode.brightness;
      sif.TrackSize = 31;//滑块值
      sif.ArrowSize = 0;//两端宽度为0（水平滑动条）
      rc_second[1].y = Set_VCENTER(rc_second[0].y+rc_second[0].h/2,31);
      CreateWindow(SCROLLBAR, L"SCROLLBAR_liangdu", WS_OWNERDRAW|WS_VISIBLE, 
                   rc_second[1].x,rc_second[1].y,180,31, hwnd, eID_SCROLLBAR, NULL, NULL);                 
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR), SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);      
 
 
			OffsetRect(&rc,0,rc.h);
      CreateWindow(BUTTON,L"饱和度",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET3,NULL,NULL);
      MakeMatrixRect(rc_third, &rc, 5, 0, 2, 1);

      sif1.cbSize = sizeof(sif1);
      sif1.fMask = SIF_ALL;
      sif1.nMin = -3;
      sif1.nMax = 3;
      sif1.nValue = cam_mode.saturation;
      sif1.TrackSize = 31;//滑块值
      sif1.ArrowSize = 0;//两端宽度为0（水平滑动条）
      rc_third[1].y = Set_VCENTER(rc_third[0].y+rc_third[0].h/2,31);
      CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_VISIBLE, 
                   rc_third[1].x,rc_third[1].y,180,31, hwnd, eID_SCROLLBAR1, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR1), SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif1);

      OffsetRect(&rc,0,rc.h);
      CreateWindow(BUTTON,L"对比度",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET4,NULL,NULL);

      MakeMatrixRect(rc_third, &rc, 5, 0, 2, 1);
      sif2.cbSize = sizeof(sif2);
      sif2.fMask = SIF_ALL;
      sif2.nMin = -3;
      sif2.nMax = 3;
      sif2.nValue = cam_mode.contrast;
      sif2.TrackSize = 31;//滑块值
      sif2.ArrowSize = 0;//两端宽度为0（水平滑动条）
      rc_third[1].y = Set_VCENTER(rc_third[0].y+rc_third[0].h/2,31);
      CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW|WS_VISIBLE, 
                   rc_third[1].x,rc_third[1].y, 180, 31, hwnd, eID_SCROLLBAR2, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_SCROLLBAR2), SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif2);


      OffsetRect(&rc,0,rc.h);
      CreateWindow(BUTTON,L"分辨率",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET5,NULL,NULL);
      MakeMatrixRect(rc_third, &rc, 0, 0, 2, 1);
      rc_third[1].y = Set_VCENTER(rc_third[0].y+rc_third[0].h/2,30);
      CreateWindow(BUTTON,L"800*480(默认)",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,
                   rc_third[1].x,rc_third[1].y,200,30,hwnd,eID_TB1,NULL,NULL); 

      switch(cur_Resolution)
      {
        case eID_RB1:
          SetWindowText(GetDlgItem(hwnd, eID_TB1), L"320*240");break;                 
        case eID_RB2:
          SetWindowText(GetDlgItem(hwnd, eID_TB1), L"480*272");break;
        case eID_RB3:
          SetWindowText(GetDlgItem(hwnd, eID_TB1), L"800*480(默认)");break;
      }          
      OffsetRect(&rc,0,rc.h);
      CreateWindow(BUTTON,L"光线模式",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET6,NULL,NULL);
      MakeMatrixRect(rc_third, &rc, 0, 0, 2, 1);
      rc_third[1].y = Set_VCENTER(rc_third[0].y+rc_third[0].h/2,40);
      CreateWindow(BUTTON,L"自动(默认)",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,
      rc_third[1].x,rc_third[1].y,200,40,hwnd,eID_TB2,NULL,NULL);                      

      switch(cur_LightMode)
      {
        case eID_RB4:
          SetWindowText(GetDlgItem(hwnd, eID_TB2), L"自动(默认)");break;                 
        case eID_RB5:
          SetWindowText(GetDlgItem(hwnd, eID_TB2), L"光照");break;
        case eID_RB6:
          SetWindowText(GetDlgItem(hwnd, eID_TB2), L"阴天");break;
        case eID_RB7:
          SetWindowText(GetDlgItem(hwnd, eID_TB2), L"办公室");break;
        case eID_RB8:
          SetWindowText(GetDlgItem(hwnd, eID_TB2), L"家里");break;
      }  
      OffsetRect(&rc,0,rc.h);
      CreateWindow(BUTTON,L"特殊效果",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,eID_SET7,NULL,NULL);

      MakeMatrixRect(rc_third, &rc, 0, 0, 2, 1);
      rc_third[1].y = Set_VCENTER(rc_third[0].y+rc_third[0].h/2,30);         
      CreateWindow(BUTTON,L"正常(默认)",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,
      rc_third[1].x,rc_third[1].y,200,30,hwnd,eID_TB3,NULL,NULL);                      
      switch(cur_SpecialEffects)
      {
        case eID_RB9:
          SetWindowText(GetDlgItem(hwnd, eID_TB3), L"冷色");break;                 
        case eID_RB10:
          SetWindowText(GetDlgItem(hwnd, eID_TB3), L"暖色");break;
        case eID_RB11:
          SetWindowText(GetDlgItem(hwnd, eID_TB3), L"黑白");break;
        case eID_RB12:
          SetWindowText(GetDlgItem(hwnd, eID_TB3), L"泛黄");break;
        case eID_RB13:
          SetWindowText(GetDlgItem(hwnd, eID_TB3), L"反色");break;
        case eID_RB14:
          SetWindowText(GetDlgItem(hwnd, eID_TB3), L"偏绿");break;
        case eID_RB15:
          SetWindowText(GetDlgItem(hwnd, eID_TB3), L"过曝");break;
        case eID_RB16:
          SetWindowText(GetDlgItem(hwnd, eID_TB3), L"正常(默认)");break;            
      }           
      GetClientRect(hwnd, &rc);
      hdc_bk = CreateMemoryDC(SURF_SCREEN, rc.w, rc.h);
      CreateWindow(BUTTON, L"F", BS_FLAT | BS_NOTIFY|WS_TRANSPARENT|WS_OWNERDRAW |WS_VISIBLE,
      0, 0, 240, 50, hwnd, ID_EXIT, NULL, NULL);   
      SetTimer(hwnd,2,20,TMR_START,NULL);
      break;
    }

		case WM_TIMER://实现窗口下落的效果
		{
			switch(wParam)
			{
				case 1:
				{
          break;
				}
				case 2:
				{
					if(GetKeyState(VK_LBUTTON)!=0)
					{
						break;
					}
					GetWindowRect(hwnd,&rc);

					if(b_close==FALSE)
					{
						if(rc.y < win_rc.y )
						{
							if((win_rc.y-rc.y)>50)
							{
								rc.y +=30;
							}
							if((win_rc.y-rc.y)>30)
							{
								rc.y +=20;
							}
							else
							{
								rc.y +=4;
							}
							ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
                     
							MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						}
					}
					else
					{
						if(rc.y > -(rc.h))
						{
							rc.y -= 40;

							ScreenToClient(GetParent(hwnd),(POINT*)&rc.x,1);
                     
							MoveWindow(hwnd,rc.x,rc.y,rc.w,rc.h,TRUE);
						}
						else
						{
							PostCloseMessage(hwnd);
						}
					}


				}
				break;
			}
      break;
		}
		
		
		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;
      NMHDR *nr;  
      u16 ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID. 
      nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.
      NM_SCROLLBAR *sb_nr;
      sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
      switch (nr->code)
      {
        case SBN_THUMBTRACK: //R滑块移动
        {
          switch(ctr_id)
          {
            case eID_SCROLLBAR://亮度
            {
              cam_mode.brightness = sb_nr->nTrackValue; //得到当前的音量值
              OV5640_BrightnessConfig(cam_mode.brightness);
              SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, cam_mode.brightness); 
              break;
            }
            case eID_SCROLLBAR1://饱和度
            {
              cam_mode.saturation = sb_nr->nTrackValue; //得到当前的音量值
              OV5640_Color_Saturation(cam_mode.saturation);
              SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, cam_mode.saturation); 
              break;
            }
            case eID_SCROLLBAR2://对比度
            {
              cam_mode.contrast = sb_nr->nTrackValue; //得到当前的音量值
              OV5640_ContrastConfig(cam_mode.contrast);
              SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, cam_mode.contrast);                      
              break;
            }

          }
          break;
        } 
      
      }
			if((id==eID_Setting1|| id == eID_TB1)&& code==BN_CLICKED)
			{
        ShowWindow(hwnd, SW_HIDE); //隐藏设置窗口
        WNDCLASS wcex;
        
        GetClientRect(Cam_hwnd,&rc);
        rc.x = rc.x+(rc.w-win_rc.w)/2;
        rc.w =400;
        rc.h =155;

        wcex.Tag	 		= WNDCLASS_TAG;
        wcex.Style			= CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc	= (WNDPROC)dlg_set_Resolution_WinProc;
        wcex.cbClsExtra		= 0;
        wcex.cbWndExtra		= 0;
        wcex.hInstance		= NULL;
        wcex.hIcon			= NULL;
        wcex.hCursor		= NULL;
        
        CreateWindowEx(WS_EX_FRAMEBUFFER,
                        &wcex,L"Set_1_xxx",
                        WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
                        rc.x,rc.y,rc.w,rc.h,
                        Cam_hwnd,0,NULL,NULL);
                         
			}

			if((id==eID_Setting2|| id == eID_TB2) && code==BN_CLICKED)
			{
        WNDCLASS wcex;
        ShowWindow(hwnd, SW_HIDE);//隐藏设置窗口
        GetClientRect(Cam_hwnd,&rc);
        
        rc.x = rc.x+(rc.w-win_rc.w)/2;
        rc.w =400;
        rc.h =225;
        wcex.Tag	 		= WNDCLASS_TAG;
        wcex.Style			= CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc	= (WNDPROC)dlg_set_LightMode_WinProc;
        wcex.cbClsExtra		= 0;
        wcex.cbWndExtra		= 0;
        wcex.hInstance		= NULL;
        wcex.hIcon			= NULL;
        wcex.hCursor		= NULL;


        CreateWindowEx(WS_EX_FRAMEBUFFER,
                        &wcex,L"Set_2_xxx",
                        WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
                        rc.x,rc.y,rc.w,rc.h,
                        Cam_hwnd,0,NULL,NULL);

			}
			if((id==eID_Setting3|| id == eID_TB3) && code==BN_CLICKED)
			{
        WNDCLASS wcex;
        ShowWindow(hwnd, SW_HIDE);
        GetClientRect(Cam_hwnd,&rc);

        rc.x = rc.x+(rc.w-win_rc.w)/2;
 
        rc.w =400;
        rc.h =325;
        wcex.Tag	 		= WNDCLASS_TAG;
        wcex.Style			= CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc	= (WNDPROC)dlg_set_SpecialEffects_WinProc;
        wcex.cbClsExtra		= 0;
        wcex.cbWndExtra		= 0;
        wcex.hInstance		= NULL;
        wcex.hIcon			= NULL;
        wcex.hCursor		= NULL;


        CreateWindowEx(WS_EX_FRAMEBUFFER,
                      &wcex,L"Set_3_xxx",
                      WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,
                      rc.x,rc.y,rc.w,rc.h,
                      Cam_hwnd,0,NULL,NULL);

			}
			if(id==ID_EXIT && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
         
			if (id == eID_switch && code == BN_CLICKED)//切换自动对焦状态
			{
				focus_status = ~focus_status;
        GUI_SemPost(set_sem);
			}               
      break;		
    }



		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			RECT rc;
			hdc =BeginPaint(hwnd,&ps); //开始绘图
      GetClientRect(hwnd, &rc);

      rc.h = 50;
      SetBrushColor(hdc_bk,MapRGB(hdc_bk,0,0,0));
      FillRect(hdc_bk, &rc);
      GetClientRect(hwnd, &rc);
      SetBrushColor(hdc_bk,MapRGB(hdc_bk,105,105,105));
      rc.y = 50;
      rc.h = rc.h-50;
      FillRect(hdc_bk, &rc);         
      SetTextColor(hdc_bk, MapRGB(hdc_bk,250,250,250));

      rc.x =100;
      rc.y =0;
      rc.w =200; 
      rc.h =50;

      DrawText(hdc_bk,L"参数设置",-1,&rc,DT_CENTER|DT_VCENTER); 
      SetPenColor(hdc_bk, MapRGB(hdc_bk, 245,245,245));
      GetClientRect(hwnd, &rc);
      //间隔线
      HLine(hdc_bk, 0, 50, 400);
      HLine(hdc_bk, 0, 100, 400);
      HLine(hdc_bk, 0, 150, 400);
      HLine(hdc_bk, 0, 200, 400);
      HLine(hdc_bk, 0, 250, 400);
      HLine(hdc_bk, 0, 300, 400);
      HLine(hdc_bk, 0, 350, 400);
      BitBlt(hdc, 0,0,rc.w, rc.h, hdc_bk,0,0,SRCCOPY);        
      EndPaint(hwnd,&ps); //结束绘图
      break;
		}
		case WM_CTLCOLOR:
		{
			u16 id;
			id =LOWORD(wParam);
      CTLCOLOR *cr;
      cr =(CTLCOLOR*)lParam;
      if((id >=eID_SET1 && id <= eID_SET7 )|| (id >= eID_TB1 && id <= eID_TB3) )
      {
        cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
        cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
        cr->BorderColor =RGB888(0,0,0);//边框颜色（RGB888颜色格式)
        return TRUE;            
      }    
			return FALSE;	
		}	

		case WM_DRAWITEM:
		{
      DRAWITEM_HDR *ds;
      ds = (DRAWITEM_HDR*)lParam;
      if (ds->ID == eID_switch)
      {
        Checkbox_owner_draw(ds); //执行自绘制按钮
      }
      if (ds->ID == eID_SCROLLBAR || ds->ID == eID_SCROLLBAR1 || ds->ID == eID_SCROLLBAR2)
      {
        Cam_scrollbar_ownerdraw(ds);
        return TRUE;
      }         
      if ((ds->ID >= eID_SET1 && ds->ID <= eID_SET7) || (ds->ID >= eID_TB1 && ds->ID <= eID_TB3)||
          (ds->ID >= eID_Setting1 && ds->ID <= eID_Setting3))
      {
        Button_owner_draw(ds); //执行自绘制按钮
      }
      if(ds->ID == ID_EXIT)  
      {
        exit_owner_draw(ds);
      }

      return TRUE;
		}
		case WM_CLOSE:
		{
			b_close=TRUE;
      
			GetWindowRect(hwnd,&rc);

			if(rc.y <= -(rc.h))
			{
				DestroyWindow(hwnd);
				return TRUE;
			}
			return FALSE;
		}
    case WM_DESTROY:
    {
      DeleteDC(hdc_bk);
      PostQuitMessage(hwnd);
      break;
    }
		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}
	return WM_NULL;
}
/*
 * @brief  摄像头窗口回调函数
*/
extern int SelectDialogBox(HWND hwndParent, RECT rc, const WCHAR *pText, const WCHAR *pCaption, const MSGBOX_OPTIONS *ops);
static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  static uint8_t OV5640_State = 0;    // 0:可以检测到摄像头
  switch(msg)
  {
    case WM_CREATE:
    {
      /* 初始化摄像头GPIO及IIC */
      OV5640_HW_Init();  
      /* 读取摄像头芯片ID，确定摄像头正常连接 */
      OV5640_ReadID(&OV5640_Camera_ID);

      if(OV5640_Camera_ID.PIDH  == 0x56)
      {
        GUI_DEBUG("OV5640 ID:%x %x",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);
      }
      else
      {
        SetTimer(hwnd, 3, 3, TMR_START | TMR_SINGLE, NULL);      // 初始化出错启动提示
        break;
      }     
//      cam_sem = GUI_SemCreate(0,1);//同步摄像头图像
//      set_sem = GUI_SemCreate(1,1);//自动对焦信号
//      //创建自动对焦线程
////      h_autofocus=rt_thread_create("Update_Dialog",(void(*)(void*))Update_Dialog,NULL,4096,4,5);
//      xTaskCreate((TaskFunction_t )(void(*)(void*))Update_Dialog,  /* 任务入口函数 */
//                            (const char*    )"Update_Dialog",/* 任务名字 */
//                            (uint16_t       )4*1024/4,  /* 任务栈大小FreeRTOS的任务栈以字为单位 */
//                            (void*          )NULL,/* 任务入口函数参数 */
//                            (UBaseType_t    )5, /* 任务的优先级 */
//                            (TaskHandle_t  )&h_autofocus);/* 任务控制块指针 */
////      rt_thread_startup(h_autofocus);
//      //默认开启自动对焦
//      Set_AutoFocus();
      
      //图像缓冲区	
//      bits = (U16 *)GUI_VMEM_Alloc(2*800*480); 
      
      SetTimer(hwnd,1,1000,TMR_START,NULL);  
//      SetTimer(hwnd,2,1,TMR_START,NULL);   
//      GetClientRect(hwnd, &rc);
//      //设置按键
//      CreateWindow(BUTTON,L"参数设置",WS_OWNERDRAW|WS_TRANSPARENT,rc.w-135,419,120,40,hwnd,eID_SET,NULL,NULL);
//      //退出按键
//      CreateWindow(BUTTON, L"O",WS_OWNERDRAW|WS_TRANSPARENT,730, 0, 70, 70, hwnd, ID_EXIT, NULL, NULL); 
//      //帧率
//      CreateWindow(BUTTON,L" ",WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,rc.w-600,400,400,72,hwnd,ID_FPS,NULL,NULL);
      break;  
    }
    case WM_DRAWITEM:
    {
      DRAWITEM_HDR *ds;
      ds = (DRAWITEM_HDR*)lParam;
      switch(ds->ID)
      {
        case eID_SET:
        {
          BtCam_owner_draw(ds); //执行自绘制按钮
          return TRUE;          
        }
        case ID_EXIT:
        {
          home_owner_draw(ds); 
          return TRUE;          
        }
        case ID_FPS:
        {
          button_owner_draw(ds);
          return TRUE;          
        }
        
      }
    }
    case WM_LBUTTONDOWN://点击屏幕，关闭窗口
    {

      PostCloseMessage(hwnd);
  

      break;
    }
    case WM_TIMER://摄像头状态机
    {
      u16 id;
      static int i = 0;
      
      id   =LOWORD(wParam); //获得产生该消息的控件ID.     
      if(id == 2)
      {
        i++;
        if(updata==1)
        {
          GUI_DEBUG("%d", i);
          i = 0;
          updata = 0;
          InvalidateRect(Cam_hwnd,NULL,FALSE);
        }
      }
      else if(id == 1)
      {
        switch(state)
        {
          case 0:
          {
            OV5640_Init();  
            OV5640_RGB565Config();
            OV5640_USER_Config();
            OV5640_FOCUS_AD5820_Init();

            if(cam_mode.auto_focus ==1)
            {
              OV5640_FOCUS_AD5820_Constant_Focus();
              focus_status = 1;
            }
            //使能DCMI采集数据
            DCMI_Cmd(ENABLE); 
            DCMI_CaptureCmd(ENABLE); 

            state = 1;
            break;
          }
          case 1:
          {
            ShowWindow(GetDlgItem(hwnd, eID_SET), SW_SHOW);
            ShowWindow(GetDlgItem(hwnd, ID_EXIT), SW_SHOW);
            state=2;
            break;
          }
          case 2:
          {
            update_flag = 1;
            break;
          }
        }
      }
      else if(id == 3)
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
        SelectDialogBox(hwnd, RC, L"没有检测到OV5640模块\n请重新检查连接。", L"错误", &ops);    // 显示错误提示框
        PostCloseMessage(hwnd);
        break; 
      }
      break;
    }
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      SURFACE *pSurf;
      HDC hdc_mem;
      HDC hdc;
      RECT rc;
//      static int switch_res = 0;
      static int old_fps = 0;
      WCHAR wbuf[128];
      hdc = BeginPaint(hwnd,&ps);
      GetClientRect(hwnd,&rc);
      if(state==0)
      {
        SetTextColor(hdc,MapRGB(hdc,250,250,250));
        SetBrushColor(hdc,MapRGB(hdc,0,0,0));
        SetPenColor(hdc,MapRGB(hdc,250,0,0));
        FillRect(hdc, &rc);
        DrawText(hdc,L"正在初始化摄像头\r\n\n请等待...",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);

      }     
      else
      {
        SetBrushColor(hdc,MapRGB(hdc,0,0,0));
        FillRect(hdc, &rc);
      }


      EndPaint(hwnd,&ps);
      break;
    }

    case WM_DESTROY:
    {
      state = 0;
      thread=0;//结束进程循环
      OV5640_Reset();//复位摄像头
      OV5640_Capture_Control(DISABLE);//关闭摄像头采集图像
      DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,DISABLE); //关闭DMA中断
      DCMI_Cmd(DISABLE); //DCMI失能
      DCMI_CaptureCmd(DISABLE); 
      
      if (!OV5640_State)
      {
//        GUI_Thread_Delete(h_autofocus);//删除自动对焦线程
//        GUI_Thread_Delete(h);//删除更新窗口线程
  //      GUI_VMEM_Free(bits);//释放图形缓冲区
      }
//      GUI_VMEM_Free(cam_buff1);
//      GUI_VMEM_Free(cam_buff0);
      //复位摄像头配置参数
      cur_Resolution = eID_RB3;
      cur_LightMode = eID_RB4;
      cur_SpecialEffects = eID_RB16;
      Camera_ReConfig();
      cur_index = 0;
      LCD_LayerCamInit((uint32_t)LCD_FRAME_BUFFER,800, 480);
      GUI_msleep(40);
      return PostQuitMessage(hwnd);	
    }    
    case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
    {
      u16 code,id;
      static int flag = 0;//设置窗口是否弹出
      code =HIWORD(wParam); //获得通知码类型.
      id   =LOWORD(wParam); //获得产生该消息的控件ID.
      if(flag == 0)//设置窗口未存在，则创建
      {
        if(id==eID_SET && code==BN_CLICKED)
        {
          flag = 1;
          WNDCLASS wcex;


          wcex.Tag	 		= WNDCLASS_TAG;
          wcex.Style			= CS_HREDRAW | CS_VREDRAW;
          wcex.lpfnWndProc	= (WNDPROC)dlg_set_WinProc;
          wcex.cbClsExtra		= 0;
          wcex.cbWndExtra		= 0;
          wcex.hInstance		= NULL;
          wcex.hIcon			= NULL;
          wcex.hCursor		= NULL;

          if(1)
          {
            RECT rc;

            GetClientRect(hwnd,&rc);
           

            win_rc.w =400;
            win_rc.h =400;

            win_rc.x = rc.x+(rc.w-win_rc.w)/2;
            win_rc.y = rc.y;//rc.y+(rc.h>>2);

            SetWIN = CreateWindowEx(
            NULL,
            &wcex,L"Set",
            WS_OVERLAPPED|WS_CLIPCHILDREN|WS_VISIBLE,

            win_rc.x,-win_rc.h-4,win_rc.w,win_rc.h,
            hwnd,0,NULL,NULL);
          }

        }
      }
      else//设置窗口已经存在，再次点击设置按钮，则关闭窗口
      {
        flag = 0;
        PostCloseMessage(SetWIN);

      }

      if(id==ID_EXIT && code==BN_CLICKED)//退出窗口
      {
        PostCloseMessage(hwnd);
      }
      break;  
    }
    case WM_Test:
    {
      InvalidateRect(hwnd,NULL,FALSE);
    }
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return WM_NULL;
}


void	GUI_Camera_DIALOG(void)
{	
	WNDCLASS	wcex;
	MSG msg;

   g_dma2d_en = TRUE;
	wcex.Tag = WNDCLASS_TAG;  
  
  
//  cam_buff0 = (uint16_t *)GUI_VMEM_Alloc(LCD_XSIZE*LCD_YSIZE*2);
//  cam_buff1 = (uint16_t *)GUI_VMEM_Alloc(LCD_XSIZE*LCD_YSIZE*2);

  
  
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	Cam_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI_Camera_Dialog",
                                    WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
									NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(Cam_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, Cam_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
  }


}
