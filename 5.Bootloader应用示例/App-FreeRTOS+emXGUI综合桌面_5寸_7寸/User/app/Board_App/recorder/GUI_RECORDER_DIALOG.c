#include "emXGUI.h"
#include "GUI_RECORDER_DIALOG.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "./mp3_player/Backend_mp3Player.h"
#include "Backend_Recorder.h"

//图标管理数组
recorder_icon_t record_icon[] = {

   {L"A",        {308, 412, 48, 48},   ID_RECORD_bPOWER},    // 0. 音量
   {L"Q",        {718, 407, 64, 64},   ID_RECORD_BUGLE},     // 1. 喇叭按钮
   {L"S",        {446, 407, 64, 64},   ID_RECORD_BACK},      // 2. 上一首
   {L"T",        {538, 405, 72, 72},   ID_RECORD_PLAY},      // 3. 播放
   {L"V",        {642, 407, 64, 64},   ID_RECORD_NEXT},      // 4. 下一首
   {L"U",        {79,  308, 64, 64},   ID_RECORD_STOP},      // 5. 停止录音
   {L"U",        {181, 308, 64, 64},   ID_RECORD_START},     // 6. 开始录音
   {L"U",        {181, 308, 72, 72},   ID_RECORD_PADNC},     // 7. 暂停继续
   {L"O",        {740,  12, 36, 36},   ID_RECORD_EXIT},      // 8. 退出

   {L"录音机",    {96,  85, 120, 30},   ID_RECORD_STATE},    // 9. 正在录音
   {L"00:00",    {106, 187,100, 30},   ID_RECORD_TIME},      // 10. 录音时长
   {L"00:00",    {302, 379, 66, 30},   ID_PLAY_TIME},        // 11. 播放时长
   {L"00:00",    {732, 379, 66, 30},   ID_PLAY_TOTAL_TIME},  // 12. 录音总时长
  
   {L" ",        {325, 47, 406,301},   ID_RECORD_LIST},      // 13. 音乐列表
   {L" ",        {370, 378,355, 30},   ID_PLAY_PROGRESS},    // 14. 播放进度条
   {L" ",        {369, 423, 74, 30},   ID_RECORD_sPOWER},    // 15. 音量进度条
  
};

static HDC hdc_bk;
//音乐播放器句柄
HWND	Recorer_hwnd;
TaskHandle_t h_record;    // 播放任务控制句柄
TaskHandle_t h_play_record;

/* 以下在 MUSIC 中定义的，这里直接用，不重新定义，以节约内存 */
extern char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN] __EXRAM;  // 播放List
extern char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN] __EXRAM;  // 显示list
extern uint8_t  music_file_num;                   // 文件个数
extern char music_name[FILE_NAME_LEN] __EXRAM;    // 歌曲名数组
extern int play_index;      // 播放歌曲的编号值
extern HWND music_wnd_time; // 歌曲进度条窗口句柄
extern uint8_t chgsch;      // 调整进度条标志位

static char path[100];     // 文件根目录
static int  power = 20;    // 音量值

/**
  * @brief  scan_files 递归扫描sd卡内的录音文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_record_files(char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[FILE_NAME_LEN];
	WCHAR wbuf[128];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif  
  res = f_opendir(&dir, path); //打开目录
  if (res == FR_OK) 
  { 
    i = strlen(path); 
    for (;;) 
    { 
      res = f_readdir(&dir, &fno); 										//读取目录下的内容
     if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //if(strstr(path,"recorder")!=NULL)continue;      // 逃过录音文件
      if (*fn == '.') continue; 											  // 点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							  // 目录，递归读取
        sprintf(&path[i], "/%s", fn); 							    // 合成完整目录名
        res = scan_record_files(path);									// 递归遍历 
        if (res != FR_OK) 
					break; 																	     	// 打开失败，跳出循环
        path[i] = 0; 
      } 
      else 
      { 
				//printf("%s/%s\r\n", path, fn);								// 输出文件名
				if(strstr(fn,".wav")||strstr(fn,".WAV"))      // 判断是否wav文件
				{
					if ((strlen(path)+strlen(fn)<FILE_NAME_LEN)&&(music_file_num<MUSIC_MAX_NUM))
					{
						sprintf(file_name, "%s/%s", path, fn);						
						memcpy(music_playlist[music_file_num], file_name, strlen(file_name) + 1);
             //     printf("%s\r\n", music_playlist[music_file_num]);
						//memcpy(music_lcdlist[music_file_num],fn,strlen(fn));
						x_mbstowcs_cp936(wbuf, fn, sizeof(wbuf));	    // 将Ansi字符转换成GUI的unicode字符.
						//在Listbox中增加一个Item项，记录文件名和文件属性.
						i = SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_GETCOUNT, 0, 0);
						SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_ADDSTRING, i, (LPARAM)wbuf);
						music_file_num++;//记录文件个数
					}
				}//if mp3||wav
      }//else
     } //for
  } 
  return res; 
}

/**
  * @brief  播放录音任务
  * @param  hwnd:主窗口句柄
  * @retval 无
  */

