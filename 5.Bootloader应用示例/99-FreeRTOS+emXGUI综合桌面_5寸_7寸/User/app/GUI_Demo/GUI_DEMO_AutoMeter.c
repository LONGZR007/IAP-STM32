#include "emXGUI.h"
#include "x_libc.h"
#include "string.h"
#include "ff.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include <stdlib.h>
#include "./pic_load/gui_pic_load.h"

/* 图片资源名 */
//#define GUI_AUTOMETER_BACKGROUNG_PIC      "autometerdesktop.jpg"        // 800*480
//#define GUI_AM_AUTOPOINTER_PIC            "autopointer.png"
//#define GUI_ABS_CHECKED_PIC               "ABS_checked.png"             // 166*70
//#define GUI_ABS_UNCHECKED_PIC             "ABS_unchecked.png"           // 166*70
//#define GUI_ALARM_CHECKED_PIC             "alarm_checked.png"           // 220*240
//#define GUI_ALARM_UNCHECKED_PIC           "alarm_unchecked.png"         // 233*243
//#define GUI_BRAKING_OPENED_PIC            "braking_opened.png"          // 5*45
//#define GUI_BRAKING_SHUT_PIC              "braking_shut.png"            // 5*65
//#define GUI_DOOR_OPENED_PIC               "door_opened.png"             // 9*103
//#define GUI_DOOR_SHUT_PIC                 "door_shut.png"               // 233*243
//#define GUI_ENGINE_ALARM_PIC              "engine_alarm.png"            // 6*20
//#define GUI_ENGINE_NORMAL_PIC             "engine_normal.png"           // 16*85
//#define GUI_ENGINE_SHUT_PIC               "engine_shut.png"             // 6*102
//#define GUI_ENGINE_OIL_CHECKED_PIC        "engine_oil_checked.png"      // 233*243
//#define GUI_ENGINE_OIL_UNCHECKED_PIC      "engine_oil_unchecked.png"    // 233*243
//#define GUI_HARNESS_CHECKED_PIC           "harness_checked.png"         // 15*61
//#define GUI_HARNESS_UNCHECKED_PIC         "harness_unchecked.png"       // 15*61
//#define GUI_HEADLIGHT_DISTANCE_PIC        "headlight_distance.png"      // 15*77
//#define GUI_HEADLIGHT_OPENED_PIC          "headlight_opened.png"        // 15*77
//#define GUI_HEADLIGHT_SHUT_PIC            "headlight_shut.png"          // 15*77
//#define GUI_LEFT_OPENED_PIC               "left_opened.png"             // 7*86
//#define GUI_LEFT_SHUT_PIC                 "left_shut.png"               // 7*86
//#define GUI_RIGHT_OPENED_PIC              "right_opened.png"            // 7*86
//#define GUI_RIGHT_SHUT_PIC                "right_shut.png"              // 7*86
//#define GUI_FOG_LAMP_OPENED               "fog_lamp_opened.png"         // 
//#define GUI_FOG_LAMP_SHUT                 "fog_lamp_shut.png"           // 
//#define GUI_LEFT1_PIC                     "left1.png"                   // 
//#define GUI_LEFT2_PIC                     "left2.png"                   // 
//#define GUI_LEFT3_PIC                     "left3.png"                   // 
//#define GUI_LEFT4_PIC                     "left4.png"                   // 
//#define GUI_LEFT5_PIC                     "left5.png"                   // 
//#define GUI_LEFT6_PIC                     "left6.png"                   // 
//#define GUI_LEFT7_PIC                     "left7.png"                   // 
//#define GUI_LEFT8_PIC                     "left8.png"                   // 
//#define GUI_LEFT9_PIC                     "left9.png"                   // 
//#define GUI_LEFT10_PIC                    "left10.png"                  // 
//#define GUI_LEFT11_PIC                    "left11.png"                  // 
//#define GUI_RIGHT1_PIC                    "right1.png"                  // 
//#define GUI_RIGHT2_PIC                    "right2.png"                  // 
//#define GUI_RIGHT3_PIC                    "right3.png"                  // 
//#define GUI_RIGHT4_PIC                    "right4.png"                  // 
//#define GUI_RIGHT5_PIC                    "right5.png"                  // 
//#define GUI_RIGHT6_PIC                    "right6.png"                  // 
//#define GUI_RIGHT7_PIC                    "right7.png"                  // 
//#define GUI_RIGHT8_PIC                    "right8.png"                  // 
//#define GUI_RIGHT9_PIC                    "right9.png"                  // 
//#define GUI_RIGHT10_PIC                   "right10.png"                 // 
//#define GUI_RIGHT11_PIC                   "right11.png"                 // 

