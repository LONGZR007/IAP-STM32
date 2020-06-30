/**
  *********************************************************************
  * @file    X_GUI_FreeRTOS.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI的FreeRTOS操作系统接口
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
/* 在工程选项中定义这个宏，以设置使用的操作系统 */  
#ifdef	X_GUI_USE_FREERTOS

#include	<stddef.h>
#include	"emXGUI_Arch.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/*===================================================================================*/
/*
函数功能: 创建一个互斥(该互斥锁必须支持嵌套使用)
返回: 互斥对象句柄(唯一标识)
说明: 互斥对象句柄按实际OS所定,可以是指针,ID号等...
*/
GUI_MUTEX*	GUI_MutexCreate(void)
{
	return (GUI_MUTEX*)xSemaphoreCreateRecursiveMutex();	
}

/*===================================================================================*/
/*
函数功能: 互斥锁定
参数: hMutex(由GUI_MutexCreate返回的句柄); 
      time 最长等待毫秒数,0立既返回,0xFFFFFFFF,一直等待
返回: TRUE:成功;FALSE:失败或超时
说明: .
*/
BOOL	GUI_MutexLock(GUI_MUTEX *hMutex,U32 time)
{
	if(xSemaphoreTakeRecursive((SemaphoreHandle_t)hMutex,pdMS_TO_TICKS(time))==pdTRUE)
	{
		return TRUE;
	}
	return	FALSE;
}

/*===================================================================================*/
/*
函数功能: 互斥解锁
参数: hMutex(由GUI_MutexCreate返回的句柄);    
返回: 无
说明: .
*/
void	GUI_MutexUnlock(GUI_MUTEX *hMutex)
{
	 xSemaphoreGiveRecursive((SemaphoreHandle_t)hMutex);
}

/*===================================================================================*/
/*
函数功能: 互斥删除
参数: hMutex(由GUI_MutexCreate返回的句柄);    
返回: 无
说明: .
*/
void	GUI_MutexDelete(GUI_MUTEX *hMutex)
{
	vSemaphoreDelete((SemaphoreHandle_t)hMutex);
}

/*===================================================================================*/
/*
函数功能: 创建一个信号量
参数: init: 信号量初始值; max: 信号量最大值
返回: 信号量对象句柄(唯一标识)
说明: 信号量对象句柄按实际OS所定,可以是指针,ID号等...
*/
GUI_SEM*	GUI_SemCreate(int init,int max)
{
	return (GUI_SEM*)xSemaphoreCreateCounting(max,init);
}

/*===================================================================================*/
/*
函数功能: 信号量等待
参数: hsem(由GUI_SemCreate返回的句柄); 
      time 最长等待毫秒数,0立既返回,0xFFFFFFFF,一直等待
返回: TRUE:成功;FALSE:失败或超时
说明: .
*/
BOOL	GUI_SemWait(GUI_SEM *hsem,U32 time)
{

	if(xSemaphoreTake((SemaphoreHandle_t)hsem,pdMS_TO_TICKS(time))== pdTRUE)
	{
		return TRUE;
	}
	return FALSE;
}

/*===================================================================================*/
/*
函数功能: 信号量发送
参数: hsem(由GUI_SemCreate返回的句柄);  
返回: 无
说明: .
*/
void	GUI_SemPost(GUI_SEM *hsem)
{
	xSemaphoreGive((SemaphoreHandle_t)hsem);
}
/*
函数功能: 信号量发送(受freertos管理的中断)
参数: hsem(由GUI_SemCreate返回的句柄);  
返回: 无
说明: 若在受freertos管理的中断中调用GUI_SemPost,会导致port.c：425
*/
void GUI_SemPostISR(GUI_SEM *hsem)
{
   
  BaseType_t pxHigherPriorityTaskWoken;
  
  
	xSemaphoreGiveFromISR((SemaphoreHandle_t)hsem, &pxHigherPriorityTaskWoken);
  
  portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}
/*===================================================================================*/
/*
函数功能: 信号量删除
参数: hsem(由GUI_SemCreate返回的句柄);    
返回: 无
说明: .
*/
void	GUI_SemDelete(GUI_SEM *hsem)
{
	vSemaphoreDelete((SemaphoreHandle_t)hsem);
}

/*===================================================================================*/
/*
函数功能: 获得当前线程句柄(唯一标识)
参数: 无  
返回: 当前线程唯一标识,按实际OS所定,可以是指针,ID号等...
说明: .
*/
HANDLE	GUI_GetCurThreadHandle(void)
{
	return	(HANDLE)xTaskGetCurrentTaskHandle();
}


/*===================================================================================*/
/*
函数功能: 获得当前系统时间(单位:毫秒)
参数: 无  
返回: 当前系统时间
说明: .
*/
U32	GUI_GetTickCount(void)
{
	U32 i;
	
	i=xTaskGetTickCount();
	
	return (i*1000)/configTICK_RATE_HZ;

}

/*===================================================================================*/
/*
函数功能: 最短时间内让出CPU
参数: 无  
返回: 无
说明: 按具体OS情况而定,最简单的方法是:OS Delay 一个 tick 周期.
*/
void	GUI_Yield(void)
{
	vTaskDelay(2);
}

/*===================================================================================*/
/*
函数功能: 延时函数
参数: ms: 延时时间(单位:毫秒) 
返回: 无
说明: 
*/
void	GUI_msleep(u32 ms)
{
	vTaskDelay(pdMS_TO_TICKS(ms));
}

/*
 * 函数功能: 创建线程
 * @param name 线程名
 * @param entry 线程入口函数
 * @param parameter 线程参数
 * @param stack_size 线程栈大小（单位字节，注意部分系统需要进行单位转换）
 * @param priority 线程优先级
 * @param tick FreeRTOS没这个功能，时间片（同优先级任务的时间片轮转）
 * @return 是否创建成功
*/
BOOL GUI_Thread_Create(void (*entry)(void *parameter),
                         const char *name,
                         u32  stack_size,
                         void *parameter,
                         u32  priority,
                         u32  tick)
{
   BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */

   xReturn = xTaskCreate((TaskFunction_t )entry,              /* 任务入口函数 */
                            (const char*    )name,            /* 任务名字 */
                            (uint16_t       )stack_size/4,    /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )NULL,            /* 任务入口函数参数 */
                            (UBaseType_t    )priority,        /* 任务的优先级 */
                            (TaskHandle_t*  )NULL);           /* 任务控制块指针 */
                            
  if(xReturn == pdPASS )
    return TRUE;
  else
  {
    GUI_ERROR("GUI Thread Create failed:%s",name);
    return FALSE; 
  }    
}

/**
 * @breif: 删除线程，可通过GUI_GetCurThreadHandle获取当前任务句柄作为输入参数
 * @return 无
*/
void GUI_Thread_Delete(HANDLE thread)
{
  vTaskDelete((TaskHandle_t) thread);
}
#endif

/********************************END OF FILE****************************/

