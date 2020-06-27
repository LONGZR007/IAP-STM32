#include "emXGUI.h"
#include "GUI_MUSICPLAYER_DIALOG.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "./mp3_player/Backend_mp3Player.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
/******************按钮控件ID值***********************/
#define ID_BUTTON_Power      0x1000   //音量 
#define ID_BUTTON_List       0x1001   //音乐List
#define ID_BUTTON_Equa       0x1002   //均衡器
#define ID_BUTTON_Folder     0x1003   //文件夹
#define ID_BUTTON_BACK       0x1004   //上一首
#define ID_BUTTON_START      0x1005   //暂停键
#define ID_BUTTON_NEXT       0x1006   //下一首
#define ID_BUTTON_MINISTOP   0x1007   //迷你版暂停键

/*****************滑动条控件ID值*********************/
#define ID_SCROLLBAR_HORN    0x1103   //音量条
#define ID_SCROLLBAR_POWER   0x1104   //音量条
#define ID_SCROLLBAR_TIMER   0x1105   //进度条
/*****************文本框控件ID值*********************/
//本例程显示五行歌词
#define ID_TEXTBOX_LRC1      0x1201   //歌词第一行
#define ID_TEXTBOX_LRC2      0x1202   //歌词第二行
#define ID_TEXTBOX_LRC3      0x1203   //歌词第三行（当前行）
#define ID_TEXTBOX_LRC4      0x1204   //歌词第四行
#define ID_TEXTBOX_LRC5      0x1205   //歌词第五行

#define ID_EXIT        0x3000

/* 外部资源名 */
#define ROTATE_DISK_NAME "rotate_disk_ARGB8888.bmp"

#define Music_Player_48 "Music_Player_48_48.xft"
#define Music_Player_64 "Music_Player_64_64.xft"
#define Music_Player_72 "Music_Player_72_72.xft"

//图标管理数组
icon_S music_icon[] = {
   {"yinliang",         {683,412,48,48},       FALSE},//音量
   {"yinyueliebiao",    {742,415,48,48},      FALSE},//音乐列表
   {"geci",             {728,404,72,72},      FALSE},//歌词栏
   {"NULL",             {0,0,0,0},            FALSE},//无
   {"NULL",             {0,0,0,0},            FALSE},//无
   {"shangyishou",      {9, 410, 60, 64},   FALSE},//上一首
   {"zanting/bofang",   {68, 406, 72, 72},   FALSE},//播放
   {"xiayishou",        {140, 410, 64, 64},   FALSE},//下一首
   {"Q",               {620, 415, 48, 48},   FALSE},     // 8. 喇叭按钮
  
};
extern HWND music_list_hwnd;
static char path[100] = "0:";   // 文件根目录
static int  power = 20;                  // 耳机音量值
static int  power_horn = 40;             // 喇叭音量值
s32 old_scrollbar_value;                 // 上一个音量值
TaskHandle_t h_music;                    // 音乐播放进程
int enter_flag = 0;                      // 切换标志位
int IsCreateList = 0;
int time2exit = 0;
static COLORREF color_bg;//透明控件的背景颜色
uint8_t chgsch=0; //调整进度条标志位
char music_name[FILE_NAME_LEN] __EXRAM;//歌曲名数组
//文件系统相关变量
FRESULT f_result; 
FIL     f_file __EXRAM;
UINT    f_num;
//歌词数组--存放歌词数据
uint8_t ReadBuffer1[1024*5] __EXRAM;
//MINI播放键、上一首、下一首控件句柄句柄
static HWND mini_start;
//歌词显示标志位
static int show_lrc = 0;
//歌词结构体
LYRIC lrc;
static HDC hdc_bk;
static HWND wnd;//音量滑动条窗口句柄 
static HWND wnd_horn;//音量滑动条窗口句柄 
static HWND wnd_power;//音量icon句柄
extern const unsigned char gImage_0[]; 
GUI_SEM *exit_sem = NULL;
/*============================================================================*/
static BITMAP bm_0;
static HDC rotate_disk_hdc;

static SURFACE *pSurf;
static HDC hdc_mem11=NULL;
SCROLLINFO sif_power_horn;
SCROLLINFO sif_power;
//HFONT Music_Player_hFont48=NULL;
//HFONT Music_Player_hFont64  =NULL;
//HFONT Music_Player_hFont72  =NULL;

/***********************外部声明*************************/
extern void	GUI_MusicList_DIALOG(HWND hwnd);

/******************读取歌词文件*************************/

static uint16_t getonelinelrc(uint8_t *buff,uint8_t *str,int16_t len)
{
	uint16_t i;
	for(i=0;i<len;i++)
	{
		*(str+i)=*(buff+i);
		if((*(buff+i)==0x0A)||(*(buff+i)==0x00))
		{
			*(buff+i)='\0';
			*(str+i)='\0';
			break;
		}
	}
	return (i+1);
}
/**
  * @brief  插入字符串
  * @param  name：  数据数组
  * @param  sfx：   带插入的数据字符串
  * @retval 无
  * @notes  本程序调用该函数为歌词文件插入.lrc后缀
  */
static void lrc_chg_suffix(uint8_t *name,const char *sfx)
{		    	     
	while(*name!='\0')name++;
	while(*name!='.')name--;
	*(++name)=sfx[0];
	*(++name)=sfx[1];
	*(++name)=sfx[2];
	*(++name)='\0';
}
/**
  * @brief  歌词文件排序
  * @param  lyric：  歌词结构体
  * @retval 无
  * @notes  无
  */