//typedef enum
//{
//  hdc_autopointer = 0,
//  hdc_ABS_checked,
//  hdc_ABS_unchecked,           // 
//  hdc_alarm_checked,           //
//  hdc_alarm_unchecked,         // 
//  hdc_braking_opened,          // 
//  hdc_braking_shut,           // 
//  hdc_door_opened,             // 
//  hdc_door_shut,               // 
//  hdc_engine_alarm,            // 
//  hdc_engine_normal,           // 
//  hdc_engine_shut,             // 
//  hdc_engine_oil_checked,      // 
//  hdc_engine_oil_unchecked,    // 
//  hdc_harness_checked,         // 
//  hdc_harness_unchecked,       // 
//  hdc_headlight_distance,      // 
//  hdc_headlight_opened,        // 
//  hdc_headlight_shut,          // 
//  hdc_left_opened,             // 
//  hdc_left_shut,               // 
//  hdc_right_opened,            // 
//  hdc_right_shut,              // 
//  hdc_fog_lamp_opened,
//  hdc_fog_lamp_shut,
//  hdc_left1,                   // 
//  hdc_left2,                   // 
//  hdc_left3,                  // 
//  hdc_left4,                   // 
//  hdc_left5,                   // 
//  hdc_left6,                   // 
//  hdc_left7,                   // 
//  hdc_left8,                   // 
//  hdc_left9,                   // 
//  hdc_left10,                  // 
//  hdc_left11,                  // 
//  hdc_right1,                  // 
//  hdc_right2,                  // 
//  hdc_right3,                  // 
//  hdc_right4,                  // 
//  hdc_right5,                  // 
//  hdc_right6,                  // 
//  hdc_right7,                  // 
//  hdc_right8,                  // 
//  hdc_right9,                  // 
//  hdc_right10,                // 
//  hdc_right11,                // 

//	hdc_am_end,              // 这个必须放在最后
//}hdc_am_png_t;

//typedef struct{
//	char *pic_name;      // 图片名
//	int w;               // 图片宽
//	int h;               // 图片高
//	hdc_am_png_t id;        // hdc 编号
//}am_hdc_t;

typedef enum 
{ 
  /****************** 按钮控件 ID 值 (顺序不可随便更改) *******************/
  ID_AM_EXIT = 0x1000,      // 退出按钮
  ID_AM_Signal_Left,        // 左转向灯
	ID_AM_Signal_Right,       // 右转向灯
	ID_AM_Car_Door,           // 车门
	ID_AM_Braking,            // 驻车制动
	ID_AM_Harness,            // 安全带
	ID_AM_ABS,                // ABS
	ID_AM_Alarm,              // 报警
	ID_AM_Engine_Oil,         // 机油
	ID_AM_Fog_Lamp,           // 雾灯
  ID_AM_Engine,             // 发动机
  ID_AM_Headlight,          // 前照灯
	ID_AM_Left1,              // 左边滑块 1
	ID_AM_Left2,              // 左边滑块 2
	ID_AM_Left3,              // 左边滑块 3 
	ID_AM_Left4,              // 左边滑块 4
	ID_AM_Left5,              // 左边滑块 5
  ID_AM_Left6,              // 左边滑块 6
	ID_AM_Left7,              // 左边滑块 7
	ID_AM_Left8,              // 左边滑块 8
	ID_AM_Left9,              // 左边滑块 9
	ID_AM_Left10,             // 左边滑块 10
  ID_AM_Left11,             // 左边滑块 11
  ID_AM_Rigth1,             // 右边滑块 1
	ID_AM_Rigth2,             // 右边滑块 2
	ID_AM_Rigth3,             // 右边滑块 3 
	ID_AM_Rigth4,             // 右边滑块 4
	ID_AM_Rigth5,             // 右边滑块 5
  ID_AM_Rigth6,             // 右边滑块 6
	ID_AM_Rigth7,             // 右边滑块 7
	ID_AM_Rigth8,             // 右边滑块 8
	ID_AM_Rigth9,             // 右边滑块 9
	ID_AM_Rigth10,            // 右边滑块 10
  ID_AM_Rigth11,            // 右边滑块 11
  /***************** 文本控件 ID 值 *********************/
  ID_AM_SPEED,              // 速度
}am_id_t;

typedef struct{
	WCHAR *icon_name;    // 图标名
	RECT rc;             // 位置信息
	uint8_t state;       // 按钮状态
  hdc_am_png_t hdc[3];
	am_id_t id;          // 按钮ID
  
}am_icon_t;

#define ICON_BTN_NUM     34     // 按钮数量

