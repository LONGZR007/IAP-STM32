
#if 1
//#ifdef	X_GUI_USE_POSIX
/*===================================================================================*/

#include <semaphore.h>
#include <sys/times.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>

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

	pthread_mutexattr_t attr;
	pthread_mutex_t *mutex = GUI_MEM_Alloc(sizeof(pthread_mutex_t));

	if(mutex!=NULL)
	{
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(mutex, &attr);
    
		pthread_mutexattr_destroy(&attr);
	}
	return	(GUI_MUTEX*)mutex;
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
	pthread_mutex_lock(hMutex);
	
	return	TRUE;
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
	pthread_mutex_unlock(hMutex);
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
	pthread_mutex_destroy(hMutex);
	
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
	sem_t *sem = GUI_MEM_Alloc(sizeof(sem_t));;
	
	if(sem!=NULL)
	{
		sem_init(sem, 0, init);
	}
	
	return	(GUI_SEM*)sem;
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
	sem_wait(hsem);
	
	return TRUE;
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
	sem_post(hsem);
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
	sem_destroy(hsem);
	
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
	return	(HANDLE)pthread_self();
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
    struct timeval time;

    gettimeofday(&time, NULL);
    
	return	 time.tv_sec*1000+ time.tv_usec/1000;
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
	msleep(1);
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
	msleep(ms);
}


/*===================================================================================*/
#endif