/* 在 Backend_Recorder.c 中定义 */
extern uint8_t Isread;              /* DMA传输完成标志 */
extern uint8_t bufflag;             /* 数据缓存区选择标志 */
extern uint32_t wavsize;            /* wav音频数据大小 */
extern FIL record_file __EXRAM;			/* file objects */
extern char recfilename[25]; 
extern WavHead rec_wav;             /* WAV设备  */
extern FRESULT result; 
extern UINT bw;            					/* File R/W count */
extern REC_TYPE Recorder;           /* 录音设备 */
extern uint16_t record_buffer0[RECBUFFER_SIZE] __EXRAM;  /* 数据缓存区1 ，实际占用字节数：RECBUFFER_SIZE*2 */
extern uint16_t record_buffer1[RECBUFFER_SIZE] __EXRAM;  /* 数据缓存区2 ，实际占用字节数：RECBUFFER_SIZE*2 */

static void App_Record(void *p)
{
  vTaskSuspend(h_record);    // 挂起线程
  
	while(1) //线程已创建了
	{     
		/* DMA传输完成 */
		if(Isread==1)
		{
			Isread=0;
      if(bufflag==0)
        result=f_write(&record_file,record_buffer0,RECBUFFER_SIZE*2,(UINT*)&bw);//写入文件							
      else
        result=f_write(&record_file,record_buffer1,RECBUFFER_SIZE*2,(UINT*)&bw);//写入文件
      wavsize+=RECBUFFER_SIZE*2;	
		}
    GUI_Yield();
   }
//  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}

/**
  * @brief  播放录音任务
  * @param  hwnd:主窗口句柄
  * @retval 无
  */
static int thread=0;
static void App_PlayRecord(HWND hwnd)
{
  HDC hdc;
                            
  thread =1;
  vTaskSuspend(h_play_record);    // 挂起线程
  
	while(thread) //线程已创建了
	{     
    int i = 0;      
     
    i = 0;
     //得到播放曲目的文件名
    while(music_playlist[play_index][i]!='\0')
    {
      music_name[i]=music_playlist[play_index][i];
      i++;
    }
    music_name[i]='\0';
    if (play_index != SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL, 0, 0))     // 检查当前要播放的项目是不是已经选中
    {
      SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, play_index, 0);            // 设置当前播放的歌曲为选中状态
    }

    if(strstr(music_name,".wav")||strstr(music_name,".WAV"))
    {
      wavplayer(music_name, power, hdc, hwnd);
    }

    printf("播放结束\n");

    //进行任务调度
    GUI_msleep(20);
	   
   }
   GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}

static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
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

static void stop_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  EnableAntiAlias(hdc, TRUE);
   
  if (ds->Style & WS_DISABLED)    // 窗口是禁止的
  {
    SetBrushColor(hdc, MapRGB(hdc, 200, 200, 200));
    SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  {
    if (ds->State & BST_PUSHED)
    { //按钮是按下状态
      SetPenColor(hdc, MapRGB(hdc, 185, 185, 185));
      SetBrushColor(hdc, MapRGB(hdc, 185, 185, 185));      //设置文字色
    }
    else
    { //按钮是弹起状态
      SetPenColor(hdc, MapRGB(hdc, 255, 255, 255));
      SetBrushColor(hdc, MapRGB(hdc, 255, 255, 255));
    }
  }
  
  /* 画圆环 */
  SetPenSize(hdc, 2);
  DrawCircle(hdc, rc.x+rc.w/2, rc.y+rc.h/2, rc.w/2);

  /* 绘制圆角矩形 */
	InflateRect(&rc, -13, -13);
  FillRoundRect(hdc, &rc, 5);
  
  EnableAntiAlias(hdc, FALSE);
}

