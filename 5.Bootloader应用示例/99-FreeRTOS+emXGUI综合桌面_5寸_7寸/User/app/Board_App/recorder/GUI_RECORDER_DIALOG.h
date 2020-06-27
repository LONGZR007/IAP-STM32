#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"

#ifndef _GUI_RECORDER_DIALOG_H
#define _GUI_RECORDER_DIALOG_H

#define MUSIC_MAX_NUM           20
#define FILE_NAME_LEN           100			
#define MUSIC_NAME_LEN          100		
#define LYRIC_MAX_SIZE          200				
#define MAX_LINE_LEN            200				
#define _DF1S                   0x81
#define BUFFER_SIZE             1024*8
#define COMDATA_SIZE				  1024*5
#define COMDATA_UTF8_SIZE		  1024*2
#define MP3BUFFER_SIZE          2304		
#define INPUTBUF_SIZE           3000	

#define LRC_GBK   1  //1表示LRC歌词为GBK编码，0表示UTF8编码
#define ID_TB1               0x1301
#define ID_TB2               0x1302
#define ID_TB5               0x1305

#define GUI_RECORDER_BACKGROUNG_PIC      "recorderdesktop.jpg"

typedef enum 
{ 
  /******************按钮控件ID值*******************/
  ID_RECORD_bPOWER = 0x1000,   // 音量 
  ID_RECORD_BACK,              // 上一首
  ID_RECORD_NEXT,              // 下一首
  ID_RECORD_PLAY,              // 播放暂停按钮
  ID_RECORD_BUGLE,             // 喇叭按钮
  ID_RECORD_PADNC,             // 录音继续暂停
  ID_RECORD_START,             // 录音开始
  ID_RECORD_STOP,              // 停止录音
  ID_RECORD_EXIT,              // 退出按钮（放最后：与其他按钮重绘函数不一样）
  /*****************滑动条控件ID值*******************/
  ID_RECORD_sPOWER,            // 音量条
  ID_PLAY_PROGRESS,            // 进度条
  /*****************文本控件ID值*********************/
  ID_RECORD_STATE = 0x1301 - 2,                // 录音状态
  ID_RECORD_TIME,                 // 录音时长
  ID_PLAY_TOTAL_TIME = 0x1301,    // 播放总时长（与音乐播放器的是同一个ID）
  ID_PLAY_TIME = 0x1302,          // 播放时长（与音乐播放器的是同一个ID）
  /*****************列表控件ID值*********************/
  ID_RECORD_LIST,              // 文件列表
  /*****************列表控件ID值*********************/
  ID_Record_Timer,
  /******************用户自定义消息******************/
  eMSG_SCAN_FILE = WM_USER + 1,

}record_id_t;

typedef struct{
   WCHAR *icon_name; // 图标名
   RECT rc;         // 位置信息
   record_id_t id;      // 按钮ID
}recorder_icon_t;

// typedef __packed struct 
// { 
// 	uint8_t 	indexsize; 	
//   uint8_t		length_tbl[LYRIC_MAX_SIZE]; 
//  	uint16_t	addr_tbl[LYRIC_MAX_SIZE];										
//  	int32_t		curtime;										
//   int32_t		oldtime;	
//   int32_t 	time_tbl[LYRIC_MAX_SIZE];	
//   uint8_t flag;//读取文件标志位
// }LYRIC;

// extern HWND MusicPlayer_hwnd;
// extern int play_index;
// extern char music_playlist[MUSIC_MAX_NUM][FILE_NAME_LEN];//播放List
// extern char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];//显示list
// extern uint8_t music_file_num;
// extern uint8_t chgsch; //进度条调整标志位
// extern HWND music_wnd_time;
// extern LYRIC lrc;
// void com_utf82gbk(const char *src, char *str);
// void com_gbk2utf8(const char *src, char *str);
// extern uint8_t *comdata;
// extern signed long hCOMUTF8Memory;
// extern uint8_t *comdataUTF8;
// extern icon_S music_icon[12];
// extern HWND wnd_lrc1;//歌词窗口句柄
// extern HWND wnd_lrc2;//歌词窗口句柄
// extern HWND wnd_lrc3;//歌词窗口句柄
// extern HWND wnd_lrc4;//歌词窗口句柄
// extern HWND wnd_lrc5;//歌词窗口句柄
// extern uint8_t ReadBuffer1[1024*5];

// extern HFONT Music_Player_hFont48;
// extern HFONT Music_Player_hFont64;
// extern HFONT Music_Player_hFont72;

#endif

