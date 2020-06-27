#ifndef _GUI_PICVIEW_H_
#define _GUI_PICVIEW_H_

#define Slide_Enable              (1)


#define PICFILE_NUM_MAX           20
#define PICFILE_NAME_MAXLEN       100	

#define UpdatePicInfo             WM_USER+1 
#define UpdateButtonState         WM_USER+2
#define CloseGif                  WM_USER+3

#define File_Path                 "0:/srcdata/"
//当前APP的背景颜色
#define  PICVIWER_BACK_GROUND      0,0,0

typedef enum
{
  Type_JPG = 0,
  Type_PNG,
  Type_GIF,
  Type_BMP,
  Type_None,
}PicTypeDef;

typedef enum 
{
  eID_Pic_Name = 0x1000,
  eID_Pic_EXIT,
  eID_Pic_PREV,
  eID_Pic_NEXT,
//  eID_Pic_MsgBOX,
  eID_Pic_Time,
  eID_Pic_Res,
  eID_Pic_Res_Value,
//  //eID_Pic_Time,
  eID_Pic_Time_Value,
//  eID_Pic_INTFLASH,
//  eID_Pic_EXTFLASH,
//  eID_Pic_SDCARD,
//  eID_Pic_Return,
//  eID_Pic_JPG,
//  eID_Pic_PNG,
//  eID_Pic_GIF,
//  eID_Pic_BMP,
//  eID_Pic_Title,
  // eID_Pic_Def,
//  eID_FILEPATH,
//  eID_ZOOMIN,
//  eID_ZOOMOUT,
  eID_Pic_PLAY,
  eID_Pic_FPS,
  eID_Pic_FPS_Value,
//  eID_LIST_1,
}GUI_PicViewer_ID;

typedef struct
{
  HANDLE m_hgif;
  
  
  uint32_t frame_num;
  uint8_t m_gif_state;
  uint8_t m_gif_curnums;
  IMAGE_INFO img_info;
  uint8_t *gif_buf;
  uint32_t gif_size;
  uint32_t m_delay;
}GIF_Typedef;

typedef struct
{
  uint16_t m_jpg_wid;
  uint16_t m_jpg_high;  
}JPG_Typedef;

typedef struct
{
  BITMAP png_bm;
}PNG_Typedef;

typedef struct
{
  BITMAPINFO bm_info;
}BMP_Typedef;

typedef struct 
{
  HWND PicView_Handle;

  
  
  uint8_t m_file_nums;
  uint8_t m_file_index;
  char** mp_file_list;
  
  
  GIF_Typedef ms_gif;
  JPG_Typedef ms_jpg;
  PNG_Typedef ms_png;
  BMP_Typedef ms_bmp;
#if Slide_Enable  
  int x_off;
  int m_slide_staus;
  int m_touch_times;
#endif
  
}PicViewer_Dialog_Typedef;
#endif /* _CDLG_PICVIEW_H_ */

