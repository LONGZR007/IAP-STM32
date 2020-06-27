#include "emXGUI.h"
#include "x_libc.h"
#include <string.h>
#include "GUI_AVIList_DIALOG.h"
#include "GUI_AVIPLAYER_DIALOG.h"
#include "./Bsp/wm8978/bsp_wm8978.h" 
#include "./mjpegplayer/Backend_vidoplayer.h"
#include "emXGUI_JPEG.h"
#include "GUI_AppDef.h"

#define AVI_Player_48 "Music_Player_48_48.xft"
#define AVI_Player_64 "Music_Player_64_64.xft"
#define AVI_Player_72 "Music_Player_72_72.xft"

#define ID_EXIT       0x3000
 
void	GUI_AVIList_DIALOG(void);
static SCROLLINFO sif;/*设置耳机音量条的参数*/
static SCROLLINFO sif_horn;/*设置喇叭音量条的参数*/
int avi_chl = 0;
static COLORREF color_bg;//透明控件的背景颜色
static HDC hdc_bk;//背景图层
static int power=20;//音量值
static int power_horn=40;//音量值
int showmenu_flag = 0;//显示菜单栏
extern uint8_t avi_file_num;
extern int Play_index;
extern uint8_t  file_nums;
extern int sw_flag;//切换标志
extern char avi_playlist[FILE_MAX_NUM][FILE_NAME_LEN] __EXRAM;//播放List
extern BOOL Player_Init(void);
//HFONT AVI_Player_hFont48=NULL;
//HFONT AVI_Player_hFont64  =NULL;
//HFONT AVI_Player_hFont72  =NULL;

int LIST_STATE = 0;
//图标管理数组
static icon_S avi_icon[] = {
   {"yinliang",         {20, 400,48,48},      FALSE},
   {"bofangliebiao",    {732,400,48,48},      FALSE},
   {"back",             {294, 400, 64, 64},      FALSE},
   {"bofang",           {364, 400, 72, 72},      FALSE},
   {"next",             {442, 400, 64, 64},      FALSE},
   {"fenbianlv",        {0,40,380,40},   FALSE},
   {"zanting/bofang",   {300, 140, 200, 200}, FALSE},
   {"xiayishou",        {600, 200, 80, 80},   FALSE},    
   {"mini_next",        {580, 4, 80, 80},     FALSE},
   {"mini_Stop",        {652, 4, 80, 80},     FALSE},
   {"mini_back",        {724, 3, 80, 80},     FALSE},  
   {"上边栏",           {0 ,0, 800, 80},     FALSE},
   {"下边栏",           {0 ,400, 800, 80},     FALSE}, 
   {"bugle",           {674,400+20,48,48},      FALSE},  
};

/****************************控件重绘函数***********************/
static void AVI_Button_OwnerDraw(DRAWITEM_HDR *ds)
{
  if(LIST_STATE == 1)
    return;
   HDC hdc; //控件窗口HDC
   HWND hwnd; //控件句柄 
   RECT rc_cli,rc_tmp;//控件的位置大小矩形
   WCHAR wbuf[128];
   hwnd = ds->hwnd;
	 hdc = ds->hDC; 
   //获取控件的位置大小信息
   GetClientRect(hwnd, &rc_cli);
   
	 GetWindowText(hwnd,wbuf,128); //获得按钮控件的文字  
  
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc_cli);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

   //设置按键的颜色
   SetTextColor(hdc, MapRGB(hdc, 250,250,250));
   if(ds->ID == ID_TB2)
    DrawText(hdc, wbuf,-1,&rc_cli,DT_VCENTER|DT_RIGHT);
   else if(ds->ID == ID_TB3)
    DrawText(hdc, wbuf,-1,&rc_cli,DT_VCENTER|DT_LEFT);
   else
    DrawText(hdc, wbuf,-1,&rc_cli,DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
   

}
/**
  * @brief  button_owner_draw 按钮控件的重绘制
  * @param  ds:DRAWITEM_HDR结构体
  * @retval NULL
  */
static void button_owner_draw(DRAWITEM_HDR *ds)
{
   HDC hdc; //控件窗口HDC
   HDC hdc_mem;//内存HDC，作为缓冲区
   HWND hwnd; //控件句柄 
   RECT rc_cli,rc_tmp;//控件的位置大小矩形
   WCHAR wbuf[128];
	hwnd = ds->hwnd;
	hdc = ds->hDC; 
   //获取控件的位置大小信息
   GetClientRect(hwnd, &rc_cli);
   //创建缓冲层，格式为SURF_ARGB4444
   hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
   
	GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc_cli);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc_mem, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
   