uint8_t AM_dial = 0;    // 表盘

//图标管理数组
am_icon_t AM_icon[] = {

  /* 按钮 */
  {L"-",  {747,  12,  36,  36},  0, {hdc_am_end, hdc_am_end, hdc_am_end}, ID_AM_EXIT},             // 0. 退出按钮
  {L"-",  { 99,   9,  40,  40},  0, {hdc_left_opened,        hdc_left_shut,            hdc_am_end},            ID_AM_Signal_Left},      // 1. 左转向灯
  {L"-",  {660,   9,  40,  40},  0, {hdc_right_opened,       hdc_right_shut,           hdc_am_end},            ID_AM_Signal_Right},     // 2. 右转向灯
  {L"-",  {380,   9,  40,  40},  0, {hdc_door_opened,        hdc_door_shut,            hdc_am_end},            ID_AM_Car_Door},         // 3. 
  {L"-",  {492,   9,  49,  40},  0, {hdc_braking_opened,     hdc_braking_shut,         hdc_am_end},            ID_AM_Braking},          // 4. 
  {L"-",  {576,   9,  40,  40},  0, {hdc_harness_checked,    hdc_harness_unchecked,    hdc_am_end},            ID_AM_Harness},          // 5. 
  {L"-",  {85,  428,  54,  40},  0, {hdc_ABS_checked,        hdc_ABS_unchecked,        hdc_am_end},            ID_AM_ABS},              // 6. 
  {L"-",  {180, 428,  40,  40},  0, {hdc_alarm_checked,      hdc_alarm_unchecked,      hdc_am_end},            ID_AM_Alarm},            // 7. 
  {L"-",  {640, 428,  56,  40},  0, {hdc_engine_oil_checked, hdc_engine_oil_unchecked, hdc_am_end},            ID_AM_Engine_Oil},       // 8. 
  {L"-",  {251,   9,  45,  40},  0, {hdc_fog_lamp_opened,    hdc_fog_lamp_shut,        hdc_am_end},            ID_AM_Fog_Lamp},         // 9. 
  {L"-",  {563, 428,  40,  40},  0, {hdc_engine_alarm,       hdc_engine_normal,        hdc_engine_shut},    ID_AM_Engine},           // 10. 
  {L"-",  {165,   9,  64,  40},  0, {hdc_headlight_opened,   hdc_headlight_distance,   hdc_headlight_shut}, ID_AM_Headlight},        // 11. 前照灯

  {L"-",  {116, 116,  55,  21},  0, {hdc_left1, hdc_am_end, hdc_am_end},   ID_AM_Left1},            // 12. 
  {L"-",  { 99, 140,  54,  21},  0, {hdc_left2, hdc_am_end, hdc_am_end},   ID_AM_Left2},            // 13. 
  {L"-",  { 86, 163,  53,  21},  0, {hdc_left3, hdc_am_end, hdc_am_end},   ID_AM_Left3},            // 14. 
  {L"-",  { 77, 186,  53,  21},  0, {hdc_left4, hdc_am_end, hdc_am_end},   ID_AM_Left4},            // 15. 
  {L"-",  { 68, 210,  54,  21},  0, {hdc_left5, hdc_am_end, hdc_am_end},   ID_AM_Left5},            // 16. 
  {L"-",  { 62, 233,  56,  21},  0, {hdc_left6, hdc_am_end, hdc_am_end},   ID_AM_Left6},            // 17. 
  {L"-",  { 58, 257,  58,  21},  0, {hdc_left7, hdc_am_end, hdc_am_end},   ID_AM_Left7},            // 18. 
  {L"-",  { 56, 280,  62,  21},  0, {hdc_left8, hdc_am_end, hdc_am_end},   ID_AM_Left8},            // 19. 
  {L"-",  { 54, 303,  68,  21},  0, {hdc_left9, hdc_am_end, hdc_am_end},   ID_AM_Left9},            // 20. 
  {L"-",  { 55, 327,  75,  21},  0, {hdc_left10, hdc_am_end, hdc_am_end},  ID_AM_Left10},           // 21. 
  {L"-",  { 56, 351,  85,  21},  0, {hdc_left11, hdc_am_end, hdc_am_end},  ID_AM_Left11},           // 22.

  {L"-",  {629, 116,  55,  21},  0, {hdc_right1, hdc_am_end, hdc_am_end},  ID_AM_Rigth1},           // 23. 
  {L"-",  {647, 140,  53,  21},  0, {hdc_right2, hdc_am_end, hdc_am_end},  ID_AM_Rigth2},           // 24. 
  {L"-",  {661, 163,  52,  21},  0, {hdc_right3, hdc_am_end, hdc_am_end},  ID_AM_Rigth3},           // 25. 
  {L"-",  {671, 186,  52,  21},  0, {hdc_right4, hdc_am_end, hdc_am_end},  ID_AM_Rigth4},           // 26. 
  {L"-",  {678, 210,  53,  21},  0, {hdc_right5, hdc_am_end, hdc_am_end},  ID_AM_Rigth5},           // 27. 
  {L"-",  {682, 233,  55,  21},  0, {hdc_right6, hdc_am_end, hdc_am_end},  ID_AM_Rigth6},           // 28. 
  {L"-",  {684, 257,  57,  21},  0, {hdc_right7, hdc_am_end, hdc_am_end},  ID_AM_Rigth7},           // 29. 
  {L"-",  {683, 280,  62,  21},  0, {hdc_right8, hdc_am_end, hdc_am_end},  ID_AM_Rigth8},           // 30. 
  {L"-",  {678, 303,  68,  21},  0, {hdc_right9, hdc_am_end, hdc_am_end},  ID_AM_Rigth9},           // 31. 
  {L"-",  {671, 327,  75,  21},  0, {hdc_right10, hdc_am_end, hdc_am_end}, ID_AM_Rigth10},          // 32. 
  {L"-",  {658, 351,  85,  21},  0, {hdc_right11, hdc_am_end, hdc_am_end}, ID_AM_Rigth11},          // 33. 

  {L"0",  {302, 327, 130, 100},  0, {hdc_am_end, hdc_am_end, hdc_am_end}, ID_AM_SPEED},            // 34. 
};

