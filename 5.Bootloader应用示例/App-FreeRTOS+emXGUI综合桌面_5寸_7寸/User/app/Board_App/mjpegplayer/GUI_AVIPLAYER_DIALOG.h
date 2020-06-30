#ifndef _GUI_MUSICPLAYER_DIALOG_H
#define _GUI_MUSICPLAYER_DIALOG_H
/*****************滑动条控件ID值*********************/
#define ID_SCROLLBAR_POWER   0x1104   //音量条
#define eID_SCROLLBAR_HORN   0x1103   // 音量条
#define ID_SCROLLBAR_TIMER   0x1105   //进度条

/******************按钮控件ID值***********************/
#define ID_BUTTON_Power      0x1000   //音量 
#define ID_BUTTON_List       0x1001   //音乐List
#define ID_BUTTON_Back       0x1002   //上一首
#define ID_BUTTON_Play       0x1003   //播放/暂停
#define ID_BUTTON_Next       0x1004   //播放/暂停
#define ID_BUTTON_Bugle      0x1005   //音乐List
/******************TextBox控件ID值*******************/
#define ID_TB1               0x1300   //文件名
#define ID_TB2               0x1301   //分辨率
#define ID_TB3               0x1302   //帧率
#define ID_TB4               0x1304   //时长
#define ID_TB5               0x1305   //当前时间

#define GUI_RGB_BACKGROUNG_PIC      "avi_desktop.jpg"

typedef struct{
   char *icon_name;//图标名
   RECT rc;        //位置信息
   BOOL state;     //状态  
}icon_S;

extern int avi_chl;
extern volatile int win_fps;
extern HWND	VideoPlayer_hwnd;

extern HFONT AVI_Player_hFont48;
extern HFONT AVI_Player_hFont64;
extern HFONT AVI_Player_hFont72;
#endif


