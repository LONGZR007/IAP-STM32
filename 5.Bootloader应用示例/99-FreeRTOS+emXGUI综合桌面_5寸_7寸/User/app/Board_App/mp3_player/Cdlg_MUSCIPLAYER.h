#ifndef _CDLG_MUSICPLAYER_H_
#define _CDLG_MUSICPLAYER_H_

#include <emXGUI.h>

#define GUI_RGB_BACKGROUNG_PIC      "avi_desktop.jpg"
#define MUSIC_MAX_NUM           20
#define MUSICFILE_NAME_LEN           100	
#define _DF1S                   0x81
typedef struct 
{
  HWND mMusicPlayer_hwnd;//²¥·ÅÆ÷¾ä±ú
  HWND mMusicList_hwnd;
  HWND mMusicLrc_hwnd;
  HWND mMusic_wnd_time;//½ø¶ÈÌõ¾ä±ú
  HDC  mhdc_bk;        //²¥·ÅÆ÷±³¾°²ã
  
  int power;
  uint8_t  music_file_num;//¸èÇúÊýÄ¿
  
  BOOL mInit_State;
  BOOL mLRC_State;
  BOOL LoadPic_State;
}Cdlg_Master_Struct;

typedef enum
{
  eID_BUTTON_Power = 0x1000,
  eID_BUTTON_List,
  eID_BUTTON_LRC,
  eID_BUTTON_BACK,
  eID_BUTTON_NEXT,
  eID_BUTTON_START,
  eID_SCROLLBAR_TIMER,
  eID_SCROLLBAR_POWER,
  eID_MUSIC_ITEM,
  eID_CUR_TIME,
  eID_ALL_TIME,
  eID_MUSIC_EXIT,
  eID_MUSICLIST,
  eMUSIC_VIEWER_ID_PREV,
  eMUSIC_VIEWER_ID_NEXT,
  eID_MUSIC_RETURN,
  eID_TEXTBOX_LRC1,
  eID_TEXTBOX_LRC2,
  eID_TEXTBOX_LRC3,
  eID_TEXTBOX_LRC4,
  eID_TEXTBOX_LRC5,
}Cdlg_Master_ID;


#endif /* _CDLG_MUSICPLAYER_H_ */



