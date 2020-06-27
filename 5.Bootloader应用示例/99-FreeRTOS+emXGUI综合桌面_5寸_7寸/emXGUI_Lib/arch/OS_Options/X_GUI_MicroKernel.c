#if 1
//#ifdef	X_GUI_USE_MICROKernel

#include	<stddef.h>
#include	"emXGUI_Arch.h"
#include 	"MicroKernel.h"

/*===================================================================================*/
/*
函数功能: 创建一个互斥
返回: 互斥对象句柄(唯一标识)
说明: 互斥对象句柄按实际OS所定,可以是指针,ID号等...
*/

GUI_MUTEX*	GUI_MutexCreate(void)
{
	mk_mutex_t *mutex;
	
	mutex =GUI_MEM_Alloc(sizeof(mk_mutex_t));
	mk_mutex_init(mutex,NULL);
	return	(HANDLE)mutex;
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
	if(	mk_mutex_lock(hMutex,time)==MK_ERR_NONE)
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
	mk_mutex_unlock(hMutex);
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
	mk_mutex_destroy(hMutex);
	GUI_MEM_Free(hMutex);
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
	mk_sem_t *sem;
	
	sem =GUI_MEM_Alloc(sizeof(mk_sem_t));
	mk_sem_init(sem,init,max,NULL);
	return	(HANDLE)sem;
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
	if(mk_sem_wait(hsem,time) == MK_ERR_NONE)
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
	mk_sem_post(hsem);
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
	mk_sem_destroy(hsem);
	GUI_MEM_Free(hsem);
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
	return	(HANDLE)mk_thread_self();
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
	return	mk_get_time_count(NULL);
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
	mk_sleep(1);
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
	mk_sleep(ms);
}

/*===================================================================================*/

#endif