static void lrc_sequence(LYRIC	*lyric)
{
	uint16_t i=0,j=0;
	uint16_t temp=0;
	if (lyric->indexsize == 0)return;
	
	for(i = 0; i < lyric->indexsize - 1; i++)
	{
		for(j = i+1; j < lyric->indexsize; j++)
		{
			if(lyric->time_tbl[i] > lyric->time_tbl[j])
			{
				temp = lyric->time_tbl[i];
				lyric->time_tbl[i] = lyric->time_tbl[j];
				lyric->time_tbl[j] = temp;

				temp = lyric->addr_tbl[i];
				lyric->addr_tbl[i] = lyric->addr_tbl[j];
				lyric->addr_tbl[j] = temp;
			}
		}
	}	
}
/**
  * @brief  歌词文件解析
  * @param  lyric：  歌词结构体
  * @param  strbuf： 存放歌词的数组
  * @retval 无
  * @notes  
  */
static void lyric_analyze(LYRIC	*lyric,uint8_t *strbuf)
{
	uint8_t strtemp[MAX_LINE_LEN]={0};
	uint8_t *pos=NULL;
	uint8_t sta=0,strtemplen=0;
	uint16_t lrcoffset=0;
	uint16_t str_len=0,i=0;
	
	pos=strbuf;
	str_len=strlen((const char *)strbuf);
	if(str_len==0)return;
	i=str_len;
   //此处的while循环用于判断歌词文件的标准
	while(--i)
	{
		if(*pos=='[')
			sta=1;
		else if((*pos==']')&&(sta==1))
			sta=2;
	  else if((sta==2)&&(*pos!=' '))
		{
			sta=3;
			break;
		}
		pos++; 
	}
	if(sta!=3)return;	
	lrcoffset=0;
	lyric->indexsize=0;
	while(lrcoffset<=str_len)
	{
		i=getonelinelrc(strbuf+lrcoffset,strtemp,MAX_LINE_LEN);
		lrcoffset+=i;
//		printf("lrcoffset:%d,i:%d\n",lrcoffset,i);
		strtemplen=strlen((const char *)strtemp);
		pos=strtemp;
		while(*pos!='[')
			pos++;
		pos++;
      
		if((*pos<='9')&&(*pos>='0'))
		{
         //记录时间标签
			lyric->time_tbl[lyric->indexsize]=(((*pos-'0')*10+(*(pos + 1)-'0'))*60+((*(pos+3)-'0')*10+(*(pos+4)-'0')))*100+((*(pos+6)-'0')*10+(*(pos+7)-'0'));
			//记录歌词内容
         lyric->addr_tbl[lyric->indexsize]=(uint16_t)(lrcoffset-strtemplen+10); 
         //记录歌词长度
			lyric->length_tbl[lyric->indexsize]=strtemplen-10;
			lyric->indexsize++;
		}		
//		else
//				continue;		
	}
}
static void Music_Button_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
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
	SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
   
   FillRect(hdc, &rc);  
   BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));


	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字

	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)

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



/**
  * @brief  创建音乐列表进程
  * @param  无
  * @retval 无
  * @notes  
  */
//static TaskHandle_t h1;

//static void App_MusicList()
//{
//	static int thread=0;
//	static int app=0;
//   
////	if(thread==0)
////	{  
////      h1=rt_thread_create("App_MusicList",(void(*)(void*))App_MusicList,NULL,4*1024,5,1);
////     xTaskCreate((TaskFunction_t )(void(*)(void*))App_MusicList,  /* 任务入口函数 */
////                            (const char*    )"App_MusicList",/* 任务名字 */
////                            (uint16_t       )4*1024/4,  /* 任务栈大小FreeRTOS的任务栈以字为单位 */
////                            (void*          )NULL,/* 任务入口函数参数 */
////                            (UBaseType_t    )5, /* 任务的优先级 */
////                            (TaskHandle_t  )&h1);/* 任务控制块指针 */
////      rt_thread_startup(h1);				
//      thread =1;
////      return;
////	}
//	while(thread) //线程已创建了
//	{
//    if(thread == 1)
//    {
//      if(app==0)
//      {
//        app=1;
//        GUI_MusicList_DIALOG();
//        app=0;
//        thread=0;
//      }
//    }
//    GUI_msleep(10);
//	}
//  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
//}
/**
  * @brief  播放音乐列表进程
  * @param  hwnd：屏幕窗口的句柄
  * @retval 无
  * @notes  
  */

