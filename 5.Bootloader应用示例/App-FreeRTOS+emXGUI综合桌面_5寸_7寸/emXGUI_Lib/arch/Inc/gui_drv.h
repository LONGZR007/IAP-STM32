/**
  *********************************************************************
  * @file    gui_drv.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   总的驱动头文件
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#ifndef	__GUI_DRV_H__
#define	__GUI_DRV_H__


#include <emXGUI.h>
#include <emXGUI_Arch.h>
#include	<stddef.h>
#include	<math.h>

#include "gui_drv_cfg.h"
#include "gui_mem_port.h"
#include "gui_lcd_port.h"
#include "gui_font_port.h"
#include "gui_log_port.h"

#if(GUI_INPUT_DEV_EN)
  #include "gui_input_port.h"
#endif

#if(GUI_RES_DEV_EN)
  #include "gui_resource_port.h"
#endif

#if(GUI_FS_EN)
  #include "gui_fs_port.h"
#endif

#ifndef	MIN
#define	MIN(a,b)	(((a)<(b))?(a):(b))
#endif

#ifndef	MAX
#define	MAX(a,b)	(((a)>(b))?(a):(b))
#endif


/*============================================================================*/


/*============================================================================*/

#endif
