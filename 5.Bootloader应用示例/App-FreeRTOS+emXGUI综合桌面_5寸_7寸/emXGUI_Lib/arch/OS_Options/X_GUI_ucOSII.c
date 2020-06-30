
#if 1
//#ifdef	X_GUI_USE_UCOSII

/*===================================================================================*/
#include	<stddef.h>

#include "ucos_ii.h"

#include	"emXGUI.h"
#include	"emXGUI_Arch.h"


struct tagGUI_MUTEX
{
	OS_EVENT *event;
	OS_TCB *tcb;
	U32 hold;
};

/*===================================================================================*/

static U32 ms_to_tick(U32 ms)
{
	return (ms*OS_TICKS_PER_SEC)/1000;
}

/*===================================================================================*/
/*
函数功能: 创建一个互斥(该互斥锁必须支持嵌套使用)
返回: 互斥对象句柄(唯一标识)
说明: 互斥对象句柄按实际OS所定,可以是指针,ID号等...
*/

GUI_MUTEX*	GUI_MutexCreate(void)
{

	GUI_MUTEX *m;
	
	m =(GUI_MUTEX*)GUI_MEM_Alloc(sizeof(GUI_MUTEX));

	m->event =OSSemCreate(1);
	m->tcb =OSTCBCur;
	m->hold  =0;
	
	return	m;
	
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
	INT8U err=OS_ERR_NONE;
	
	if(time==0UL)
	{
		time=0xFFFFFFFE;
	}
	else if(time==(U32)0xFFFFFFFF)
	{
		time =0;
	}
	else
	{
		time =ms_to_tick(time);
	}
	
	if(OSTCBCur == hMutex->tcb)
	{	
		if(hMutex->hold++ == 0)
		{
			OSSemPend(hMutex->event,time,&err);
		}
	}
	else
	{
		OSSemPend(hMutex->event,time,&err);
		hMutex->tcb =OSTCBCur;
		hMutex->hold =1;
	}

	return TRUE;

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
	if(OSTCBCur == hMutex->tcb)
	{
		if(hMutex->hold > 0)
		{
			if(--hMutex->hold == 0)
			{
				OSSemPost(hMutex->event);
			}
		}
	}
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
	INT8U err;
	OSSemDel(hMutex->event,OS_DEL_ALWAYS,&err);
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
	OS_EVENT *sem;
	
	sem =OSSemCreate(init);
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
	INT8U err;
	
	if(time==0UL)
	{
		time=0xFFFFFFFE;
	}
	else if(time==(U32)0xFFFFFFFF)
	{
		time =0;
	}
	else
	{
		time =ms_to_tick(time);
	}

	OSSemPend((OS_EVENT*)hsem,time,&err);

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
	OSSemPost((OS_EVENT*)hsem);
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
	INT8U err;
	OSSemDel((OS_EVENT*)hsem,OS_DEL_ALWAYS,&err);
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
	return	(HANDLE)OSTCBCur;
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
	return (OSTime*1000)/OS_TICKS_PER_SEC;
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
	OSTimeDly(1);
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
	if(ms==0)
	{
		ms=1;
	}
	OSTimeDlyHMSM(0,0,0,ms);
}


/*===================================================================================*/
#endif
