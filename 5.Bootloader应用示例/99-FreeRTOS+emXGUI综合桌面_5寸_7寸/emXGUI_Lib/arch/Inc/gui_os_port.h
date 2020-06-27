/**
  *********************************************************************
  * @file    gui_os_port.h
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   不同系统的接口
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#ifndef	__GUI_OS_PORT_H__
#define	__GUI_OS_PORT_H__

/* 在工程选项中定义以下的宏，以设置使用的操作系统 */

//X_GUI_USE_RTTHREAD
//X_GUI_USE_FREERTOS

#ifdef	X_GUI_USE_RTTHREAD
  /* RT-Thread 系统头文件 */ 
  #include	"rtthread.h"
  
  /* 系统管理的内存堆，gui_mem_port.c */
  #define OS_MALLOC   rt_malloc
  #define OS_FREE     rt_free
    
  /* 调试或错误、信息输出接口 gui_log_port.h */
  #define   GUI_Log     rt_kprintf

#elif X_GUI_USE_FREERTOS
  /* FreeRTOS 系统头文件 */ 
  #include <stdio.h>
  #include "FreeRTOS.h"
  #include "task.h"  
  
  /* 系统管理的内存堆，gui_mem_port.c */
  #define OS_MALLOC   pvPortMalloc
  #define OS_FREE     vPortFree
    
  /* 调试或错误、信息输出接口 gui_log_port.h */
  #define   GUI_Log     printf
  
#else
  /* 没定义的错误提示 */
  #error No OS type select,please define macro 'X_GUI_USE_RTTHREAD'/'X_GUI_USE_FREERTOS' or else.
#endif



#endif
