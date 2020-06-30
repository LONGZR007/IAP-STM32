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

/*============================================================================*/


/*========================触摸接口部分===================================*/
/**
  * @brief  触摸初始化接口,会被gui_input_port.c文件的GUI_InputInit函数调用
  * @note  需要在本函数初始化触摸屏相关硬件
  * @retval 是否初始化正常
  */
BOOL TouchDev_Init(void)
{	
  /* 初始化配套的5/7寸屏 */
  if(GTP_Init_Panel() == 0)
    return TRUE;
  else
    return FALSE;    
}


/**
  * @brief  获取触摸状态及坐标，不需要用户修改 
  * @note  本函数依赖GTP_Execu接口，该接口需要返回触摸坐标和是否被按下的状态，
  *        本例子在bsp_touch_gt9xx.c文件实现
  * @param  pt[out] 存储获取到的x y坐标
  * @retval 触摸状态
  *    @arg  TS_ACT_DOWN  触摸按下
  *    @arg  TS_ACT_UP    触摸释放
  *    @arg  TS_ACT_NONE  无触摸动作
  */
BOOL TouchDev_GetPoint(POINT *pt)
{
  static int ts_state=TS_ACT_NONE;

  /* 通过GTP_Execu获取触摸坐标和状态 */
	if(GTP_Execu(&pt->x,&pt->y) > 0)
	{
			ts_state =TS_ACT_DOWN;
	}
	else
	{
		if(ts_state==TS_ACT_DOWN)
		{
			ts_state =TS_ACT_UP;
		}
		else
		{
			ts_state =TS_ACT_NONE;
		}
	}
	return ts_state;

}

/**
  * @brief  需要被定时调用的触摸处理函数
  * @note   本例子中通过gui_input_port.c文件的GUI_InputHandler被定时调用
  * @param  无
  * @retval 无
  */
void	GUI_TouchHandler(void)
{
	int act;
	POINT pt;

  /* 判断触摸状态及坐标 */
	act =TouchDev_GetPoint(&pt);
	if(act==TS_ACT_DOWN)
	{
    /* 触摸按下，使用触摸坐标作为输入 */
		MouseInput(pt.x,pt.y,MK_LBUTTON);
	}

	if(act==TS_ACT_UP)
	{
    /* 触摸释放，使用当前光标作为输入*/
		GetCursorPos(&pt);
		MouseInput(pt.x,pt.y,0);
	}
}


/********************************END OF FILE****************************/