int stop_flag = 0;
static int thread=0;
static void App_PlayMusic(HWND hwnd)
{
	int app=0;
   HDC hdc;
//	if(thread==0)
//	{  
//   xTaskCreate((TaskFunction_t )(void(*)(void*))App_PlayMusic,  /* 任务入口函数 */
//                            (const char*    )"App_PlayMusic",/* 任务名字 */
//                            (uint16_t       )5*1024,  /* 任务栈大小FreeRTOS的任务栈以字为单位 */
//                            (void*          )NULL,/* 任务入口函数参数 */
//                            (UBaseType_t    )5, /* 任务的优先级 */
//                            (TaskHandle_t  )&h_music);/* 任务控制块指针 */  
                            
      thread =1;
//      return;
//	}
	while(thread) //线程已创建了
	{     
		if(app==0)
		{
			app=1;
         //hdc = GetDC(hwnd);   
         int i = 0;      
         //读取歌词文件
         while(music_playlist[play_index][i]!='\0')
         {
           music_name[i]=music_playlist[play_index][i];
           i++;
         }			         
         music_name[i]='\0';
         //为歌词文件添加.lrc后缀
         lrc_chg_suffix((uint8_t *)music_name,"lrc");
         i=0;
         //初始化数组内容
         while(i<LYRIC_MAX_SIZE)
         {
           lrc.addr_tbl[i]=0;
           lrc.length_tbl[i]=0;
           lrc.time_tbl[i]=0;
           i++;
         }
         lrc.indexsize=0;
         lrc.oldtime=0;
         lrc.curtime=0;
         //打开歌词文件
         f_result=f_open(&f_file, music_name,FA_OPEN_EXISTING | FA_READ);
         //打开成功，读取歌词文件，分析歌词文件，同时将flag置1，表示文件读取成功
         if((f_result==FR_OK)&&(f_file.fsize<COMDATA_SIZE))
         {					
           f_result=f_read(&f_file,ReadBuffer1, sizeof(ReadBuffer1),&f_num);		
           if(f_result==FR_OK) 
           {  
              lyric_analyze(&lrc,ReadBuffer1);
              lrc_sequence(&lrc);
              lrc.flag = 1;      
           }
         }
         //打开失败（未找到该歌词文件），则将flag清零，表示没有读取到该歌词文件
         else
         {
            lrc.flag = 0;
            printf("读取歌词失败\n");
         }
         //关闭文件
			f_close(&f_file);	 
         
         i = 0;
         //得到播放曲目的文件名
         while(music_playlist[play_index][i]!='\0')
			{
				music_name[i]=music_playlist[play_index][i];
				i++;
			}
			music_name[i]='\0';
         
         //power = SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_POWER), SBM_GETVALUE, NULL, NULL);
         //SendMessage(GetDlgItem(hwnd, ID_SCROLLBAR_POWER),SBM_GETSCROLLINFO,0,(LPARAM)&sif);
         //power = sif.nValue;
         if(strstr(music_name,".wav")||strstr(music_name,".WAV"))
         {
            printf("wav\r");
           wavplayer(music_name, power, hdc, hwnd);
         }
         else
         {
           mp3PlayerDemo(hwnd, music_name, power, power_horn, hdc);  
         }
			 
         printf("播放结束\n");
         
			app=0;
         //使用 GETDC之后需要释放掉HDC
         //ReleaseDC(hwnd, hdc);
         //进行任务调度
         GUI_msleep(20);
		}
	   
   }
  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
   
}
/**
  * @brief  scan_files 递归扫描sd卡内的歌曲文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[FILE_NAME_LEN];	
	
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
      if(strstr(path,"recorder")!=NULL)continue;       //逃过录音文件
      if (*fn == '.') continue; 											//点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
        sprintf(&path[i], "/%s", fn); 							//合成完整目录名
        res = scan_files(path);											//递归遍历 
        if (res != FR_OK) 
					break; 																		//打开失败，跳出循环
        path[i] = 0; 
      } 
      else 
		{ 
				//printf("%s/%s\r\n", path, fn);								//输出文件名
				if(strstr(fn,".wav")||strstr(fn,".WAV")||strstr(fn,".mp3")||strstr(fn,".MP3"))//判断是否mp3或wav文件
				{
					if ((strlen(path)+strlen(fn)+2<FILE_NAME_LEN)&&(music_file_num<MUSIC_MAX_NUM))
					{
						sprintf(file_name, "%s/%s", path, fn);						
						strcpy(music_playlist[music_file_num],file_name);
                  //printf("%s\r\n", music_playlist[music_file_num]);
						strcpy(music_lcdlist[music_file_num],fn);						
						music_file_num++;//记录文件个数
					}
				}//if mp3||wav
      }//else
     } //for
  } 
  return res; 
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

   //获取控件的位置大小信息
   GetClientRect(hwnd, &rc_cli);
   //创建缓冲层，格式为SURF_ARGB4444
   hdc_mem = CreateMemoryDC(SURF_SCREEN, rc_cli.w, rc_cli.h);
   
	GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  


   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc_cli);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc_mem, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

   //播放键使用100*100的字体
   if(ds->ID == ID_BUTTON_START)
      SetFont(hdc_mem, controlFont_72);
   else if(ds->ID == ID_BUTTON_NEXT || ds->ID == ID_BUTTON_BACK)
      SetFont(hdc_mem, controlFont_64);
   else
      SetFont(hdc_mem, controlFont_48);
   //设置按键的颜色
   SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,250,250,250));
   //NEXT键、BACK键和LIST键按下时，改变颜色
	if((ds->State & BST_PUSHED) )
	{ //按钮是按下状态
		SetTextColor(hdc_mem, MapARGB(hdc_mem, 250,105,105,105));      //设置文字色     
	}
 
   DrawText(hdc_mem, wbuf,-1,&rc_cli,DT_VCENTER);//绘制文字(居中对齐方式)
   
   BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc_cli.h, hdc_mem, 0, 0, SRCCOPY);
   
   DeleteDC(hdc_mem);  
}

//透明文本
static void _music_textbox_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
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

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));


  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
  if(ds->ID == ID_TEXTBOX_LRC3)
    SetTextColor(hdc, MapRGB(hdc, 255, 0, 0));
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
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
	draw_scrollbar(hwnd, hdc_mem, color_bg, RGB888(	50, 205, 50), RGB888(50, 205, 50));
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
 * @brief  音量绘制滚动条
 * @param  hwnd:   滚动条的句柄值
 * @param  hdc:    绘图上下文
 * @param  back_c：背景颜色
 * @param  Page_c: 滚动条Page处的颜色
 * @param  fore_c：滚动条滑块的颜色
 * @retval NONE
*/
static void power_draw_scrollbar(HWND hwnd, HDC hdc, COLOR_RGB32 back_c, COLOR_RGB32 Page_c, COLOR_RGB32 fore_c)
{
	RECT rc,rc_tmp;
   RECT rc_scrollbar;
	GetClientRect(hwnd, &rc);
	/* 背景 */
   GetClientRect(hwnd, &rc_tmp);//得到控件的位置
   GetClientRect(hwnd, &rc);//得到控件的位置
   WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换
   
   BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

   rc_scrollbar.x = rc.w/2-1;
   rc_scrollbar.y = rc.y;
   rc_scrollbar.w = 2;
   rc_scrollbar.h = rc.h;
   
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
 * @brief  自定义音量滑动条绘制函数
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void power_scrollbar_owner_draw(DRAWITEM_HDR *ds)
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
	power_draw_scrollbar(hwnd, hdc_mem1, color_bg, RGB888( 250, 250, 250), RGB888( 255, 255, 255));
	//绘制绿色类型的滚动条
	power_draw_scrollbar(hwnd, hdc_mem, color_bg, RGB888(	50, 205, 50), RGB888(50, 205, 50));
   SendMessage(hwnd, SBM_GETTRACKRECT, 0, (LPARAM)&rc);   

	//上
	BitBlt(hdc, rc_cli.x, rc_cli.y, rc_cli.w, rc.y, hdc_mem1, 0, 0, SRCCOPY);
	//下
	BitBlt(hdc, 0, rc.y + rc.h, rc_cli.w, rc_cli.h - (rc.y + rc.h), hdc_mem, 0, rc.y + rc.h, SRCCOPY);

	//绘制滑块
	if (ds->State & SST_THUMBTRACK)//按下
	{
      BitBlt(hdc, 0, rc.y, rc_cli.w, rc.h, hdc_mem1, 0, rc.y, SRCCOPY);
		
	}
	else//未选中
	{
		BitBlt(hdc, 0, rc.y, rc_cli.w, rc.h, hdc_mem, 0, rc.y, SRCCOPY);
	}
	//释放内存MemoryDC
	DeleteDC(hdc_mem1);
	DeleteDC(hdc_mem);
}