static void start_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HDC hdc;
	RECT rc;

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  EnableAntiAlias(hdc, TRUE);
 
  //设置按键的颜色
	if (ds->Style & WS_DISABLED)    // 窗口是禁止的
  {
    SetBrushColor(hdc, MapRGB(hdc, 180, 20, 20));      //设置文字色
    SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
  }
  else
  {
    if (ds->State & BST_PUSHED)
    { //按钮是按下状态
      SetBrushColor(hdc, MapRGB(hdc, 180, 20, 20));      //设置文字色
      SetPenColor(hdc, MapRGB(hdc, 200, 200, 200));
    }
    else
    { //按钮是弹起状态
      SetBrushColor(hdc, MapRGB(hdc, 240, 10, 10));
      SetPenColor(hdc, MapRGB(hdc, 255, 255, 255));
    }
  }
  
  /* 画圆环 */
	SetPenSize(hdc, 2);
  DrawCircle(hdc, rc.x+rc.w/2, rc.y+rc.h/2, rc.w/2);

  /* 绘制圆角矩形 */
  FillCircle(hdc, rc.x+rc.w/2, rc.x+rc.w/2, 27);
  
  EnableAntiAlias(hdc, FALSE);
}

/***********************控件重绘函数********************************/
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
   RECT rc_cli, rc_tmp;//控件的位置大小矩形
   WCHAR wbuf[128];
	hwnd = ds->hwnd;
	hdc = ds->hDC; 
//   if(ds->ID ==  ID_BUTTON_START && show_lrc == 1)
//      return;
   //获取控件的位置大小信息
   GetClientRect(hwnd, &rc_cli);
   //创建缓冲层，格式为SURF_ARGB4444
   hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
   
	GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
//   if(ds->ID == ID_BUTTON_Power || ds->ID == ID_BUTTON_MINISTOP){
//      SetBrushColor(hdc, color_bg);
//      FillRect(hdc, &rc_cli);
//   }
//   
//   SetBrushColor(hdc_mem,MapARGB(hdc_mem, 0, 255, 250, 250));
//   FillRect(hdc_mem, &rc_cli);

   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc_cli);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc_mem, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

   //播放键使用100*100的字体
   if(ds->ID == ID_RECORD_PLAY || ds->ID == ID_RECORD_PADNC)
      SetFont(hdc_mem, controlFont_72);
   else if(ds->ID == ID_RECORD_bPOWER)
      SetFont(hdc_mem, controlFont_48);
   else
      SetFont(hdc_mem, controlFont_64);

	//设置按键的颜色
	if (ds->Style & WS_DISABLED)    // 窗口是禁止的
  {
    SetTextColor(hdc_mem, MapARGB(hdc_mem, 250, 220, 220, 220));      //设置文字色
  }
  else
  {
    if (ds->State & BST_PUSHED)
    { //按钮是按下状态
      SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,105,105,105));      //设置文字色
    }
    else
    { //按钮是弹起状态
      SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));
    }
  }
 
   DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER);//绘制文字(居中对齐方式)
   
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
   
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
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

   rc_scrollbar.x = rc.x;
   rc_scrollbar.y = rc.h/2-1;
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
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
   InflateRect(&rc, -2, -2);

	SetBrushColor(hdc, MapRGB888(hdc, fore_c));
	FillCircle(hdc, rc.x + rc.w / 2, rc.y + rc.h / 2, rc.h / 2 - 1);
   //FillRoundRect(hdc, &rc, MIN(rc.w, rc.h) >> 2);
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
   RECT rc,rc_tmp;
	WCHAR wbuf[128];

	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.

	/* 背景 */
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

	SetTextColor(hdc, MapRGB(hdc, 50, 50, 50));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
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
	draw_scrollbar(hwnd, hdc_mem1, 0, RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//绘制绿色类型的滚动条
	draw_scrollbar(hwnd, hdc_mem, 0, RGB888(	50, 205, 50), RGB888(50, 205, 50));
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

/*
 * @brief  重绘列表
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void listbox_owner_draw(DRAWITEM_HDR *ds)
{
	HWND hwnd;
	HDC hdc;
	RECT rc,rc_tmp;
	int i,count,cursel;
	WCHAR wbuf[128];
	hwnd =ds->hwnd;
	hdc =ds->hDC;

//	hdc =CreateMemoryDC(SURF_ARGB4444,ds->rc.w,ds->rc.h); //创建一个内存DC来绘图.

	rc =ds->rc;

   /* 背景 */
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  
  if (!SendMessage(hwnd,LB_GETCOUNT,0,0))
  {
    /* 列表为空，显示提示信息然后直接返回 */
    DrawText(hdc, L"还没有录音文件！", -1, &rc, DT_CENTER|DT_VCENTER);
    return;
  }

	i=SendMessage(hwnd,LB_GETTOPINDEX,0,0);
	count=SendMessage(hwnd,LB_GETCOUNT,0,0);
	cursel=SendMessage(hwnd,LB_GETCURSEL,0,0);

	while(i<count)
	{
		SendMessage(hwnd,LB_GETITEMRECT,i,(LPARAM)&rc);
		if(rc.y > ds->rc.h)
		{
			break;
		}

    SetTextColor(hdc, MapRGB(hdc,50,10,10));

    if(i==cursel)
    {
      SetTextColor(hdc, MapRGB(hdc, 250, 10, 10));
    }
    else
    {
      SetTextColor(hdc, MapRGB(hdc, 10, 10, 10));
    }

    /* 画前面的图标 */
    // SetBrushColor(hdc, MapRGB(hdc, 10, 10, 10));
    // EnableAntiAlias(hdc, TRUE);
    // FillPolygon(hdc, 0, 0, pt, 5);
    // EnableAntiAlias(hdc, FALSE);

    SendMessage(hwnd, LB_GETTEXT, i, (LPARAM)wbuf);

    DrawText(hdc, wbuf, -1, &rc, DT_SINGLELINE | DT_LEFT | DT_VCENTER);      // 显示列表文本
    
    SetPenColor(hdc, MapRGB(hdc, 10, 10, 10));
    HLine(hdc, rc.x, rc.y + rc.h - 1, rc.x + rc.w);                         // 画一条线

    i++; 
  }
}

