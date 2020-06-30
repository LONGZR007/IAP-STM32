#include "emXGUI.h"

#ifndef _GUI_PIC_LOAD_H
#define _GUI_PIC_LOAD_H

/********************************************************************************************
*                                时钟 App 图片                                              *
********************************************************************************************/
#define GUI_CLOCK_BACKGROUNG_PIC      "clockdesktop.jpg"            // 800*480
#define GUI_CLOCK_BTN_PIC             "clock_but.png"               // 166*70
#define GUI_CLOCK_BTN_PRESS_PIC       "clock_but_press.png"         // 166*70
#define GUI_CLOCK_CALENDAR_PIC        "settings_calendar.png"       // 220*240
#define GUI_CLOCK_00BACK_PIC          "clock_00_background.png"     // 233*243
#define GUI_CLOCK_00H_PIC             "clock_00_hour_hand.png"      // 5*45
#define GUI_CLOCK_00M_PIC             "clock_00_minute_hand.png"    // 5*65
#define GUI_CLOCK_00S_PIC             "clock_00_second_hand.png"    // 9*103
#define GUI_CLOCK_01BACK_PIC          "clock_01_background.png"     // 233*243
#define GUI_CLOCK_01H_PIC             "clock_01_hour_hand.png"      // 6*20
#define GUI_CLOCK_01M_PIC             "clock_01_minute_hand.png"    // 16*85
#define GUI_CLOCK_01S_PIC             "clock_01_second_hand.png"    // 6*102
#define GUI_CLOCK_02BACK_PIC          "clock_02_background.png"     // 233*243
#define GUI_CLOCK_02H_PIC             "clock_02_hour_hand.png"      // 15*61
#define GUI_CLOCK_02M_PIC             "clock_02_minute_hand.png"    // 15*77
#define GUI_CLOCK_02S_PIC             "clock_02_second_hand.png"    // 7*86
#define GUI_CLOCK_CHCKED_PIC          "blue_glow.png"               // 272*272

typedef enum
{
	hdc_clock_btn = 0,
	hdc_clock_btn_press,
	hdc_clock_calendar,
	hdc_clock_back_00,
	hdc_clock_h_00,
	hdc_clock_m_00,
	hdc_clock_s_00,
	hdc_clock_back_01,
	hdc_clock_h_01,
	hdc_clock_m_01,
	hdc_clock_s_01,
	hdc_clock_back_02,
	hdc_clock_h_02,
	hdc_clock_m_02,
	hdc_clock_s_02,
	hdc_clock_chcked,

	hdc_clock_end,              // 这个必须放在最后
}hdc_clock_png_t;

typedef struct{
	char *pic_name;      // 图片名
	int w;               // 图片宽
	int h;               // 图片高
	hdc_clock_png_t id;  // hdc 编号
}clock_hdc_t;

extern HDC hdc_clock_bk;
extern HDC hdc_clock_png[hdc_clock_end];

BOOL PIC_Load_To_SDRAM(void);
/********************************************************************************************
*                                时钟 App 图片 END                                          *
********************************************************************************************/