/* 图片管理数组——HDC */
//const am_hdc_t am_png_info[hdc_am_end] = 
//{
//  {GUI_AM_AUTOPOINTER_PIC,         34, 170+137, hdc_autopointer},
//  {GUI_ABS_CHECKED_PIC,            54,  40,     hdc_ABS_checked},
//  {GUI_ABS_UNCHECKED_PIC,          54,  40,     hdc_ABS_unchecked},
//  {GUI_ALARM_CHECKED_PIC,          40,  40,     hdc_alarm_checked},
//  {GUI_ALARM_UNCHECKED_PIC,        40,  40,     hdc_alarm_unchecked},
//  {GUI_BRAKING_OPENED_PIC,         49,  40,     hdc_braking_opened},
//  {GUI_BRAKING_SHUT_PIC,           49,  40,     hdc_braking_shut},
//  {GUI_DOOR_OPENED_PIC,            40,  40,     hdc_door_opened},
//  {GUI_DOOR_SHUT_PIC,              40,  40,     hdc_door_shut},
//  {GUI_ENGINE_ALARM_PIC,           40,  40,     hdc_engine_alarm},
//  {GUI_ENGINE_NORMAL_PIC,          40,  40,     hdc_engine_normal},
//  {GUI_ENGINE_SHUT_PIC,            40,  40,     hdc_engine_shut},
//  {GUI_ENGINE_OIL_CHECKED_PIC,     56,  40,     hdc_engine_oil_checked},
//  {GUI_ENGINE_OIL_UNCHECKED_PIC,   56,  40,     hdc_engine_oil_unchecked},
//  {GUI_HARNESS_CHECKED_PIC,        40,  40,     hdc_harness_checked},
//  {GUI_HARNESS_UNCHECKED_PIC,      40,  40,     hdc_harness_unchecked},
//  {GUI_HEADLIGHT_DISTANCE_PIC,     64,  40,     hdc_headlight_distance},
//  {GUI_HEADLIGHT_OPENED_PIC,       64,  40,     hdc_headlight_opened},
//  {GUI_HEADLIGHT_SHUT_PIC,         64,  40,     hdc_headlight_shut},
//  {GUI_LEFT_OPENED_PIC,            40,  40,     hdc_left_opened},
//  {GUI_LEFT_SHUT_PIC,              40,  40,     hdc_left_shut},
//  {GUI_RIGHT_OPENED_PIC,           40,  40,     hdc_right_opened},
//  {GUI_RIGHT_SHUT_PIC,             40,  40,     hdc_right_shut},
//  {GUI_FOG_LAMP_OPENED,            45,  40,     hdc_fog_lamp_opened},
//  {GUI_FOG_LAMP_SHUT,              45,  40,     hdc_fog_lamp_shut},
//  {GUI_LEFT1_PIC,                  55,  21,     hdc_left1},
//  {GUI_LEFT2_PIC,                  54,  21,     hdc_left2},
//  {GUI_LEFT3_PIC,                  53,  21,     hdc_left3},
//  {GUI_LEFT4_PIC,                  53,  21,     hdc_left4},
//  {GUI_LEFT5_PIC,                  54,  21,     hdc_left5},
//  {GUI_LEFT6_PIC,                  56,  21,     hdc_left6},
//  {GUI_LEFT7_PIC,                  58,  21,     hdc_left7},
//  {GUI_LEFT8_PIC,                  62,  21,     hdc_left8},
//  {GUI_LEFT9_PIC,                  68,  21,     hdc_left9},
//  {GUI_LEFT10_PIC,                 75,  21,     hdc_left10},
//  {GUI_LEFT11_PIC,                 85,  21,     hdc_left11}, 
//  {GUI_RIGHT1_PIC,                 55,  21,     hdc_right1},
//  {GUI_RIGHT2_PIC,                 53,  21,     hdc_right2},
//  {GUI_RIGHT3_PIC,                 52,  21,     hdc_right3},
//  {GUI_RIGHT4_PIC,                 52,  21,     hdc_right4},           
//  {GUI_RIGHT5_PIC,                 53,  21,     hdc_right5},
//  {GUI_RIGHT6_PIC,                 55,  21,     hdc_right6},
//  {GUI_RIGHT7_PIC,                 57,  21,     hdc_right7},
//  {GUI_RIGHT8_PIC,                 62,  21,     hdc_right8},
//  {GUI_RIGHT9_PIC,                 68,  21,     hdc_right9},
//  {GUI_RIGHT10_PIC,                75,  21,     hdc_right10},
//  {GUI_RIGHT11_PIC,                85,  21,     hdc_right11},