//   SetBrushColor(hdc_mem,MapARGB(hdc_mem, 0, 255, 250, 250));
//   FillRect(hdc_mem, &rc_cli);
   //设置按键的颜色
   SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));
   if((ds->ID == ID_BUTTON_Back || ds->ID == ID_BUTTON_Next)&& ds->State & BST_PUSHED)
      SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,105,105,105));
   if(ds->ID == ID_BUTTON_Back || ds->ID == ID_BUTTON_Next)
   {
      SetFont(hdc_mem, controlFont_64);

   }
   else if(ds->ID == ID_BUTTON_Play || ds->ID == ID_BUTTON_Play)
   {
      SetFont(hdc_mem, controlFont_72);
   }
   else
   {
      //设置按钮字体
      SetFont(hdc_mem, controlFont_48);
   }
 
   DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
   
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
   
   //StretchBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, rc_cli.w, rc_cli.h, SRCCOPY);
   
   DeleteDC(hdc_mem);  
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
	GetClientRect(hwnd, &rc);
	/* 背景 */
//	SetBrushColor(hdc, color_bg);
//	FillRect(hdc, &rc);
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  
  
   rc_scrollbar.x = rc.x;
   rc_scrollbar.y = rc.h/2;
   rc_scrollbar.w = rc.w;
   rc_scrollbar.h = 2;
   
	SetBrushColor(hdc, MapRGB888(hdc, Page_c));
	FillRect(hdc, &rc_scrollbar);

	/* 滑块 */
	SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);

	SetBrushColor(hdc, MapRGB(hdc, 169, 169, 169));
	//rc.y += (rc.h >> 2) >> 1;
	//rc.h -= (rc.h >> 2);
	/* 边框 */
	//FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
	FillCircle(hdc, rc.x + rc.h / 2, rc.y + rc.h / 2, rc.h / 2);
   InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillCircle(hdc, rc.x + rc.h / 2, rc.y + rc.h / 2, rc.h / 2);
   //FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
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
	HDC hdc_mem1;
	RECT rc;
	RECT rc_cli;
	//	int i;

	hwnd = ds->hwnd;
	hdc = ds->hDC;
	GetClientRect(hwnd, &rc_cli);

	hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
	hdc_mem1 = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);   
         
   	
	//绘制白色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem1, color_bg, RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//绘制绿色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem, color_bg, RGB888( 250, 0, 0), RGB888( 250, 0, 0));
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
		BitBlt(hdc, rc.x, 0, rc.w, rc_cli.h, hdc_mem, rc.x, 0, SRCCOPY);
	}
	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}

/**
  * @brief  播放视频进程
  * @param  hwnd：屏幕窗口的句柄
  * @retval 无
  * @notes  
  */
TaskHandle_t h_avi;//音乐播放进程
static int thread=0;
static void App_PlayVEDIO(HWND hwnd)
{
	
	int app=0;
   //HDC hdc;
   
	if(thread==0)
	{  
//      h_avi=rt_thread_create("App_PlayVEDIO",(void(*)(void*))App_PlayVEDIO,NULL,10*1024,1,5);
      xTaskCreate((TaskFunction_t )(void(*)(void*))App_PlayVEDIO,  /* 任务入口函数 */
                            (const char*    )"App_PlayVEDIO",/* 任务名字 */
                            (uint16_t       )8*1024/2,  /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )hwnd,/* 任务入口函数参数 */
                            (UBaseType_t    )7, /* 任务的优先级 */
                            (TaskHandle_t  )&h_avi);/* 任务控制块指针 */
      thread =1;
//      rt_thread_startup(h_avi);//启动线程
      power = sif.nValue;				
      return;
	}
	while(thread) //线程已创建了
	{     
		if(app==0)
		{
         //hdc = GetDC(hwnd);
			app=1;
//      GUI_DEBUG("%s", avi_playlist[Play_index]);
      AVI_play(avi_playlist[Play_index], hwnd, power, power_horn);         
			app=0;
        // ReleaseDC(hwnd, hdc);
		}
	}
  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}
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

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

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
  * @brief  创建音乐列表进程
  * @param  无
  * @retval 无
  * @notes  
  */