/********************************************************************************************
*                             汽车仪表 App 图片                                             *
********************************************************************************************/
#define GUI_AUTOMETER_BACKGROUNG_PIC      "autometerdesktop.jpg"        // 800*480
#define GUI_AM_AUTOPOINTER_PIC            "autopointer.png"
#define GUI_ABS_CHECKED_PIC               "ABS_checked.png"             // 166*70
#define GUI_ABS_UNCHECKED_PIC             "ABS_unchecked.png"           // 166*70
#define GUI_ALARM_CHECKED_PIC             "alarm_checked.png"           // 220*240
#define GUI_ALARM_UNCHECKED_PIC           "alarm_unchecked.png"         // 233*243
#define GUI_BRAKING_OPENED_PIC            "braking_opened.png"          // 5*45
#define GUI_BRAKING_SHUT_PIC              "braking_shut.png"            // 5*65
#define GUI_DOOR_OPENED_PIC               "door_opened.png"             // 9*103
#define GUI_DOOR_SHUT_PIC                 "door_shut.png"               // 233*243
#define GUI_ENGINE_ALARM_PIC              "engine_alarm.png"            // 6*20
#define GUI_ENGINE_NORMAL_PIC             "engine_normal.png"           // 16*85
#define GUI_ENGINE_SHUT_PIC               "engine_shut.png"             // 6*102
#define GUI_ENGINE_OIL_CHECKED_PIC        "engine_oil_checked.png"      // 233*243
#define GUI_ENGINE_OIL_UNCHECKED_PIC      "engine_oil_unchecked.png"    // 233*243
#define GUI_HARNESS_CHECKED_PIC           "harness_checked.png"         // 15*61
#define GUI_HARNESS_UNCHECKED_PIC         "harness_unchecked.png"       // 15*61
#define GUI_HEADLIGHT_DISTANCE_PIC        "headlight_distance.png"      // 15*77
#define GUI_HEADLIGHT_OPENED_PIC          "headlight_opened.png"        // 15*77
#define GUI_HEADLIGHT_SHUT_PIC            "headlight_shut.png"          // 15*77
#define GUI_LEFT_OPENED_PIC               "left_opened.png"             // 7*86
#define GUI_LEFT_SHUT_PIC                 "left_shut.png"               // 7*86
#define GUI_RIGHT_OPENED_PIC              "right_opened.png"            // 7*86
#define GUI_RIGHT_SHUT_PIC                "right_shut.png"              // 7*86
#define GUI_FOG_LAMP_OPENED               "fog_lamp_opened.png"         // 
#define GUI_FOG_LAMP_SHUT                 "fog_lamp_shut.png"           // 
#define GUI_LEFT1_PIC                     "left1.png"                   // 
#define GUI_LEFT2_PIC                     "left2.png"                   // 
#define GUI_LEFT3_PIC                     "left3.png"                   // 
#define GUI_LEFT4_PIC                     "left4.png"                   // 
#define GUI_LEFT5_PIC                     "left5.png"                   // 
#define GUI_LEFT6_PIC                     "left6.png"                   // 
#define GUI_LEFT7_PIC                     "left7.png"                   // 
#define GUI_LEFT8_PIC                     "left8.png"                   // 
#define GUI_LEFT9_PIC                     "left9.png"                   // 
#define GUI_LEFT10_PIC                    "left10.png"                  // 
#define GUI_LEFT11_PIC                    "left11.png"                  // 
#define GUI_RIGHT1_PIC                    "right1.png"                  // 
#define GUI_RIGHT2_PIC                    "right2.png"                  // 
#define GUI_RIGHT3_PIC                    "right3.png"                  // 
#define GUI_RIGHT4_PIC                    "right4.png"                  // 
#define GUI_RIGHT5_PIC                    "right5.png"                  // 
#define GUI_RIGHT6_PIC                    "right6.png"                  // 
#define GUI_RIGHT7_PIC                    "right7.png"                  // 
#define GUI_RIGHT8_PIC                    "right8.png"                  // 
#define GUI_RIGHT9_PIC                    "right9.png"                  // 
#define GUI_RIGHT10_PIC                   "right10.png"                 // 
#define GUI_RIGHT11_PIC                   "right11.png"                 // 

typedef enum
{
  hdc_autopointer = 0,
  hdc_ABS_checked,
  hdc_ABS_unchecked,           // 
  hdc_alarm_checked,           //
  hdc_alarm_unchecked,         // 
  hdc_braking_opened,          // 
  hdc_braking_shut,           // 
  hdc_door_opened,             // 
  hdc_door_shut,               // 
  hdc_engine_alarm,            // 
  hdc_engine_normal,           // 
  hdc_engine_shut,             // 
  hdc_engine_oil_checked,      // 
  hdc_engine_oil_unchecked,    // 
  hdc_harness_checked,         // 
  hdc_harness_unchecked,       // 
  hdc_headlight_distance,      // 
  hdc_headlight_opened,        // 
  hdc_headlight_shut,          // 
  hdc_left_opened,             // 
  hdc_left_shut,               // 
  hdc_right_opened,            // 
  hdc_right_shut,              // 
  hdc_fog_lamp_opened,
  hdc_fog_lamp_shut,
  hdc_left1,                   // 
  hdc_left2,                   // 
  hdc_left3,                  // 
  hdc_left4,                   // 
  hdc_left5,                   // 
  hdc_left6,                   // 
  hdc_left7,                   // 
  hdc_left8,                   // 
  hdc_left9,                   // 
  hdc_left10,                  // 
  hdc_left11,                  // 
  hdc_right1,                  // 
  hdc_right2,                  // 
  hdc_right3,                  // 
  hdc_right4,                  // 
  hdc_right5,                  // 
  hdc_right6,                  // 
  hdc_right7,                  // 
  hdc_right8,                  // 
  hdc_right9,                  // 
  hdc_right10,                // 
  hdc_right11,                // 

	hdc_am_end,              // 这个必须放在最后
}hdc_am_png_t;

