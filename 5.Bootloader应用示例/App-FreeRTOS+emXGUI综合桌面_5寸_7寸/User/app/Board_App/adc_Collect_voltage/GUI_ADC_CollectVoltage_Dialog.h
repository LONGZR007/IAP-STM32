#ifndef _GUI_ADCVIEW_H_
#define _GUI_ADCVIEW_H_

#define Slide_Enable              (1)


#define ADCFILE_NUM_MAX           20
#define ADCFILE_NAME_MAXLEN       100	

#define UpdateADCInfo             WM_USER+1 
#define UpdateButtonState         WM_USER+2
#define CloseGif                  WM_USER+3

#define File_Path                 "0:/srcdata/"
//当前APP的背景颜色
#define  ADCVIWER_BACK_GROUND      0,0,0

typedef enum
{
  Type_JPG = 0,
  Type_PNG,
  Type_GIF,
  Type_BMP,
  Type_None,
}ADCTypeDef;

typedef enum 
{
  eID_ADC_Name = 0x1000,
  eID_ADC_EXIT,
  eID_ADC_PREV,
  eID_ADC_NEXT,
//  eID_ADC_MsgBOX,
  eID_ADC_Time,
  eID_ADC_Res,
  eID_ADC_Res_Value,
//  //eID_ADC_Time,
  eID_ADC_Time_Value,
//  eID_ADC_INTFLASH,
//  eID_ADC_EXTFLASH,
//  eID_ADC_SDCARD,
//  eID_ADC_Return,
//  eID_ADC_JPG,
//  eID_ADC_PNG,
//  eID_ADC_GIF,
//  eID_ADC_BMP,
//  eID_ADC_Title,
  eID_ADC_Def,
//  eID_FILEPATH,
//  eID_ZOOMIN,
//  eID_ZOOMOUT,
  eID_ADC_FPS,
  eID_ADC_FPS_Value,
//  eID_LIST_1,
}GUI_ADCViewer_ID;

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
  HWND ADCView_Handle;

  
  
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
  
}ADCViewer_Dialog_Typedef;
#endif /* _CDLG_ADCVIEW_H_ */

