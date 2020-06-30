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
#include "gui_touch_port.h"
#include "gui_input_port.h"
/*============================================================================*/
GUI_SEM *Input_Sem;
/**
  * @brief  GUI输入设备的初始化
  * @param  无
  * @retval 是否初始化正常
  */
BOOL GUI_InputInit(void)
{
  BOOL state = FALSE;
  #if(GUI_KEYBOARD_EN)
  {
   state = KeyBoardDev_Init();
  }
  #endif

  #if(GUI_TOUCHSCREEN_EN)
  {
    Input_Sem = GUI_SemCreate(0, 1);
    state = TouchDev_Init();
    if(state) /*触摸屏设备初始化*/
    {
    #if(GUI_TOUCHSCREEN_CALIBRATE)
        TS_CFG_DATA ts_cfg;
        if(TouchDev_LoadCfg(&ts_cfg)) /*加载校正数据(电阻屏需要)*/
        {
          TouchPanel_TranslateInit(&ts_cfg); /*初始化坐标转换函数(电阻屏需要)*/
        }
    #endif
    }

  }
  #endif

  #if(GUI_MOUSE_EN)
  {
    state = MouseDev_Init();
  }
  #endif
  return state;
}

/*===================================================================================*/

/**
  * @brief  GUI输入设备的定时处理函数
  * @note   该函数需要被定时执行，
  *         如使用独立的线程调用 或 在桌面的定时器消息中调用
  *         
  * @param  无
  * @retval 无
  */
void GUI_InputHandler(void)
{

  #if(GUI_KEYBOARD_EN)
  {
    if(KeyBoardDev_IsActive())
    {
      KeyBoardDev_Handler();
    }
  }
  #endif

  #if(GUI_TOUCHSCREEN_EN)
  {
    GUI_TouchHandler(); //调用触摸屏处理函数
  }
  #endif

  #if(GUI_MOUSE_EN)
  {
    if(MouseDev_IsActive())
    {
      MouseDev_Handler();
    }
  }
  #endif

}

/********************************END OF FILE****************************/

