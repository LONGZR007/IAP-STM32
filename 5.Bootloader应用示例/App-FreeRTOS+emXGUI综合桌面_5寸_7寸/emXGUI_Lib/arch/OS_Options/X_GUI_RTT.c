/**
  *********************************************************************
  * @file    X_GUI_RTT.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   emXGUI的RT-Thread操作系统接口
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
  
/* 在工程选项中定义这个宏，以设置使用的操作系统 */ 
#ifdef	X_GUI_USE_RTTHREAD

#include	<stddef.h>
#include	"emXGUI_Arch.h"

#include  	"rtthread.h"


/*===================================================================================*/
/*
函数功能: 创建一个互斥(该互斥锁必须支持嵌套使用)
返回: 互斥对象句柄(唯一标识)
说明: 互斥对象句柄按实际OS所定,可以是指针,ID号等...
*/
GUI_MUTEX*	GUI_MutexCreate(void)
{
	return (GUI_MUTEX*)rt_mutex_create(NULL,RT_IPC_FLAG_FIFO);
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
	
	if(rt_mutex_take((rt_mutex_t)hMutex,rt_tick_from_millisecond(time))==RT_EOK)
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
	rt_mutex_release((rt_mutex_t)hMutex);
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
	rt_mutex_delete((rt_mutex_t)hMutex);
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
	return (GUI_SEM*)rt_sem_create(NULL,init,RT_IPC_FLAG_FIFO);
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

	if(rt_sem_take((rt_sem_t)hsem,rt_tick_from_millisecond(time))== RT_EOK)
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
	rt_sem_release((rt_sem_t)hsem);
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
	rt_sem_delete((rt_sem_t)hsem);
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
	return	(HANDLE)rt_thread_self();
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
	
	i=rt_tick_get();
	
	return (i*1000)/RT_TICK_PER_SECOND;

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
	//rt_thread_yield();
	rt_thread_delay(2);
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
	ms=rt_tick_from_millisecond(ms);
	rt_thread_delay(ms);
}

/**
 * @breif: 创建线程
 * @param name 线程名
 * @param entry 线程入口函数
 * @param parameter 线程参数
 * @param stack_size 线程栈大小（单位字节，注意部分系统需要进行单位转换）
 * @param priority 线程优先级
 * @param tick 时间片（同优先级任务的时间片轮转）
 * @return 是否创建成功
*/
BOOL GUI_Thread_Create(void (*entry)(void *parameter),
                         const char *name,
                         u32  stack_size,
                         void *parameter,
                         u32  priority,
                         u32  tick)
{
  rt_thread_t h;

  h=rt_thread_create((const char *)name,           /* 任务名字 */
                        (void (*)(void *))entry,    /* 任务入口函数 */
                        (void*          )parameter,  /* 任务入口函数参数 */
                        (rt_uint32_t)stack_size,    /* 任务栈大小 */
                        (rt_uint8_t)priority,       /* 任务的优先级 */
                        (rt_uint32_t)tick);         /* 时间片，此处默认分配 */
  
  rt_thread_startup(h);		
  
  if(h == RT_NULL)
  {
    GUI_ERROR("GUI Thread Create failed:%s",name);
    return FALSE;
  }
  else
    return TRUE;
}

/**
 * @breif: 删除线程，可通过GUI_GetCurThreadHandle获取当前任务句柄作为输入参数
 * @return 无
*/
void GUI_Thread_Delete(HANDLE thread)
{
  rt_thread_delete((rt_thread_t) thread);
}

#endif

/********************************END OF FILE****************************/

