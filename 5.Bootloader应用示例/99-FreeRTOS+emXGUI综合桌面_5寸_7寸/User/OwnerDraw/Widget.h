#ifndef _EXITBUTTON_H_
#define _EXITBUTTON_H_

#include <emXGUI.h>


typedef enum
{
   dir_V,//垂直
   dir_H //水平
}dir_e;

//控件图标管理结构体
typedef struct{
   char *icon_name;//图标名
   RECT rc;        //位置信息
   BOOL state;     //状态  
}icon_S;

typedef struct
{
   dir_e dir;//滑动条方向
   int width;//只用于垂直滑动条
   int high;//只用于水平滑动条
   COLOR_RGB32 back_c;//背景色
   
   COLOR_RGB32 page_c;//非滑块区域颜色
   COLOR_RGB32 fore_c;//滑块颜色
 
}ScrollBar_S;


extern void home_owner_draw(DRAWITEM_HDR *ds);
extern void scrollbar_owner_draw(DRAWITEM_HDR *ds, ScrollBar_S ScrollBar_parm1, ScrollBar_S ScrollBar_parm2);
#endif /* _EXITBUTTON_H_ */

