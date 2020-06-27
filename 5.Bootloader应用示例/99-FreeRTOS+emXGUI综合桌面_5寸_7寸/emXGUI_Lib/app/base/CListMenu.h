
#ifndef	__CLISTMENU__
#define	__CLISTMENU__

#ifdef	__cplusplus
extern "C" {
#endif
    /*============================================================================*/

#include "emXGUI.h"

/*============================================================================*/
////ListMenu 消息
    enum	eMSG_LISTMENU {
        MSG_SETCFG = WM_WIDGET + 0,
        MSG_MOVE_PREV,
        MSG_MOVE_NEXT,
		MSG_SET_SEL,              // [设置选中的对象]: <wParam>设置被选中的索引值; <lParam>忽略; <返回>忽略.
	MSG_LIST_DRAG_ENABLE,     // [列表滑动使能]: <wParam>TRUE:使能;FALSE：禁止; <lParam>忽略; <返回>忽略.
		MSG_LISTMENU_END,
    };


    ////ListMenu 私有风格标志(只能用低16位)
#define	LMS_HOR			(1<<0) //水平滑动的
#define	LMS_VER			(1<<1) //垂直滑动的
#define	LMS_ICONFRAME	(1<<2) //图标带框
#define	LMS_PAGEMOVE	(1<<3) //按整页移动.

#define	LMS_ICONTITLE	(1<<4) //图标带标题
#define	LMS_ICONINNERFRAME	(1<<5) //图标带内框
#define LMS_TOUCHSHADOW	(1<<6) //触摸时是否带阴影

////ListMenu 通知码
#define	LMN_CLICKED   0 //单击
#define	LMN_SELCHANGE 1

    typedef struct {
        NMHDR hdr;
        int idx;
    }LM_NMHDR;


    struct	__obj_list {

        /* APP下方的文字 */
        const WCHAR *pName;
        /* 图标的bmp数据内容，bmp为NULL时使用icon字符索引 */
        void *bmp;
        /* 图标的字符索引，bmp非空时本配置无效 */
        void *icon;
        /* 图标使用的颜色 */
        u32 color;
        /* 图标对应的应用程序 */
        void(*cbStartup)(void*);
    };

    typedef struct
    {

        const struct __obj_list *list_objs;
        int x_num;
        int y_num;
        u32 bg_color; /* 背景颜色 */

    }list_menu_cfg_t;



    extern const WNDCLASS wcex_ListMenu;

    /*============================================================================*/
    /*============================================================================*/
    /*============================================================================*/
    /*============================================================================*/
#ifdef	__cplusplus
}
#endif
#endif /*__CLISTMENU__*/