//};

///* HDC */
//static HDC hdc_am_bk;
//static HDC hdc_am_png[hdc_am_end];

/* 时钟位图 */
static BITMAP bm_autopointer;    // 指针

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

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_am_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 120, 120, 120));      //设置文字色
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -2);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }

}

static void btn_owner_draw(DRAWITEM_HDR *ds, uint8_t start, HDC hdc1, HDC hdc2, HDC hdc3)    // 绘制一个按钮外观
{
	HDC hdc;
	RECT rc, rc_tmp;
  HWND hwnd;
  
  hwnd = ds->hwnd;
	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.

  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_am_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  switch (start)
  {
    case 0:
    {
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc1, 0, 0, SRCCOPY);
    }
    break;

    case 1:
    {
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc2, 0, 0, SRCCOPY);
    }
    break;

    case 2:
    {
      BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc3, 0, 0, SRCCOPY);
    }
    break;
  }
}

/*
 * @brief  重绘数字透明文本
 * @param  ds:	自定义绘制结构体
 * @retval NONE
*/
static void Num_Textbox_OwnerDraw(DRAWITEM_HDR *ds)
{
  HWND hwnd;
	HDC hdc;
  RECT rc,rc_tmp;
	WCHAR wbuf[128];

  hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  rc = ds->rc;

	/* 背景 */
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_am_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  SetFont(hdc, controlFont_72);
	SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字
	DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_RIGHT);//绘制文字(居中对齐方式)
  SetFont(hdc, defaultFont);
}