static TaskHandle_t h1;
static int avilist_thread=0;
static void App_AVIList()
{
//	static int thread=0;
	static int app=0;
   
	if(avilist_thread==0)
	{  
//      h1=rt_thread_create("App_AVIList",(void(*)(void*))App_AVIList,NULL,4096,5,5);
      xTaskCreate((TaskFunction_t )(void(*)(void*))App_AVIList,  /* 任务入口函数 */
                            (const char*    )"App_AVIList",/* 任务名字 */
                            (uint16_t       )2*1024/4,  /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )NULL,/* 任务入口函数参数 */
                            (UBaseType_t    )11, /* 任务的优先级 */
                            (TaskHandle_t  )&h1);/* 任务控制块指针 */
//      rt_thread_startup(h1);				
      avilist_thread =1;
      return;
	}
//  while(1)
    {
    if(avilist_thread==1) //线程已创建了
    {
      if(app==0)
      {
        app=1;
        GUI_AVIList_DIALOG();
        app=0;
        avilist_thread=0;
      }
    }
//    GUI_msleep(10);
  }
  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}


static SCROLLINFO sif_time;/*设置进度条的参数*/
HWND avi_wnd_time;


static HWND wnd;

static HWND wnd_power;//音量icon句柄
static HWND wnd_horn;

static int Set_Widget_VCENTER(int y0, int h)
{
  return y0-h/2;
}
//HDC hdc_AVI=NULL;

HWND hwnd_AVI = NULL;
GUI_MUTEX*	AVI_JPEG_MUTEX = NULL;    // 用于确保一帧图像用后被释放完在退出线程
static int t0=0;
static int frame=0;
volatile int win_fps=0;
extern volatile int avi_fps;
extern UINT      BytesRD;
extern uint8_t   Frame_buf[];

