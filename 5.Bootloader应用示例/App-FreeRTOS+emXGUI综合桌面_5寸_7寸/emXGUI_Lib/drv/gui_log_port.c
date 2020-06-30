/**
  *********************************************************************
  * @file    gui_input_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   输入设备驱动接口
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
#include "board.h"
#include "gui_drv.h"
#include "gui_drv_cfg.h"
#include "gui_log_port.h"



/*============================================================================*/
/**
  * @brief  初始化GUI日志接口
  * @param  无
  * @retval 是否初始化正常
  */
BOOL	GUI_Log_Init(void)
{
  /* 使用串口作为日志输出接口 */
  /* 初始化串口 */
  Debug_USART_Config();
  return TRUE;
}



/**
  * @brief  格式化字符串输出
  * @param  可变参数
  * @note   如果不用输出GUI调试信息,这个函数可以为空
  * @retval 无
  */
void	GUI_Printf(const char *fmt,...)
{
#if 0
    x_va_list ap;
    char *buf;
    static	char str_buf[256];
    static  GUI_MUTEX *mutex_gui_printf=NULL;
    ////
    buf	=str_buf/*(char*)GUI_MEM_Alloc(256)*/;
    if(buf)
    {

      if(GUI_MutexLock(mutex_gui_printf,0xFFFFFFFF))
      {
          x_va_start(ap,fmt);
          x_vsprintf(buf,fmt,ap);
          /* 底层需要提供DebugPuts接口进行输出 */
          DebugPuts(buf);
          //DebugOutput(buf);

          GUI_MutexUnlock(mutex_gui_printf);
          x_va_end(ap);
      }

    //GUI_MEM_Free(buf);
  }
#else

//  rt_kprintf(fmt);  
#endif   
  
}


///**
//  * @brief  格式化字符串输出
//  * @param  可变参数
//  * @note   如果不用输出GUI调试信息,这个函数可以为空
//  * @retval 无
//  */
//void	GUI_Log(const char *fmt,...)
//{
//  rt_kprintf(fmt);
//}
/********************************END OF FILE****************************/