extern int SelectDialogBox(HWND hwndParent, RECT rc, const WCHAR *pText, const WCHAR *pCaption, const MSGBOX_OPTIONS *ops);
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static uint8_t  BUGLE_STATE = 0;     // 喇叭状态
	 static uint32_t Record_Timer = 0;    // 录音计时器
  
   switch(msg){
      case WM_CREATE:
      {
         u8 *jpeg_buf;
         u32 jpeg_size;
         JPG_DEC *dec;
         BOOL res = NULL;

         res = RES_Load_Content(GUI_RECORDER_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
         //res = FS_Load_Content(GUI_RECORDER_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
         hdc_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
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

//         exit_sem = GUI_SemCreate(0,1);//创建一个信号量        

         for (uint8_t xC=0; xC<9; xC++)     // 0~7 ：按钮
         {
            /* 循环创建按钮 */
            CreateWindow(BUTTON, record_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE | WS_TRANSPARENT,
                         record_icon[xC].rc.x, record_icon[xC].rc.y,
                         record_icon[xC].rc.w,record_icon[xC].rc.h,
                         hwnd, record_icon[xC].id, NULL, NULL); 
         }
          
         ShowWindow(GetDlgItem(hwnd, ID_RECORD_PADNC), SW_HIDE);     // 窗口隐藏继续和暂停的按钮
         EnableWindow(GetDlgItem(hwnd, ID_RECORD_STOP), DISABLE);    // 禁用停止按钮

         for (uint8_t xC=9; xC<13; xC++)    // 8~11 ：文本框
         {
            /* 循环创建文本框 */
            CreateWindow(TEXTBOX, record_icon[xC].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                         record_icon[xC].rc.x, record_icon[xC].rc.y,
                         record_icon[xC].rc.w,record_icon[xC].rc.h,
                         hwnd, record_icon[xC].id, NULL, NULL); 
         }

         /*********************歌曲进度条******************/
         SCROLLINFO sif;
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 255;
         sif.nValue = 0;//初始值
         sif.TrackSize = 30;//滑块值
         sif.ArrowSize = 0;//两端宽度为0（水平滑动条）          
         music_wnd_time = CreateWindow(SCROLLBAR, record_icon[14].icon_name, WS_OWNERDRAW | WS_VISIBLE,
                           record_icon[14].rc.x, record_icon[14].rc.y, record_icon[14].rc.w,
                           record_icon[14].rc.h, hwnd, record_icon[14].id, NULL, NULL);
         SendMessage(music_wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);         

         /*********************音量值滑动条******************/
         SCROLLINFO sif_power;
         sif_power.cbSize = sizeof(sif_power);
         sif_power.fMask = SIF_ALL;
         sif_power.nMin = 0;
         sif_power.nMax = 63;//音量最大值为63
         sif_power.nValue = 20;//初始音量值
         sif_power.TrackSize = 30;//滑块值
         sif_power.ArrowSize = 0;//两端宽度为0（水平滑动条）
         
         HWND wnd;
         wnd = CreateWindow(SCROLLBAR, record_icon[15].icon_name, WS_OWNERDRAW,
                           record_icon[15].rc.x, record_icon[15].rc.y, record_icon[15].rc.w,
                           record_icon[15].rc.h, hwnd, record_icon[15].id, NULL, NULL);
         SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_power);
    
         BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
         xReturn = xTaskCreate((TaskFunction_t )(void(*)(void*))App_PlayRecord,  /* 任务入口函数 */
                            (const char*    )"App_PlayMusic",          /* 任务名字 */
                            (uint16_t       )5*1024/4,                   /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )hwnd,                     /* 任务入口函数参数 */
                            (UBaseType_t    )5,                        /* 任务的优先级 */
                            (TaskHandle_t  )&h_play_record);           /* 任务控制块指针 */

				if(xReturn != pdPASS )
				{
					GUI_ERROR("GUI rec play Thread Create failed");
				} 
                            
        xReturn = xTaskCreate((TaskFunction_t )(void(*)(void*))App_Record,  /* 任务入口函数 */
                            (const char*    )"Record Task",       /* 任务名字 */
                            (uint16_t       )4*1024/4,              /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )NULL,                /* 任务入口函数参数 */
                            (UBaseType_t    )5,                   /* 任务的优先级 */
                            (TaskHandle_t  )&h_record);           /* 任务控制块指针 */

				if(xReturn != pdPASS )
				{
					GUI_ERROR("GUI rec Thread Create failed");
				} 

         /* 创建录音文件列表 */
         CreateWindow(LISTBOX, record_icon[13].icon_name, WS_OWNERDRAW |  WS_VISIBLE | LBS_NOTIFY,
                           record_icon[13].rc.x, record_icon[13].rc.y, record_icon[13].rc.w,
                           record_icon[13].rc.h, hwnd, record_icon[13].id, NULL, NULL);
         
         PostAsyncMessage(hwnd, eMSG_SCAN_FILE, 0, 0);     // 执行一次文件扫描

         break;
      }

      case WM_TIMER:
      {
        int tmr_id;
				WCHAR wbuf[10];

				tmr_id = wParam;    // 定时器 ID

				if (tmr_id == ID_Record_Timer)    // 每秒录音计时定时器
				{
					Record_Timer++;
					x_wsprintf(wbuf, L"%02d:%02d", Record_Timer / 60, Record_Timer % 60);
          SetWindowText(GetDlgItem(hwnd, ID_RECORD_TIME), wbuf);
				}
      }  
			break;     
      
      case eMSG_SCAN_FILE:
      {
        HWND wnd = GetDlgItem(hwnd, ID_RECORD_LIST);
               
        SendMessage(wnd, LB_RESETCONTENT, 0, 0);
        scan_record_files(path);
        SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, 0, 0);    // 默认选中第一项
        InvalidateRect(wnd, NULL, TRUE);
      }
      break;
      
      case WM_NOTIFY:
      {
         u16 code,  id, ctr_id;;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID. 
         
         NMHDR *nr;        

         //发送单击
        if(code == BN_CLICKED)
        {
          switch (id)
          {
            /* 退出按钮按下 */
            case ID_RECORD_EXIT:
            {
              PostCloseMessage(hwnd);    // 发送关闭窗口的消息
            }
            break;
            
            /* 上一首 */
            case ID_RECORD_BACK:
            {
              int i, num;
              WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
              
              i = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);     // 获得当前选中项
              num = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCOUNT,0,0);    // 获得总项数
              SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, i == 0 ? num-1 : --i, 0);     // 获得设置当前选中项
            
              GetWindowText(wnd, wbuf, 3);
              if (wbuf[0] == L'T')
              {
                vTaskResume(h_play_record);            // 当前是挂起的，恢复任务
                SetWindowText(wnd, L"U");
                EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // 禁用开始录音按钮
              }
                
              I2S_Play_Start();
              play_index = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);    // 获得当前选中项
              mp3player.ucStatus = STA_SWITCH;
            }
            break;
            
            /* 下一首 */
            case ID_RECORD_NEXT:
            {
              int i, num;
              WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
              
              i = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);     // 获得当前选中项
              num = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCOUNT,0,0);    // 获得总项数
              SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_SETCURSEL, i==(num-1) ? 0 : ++i, 0);     // 获得设置当前选中项
              
              GetWindowText(wnd, wbuf, 3);
              if (wbuf[0] == L'T')
              {
                vTaskResume(h_play_record);            // 当前是挂起的，恢复任务
                SetWindowText(wnd, L"U");
                EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // 禁用开始录音按钮
              }
                
              I2S_Play_Start();
              play_index = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);    // 获得当前选中项
              mp3player.ucStatus = STA_SWITCH;
            }
            break;
            
            /* 播放录音 */
            case ID_RECORD_PLAY:
            {
							WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
              
							GetWindowText(wnd, wbuf, 3);
              
							if (wbuf[0] == L'U')
							{
                vTaskSuspend(h_play_record);
                I2S_Play_Stop(); 
								SetWindowText(wnd, L"T");
								EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), ENABLE);      // 使能开始录音按钮
							}
							else
							{
                vTaskResume(h_play_record);
                I2S_Play_Start();
								SetWindowText(wnd, L"U");
								EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // 禁用开始录音按钮
							}
            }
            break;
            
            /* 开始录音 */
            case ID_RECORD_START:
						{
              int i = 0;
              DIR dir;
              
              /*  如果路径不存在，创建文件夹  */
              result = f_opendir(&dir,RECORDERDIR);
              while(result != FR_OK)
              {
                i++;
                if (i > 5)
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
                  SelectDialogBox(hwnd, RC, L"没有检测到SD卡\n请确认SD已插入。", L"错误", &ops);    // 显示错误提示框
                  break;
                }
                f_mkdir(RECORDERDIR);
                result = f_opendir(&dir,RECORDERDIR);
              }
              
              if (i >= 5)break;
              
              /* 寻找合适文件名 */
              for(i=1;i<0xff;++i)
              {
                sprintf(recfilename,"0:/recorder/rec%03d.wav",i);
                result=f_open(&record_file,(const TCHAR *)recfilename,FA_READ);
                if(result==FR_NO_FILE)break;					
              }
              f_close(&record_file);
              
              if(i==0xff)
              {
                Recorder.ucStatus =STA_ERR;
                break;
              }
              /* 开始录音 */
              vTaskResume(h_record);            // 恢复录音任务
              StartRecord(recfilename);
							SetTimer(hwnd, ID_Record_Timer, 1000, TMR_START, NULL);                    // 开启录音计时定时器
							/* 按钮处理 */
              SetWindowText(GetDlgItem(hwnd, ID_RECORD_BUGLE), L"Q");
							SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"正在录音");             // 设置录音机状态
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_START), SW_HIDE);                    // 窗口隐藏开始录音的按钮
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_PADNC), SW_SHOW);                    // 窗口显示继续和暂停的按钮
							SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_LOCKCURSEL, TRUE, 0);     // 锁定列表不能重新修改选中的项
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_PLAY), DISABLE);                   // 禁用播放按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_BACK), DISABLE);                   // 禁用上一首按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_NEXT), DISABLE);                   // 禁用下一首按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_STOP), ENABLE);                    // 使能停止录音按钮
            }
            break;

            case ID_RECORD_STOP:
            {
              vTaskSuspend(h_play_record);            // 挂起录音任务
              KillTimer(hwnd, ID_Record_Timer);       // 删除录音计时定时器
              Record_Timer = 0;
              SetWindowText(GetDlgItem(hwnd, ID_RECORD_TIME), L"00:00");
              /* 对于录音，需要把WAV文件内容填充完整 */
              if(Recorder.ucStatus == STA_RECORDING)
              {
                I2Sxext_Recorde_Stop();
                I2S_Play_Stop();
                rec_wav.size_8=wavsize+36;
                rec_wav.datasize=wavsize;
                result=f_lseek(&record_file,0);
                result=f_write(&record_file,(const void *)&rec_wav,sizeof(rec_wav),&bw);
                result=f_close(&record_file);
                printf("录音结束\r\n");
              }
              //ucRefresh = 1;
              Recorder.ucStatus = STA_IDLE;		/* 待机状态 */
              I2S_Stop();		/* 停止I2S录音和放音 */
              wm8978_Reset();	/* 复位WM8978到复位状态 */
              
              /* 文件记录处理 */		
              char *cbuf = NULL;
              WCHAR wbuf[128];
              int i = 0;
              memcpy(music_playlist[music_file_num], recfilename, strlen(recfilename) + 1);
              printf("%s\r\n", music_playlist[music_file_num]);
              cbuf = strstr(recfilename, "/");    // 跳过第一个 / 
              cbuf = strstr(cbuf+1, "/");         // 跳过第二个 /
              if (cbuf != NULL)
              { 
              }
              x_mbstowcs_cp936(wbuf, cbuf + 1, sizeof(wbuf));
              //在Listbox中增加一个Item项
              i = SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_GETCOUNT, 0, 0);
              SendMessage(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), LB_ADDSTRING, i, (LPARAM)wbuf);
              music_file_num++;//记录文件个数
              InvalidateRect(GetDlgItem(Recorer_hwnd, ID_RECORD_LIST), NULL, TRUE);    // 无效化列表，使其重绘
              
              /* 停止录音 */
							SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"录音机");                // 设置录音机状态
							SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_LOCKCURSEL, FALSE, 0);     // 解锁列表可以重新修改选中的项
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_START), SW_SHOW);                     // 显示开始录音的按钮
							ShowWindow(GetDlgItem(hwnd, ID_RECORD_PADNC), SW_HIDE);                     // 隐藏继续和暂停的按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_BACK), ENABLE);                     // 禁用上一首按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_NEXT), ENABLE);                     // 禁用下一首按钮
              EnableWindow(GetDlgItem(hwnd, ID_RECORD_PLAY), ENABLE);                     // 使能播放按钮
							EnableWindow(GetDlgItem(hwnd, ID_RECORD_STOP), DISABLE);                    // 禁用停止录音按钮
            }
            break;

						/* 暂停继续录音按钮 */
            case ID_RECORD_PADNC:
            {
							WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PADNC);
              
							GetWindowText(wnd, wbuf, 3);
							if (wbuf[0] == L'U')
							{
								KillTimer(hwnd, ID_Record_Timer);       // 删除录音计时定时器
                vTaskSuspend(h_record);    // 挂起录音任务
                I2Sxext_Recorde_Stop();    // 停止录音
								SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"已暂停");          // 设置录音机状态
								SetWindowText(wnd, L"T");
							}
							else
							{
								SetTimer(hwnd, ID_Record_Timer, 1000, TMR_START, NULL);               // 继续录音计时定时器
                vTaskResume(h_record);      // 恢复录音任务
                I2Sxext_Recorde_Start();    // 继续录音
								SetWindowText(GetDlgItem(hwnd, ID_RECORD_STATE), L"正在录音");         // 设置录音机状态
								SetWindowText(wnd, L"U");
							}
            }
            break;
            
            /* 音频输出选择按钮 */
            case ID_RECORD_BUGLE:
            {
							WCHAR wbuf[3];
              HWND  wnd = GetDlgItem(hwnd, ID_RECORD_BUGLE);
              
							GetWindowText(wnd, wbuf, 3);
							if (wbuf[0] == L'P')
							{
								SetWindowText(wnd, L"Q");
                wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);    // 配置为耳机输出
							}
							else
							{
								SetWindowText(wnd, L"P");
                wm8978_CfgAudioPath(DAC_ON, SPK_ON);                        // 配置为扬声器输出
							}
            }
            break;
            
            /* 音量调节按钮被按下 */
            case ID_RECORD_bPOWER:
            {
							if (BUGLE_STATE == 1)
							{
                BUGLE_STATE = 0;
								ShowWindow(GetDlgItem(hwnd, ID_RECORD_sPOWER), SW_HIDE); //窗口隐藏
							}
							else
							{
                BUGLE_STATE = 1;
								ShowWindow(GetDlgItem(hwnd, ID_RECORD_sPOWER), SW_SHOW); //窗口显示
							}
            }
            break;
            
          }
        }
        else if (LBN_SELCHANGE == code)   // 列表选中项改变
        {
          if (id == ID_RECORD_LIST)
          {
            
            WCHAR wbuf[3];
            HWND  wnd = GetDlgItem(hwnd, ID_RECORD_PLAY);
            
            GetWindowText(wnd, wbuf, 3);
            if (wbuf[0] == L'T')
            {
              vTaskResume(h_play_record);            // 当前是挂起的，恢复任务
              SetWindowText(wnd, L"U");
						  EnableWindow(GetDlgItem(hwnd, ID_RECORD_START), DISABLE);     // 禁用开始录音按钮
            }
              
            I2S_Play_Start();
            play_index = SendMessage(GetDlgItem(hwnd, ID_RECORD_LIST), LB_GETCURSEL,0,0);    // 获得当前选中项
            mp3player.ucStatus = STA_SWITCH;
          }
        }
         
         /* 进度条处理 */
          nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
          //音量条处理case
          if (ctr_id == ID_RECORD_sPOWER)
          {
             NM_SCROLLBAR *sb_nr;
             sb_nr = (NM_SCROLLBAR*)nr; //Scrollbar的通知消息实际为 NM_SCROLLBAR扩展结构,里面附带了更多的信息.
             static int ttt = 0;
             switch (nr->code)
             {
                case SBN_THUMBTRACK: //R滑块移动
                {
                   uint16_t power;
                   power= sb_nr->nTrackValue; //得到当前的音量值
                   if(power == 0) 
                   {
                      wm8978_OutMute(1);//静音
                      SetWindowText(GetDlgItem(hwnd, ID_RECORD_bPOWER), L"J");
                      ttt = 1;
                   }
                   else
                   {
                      if(ttt == 1)
                      {
                         ttt = 0;
                         SetWindowText(GetDlgItem(hwnd, ID_RECORD_bPOWER), L"A");
                      }
                       wm8978_OutMute(0);
                       wm8978_SetOUT1Volume(power);//设置WM8978的音量值
                   } 
                   SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, power); //发送SBM_SETVALUE，设置音量值
                }
                break;
             }
          }

          //进度条处理case
          if (ctr_id == ID_PLAY_PROGRESS)
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
                   chgsch = 1;
                }
                break;
             }
          }         
      
      break;
    }

      //重绘制函数消息
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_RECORD_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_STOP)
         {
            stop_owner_draw(ds);    // 重绘停止录音按钮
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_START)
         {
            start_owner_draw(ds);    // 重绘开始录音按钮
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_sPOWER || ds->ID == ID_PLAY_PROGRESS)   
         {
            scrollbar_owner_draw(ds);    // 进度条重绘
            return TRUE;
         }
         else if (ds->ID >= ID_RECORD_bPOWER && ds->ID<= ID_RECORD_PADNC)
         {
            button_owner_draw(ds);    // 按钮重绘
            return TRUE;
         }
         else if (ds->ID >= ID_RECORD_STATE && ds->ID<= ID_PLAY_TIME)
         {
            Brigh_Textbox_OwnerDraw(ds);    // 重绘透明文本
            return TRUE;
         }
         else if (ds->ID == ID_RECORD_LIST)
         {
            listbox_owner_draw(ds);    // 重绘列表
            return TRUE;
         }
         return FALSE;
      }     
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
        HDC hdc;//屏幕hdc
        RECT rc = {471, 13, 115, 34};

        //开始绘制
        hdc = BeginPaint(hwnd, &ps); 

        DrawText(hdc, L"录音文件", -1, &rc, NULL);//绘制文字(居中对齐方式)

        EndPaint(hwnd, &ps);
        break;
      }
      
      case WM_ERASEBKGND:
      {
         HDC hdc =(HDC)wParam;
         RECT rc =*(RECT*)lParam;
         //GetClientRect(hwnd, &rc_cli);//获取客户区位置信息
         SetBrushColor(hdc, MapRGB(hdc, 250,0,0));
         FillRect(hdc, &rc); 
         
         BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc.x, rc.y, SRCCOPY);         
 
         return TRUE;
      }

      //关闭窗口消息处理case
      case WM_CLOSE:
      {        
        Record_Timer = 0;     // 复位录音计时
        mp3player.ucStatus = STA_IDLE;
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {        
        BUGLE_STATE = 0;
        DeleteDC(hdc_bk);
        vTaskDelete(h_record);
        vTaskDelete(h_play_record);
        music_file_num = 0;   // 复位文件记录
        thread = 0;
        I2S_Stop();		        /* 停止I2S录音和放音 */
        wm8978_Reset();	      /* 复位WM8978到复位状态 */ 

        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_RECORDER_DIALOG(void)
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
	Recorer_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                                    &wcex,
                                    L"GUI RECORDER DIALOG",
                                    WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(Recorer_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, Recorer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