static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static int ttt = 0;
   static BOOL res;
  static int IsCreate = 0;
  
   switch(msg)
   {
      case WM_CREATE:
      {
					t0 =GUI_GetTickCount();
				  frame =0;
					win_fps =0;
        
//          AVI_Player_hFont48 = GUI_Init_Extern_Font(AVI_Player_48);
//          AVI_Player_hFont64 = GUI_Init_Extern_Font(AVI_Player_64);
//          AVI_Player_hFont72 = GUI_Init_Extern_Font(AVI_Player_72);
//          AVI_Player_hFont48 = controlFont_48;
//          AVI_Player_hFont64 = controlFont_64;
//          AVI_Player_hFont72 = controlFont_72;
				

         AVI_JPEG_MUTEX = GUI_MutexCreate();    // 创建一个递归信号量
        
					//hdc_AVI =CreateMemoryDC(SURF_SCREEN,480,272);
#if 1 
         //音量icon（切换静音模式），返回控件句柄值
         avi_icon[0].rc.y = Set_Widget_VCENTER(440, avi_icon[0].rc.h);
         wnd_power = CreateWindow(BUTTON,L"A",WS_OWNERDRAW| WS_VISIBLE,//按钮控件，属性为自绘制和可视
                                  avi_icon[0].rc.x,avi_icon[0].rc.y,//位置坐标和控件大小
                                  avi_icon[0].rc.w,avi_icon[0].rc.h,//由avi_icon[0]决定
                                  hwnd,ID_BUTTON_Power,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_Power，附加参数为： NULL
         avi_icon[1].rc.y = Set_Widget_VCENTER(440, avi_icon[1].rc.h);                         
         //播放列表icon
         CreateWindow(BUTTON,L"D",WS_OWNERDRAW|WS_VISIBLE, //按钮控件，属性为自绘制和可视
                      avi_icon[1].rc.x,avi_icon[1].rc.y,//位置坐标
                      avi_icon[1].rc.w,avi_icon[1].rc.h,//控件大小
                      hwnd,ID_BUTTON_List,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_List，附加参数为： NULL

         CreateWindow(BUTTON,L"Q",WS_OWNERDRAW|WS_VISIBLE, //按钮控件，属性为自绘制和可视
                      avi_icon[13].rc.x,avi_icon[13].rc.y,//位置坐标
                      avi_icon[13].rc.w,avi_icon[13].rc.h,//控件大小
                      hwnd,ID_BUTTON_Bugle,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_Bugle，附加参数为： NULL

         avi_icon[2].rc.y = Set_Widget_VCENTER(440, avi_icon[2].rc.h);
         //上一首icon
         CreateWindow(BUTTON,L"S",WS_OWNERDRAW| WS_VISIBLE, //按钮控件，属性为自绘制和可视
                      avi_icon[2].rc.x,avi_icon[2].rc.y,//位置坐标
                      avi_icon[2].rc.w,avi_icon[2].rc.h,//控件大小
                      hwnd,ID_BUTTON_Back,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_List，附加参数为： NULL
         avi_icon[3].rc.y = Set_Widget_VCENTER(440, avi_icon[3].rc.h);
         //播放icon
         CreateWindow(BUTTON,L"U",WS_OWNERDRAW| WS_VISIBLE, //按钮控件，属性为自绘制和可视
                      avi_icon[3].rc.x,avi_icon[3].rc.y,//位置坐标
                      avi_icon[3].rc.w,avi_icon[3].rc.h,//控件大小
                      hwnd,ID_BUTTON_Play,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_List，附加参数为： NULL
         avi_icon[4].rc.y = Set_Widget_VCENTER(440, avi_icon[4].rc.h);
         //下列icon
         CreateWindow(BUTTON,L"V",WS_OWNERDRAW| WS_VISIBLE, //按钮控件，属性为自绘制和可视
                      avi_icon[4].rc.x,avi_icon[4].rc.y,//位置坐标
                      avi_icon[4].rc.w,avi_icon[4].rc.h,//控件大小
                      hwnd,ID_BUTTON_Next,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_List，附加参数为： NULL
         
         CreateWindow(BUTTON,L"分辨率：0*0",WS_OWNERDRAW| WS_VISIBLE,
                      0,40,380,40,hwnd,ID_TB2,NULL,NULL);
 
         
         //歌曲名字
         CreateWindow(BUTTON,L"",WS_OWNERDRAW| WS_VISIBLE,
                      100,0,600,40,hwnd,ID_TB1,NULL,NULL);
                      
         //总时间        
         CreateWindow(BUTTON,L"00:00:00",WS_OWNERDRAW| WS_VISIBLE,
                      680,Set_Widget_VCENTER(382, 30) ,120,30,hwnd,ID_TB4,NULL,NULL);

         //当前时间           
         CreateWindow(BUTTON,L"00:00:00",WS_OWNERDRAW| WS_VISIBLE,
                      0,Set_Widget_VCENTER(382, 30) ,120,30,hwnd,ID_TB5,NULL,NULL);
     
                     
         CreateWindow(BUTTON,L"帧率:0FPS/s",WS_OWNERDRAW| WS_VISIBLE,
                      420,40,300,40,hwnd,ID_TB3,NULL,NULL);

         /*********************歌曲进度条******************/
         sif_time.cbSize = sizeof(sif_time);
         sif_time.fMask = SIF_ALL;
         sif_time.nMin = 0;
         sif_time.nMax = 255;
         sif_time.nValue = 0;//初始值
         sif_time.TrackSize = 35;//滑块值
         sif_time.ArrowSize = 0;//两端宽度为0（水平滑动条）          
         avi_wnd_time = CreateWindow(SCROLLBAR, L"SCROLLBAR_Time",  WS_OWNERDRAW|WS_VISIBLE, 
                         120, 365, 560, 35, hwnd, ID_SCROLLBAR_TIMER, NULL, NULL);
         SendMessage(avi_wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_time);
         /*********************音量值滑动条******************/
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 63;//音量最大值为63
         sif.nValue = 20;//初始音量值
         sif.TrackSize = 31;//滑块值
         sif.ArrowSize = 0;//两端宽度为0（水平滑动条）
         wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW, 
                            70, Set_Widget_VCENTER(440, 31), 150, 31, hwnd, ID_SCROLLBAR_POWER, NULL, NULL);
         SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);   

          /*********************喇叭音量值滑动条******************/
         sif_horn.cbSize = sizeof(sif);
         sif_horn.fMask = SIF_ALL;
         sif_horn.nMin = 0;
         sif_horn.nMax = 63;//音量最大值为63
         sif_horn.nValue = 40;//初始音量值
         sif_horn.TrackSize = 31;//滑块值
         sif_horn.ArrowSize = 0;//两端宽度为0（水平滑动条）
         wnd_horn = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_OWNERDRAW, 
                            70, Set_Widget_VCENTER(440, 31), 150, 31, hwnd, eID_SCROLLBAR_HORN, NULL, NULL);
         SendMessage(wnd_horn, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_horn); 
         
         
         CreateWindow(BUTTON, L"O",WS_OWNERDRAW|WS_VISIBLE,
                        740, 12, 36, 36, hwnd, ID_EXIT, NULL, NULL);         
 #endif   
         
         
         u8 *jpeg_buf;
         u32 jpeg_size;
         JPG_DEC *dec;
         res = RES_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
         hdc_bk = CreateMemoryDC(SURF_SCREEN, 800, 480);
         if(res)
         {
            /* 根据图片数据创建JPG_DEC句柄 */
            dec = JPG_Open(jpeg_buf, jpeg_size);

            /* 绘制至内存对象 */
            JPG_Draw(hdc_bk, 0, 0, dec);

            /* 关闭JPG_DEC句柄 */
            JPG_Close(dec);
         }
         /* 释放图片内容空间 */
         RES_Release_Content((char **)&jpeg_buf);         
         //GUI_AVIList_DIALOG();
         Player_Init();
         break;
      }

      case WM_DRAWITEM:
      {     
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;
         if (ds->ID == ID_SCROLLBAR_POWER || ds->ID == ID_SCROLLBAR_TIMER || ds->ID == eID_SCROLLBAR_HORN)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }//         
         if(ds->ID == ID_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;         
         }
         else if ((ds->ID >= 0x1000 && ds->ID<= 0x1099))
         {
            button_owner_draw(ds);
            return TRUE;
         }
         if(ds->ID >= ID_TB1 && ds->ID <= ID_TB5)
         {
            AVI_Button_OwnerDraw(ds);
            return TRUE;
         }

      }  
//		case	WM_CTLCOLOR:
//		{
//			/* 控件在绘制前，会发送 WM_CTLCOLOR到父窗口.
//			 * wParam参数指明了发送该消息的控件ID;lParam参数指向一个CTLCOLOR的结构体指针.
//			 * 用户可以通过这个结构体改变控件的颜色值.用户修改颜色参数后，需返回TRUE，否则，系统
//			 * 将忽略本次操作，继续使用默认的颜色进行绘制.
//			 *
//			 */
//			u16 id;
//			id =LOWORD(wParam);
//			if(id== ID_TB1 || id== ID_TB2 || id== ID_TB4 || id== ID_TB3)
//			{
//				CTLCOLOR *cr;
//				cr =(CTLCOLOR*)lParam;
//				cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
//				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
//				cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)
//				return TRUE;
//			}
//			else
//			{
//				return FALSE;
//			}
//		}			
      //绘制窗口界面消息
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc;//屏幕hdc
         
//				WCHAR wbuf[40];
         RECT rc;
         int t1;
         GetClientRect(hwnd, &rc);
         hdc = BeginPaint(hwnd, &ps);   
//         SetBrushColor(hdc, MapRGB(hdc, 0,0,0));
//         FillRect(hdc, &rc);
         frame++;
         t1 =GUI_GetTickCount();
         if((t1-t0)>=1000)
         {
            win_fps =frame;
            t0 =t1;
            frame =0;
         }
                
         
         
         color_bg = GetPixel(hdc, 385, 404);
         EndPaint(hwnd, &ps);
         if(ttt == 0)
         {
            ttt = 1;
            App_PlayVEDIO(hwnd);
         }
         break;
      }
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         RECT rc =*(RECT*)lParam;
         
         if(res!=FALSE)
            BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc.x, rc.y, SRCCOPY);
    


         return TRUE;

      }      
      
      case WM_NOTIFY:
      {
         u16 code, id, ctr_id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         //发送单击
         if(code == BN_CLICKED)
         { 
            switch(id)
            {
               //音量icon处理case
               case ID_BUTTON_Power:
               {

                  avi_icon[0].state = ~avi_icon[0].state;
                  //InvalidateRect(hwnd, &music_icon[0].rc, TRUE);
                  //当音量icon未被按下时
                  if(avi_icon[0].state == FALSE)
                  {
                      WCHAR wbuf[3];
                    HWND  wnd1 = GetDlgItem(hwnd, ID_BUTTON_Bugle);

                    GetWindowText(wnd1, wbuf, 3);
                    if (wbuf[0] == L'P')//为扬声器输出
                    {
                      ShowWindow(wnd_horn, SW_HIDE); //窗口隐藏
                    }
                    else// 为耳机输出
                    {
                      ShowWindow(wnd, SW_HIDE); //窗口隐藏
                    }
                  }
                  //当音量icon被按下时，设置为静音模式
                  else
                  {                
                    WCHAR wbuf[3];
                    HWND  wnd1 = GetDlgItem(hwnd, ID_BUTTON_Bugle);
                 
                    GetWindowText(wnd1, wbuf, 3);
                    if (wbuf[0] == L'P')//为扬声器输出
                    {
                       ShowWindow(wnd_horn, SW_SHOW); //窗口隐藏
                    }
                    else// 为耳机输出
                    {
                       ShowWindow(wnd, SW_SHOW); //窗口显示
                    }  
                  }
                  break;
               }              
            
//               case ID_BUTTON_List:
//               {
//                 //if(IsCreate == 0)
//                 {
//                   IsCreate = 1;
//                   App_AVIList();
//                 }
////                 else
////                   avilist_thread = 1;
//                  break;
//               }

               /* 音频输出选择按钮 */
               case ID_BUTTON_Bugle:
               {
                  WCHAR wbuf[3];
                  HWND  wnd1 = GetDlgItem(hwnd, ID_BUTTON_Bugle);
               
                  GetWindowText(wnd, wbuf, 3);
                  if (wbuf[0] == L'P')
                  {
                     SetWindowText(wnd1, L"Q");
                     wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);    // 配置为耳机输出
                  }
                  else
                  {
                     SetWindowText(wnd1, L"P");
                     wm8978_CfgAudioPath(DAC_ON, SPK_ON);                        // 配置为扬声器输出
                  }
                  
                  if(avi_icon[0].state != FALSE)    // 音量调节滑动条已弹出，切换调节滑动调
                  {
                     if (wbuf[0] == L'P')     // 为耳机输出（上面刚刚改变了！）
                     {
                        ShowWindow(wnd_horn, SW_HIDE); // 窗口显示
                        ShowWindow(wnd, SW_SHOW);      // 窗口显示
                     }
                     else         // 为喇叭输出（上面刚刚改变了！）
                     {
                        ShowWindow(wnd_horn, SW_SHOW); // 窗口显示
                        ShowWindow(wnd, SW_HIDE);      // 窗口隐藏
                     }
                  }
               }
               break; 
               case ID_BUTTON_Play:
               {

                  avi_icon[3].state = ~avi_icon[3].state;
                  //InvalidateRect(hwnd, &avi_icon[0].rc, TRUE);
                  //当音量icon未被按下时
                  if(avi_icon[3].state == FALSE)
                  {
                     I2S_Play_Start();
                     TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
                     TIM_Cmd(TIM3,ENABLE); //使能定时器3                        
                     
                     SetWindowText(GetDlgItem(hwnd, ID_BUTTON_Play), L"U");
                              
                  }
                  //当音量icon被按下时，暂停
                  else
                  {          
                     I2S_Play_Stop();
                     TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //允许定时器3更新中断
                     TIM_Cmd(TIM3,DISABLE); //使能定时器3                     
                     SetWindowText(GetDlgItem(hwnd, ID_BUTTON_Play), L"T");
                  }
                  //
                  break;
               }
               case ID_BUTTON_Back:
               {
            
                  Play_index--;
                  if(Play_index < 0)
                     Play_index = file_nums - 1;  
                  sw_flag = 1;   

                  
                  sif_time.nValue = 0;//设置为0
                  SendMessage(avi_wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_time);                  
                  

                  break;
               }
               case ID_BUTTON_Next:
               {                  
                  Play_index++;
                  
                  if(Play_index > file_nums -1 )
                     Play_index = 0;
                  sw_flag = 1;

                  sif_time.nValue = 0;//设置为0
                  SendMessage(avi_wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_time);                    
                  break;
               }
               case ID_EXIT:
               {
                  PostCloseMessage(hwnd);
                  break;
               }
            }
         }
        if(id==ID_BUTTON_List && code==BN_CLICKED)
        {
          GUI_MutexLock(AVI_JPEG_MUTEX,0xFFFFFFFF);    // 获取互斥量
          WNDCLASS wcex;
          LIST_STATE = 1;
          wcex.Tag	 		= WNDCLASS_TAG;
          wcex.Style			= CS_HREDRAW | CS_VREDRAW;
          wcex.lpfnWndProc	= (WNDPROC)list_win_proc;
          wcex.cbClsExtra		= 0;
          wcex.cbWndExtra		= 0;
          wcex.hInstance		= NULL;
          wcex.hIcon			= NULL;
          wcex.hCursor		= NULL;
          if(1)
          {
            RECT rc;

            GetClientRect(hwnd,&rc);

            CreateWindowEx(NULL,
                            &wcex,L"VideoList",
                            WS_VISIBLE|WS_CLIPCHILDREN,
                            0,0,800,480,
                            hwnd,0,NULL,NULL);
            GUI_MutexUnlock(AVI_JPEG_MUTEX);              // 解锁互斥量
            
          }
        }
         
         NMHDR *nr;  
         ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID. 
         nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
         //进度条处理case
         if (ctr_id == ID_SCROLLBAR_TIMER)
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
                  //置位进度条变更位置
                  avi_chl = 1;//滑动标志
               }
               break;
            }
         }
         //音量条处理case
         static int ttt = 0;
         if (ctr_id == ID_SCROLLBAR_POWER)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R滑块移动
               {
                  power= sb_nr->nTrackValue; //得到当前的音量值
                  if(power == 0) 
                  {
                     wm8978_OutMute(1);//静音
                    SendMessage(wnd_horn, SBM_SETVALUE, TRUE, power_horn); //发送SBM_SETVALUE，设置音量值
                     SetWindowText(wnd_power, L"J");
                     ttt = 1;
                     
                  }
                  else
                  {
                     if(ttt == 1)
                     {
                        SetWindowText(wnd_power, L"A");
                        ttt = 0;
                     }
                     wm8978_OutMute(0);
                     wm8978_SetOUT1Volume(power);//设置WM8978的音量值
                  } 
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, power); //发送SBM_SETVALUE，设置音量值
               }
               break;
            }
         }