typedef struct{
	char *pic_name;      // 图片名
	int w;               // 图片宽
	int h;               // 图片高
	hdc_am_png_t id;        // hdc 编号
}am_hdc_t;

extern HDC hdc_am_bk;
extern HDC hdc_am_png[hdc_am_end];
/********************************************************************************************
*                            汽车仪表 App 图片 END                                          *
********************************************************************************************/


/********************************************************************************************
*                                                  电压表 App 图片                           *
********************************************************************************************/
#define GUI_ADC_BACKGROUNG_PIC    "adc_desktop.jpg"
#define GUI_ADC_SLIDER_BTN_PIC    "slider_button.png"    // 90 * 90
#define GUI_ADC_SLIDER_PIC        "slider.png"           // 600 * 45 
#define GUI_ADC_CIRCLE_PIC        "adc_circle.png"       // 270 * 270
#define GUI_ADC_F429_RP_PIC       "F429_RP.png"          // 350 * 340   

typedef enum
{
  hdc_adc_slider_btn = 0,
  hdc_adc_slider,
  hdc_adc_circle,
  hdc_adc_F429_RP,
  
  hdc_adc_end,
}hdc_adc_png_t;

typedef struct{
	char *pic_name;      // 图片名
	int w;               // 图片宽
	int h;               // 图片高
	hdc_adc_png_t id;        // hdc 编号
}adc_hdc_t;

extern HDC hdc_adc_bk;
extern HDC hdc_adc_png[hdc_adc_end];
/********************************************************************************************
*                             电压表 App 图片 END                                            *
********************************************************************************************/

/********************************************************************************************
*                                     主页面图标                                             *
********************************************************************************************/
#define GUI_HOME_BACKGROUNG_PIC   "home_desktop.jpg"

#define ICON_BMP_ENABLE           0    // 主页图标使能1:使用主页图标 0：使用字体的方式（以下图片从sd卡加载，源图片在 资源文件_5寸/主页图标 里面）

#if ICON_BMP_ENABLE
#define GUI_ADC_ICON_PIC          "0:/adc_icon.bmp"
#define GUI_MUSIC_ICON_PIC        "0:/music_icon.bmp"
#define GUI_PHOTO_ICON_PIC        "0:/photo_icon.bmp"
#define GUI_CLOCK_ICON_PIC        "0:/clock_icon.bmp"
#define GUI_CAMERA_ICON_PIC       "0:/camera_icon.bmp"
#define GUI_GYRO_ICON_PIC         "0:/gyro_icon.bmp"
#define GUI_HUMITURE_ICON_PIC     "0:/humiture_icon.bmp"
#define GUI_VIDEO_ICON_PIC        "0:/video_icon.bmp"
#define GUI_RGBLEN_ICON_PIC       "0:/rgbled_icon.bmp"
#define GUI_GUIUSE_ICON_PIC       "0:/guiuse_icon.bmp"
#define GUI_SUDISH_ICON_PIC       "0:/sudish_icon.bmp"
#define GUI_NETWORK_ICON_PIC      "0:/entwork_icon.bmp"
//#define GUI_WIFI_ICON_PIC         "0:/wifi_icon.bmp"
#define GUI_PHONE_ICON_PIC        "0:/phone_icon.bmp"
#define GUI_NOTE_ICON_PIC         "0:/note_icon.bmp"
#define GUI_QRCODE_ICON_PIC       "0:/QRcode_icon.bmp"
#define GUI_RECORD_ICON_PIC       "0:/record_icon.bmp"
#define GUI_WIDGET_ICON_PIC       "0:/widget_icon.bmp"
#define GUI_FLASH_ICON_PIC        "0:/flash_icon.bmp"
#define GUI_SETTING_ICON_PIC      "0:/setting_icon.bmp"
#define GUI_BEEPER_ICON_PIC       "0:/beeper_icon.bmp"
#define GUI_KEYLED_ICON_PIC       "0:/keyled_icon.bmp"