static LRESULT win_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{ 
//  static uint8_t Load_Flag = 0;
  static uint8_t Speed = 0;
  static uint8_t Bilnk_Flag = 0;           // 闪烁标志
  static uint8_t Signal_Bilnk_Flag = 0;    // 转向灯闪烁标志
  static uint8_t speed_flag = 0;           // 速度标志

   switch(msg){
      case WM_CREATE:
      {
//        u8 *jpeg_buf;
//        u32 jpeg_size;
//        JPG_DEC *dec;
//        BOOL res = NULL;

//        res = RES_Load_Content(GUI_AUTOMETER_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
//        //res = FS_Load_Content(GUI_AUTOMETER_BACKGROUNG_PIC, (char**)&jpeg_buf, &jpeg_size);
//        hdc_am_bk = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
//        if(res)
//        {
//          /* 根据图片数据创建JPG_DEC句柄 */
//          dec = JPG_Open(jpeg_buf, jpeg_size);

//          /* 绘制至内存对象 */
//          JPG_Draw(hdc_am_bk, 0, 0, dec);

//          /* 关闭JPG_DEC句柄 */
//          JPG_Close(dec);
//        }
//        /* 释放图片内容空间 */
//        RES_Release_Content((char **)&jpeg_buf);

//        u8 *pic_buf;
//        u32 pic_size;
//        PNG_DEC *png_dec;
//        BITMAP png_bm;
//        
//        for (uint8_t xC=0; xC<hdc_am_end; xC++)
//        {
//          /* 创建 HDC */
//          hdc_am_png[am_png_info[xC].id] = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, am_png_info[xC].w, am_png_info[xC].h);
//          ClrDisplay(hdc_am_png[am_png_info[xC].id], NULL, 0);
//          res = RES_Load_Content(am_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);
//          // res = FS_Load_Content(am_png_info[xC].pic_name, (char**)&pic_buf, &pic_size);
//          if(res)
//          {
//            png_dec = PNG_Open(pic_buf);
//            PNG_GetBitmap(png_dec, &png_bm);
//            DrawBitmap(hdc_am_png[am_png_info[xC].id], 0, 0, &png_bm, NULL);
//            PNG_Close(png_dec);
//          }
//          /* 释放图片内容空间 */
//          RES_Release_Content((char **)&pic_buf);
//        }

        /* 转换成bitmap */
        DCtoBitmap(hdc_am_png[hdc_autopointer], &bm_autopointer);

        for (uint8_t xC=0; xC<ICON_BTN_NUM; xC++)     //  按钮
        {
          /* 循环创建按钮 */
          CreateWindow(BUTTON, AM_icon[xC].icon_name,  WS_OWNERDRAW | WS_DISABLED,
                        AM_icon[xC].rc.x, AM_icon[xC].rc.y,
                        AM_icon[xC].rc.w, AM_icon[xC].rc.h,
                        hwnd, AM_icon[xC].id, NULL, NULL); 
        }

        EnableWindow(GetDlgItem(hwnd, ID_AM_EXIT), ENABLE);      // 使能返回按钮
        ShowWindow(GetDlgItem(hwnd, ID_AM_EXIT), SW_SHOW);       // 显示按钮

        /* 循环创建文本框 */
        CreateWindow(TEXTBOX, AM_icon[34].icon_name, WS_VISIBLE | WS_OWNERDRAW,
                      AM_icon[34].rc.x, AM_icon[34].rc.y,
                      AM_icon[34].rc.w,AM_icon[34].rc.h,
                      hwnd, AM_icon[34].id, NULL, NULL); 

        SetTimer(hwnd, 2, 200, TMR_START, NULL);    // 资源加载定时器

        break;
      }

      case WM_TIMER:
      {
        int tmr_id;
        WCHAR wbuf[10];
        RECT rc = {228, 60, 346, 342};

				tmr_id = wParam;    // 定时器 ID

				if (tmr_id == 1)     // 表盘定时器
        {
          if (speed_flag == 0)
          {
            Speed++;
            if (Speed > 180)
            {
              speed_flag = 1;
            }
          }
          else
          {
            Speed--;
            if (Speed == 0)
            {
              speed_flag = 0;
            }
          }
          
          x_wsprintf(wbuf, L"%d", Speed);
          SetWindowText(GetDlgItem(hwnd, ID_AM_SPEED), wbuf);
          InvalidateRect(hwnd, &rc, TRUE);    // 重绘窗口
        }
        else if (tmr_id == 2)    // 进入 APP 闪烁定时器
        {
          for (uint8_t xC=0; xC<11; xC++)    // 除退出按钮外都闪烁
          {
            if (Bilnk_Flag == 11)
            {
              AM_icon[1].state = 1;    // 把左转向灯设为灰色
              AM_icon[2].state = 1;    // 把右转向灯设为灰色
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Left), NULL, TRUE);     // 重绘转向灯
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Right), NULL, TRUE);    // 重绘转向灯
            }
            else if (Bilnk_Flag%2 == 0)
            {
              ShowWindow(GetDlgItem(hwnd, ID_AM_Signal_Left + xC), SW_SHOW);    // 显示按钮
            }
            else
            {
              ShowWindow(GetDlgItem(hwnd, ID_AM_Signal_Left + xC), SW_HIDE);    // 隐藏按钮
            }
          }

          if (11 > Bilnk_Flag)
          {
            /* 逐个显示两边的按钮 */
            ShowWindow(GetDlgItem(hwnd, ID_AM_Left11 - Bilnk_Flag), SW_SHOW);    // 显示按钮
            ShowWindow(GetDlgItem(hwnd, ID_AM_Rigth11 - Bilnk_Flag), SW_SHOW);    // 显示按钮
          }
          else
          {
            for (uint8_t xC=0; xC<33; xC++)
            {
              EnableWindow(GetDlgItem(hwnd, ID_AM_Signal_Left + xC), ENABLE);      // 使能全部按钮
            }

            Bilnk_Flag = 0;
            SetTimer(hwnd, 1, 50, TMR_START, NULL);               // 仪表定时器
            SetTimer(hwnd, 3, 500, TMR_START, NULL);              // 转向灯闪烁定时器
            KillTimer(hwnd, 2);                                   // 完成闪烁，删除定时器
          }

          Bilnk_Flag++;
        }
        else if (tmr_id == 3)     /* 转向灯闪烁定时器 */
        {
          if (Bilnk_Flag%2 == 0)
          {
            if (Signal_Bilnk_Flag & 1)
            {
              AM_icon[1].state = 0;       // 将按钮设为绿色
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Left), NULL, FALSE);           // 重绘按钮
            }
            if (Signal_Bilnk_Flag & 0x80)
            {
              AM_icon[2].state = 0;       // 将按钮设为绿色
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Right), NULL, FALSE);           // 重绘按钮
            }
          }
          else
          {
            if (Signal_Bilnk_Flag & 1)
            {
              AM_icon[1].state = 4;       // 将不显示图标
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Left), NULL, FALSE);           // 重绘按钮
            }
            if (Signal_Bilnk_Flag & 0x80)
            {
              AM_icon[2].state = 4;       // 将不显示图标
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Right), NULL, FALSE);           // 重绘按钮
            }
          }
          Bilnk_Flag++;
        }
      }  
			break;     
      
      case WM_NOTIFY:
      {
         u16 code,  id;
         id  =LOWORD(wParam);//获取消息的ID码
         code=HIWORD(wParam);//获取消息的类型   

         //发送单击
        if(code == BN_CLICKED)
        {
          if (id == ID_AM_EXIT)
          {
            PostCloseMessage(hwnd);    // 发送关闭窗口的消息
          }
          else if (id == ID_AM_Signal_Left)
          {
            if (Signal_Bilnk_Flag & 1)
            {
              Signal_Bilnk_Flag &= ~1;    // 清除第一位
              AM_icon[1].state = 1;       // 将按钮设为灰色
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Left), NULL, FALSE);           // 重绘按钮
            }
            else
            {
              Signal_Bilnk_Flag |= 1;     // 置位第一位
              AM_icon[1].state = 0;       // 将按钮设为绿色
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Left), NULL, FALSE);           // 重绘按钮
            }
          }
          else if (id == ID_AM_Signal_Right)
          {
            if (Signal_Bilnk_Flag  & 0x80)
            {
              Signal_Bilnk_Flag &= ~0x80;    // 清除第一位
              AM_icon[2].state = 1;          // 将按钮设为灰色
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Right), NULL, FALSE);           // 重绘按钮
            }
            else
            {
              Signal_Bilnk_Flag |= 0x80;     // 置位最高位
              AM_icon[2].state = 0;          // 将按钮设为绿色
              InvalidateRect(GetDlgItem(hwnd, ID_AM_Signal_Right), NULL, FALSE);           // 重绘按钮
            }
          }
          else if (id >= ID_AM_Car_Door && id <= ID_AM_Fog_Lamp)
          {
            AM_icon[id - ID_AM_EXIT].state = !AM_icon[id - ID_AM_EXIT].state;         // 切换显示状态
          }
          else if (id == ID_AM_Engine || id == ID_AM_Headlight)
          {
            AM_icon[id - ID_AM_EXIT].state = ++AM_icon[id - ID_AM_EXIT].state % 3;    // 3 种状态切换显示
          }
          else if (id >= ID_AM_Left1 && id <= ID_AM_Left11)
          {
            for (am_id_t xC=ID_AM_Left1; xC<ID_AM_Left11+1; xC++)          // 检查是哪个按下了
            {
              AM_icon[xC - ID_AM_EXIT].state = (xC >= id) ? (0) : (3);     // 0:显示   3:不显示
              InvalidateRect(GetDlgItem(hwnd, xC), NULL, FALSE);           // 重绘按钮
            }
          }
          else if (id >= ID_AM_Rigth1 && id <= ID_AM_Rigth11)
          {
            for (am_id_t xC=ID_AM_Rigth1; xC<ID_AM_Rigth11+1; xC++)       // 检查是哪个按下了
            {
              AM_icon[xC - ID_AM_EXIT].state = (xC >= id) ? (0) : (3);    // 0:显示   3:不显示
              InvalidateRect(GetDlgItem(hwnd, xC), NULL, FALSE);          // 重绘按钮
            }
          }
        }

        break;
      }

      //重绘制函数消息
      case WM_DRAWITEM:
      {
         DRAWITEM_HDR *ds;
         ds = (DRAWITEM_HDR*)lParam;        
         if(ds->ID == ID_AM_EXIT)
         {
            exit_owner_draw(ds);
            return TRUE;
         }
         else if(ds->ID == ID_AM_SPEED)
         {
            Num_Textbox_OwnerDraw(ds);
            return TRUE;
         }
         else if(ds->ID >= ID_AM_Signal_Left && ds->ID <= ID_AM_Rigth11)
         {
           /* 重绘按钮 */
           if (AM_icon[ds->ID - ID_AM_EXIT].hdc[0] == hdc_am_end)
           {
             return FALSE;
           }
           else if (AM_icon[ds->ID - ID_AM_EXIT].hdc[1] == hdc_am_end)
           {
              btn_owner_draw(ds, AM_icon[ds->ID - ID_AM_EXIT].state, 
                            hdc_am_png[AM_icon[ds->ID - ID_AM_EXIT].hdc[0]],
                            0,
                            0);
           }
           else if (AM_icon[ds->ID - ID_AM_EXIT].hdc[1] == hdc_am_end)
           {
             btn_owner_draw(ds, AM_icon[ds->ID - ID_AM_EXIT].state, \
                              hdc_am_png[AM_icon[ds->ID - ID_AM_EXIT].hdc[0]],\
                              hdc_am_png[AM_icon[ds->ID - ID_AM_EXIT].hdc[1]],\
                              0);
           }
           else
           {
              btn_owner_draw(ds, AM_icon[ds->ID - ID_AM_EXIT].state, \
                              hdc_am_png[AM_icon[ds->ID - ID_AM_EXIT].hdc[0]],\
                              hdc_am_png[AM_icon[ds->ID - ID_AM_EXIT].hdc[1]],\
                              hdc_am_png[AM_icon[ds->ID - ID_AM_EXIT].hdc[2]]);
           }
           return TRUE;
         }

        return FALSE;
      }
      
      //绘制窗口界面消息
      case WM_PAINT:
      {
        PAINTSTRUCT ps;
        RECT rc2 = {433, 377, 65, 29};
        HDC hdc;
        int angle = Speed / 180.0 * 270 - 135;

        angle = (angle < 0) ? (angle+360) : angle;


        //开始绘制
        hdc = BeginPaint(hwnd, &ps); 

        EnableAntiAlias(hdc, TRUE);                             // 使能抗锯齿
        RotateBitmap(hdc, 400, 231, &bm_autopointer, angle);
        EnableAntiAlias(hdc, FALSE);                            // 禁用抗锯齿

        SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
        DrawText(hdc, L"km/h", -1, &rc2, DT_VCENTER|DT_LEFT);//绘制文字(居中对齐方式)
        
        EndPaint(hwnd, &ps);
        break;
      }
      
      case WM_ERASEBKGND:
      {
        HDC hdc =(HDC)wParam;
        RECT rc =*(RECT*)lParam;
        RECT rc2 = {0, 0, GUI_XSIZE, GUI_YSIZE};
        if (1)     // 资源加载完成
        {
          BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_am_bk, rc.x, rc.y, SRCCOPY);
        }
        else
        {    /* 资源加载未完成 */ 
          SetBrushColor(hdc, MapRGB(hdc, 10, 10, 10));
          FillRect(hdc, &rc2);
          SetTextColor(hdc, MapRGB(hdc, 250, 250, 250));
          DrawText(hdc, L"资源加载中，请稍等...", -1, &rc2, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
        }

        return TRUE;
      }

      //关闭窗口消息处理case
      case WM_CLOSE:
      {   
        DestroyWindow(hwnd);
        return TRUE;	
      }
    
      //关闭窗口消息处理case
      case WM_DESTROY:
      {        
//        Load_Flag = 0;
        Bilnk_Flag = 0;           // 复位标志
        Speed = 0;                // 复位速度
        speed_flag = 0;           // 复位标志
        Signal_Bilnk_Flag = 0;    // 复位标志
        for (uint8_t xC=1; xC<34; xC++)
        {
          AM_icon[xC].state = 0;    // 复位全部按钮状态
        }
//        DeleteDC(hdc_am_bk);
//        for (int i=0; i<hdc_am_end; i++)
//        {
//          DeleteDC(hdc_am_png[i]);
//        }
        return PostQuitMessage(hwnd);		
      }
      
      default:
         return DefWindowProc(hwnd, msg, wParam, lParam);
   }
     
   return WM_NULL;
}


void GUI_Auto_Meter_DIALOG(void)
{ 	
	WNDCLASS	wcex;
  HWND hwnd;
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
	hwnd = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,//
                                    &wcex,
                                    L"GUI AM DIALOG",
                                    WS_VISIBLE|WS_CLIPCHILDREN,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                    NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