//喇叭音量条处理case

         if (ctr_id == eID_SCROLLBAR_HORN)
         {
            NM_SCROLLBAR *sb_nr;
            sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
            
            switch (nr->code)
            {
               case SBN_THUMBTRACK: //R滑块移动
               {
                  power_horn= sb_nr->nTrackValue; //得到当前的音量值
                  if(power_horn == 0) 
                  {
                     wm8978_OutMute(1);//静音
                     SendMessage(wnd, SBM_SETVALUE, TRUE, power); //发送SBM_SETVALUE，设置音量值
                     SetWindowText(wnd_power, L"J");
                     ttt = 1;
                     
                  }
                  else
                  {
                     if(ttt == 1)
                     {
                        SetWindowText(wnd_power, L"A");
                        ttt = 0;
                     }
                     wm8978_OutMute(0);
                     wm8978_SetOUT2Volume(power_horn);//设置WM8978的音量值
                  } 
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, power_horn); //发送SBM_SETVALUE，设置音量值
               }
               break;
            }
         } 

      break; 
      
      }       
      case WM_CLOSE:
      {
        GUI_MutexLock(AVI_JPEG_MUTEX,0xFFFFFFFF);    // 获取互斥量确保一帧图像的内存使用后已释放
         if(IsCreate)
         {
          IsCreate=0;
          GUI_Thread_Delete(h1);
         }
         GUI_MutexDelete(AVI_JPEG_MUTEX);
         
         thread_ctrl = 0;
         I2S_Play_Stop();
         I2S_Stop();		/* 停止I2S录音和放音 */
         wm8978_Reset();	/* 复位WM8978到复位状态 */
         TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //允许定时器3更新中断
         thread = 0;
         ttt = 0;//部分内容只执行一次的记录变量
         //rt_thread_delete(h1);
         power=20;
         Play_index = 0;
         avi_file_num = 0;
         res = FALSE;
         GUI_Thread_Delete(h_avi);
         
         DeleteDC(hdc_bk);
         return DestroyWindow(hwnd); //调用DestroyWindow函数来销毁窗口（该函数会产生WM_DESTROY消息）。; //关闭窗口返回TRUE。
      }
      default :
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }  
   return WM_NULL;
}



//视频播放器句柄
HWND	VideoPlayer_hwnd;
void	GUI_VideoPlayer_DIALOG(void)
{	
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

	//创建主窗口
	VideoPlayer_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                                    &wcex,
                                    L"GUI_MUSICPLAYER_DIALOG",
                                    WS_VISIBLE|WS_OVERLAPPED|WS_CLIPCHILDREN,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(VideoPlayer_hwnd, SW_SHOW);
  
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, VideoPlayer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