typedef enum
{
  bmp_adc_icon = 0,
  bmp_music_icon,
  bmp_photo_icon,
  bmp_clock_icon,
  bmp_camera_icon,
  bmp_gyro_icon,
  bmp_humiture_icon,
  bmp_video_icon,
  bmp_rgbled_icon,
  bmp_guiuse_icon,
  bmp_sudish_icon,
  bmp_entwork_icon,
//  bmp_wifi_icon,
  bmp_phone_icon,
  bmp_note_icon,
  bmp_QRcode_icon,
  bmp_record_icon,
  bmp_widget_icon,
  bmp_flash_icon,
  bmp_setting_icon,
  bmp_beeper_icon,
  bmp_keyled_icon,
  
  bmp_icon_end,
}bmp_icon_t;

typedef struct{
	char *pic_name;      // 图片名
	int w;               // 图片宽
	int h;               // 图片高
	bmp_icon_t id;        // hdc 编号
}icon_info_t;

extern u8 * bmp_icon[bmp_icon_end];
#endif

extern HDC hdc_home_bk;
/********************************************************************************************
 *                             主页面图标  END                                               *
 ********************************************************************************************/

/********************************************************************************************
 *                                     蜂鸣器控制图片                                       *
 ********************************************************************************************/
#define GUI_BEEPER_BACKGROUNG_PIC   "beeper_desktop.jpg"
#define GUI_BEEPER_HORN_PIC         "beeper_horn.png"
#define GUI_BEEPER_HORN1_PIC        "beeper_horn1.png"
#define GUI_BEEPER_HORN2_PIC        "beeper_horn2.png"
#define GUI_BEEPER_HORN3_PIC        "beeper_horn3.png"
#define GUI_BEEPER_BUTTON_PIC       "beeper_button.png"


typedef enum
{
  hdc_beeper_horn = 0,
	hdc_beeper_horn1,
	hdc_beeper_horn2,
	hdc_beeper_horn3,
	hdc_beeper_button,
  
	hdc_beeper_end,
}hdc_beep_png_t;

typedef struct{
	char *pic_name;      // 图片名
	int w;               // 图片宽
	int h;               // 图片高
	hdc_beep_png_t id;   // hdc 编号
}beeper_hdc_t;

extern HDC hdc_beeper_bk;
extern HDC hdc_beeper_png[hdc_beeper_end];
/********************************************************************************************
 *                             蜂鸣器控制图片  END                                          *
 ********************************************************************************************/


/********************************************************************************************
*                             LED&KEY 图片                                                  *
********************************************************************************************/
#define GUI_LED_KEY_OFF_PIC       "LED_KEY0.png"
#define GUI_LED_KEY_1_PIC         "LED_KEY1.png"
#define GUI_LED_KEY_2_PIC         "LED_KEY2.png"
#define GUI_LED_KEY_3_PIC         "LED_KEY3.png"
typedef enum
{
  hdc_led_key_off_btn = 0,
  hdc_led_key_1_btn,
	hdc_led_key_2_btn,
	hdc_led_key_3_btn,
	
  hdc_led_key_end,
}hdc_led_key_png_t;

typedef struct{
	char *pic_name;      // 图片名
	int w;               // 图片宽
	int h;               // 图片高
	hdc_led_key_png_t id;        // hdc 编号
}led_key_hdc_t;

extern HDC hdc_led_key_png[hdc_led_key_end];
/********************************************************************************************
*                             LED&KEY 图片   END                                            *
********************************************************************************************/

BOOL PIC_Load_To_SDRAM(void);

#endif