HWND music_wnd_time;//歌曲进度条窗口句柄
SCROLLINFO sif;/*设置滑动条的参数*/

HWND wnd_lrc1;//歌词窗口句柄
HWND wnd_lrc2;//歌词窗口句柄
HWND wnd_lrc3;//歌词窗口句柄
HWND wnd_lrc4;//歌词窗口句柄
HWND wnd_lrc5;//歌词窗口句柄
HWND sub11_wnd; //播放键句柄
static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){


   RECT rc;
   static int tt = 0;
   static int a=0;
   static BOOL res;
   switch(msg){
      case WM_CREATE:
      {
         u8 *jpeg_buf;
         u32 jpeg_size;
         JPG_DEC *dec;

         res = RES_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
         //res = FS_Load_Content(GUI_RGB_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
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
         exit_sem = GUI_SemCreate(0,1);//创建一个信号量        
         //music_icon[0].rc.y = 440-music_icon[0].rc.h/2;//居中
         //音量icon（切换静音模式），返回控件句柄值
         wnd_power = CreateWindow(BUTTON,L"A",WS_OWNERDRAW |WS_VISIBLE,//按钮控件，属性为自绘制和可视
                                  music_icon[0].rc.x,music_icon[0].rc.y,//位置坐标和控件大小
                                  music_icon[0].rc.w,music_icon[0].rc.h,//由music_icon[0]决定
                                  hwnd,ID_BUTTON_Power,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_Power，附加参数为： NULL
         //音乐列表icon
         CreateWindow(BUTTON,L"G",WS_OWNERDRAW |WS_VISIBLE, //按钮控件，属性为自绘制和可视
                      music_icon[1].rc.x,music_icon[1].rc.y,//位置坐标
                      music_icon[1].rc.w,music_icon[1].rc.h,//控件大小
                      hwnd,ID_BUTTON_List,NULL,NULL);//父窗口hwnd,ID为ID_BUTTON_List，附加参数为： NULL
         //歌词icon
         // CreateWindow(BUTTON,L"W",WS_OWNERDRAW |WS_VISIBLE,
         //              music_icon[2].rc.x,music_icon[2].rc.y,
         //              music_icon[2].rc.w,music_icon[2].rc.h,
         //              hwnd,ID_BUTTON_Equa,NULL,NULL);

         //上一首
         CreateWindow(BUTTON,L"S",WS_OWNERDRAW |WS_VISIBLE,
                      music_icon[5].rc.x,music_icon[5].rc.y,
                      music_icon[5].rc.w,music_icon[5].rc.h,
                      hwnd,ID_BUTTON_BACK,NULL,NULL);
         //下一首
         CreateWindow(BUTTON,L"V",WS_OWNERDRAW |WS_VISIBLE,
                      music_icon[7].rc.x,music_icon[7].rc.y,
                      music_icon[7].rc.w,music_icon[7].rc.h,
                      hwnd,ID_BUTTON_NEXT,NULL,NULL);
         //播放键
         sub11_wnd = CreateWindow(BUTTON,L"U",WS_OWNERDRAW |WS_VISIBLE,
                      music_icon[6].rc.x,music_icon[6].rc.y,
                      music_icon[6].rc.w,music_icon[6].rc.h,
                      hwnd,ID_BUTTON_START,NULL,NULL); 

        CreateWindow(BUTTON,L"Q",WS_OWNERDRAW |WS_VISIBLE,
                      music_icon[8].rc.x,music_icon[8].rc.y,
                      music_icon[8].rc.w,music_icon[8].rc.h,
                      hwnd,ID_BUTTON_BUGLE,NULL,NULL); 

                      

//         CreateWindow(BUTTON, L"N", BS_FLAT | BS_NOTIFY|WS_OWNERDRAW |WS_VISIBLE,
//                        0, 0, 80, 80, hwnd, ID_EXIT, NULL, NULL); 
         /*********************歌曲进度条******************/
         sif.cbSize = sizeof(sif);
         sif.fMask = SIF_ALL;
         sif.nMin = 0;
         sif.nMax = 255;
         sif.nValue = 0;//初始值
         sif.TrackSize = 30;//滑块值
         sif.ArrowSize = 0;//两端宽度为0        
         music_wnd_time = CreateWindow(SCROLLBAR, L"SCROLLBAR_Time",  WS_OWNERDRAW| WS_VISIBLE, 
                         270, 424, 277, 30, hwnd, ID_SCROLLBAR_TIMER, NULL, NULL);
         SendMessage(music_wnd_time, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif);         

         
         
         //以下控件为TEXTBOX的创建
         wnd_lrc1 = CreateWindow(TEXTBOX, L" ", WS_OWNERDRAW| WS_VISIBLE, 
                                417, 132, 373, 30, hwnd, ID_TEXTBOX_LRC1, NULL, NULL);  
         SendMessage(wnd_lrc1,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);                                
         wnd_lrc2 = CreateWindow(TEXTBOX, L" ", WS_OWNERDRAW| WS_VISIBLE, 
                                417, 180, 373, 30, hwnd, ID_TEXTBOX_LRC2, NULL, NULL); 
         SendMessage(wnd_lrc2,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);
         wnd_lrc3 = CreateWindow(TEXTBOX, L" ", WS_OWNERDRAW| WS_VISIBLE, 
                                417, 242, 373, 30, hwnd, ID_TEXTBOX_LRC3, NULL, NULL);  
         SendMessage(wnd_lrc3,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND);     
         wnd_lrc4 = CreateWindow(TEXTBOX, L" ", WS_OWNERDRAW| WS_VISIBLE, 
                                417, 304, 373, 30, hwnd, ID_TEXTBOX_LRC4, NULL, NULL);  
         SendMessage(wnd_lrc4,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND); 
         wnd_lrc5 = CreateWindow(TEXTBOX, L" ", WS_OWNERDRAW| WS_VISIBLE, 
                                417, 352, 373, 30, hwnd, ID_TEXTBOX_LRC5, NULL, NULL);  
         SendMessage(wnd_lrc5,TBM_SET_TEXTFLAG,0,DT_VCENTER|DT_CENTER|DT_BKGND); 

         show_lrc = 1;//歌词窗口已弹出 	

         CreateWindow(BUTTON,L"歌曲文件名",WS_OWNERDRAW|WS_VISIBLE,
                      417,67,373,25,hwnd,ID_TB5,NULL,NULL);

         CreateWindow(BUTTON,L"00:00",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                      554,424,65,30,hwnd,ID_TB1,NULL,NULL);
     

         CreateWindow(BUTTON,L"00:00",WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,
                       199,424,65,30,hwnd,ID_TB2,NULL,NULL);
    
         //获取音乐列表
         memset(music_playlist,0,sizeof(music_playlist));
				 memset(music_lcdlist,0,sizeof(music_lcdlist));	
         scan_files(path);
         //创建音乐播放线程
         //App_PlayMusic(hwnd);
        xTaskCreate((TaskFunction_t )(void(*)(void*))App_PlayMusic,  /* 任务入口函数 */
                            (const char*    )"App_PlayMusic",/* 任务名字 */
                            (uint16_t       )5*1024/4,  /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )hwnd,/* 任务入口函数参数 */
                            (UBaseType_t    )6, /* 任务的优先级 */
                            (TaskHandle_t  )&h_music);/* 任务控制块指针 */
         
        
         CreateWindow(BUTTON, L"O", BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT,
                        740, 12, 36, 36, hwnd, ID_EXIT, NULL, NULL); 


         GetClientRect(hwnd,&rc); //获得窗口的客户区矩形
                            
         /*********************音量值滑动条******************/
         sif_power.cbSize = sizeof(sif_power);
         sif_power.fMask = SIF_ALL;
         sif_power.nMin = 0;
         sif_power.nMax = 63;//音量最大值为63
         sif_power.nValue = 20;//初始音量值
         sif_power.TrackSize = 30;//滑块值
         sif_power.ArrowSize = 0;//上下端宽度为0
         
         /* 耳机音量调节 */
         wnd = CreateWindow(SCROLLBAR, L"SCROLLBAR_R", WS_TRANSPARENT|SBS_VERT|WS_OWNERDRAW|SBS_BOTTOM_ALIGN|SBS_NOARROWS,
                            688, 294, 31, 100, hwnd, ID_SCROLLBAR_POWER, NULL, NULL);
         SendMessage(wnd, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_power);
         
         /*********************音量值滑动条******************/
         sif_power_horn.cbSize = sizeof(sif_power_horn);
         sif_power_horn.fMask = SIF_ALL;
         sif_power_horn.nMin = 0;
         sif_power_horn.nMax = 63;//音量最大值为63
         sif_power_horn.nValue = 20;//初始音量值
         sif_power_horn.TrackSize = 30;//滑块值
         sif_power_horn.ArrowSize = 0;//上下端宽度为0
         /* 喇叭音量调节 */
         sif_power_horn.nValue = 40;//初始音量值
         wnd_horn = CreateWindow(SCROLLBAR, L"SCROLLBAR_H", WS_TRANSPARENT|SBS_VERT|WS_OWNERDRAW|SBS_BOTTOM_ALIGN|SBS_NOARROWS,
                            688, 294, 31, 100, hwnd, ID_SCROLLBAR_HORN, NULL, NULL);
         SendMessage(wnd_horn, SBM_SETSCROLLINFO, TRUE, (LPARAM)&sif_power_horn);

			//设置位图结构参数
//			bm_0.Format	= BM_ARGB8888;     //位图格式
//			bm_0.Width  = 240;              //宽度
//			bm_0.Height = 240;             //高度
//			bm_0.WidthBytes =bm_0.Width*4; //每行字节数
//			bm_0.LUT =NULL;                //查找表(RGB/ARGB格式不使用该参数)
//			bm_0.Bits =(void*)gImage_0;    //位图数据
      
        /* 创建蓝鱼的memdc */
         // rotate_disk_hdc = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888,240,240); 
         // /* 清空背景为透明 */
         // ClrDisplay(rotate_disk_hdc,NULL,0);
         // //BitBlt(rotate_disk_hdc, 0, 0, 240, 240, hdc_bk, 280, 120, SRCCOPY);
         // /* 绘制bmp到hdc */
         // RECT rc = {0,0,240,240};
         // SetTextColor(rotate_disk_hdc, MapARGB(rotate_disk_hdc, 255, 50, 205, 50));
         // SetFont(rotate_disk_hdc, iconFont_252);
         // DrawTextEx(rotate_disk_hdc,L"a",-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER,NULL);
         // /* 转换成bitmap */
         // DCtoBitmap(rotate_disk_hdc,&bm_0);

         //pSurf =CreateSurface(SURF_RGB565,240,240,-1,NULL);
         
         
         //SetTimer(hwnd, 1, 200, TMR_START,NULL);
          GUI_MusicList_DIALOG(hwnd);
          rc.x =0;
          rc.y =0;
          rc.w =240;
          rc.h =240;
          //hdc_mem11 =CreateDC(pSurf,&rc);

         break;
      }
		case WM_TIMER:
      {
         if(!show_lrc)
         {
            if(0)//旋转位图会很卡
            {
               
               a+=5;
               a%=360;
               //ClrDisplay(hdc_mem11,NULL,MapRGB(hdc_mem11,0,0,0));
               BitBlt(hdc_mem11, 0, 0, 240, 240, hdc_bk, 280, 120, SRCCOPY);
               RotateBitmap(hdc_mem11,120,120,&bm_0,a);
            }
            rc.x=280;
            rc.y=120;
            rc.w=240;
            rc.h=240;

            InvalidateRect(hwnd,&rc,FALSE);
         }
			break;
      }         
      
      
      case WM_NOTIFY:
      {
         u16 code,  id, ctr_id;;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型
         ctr_id = LOWORD(wParam); //wParam低16位是发送该消息的控件ID. 
         
         NMHDR *nr;        
         HDC hdc;
         //发送单击
         if(code == BN_CLICKED)
         { 
            switch(id)
            {
               //音乐ICON处理case
               case ID_EXIT:
               {
//                 vTaskDelete(h_music);
                  PostCloseMessage(hwnd);
                  break;
               }
               case ID_BUTTON_List:
               {
                  music_icon[1].state = ~music_icon[1].state;
                  if (music_icon[1].state == FALSE)
                  {
//                    PostCloseMessage(music_list_hwnd);
                    show_lrc = 1;//歌词窗口已弹出
                     ShowWindow(wnd_lrc1, SW_SHOW);  
                     ShowWindow(wnd_lrc2, SW_SHOW);
                     ShowWindow(wnd_lrc3, SW_SHOW);
                     ShowWindow(wnd_lrc4, SW_SHOW);
                     ShowWindow(wnd_lrc5, SW_SHOW);
                    ShowWindow(GetDlgItem(hwnd, ID_TB5), SW_SHOW);
                    ShowWindow(music_list_hwnd, SW_HIDE);
                  }
                  else
                  {
                    show_lrc = 0; //未弹出歌词窗口
                    if (music_icon[0].state != FALSE)
                    {
                      music_icon[0].state = ~music_icon[0].state;
                      ShowWindow(wnd, SW_HIDE);
                      ShowWindow(wnd_horn, SW_HIDE);
                    }
                    ShowWindow(GetDlgItem(hwnd, ID_TB5), SW_HIDE);
                    ShowWindow(wnd_lrc1, SW_HIDE);
                    ShowWindow(wnd_lrc2, SW_HIDE);
                    ShowWindow(wnd_lrc3, SW_HIDE);
                    ShowWindow(wnd_lrc4, SW_HIDE);
                    ShowWindow(wnd_lrc5, SW_HIDE);
                    ShowWindow(music_list_hwnd, SW_SHOW);
                     
                  }
                        
                  break;
               }
               //音量icon处理case
               case ID_BUTTON_Power:
               {
                  music_icon[0].state = ~music_icon[0].state;
                  //InvalidateRect(hwnd, &music_icon[0].rc, TRUE);
                  //当音量icon未被按下时
                  if(music_icon[0].state == FALSE)
                  {
                     RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
                     WCHAR wbuf[3];
                      HWND  wnd1 = GetDlgItem(hwnd, ID_BUTTON_BUGLE);

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
                     if (music_icon[1].state != FALSE)
                     {
                       music_icon[1].state = ~music_icon[1].state;
                       ShowWindow(music_list_hwnd, SW_HIDE);
                       show_lrc = 1;//歌词窗口已弹出
                       ShowWindow(wnd_lrc1, SW_SHOW);  
                       ShowWindow(wnd_lrc2, SW_SHOW);
                       ShowWindow(wnd_lrc3, SW_SHOW);
                       ShowWindow(wnd_lrc4, SW_SHOW);
                       ShowWindow(wnd_lrc5, SW_SHOW);
                       ShowWindow(GetDlgItem(hwnd, ID_TB5), SW_SHOW);
                     }

                     WCHAR wbuf[3];
                      HWND  wnd1 = GetDlgItem(hwnd, ID_BUTTON_BUGLE);
                   
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
               
               //歌词icon处理case
               case ID_BUTTON_Equa:
               {
                  music_icon[2].state = ~music_icon[2].state;
                  if(music_icon[2].state == FALSE)
                  {
                     show_lrc = 0; //未弹出歌词窗口
                     //隐藏歌词栏和所有MINI按键
//                     ShowWindow(mini_next, SW_HIDE);                     
//                     ShowWindow(mini_start, SW_HIDE); 
//                     ShowWindow(mini_back, SW_HIDE);
                     ShowWindow(wnd_lrc1, SW_HIDE);
                     ShowWindow(wnd_lrc2, SW_HIDE);
                     ShowWindow(wnd_lrc3, SW_HIDE);
                     ShowWindow(wnd_lrc4, SW_HIDE);
                     ShowWindow(wnd_lrc5, SW_HIDE);
                     RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
                     ResetTimer(hwnd, 1, 200, TMR_START,NULL);
                  }
                  else
                  {
                     show_lrc = 1;//歌词窗口已弹出
                     //显示歌词栏目以及所有的MINI按键
//                     ShowWindow(mini_next, SW_SHOW);                     
//                     ShowWindow(mini_start, SW_SHOW); 
//                     ShowWindow(mini_back, SW_SHOW);
                     ShowWindow(wnd_lrc1, SW_SHOW);  
                     ShowWindow(wnd_lrc2, SW_SHOW);
                     ShowWindow(wnd_lrc3, SW_SHOW);
                     ShowWindow(wnd_lrc4, SW_SHOW);
                     ShowWindow(wnd_lrc5, SW_SHOW);

                     ResetTimer(hwnd, 1, 200, NULL,NULL);
                  }                                              
                  break;
               }
               //播放icon处理case
               case ID_BUTTON_START:
               {
//                     WCHAR wbuf[128];
                     music_icon[6].state = ~music_icon[6].state;
                     //擦除icon的背景
                     //

                     if(music_icon[6].state == FALSE)
                     {

                        vTaskResume(h_music);
                        I2S_Play_Start();
                        SetWindowText(sub11_wnd, L"U");
                        ResetTimer(hwnd, 1, 200, TMR_START,NULL);
                        
                     }
                     else if(music_icon[6].state != FALSE)
                     {
                        vTaskSuspend(h_music);
                        I2S_Play_Stop();                    
                        SetWindowText(sub11_wnd, L"T");
                        ResetTimer(hwnd, 1, 200, NULL,NULL);                       

                        
                     }  
                     InvalidateRect(hwnd, &music_icon[6].rc, TRUE);                     
                  break;                  
               }
               //下一首icon处理case
               case ID_BUTTON_NEXT:
               {     
                  WCHAR wbuf[128];
                  play_index++;
                  if(play_index >= music_file_num) play_index = 0;
                  if(play_index < 0) play_index = music_file_num - 1;
                  mp3player.ucStatus = STA_SWITCH;
                  hdc = GetDC(hwnd);
                                
                  x_mbstowcs_cp936(wbuf, music_lcdlist[play_index], FILE_NAME_LEN);
                  SetWindowText(GetDlgItem(hwnd, ID_TB5), wbuf);
                                 
                  SendMessage(music_wnd_time, SBM_SETVALUE, TRUE, 0); //设置进度值
                  SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB1), L"00:00"); 
                  SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB2), L"00:00"); 
//                  DrawText(hdc, wbuf, -1, &rc_musicname, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
//                  ClrDisplay(hdc, &rc_MusicTimes, color);
//                  DrawText(hdc, L"00:00", -1, &rc_MusicTimes, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                  ReleaseDC(hwnd, hdc);
                  
                  break;
               }
               //上一首icon处理case
               case ID_BUTTON_BACK:
               {
                  play_index--;
                  if(play_index > music_file_num) play_index = 0;
                  if(play_index < 0) play_index = music_file_num - 1;
                  mp3player.ucStatus = STA_SWITCH;   
                  hdc = GetDC(hwnd);
                  ReleaseDC(hwnd, hdc);            
                  break;
               }            
            
               /* 音频输出选择按钮 */
               case ID_BUTTON_BUGLE:
               {
                  WCHAR wbuf[3];
                  HWND  wnd1 = GetDlgItem(hwnd, ID_BUTTON_BUGLE);
               
                  GetWindowText(wnd1, wbuf, 3);
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
                  
                  if(music_icon[0].state != FALSE)    // 音量调节滑动条已弹出，切换调节滑动调
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
                       
            
               //MINI播放/暂停处理case
               case ID_BUTTON_MINISTOP:
               {
                     music_icon[9].state = ~music_icon[9].state;
                     //InvalidateRect(hwnd, &music_icon[9].rc, TRUE);
                     //ShowWindow(sub11_wnd, SW_HIDE);
                     //RedrawWindow(hwnd, NULL, RDW_ALLCHILDREN|RDW_INVALIDATE);
                     if(music_icon[9].state == FALSE)
                     {
                        
                        vTaskSuspend(h_music);
                        I2S_Play_Stop();                    
                        SetWindowText(mini_start, L"I");
                        
                        SetWindowText(sub11_wnd, L"I");
                        
                     }
                     else if(music_icon[9].state != FALSE)
                     {
                        
                        vTaskResume(h_music);
                        I2S_Play_Start();
                        SetWindowText(mini_start, L"H");
                        SetWindowText(sub11_wnd, L"H");
                        
                     }

                     
                     //ShowWindow(sub11_wnd, SW_HIDE);                  
                  break;                  
               }
            }
         }//end of if(code == BN_CLICKED) 
         
      	nr = (NMHDR*)lParam; //lParam参数，是以NMHDR结构体开头.
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
                     SendMessage(wnd_horn, SBM_SETVALUE, TRUE, power_horn); // 发送SBM_SETVALUE，将喇叭音量也设置为
                     SetWindowText(wnd_power, L"J");
                     ttt = 1;
                  }
                  else
                  {
                     if(ttt == 1)
                     {
                        ttt = 0;
                        SetWindowText(wnd_power, L"A");
                     }
                     wm8978_OutMute(0);
                     wm8978_SetOUT1Volume(power);//设置WM8978的耳机音量值
                  } 
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, power); //发送SBM_SETVALUE，设置音量值
               }
               break;
            }
         }
         
         //音量条处理case
         if (ctr_id == ID_SCROLLBAR_HORN)
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
                     SendMessage(wnd, SBM_SETVALUE, TRUE, power_horn); // 发送SBM_SETVALUE，将耳机音量也设置为0
                     SetWindowText(wnd_power, L"J");
                     ttt = 1;
                  }
                  else
                  {
                     if(ttt == 1)
                     {
                        ttt = 0;
                        SetWindowText(wnd_power, L"A");
                     }
                     wm8978_OutMute(0);
                     wm8978_SetOUT2Volume(power_horn);//设置WM8978的喇叭音量值
                  } 
                  SendMessage(nr->hwndFrom, SBM_SETVALUE, TRUE, power_horn); //发送SBM_SETVALUE，设置音量值
               }
               break;
            }
         }
         
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
         if(ds->ID == ID_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         if (ds->ID == ID_SCROLLBAR_POWER || ds->ID == ID_SCROLLBAR_HORN)
         {
            power_scrollbar_owner_draw(ds);
            return TRUE;
         }
         if (ds->ID == ID_SCROLLBAR_TIMER)
         {
            scrollbar_owner_draw(ds);
            return TRUE;
         }
         if (ds->ID >= ID_BUTTON_Power && ds->ID<= ID_BUTTON_BUGLE)
         {
            button_owner_draw(ds);
            return TRUE;
         }
         if(ds->ID == ID_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         if(ds->ID == ID_TB1 || ds->ID == ID_TB2 || ds->ID == ID_TB5)
         {
            Music_Button_OwnerDraw(ds);
           return TRUE;
         }
         if(ds->ID >= ID_TEXTBOX_LRC1 && ds->ID <= ID_TEXTBOX_LRC5)
         {
            _music_textbox_OwnerDraw(ds);
            return TRUE;
         }


      }     
      //绘制窗口界面消息
      case WM_PAINT:
      {
         PAINTSTRUCT ps;
         HDC hdc;//屏幕hdc
//         HDC hdc_mem;//缓冲区
//         RECT rc_top = {0 ,0, 800, 80};//上边栏
//         RECT rc_bot = {0 ,400, 800, 80};//下边栏
         //RECT test={0,90,100,100};
         
         
         //开始绘制
         hdc = BeginPaint(hwnd, &ps); 
         // if(tt == 0)
         // {
         //    tt = 1;
         //    BitBlt(hdc_mem11, 0, 0, 240, 240, hdc_bk, 280, 120, SRCCOPY);
         //    RotateBitmap(hdc_mem11,120,120,&bm_0,0);
         // }            
         //显示歌曲时长
         //DrawText(hdc, L"00:00", -1, &rc_MusicTimes, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
         /*上边栏目*/
//         SetBrushColor(hdc_mem, MapARGB(hdc_mem, 50, 0, 0, 0));
//         FillRect(hdc_mem, &rc_top);
//         BitBlt(hdc, rc_top.x, rc_top.y, rc_top.w, rc_top.h, 
//                hdc_mem, rc_top.x, rc_top.y, SRCCOPY);         
//         /*下边栏目*/ 
//         SetBrushColor(hdc_mem, MapARGB(hdc_mem, 50, 0, 0, 0));
//         FillRect(hdc_mem, &rc_bot);                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
//         BitBlt(hdc, rc_bot.x, rc_bot.y, rc_bot.w, rc_bot.h, 
//                hdc_mem, rc_bot.x, rc_bot.y, SRCCOPY);
         
         // rc.x=280;
			// rc.y=120;
			// rc.w=240;
			// rc.h=240;
         
			// BitBlt(hdc,rc.x,rc.y,rc.w,rc.h,hdc_bk,0,0,SRCCOPY);
         
         
         
         //获取屏幕点（385，404）的颜色，作为透明控件的背景颜色
         color_bg = GetPixel(hdc, 385, 404);
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
         if(res!=FALSE)
            BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc.x, rc.y, SRCCOPY);         
 
         return TRUE;
      }
      //设置TEXTBOX的背景颜色以及文字颜色
		case	WM_CTLCOLOR:
		{
			/* 控件在绘制前，会发送 WM_CTLCOLOR到父窗口.
			 * wParam参数指明了发送该消息的控件ID;lParam参数指向一个CTLCOLOR的结构体指针.
			 * 用户可以通过这个结构体改变控件的颜色值.用户修改颜色参数后，需返回TRUE，否则，系统
			 * 将忽略本次操作，继续使用默认的颜色进行绘制.
			 *
			 */
			u16 id;
			id =LOWORD(wParam);
         //第三个TEXTBOX为当前的歌词行
			if(id== ID_TEXTBOX_LRC3)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				//cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;
			}
			else if(id == ID_TEXTBOX_LRC1||id == ID_TEXTBOX_LRC2||id == ID_TEXTBOX_LRC5||id == ID_TEXTBOX_LRC4)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(250,0,0);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				//cr->BorderColor =RGB888(255,10,10);//边框颜色（RGB888颜色格式)
				return TRUE;				
			}
         if(id== ID_TB1 || id== ID_TB2 || id== ID_TB5 )
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
				cr->TextColor =RGB888(255,255,255);//文字颜色（RGB888颜色格式)
				cr->BackColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BorderColor =RGB888(255,0,0);//边框颜色（RGB888颜色格式)
				return TRUE;
			}
         return FALSE;
		}     
    
    //关闭窗口消息处理case
      case WM_CLOSE:
      {        
        music_icon[0].state = FALSE;
        music_icon[1].state = FALSE;
         DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {        
        mp3player.ucStatus = STA_IDLE;		/* 待机状态 */
        time2exit = 1;
        //GUI_SemWait(exit_sem, 0xFFFFFFFF);
        vTaskDelete(h_music);//暂时挂起
//        if(IsCreateList == 1)
//        {
//          IsCreateList = 0;
//          vTaskDelete(h1);
//        }
        GUI_SemDelete(exit_sem);
        DeleteSurface(pSurf);
        //DeleteDC(hdc_mem11);
        DeleteDC(hdc_bk);
       // DeleteDC(rotate_disk_hdc);
        thread = 0;
//        DeleteFont(Music_Player_hFont48);
//        DeleteFont(Music_Player_hFont64);
//        DeleteFont(Music_Player_hFont72);
        play_index = 0;
        res = FALSE;
        tt = 0;
        music_file_num = 0;
        power = 20;
        a = 0;
        
        I2S_Stop();		/* 停止I2S录音和放音 */
        wm8978_Reset();	/* 复位WM8978到复位状态 */ 
        a = PostQuitMessage(hwnd);	        
        return TRUE;	
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


//音乐播放器句柄
HWND	MusicPlayer_hwnd;
void	GUI_MUSICPLAYER_DIALOG(void)
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
	MusicPlayer_hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
		&wcex,
		L"GUI_MUSICPLAYER_DIALOG",
		WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
		0, 0, GUI_XSIZE, GUI_YSIZE,
		NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(MusicPlayer_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, MusicPlayer_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


