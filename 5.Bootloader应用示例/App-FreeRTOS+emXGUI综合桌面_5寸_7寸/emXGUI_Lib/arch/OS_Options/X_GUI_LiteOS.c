
#if 1
//#ifdef	X_GUI_USE_LITEOS

/*===================================================================================*/
#include	<stddef.h>

#include "los_sys.h"
#include "los_tick.h"
#include "los_mux.h"
#include "los_sem.h"

#include	"emXGUI.h"
#include	"emXGUI_Arch.h"

/*===================================================================================*/
/*
函数功能: 创建一个互斥(该互斥锁必须支持嵌套使用)
返回: 互斥对象句柄(唯一标识)
说明: 互斥对象句柄按实际OS所定,可以是指针,ID号等...
*/

GUI_MUTEX*	GUI_MutexCreate(void)
{
	UINT32 mutex=0;

	if(LOS_MuxCreate(&mutex)==LOS_OK)
	{
		return	(GUI_MUTEX*)mutex;
	}
	return NULL;
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
	if(LOS_MuxPend((UINT32)hMutex,time)==LOS_OK)
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
	LOS_MuxPost((UINT32)hMutex);
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
	LOS_MuxDelete((UINT32)hMutex);
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
	UINT32 sem=0;
	
	if(LOS_SemCreate(init,&sem)==LOS_OK)
	{
		return	(GUI_SEM*)sem;
	}
	return NULL;
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
	if(LOS_SemPend((UINT32)hsem,time)== LOS_OK)
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
	LOS_SemPost((UINT32)hsem);
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
	LOS_SemDelete((UINT32)hsem);
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
	return	(HANDLE)LOS_CurTaskIDGet();
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
	UINT32 ticks;
	
	ticks=LOS_TickCountGet();
	
	return	LOS_Tick2MS(ticks);
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
	LOS_Msleep(1);
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
	LOS_Msleep(ms);
}


/*===================================================================================*/
#endif
