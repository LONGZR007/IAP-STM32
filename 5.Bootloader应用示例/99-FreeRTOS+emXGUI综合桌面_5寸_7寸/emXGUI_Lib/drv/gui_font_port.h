#ifndef __GUI_FONT_PORT_H
#define	__GUI_FONT_PORT_H

#ifdef __cplusplus
extern "C" {
#endif 
  
#include	"emXGUI.h"
#include	"gui_drv_cfg.h"
  
  /** 
  * @brief  字体参数
  *  为适应不同分辨率的屏幕，使用不同的默认字体 
  */   
typedef struct
{
  const char *default_en;  //默认英文字体
  const char *default_extern_cn;  //默认中文字体
  const char *default_extern_logo100;  //LOGO字体

}FONT_PARAM_TypeDef;

/* 字体参数配置 */
extern const FONT_PARAM_TypeDef gui_font_param[];
extern HFONT logoFont100;
/* 默认字体 */
extern HFONT defaultFont;
/* 默认英文字体 */
extern HFONT defaultFontEn;

/* logo字体 */
extern HFONT logoFont;
/* 图标字体 */
extern HFONT iconFont_100;
extern HFONT iconFont_252;
/* 控制图标字体 */
extern HFONT controlFont_48;
extern HFONT controlFont_64;
extern HFONT controlFont_72;
extern HFONT controlFont_32;
extern HFONT controlFont_100;
/* 其它 */
extern HFONT GB2312_32_Font;
extern HFONT iconFont_200;
extern HFONT logoFont_200;
  
/* 用于标记是否有资源文件无法找到 */
extern BOOL res_not_found_flag;

HFONT GUI_Default_FontInit(void);
HFONT GUI_Init_Extern_Font_Stream(const char* res_name);
HFONT GUI_Init_Extern_Font_2RAM(const char* res_name,u8** buf);
HFONT GUI_Init_Extern_Font(void);
  
#ifdef	__cplusplus
}
#endif

#endif /* __GUI_FONT_PORT_H */
