/**
  *********************************************************************
  * @file    gui_mem_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   动态内存管理接口
  *          
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 
#include "x_libc.h" // x_libc里，提供了通用的内存堆管理函数(x_heap)...
#include "board.h"
#include "gui_drv.h"
#include	"emXGUI_Arch.h"
#include "gui_mem_port.h"


/**
* @note  vmem/vmalloc/vfree 提供给应用程序自由使用，
*        通常使用一块较大的外部SRAM、SDRAM空间进行分配。
*        
*        GUI_MEM_Init/Alloc/Free 提供给GUI内核对象使用，
*        通常使用控制器的内部SRAM进行分配。
*
*        GUI_GRAM_Alloc/Free 在申请液晶显存空间的时候使用，
*        本例子显存空间固定，所以没有用到
*
*/

#if(GUI_VMEM_EN)    
/* VMEM内存管理 */
/* 互斥信号量 */
static GUI_MUTEX *mutex_vmem = NULL;

/* 内存堆管理句柄 */
heap_t heap_vmem;

/* VMEM缓冲区 */
static uint8_t buff_vmem[VMEM_SIZE] __attribute__((at(VMEM_BASE)));
#endif

/* GUI_CORE内存管理 */
//static GUI_MUTEX *mutex_core_mem = NULL;
//static	heap_t heap_core_mem;
//static uint8_t buff_core_mem[GUI_CORE_MEM_SIZE] __attribute__((at(GUI_CORE_MEM_BASE)));


/**
  * @brief  创建一个内存堆
  * @note  使用vmalloc前必须调用本函数初始化内存堆句柄
  * @retval 无
  */
void GUI_VMEM_Init(void)
{
#if(GUI_VMEM_EN)      
	mutex_vmem = GUI_MutexCreate();
	x_heap_init(&heap_vmem,
                (void*)buff_vmem,
                  VMEM_SIZE,
                  VMEM_ALLOC_UNIT);	 /* 创建一个内存堆 */
#endif
}
  
/**
* @brief  从内存堆里申请空间
* @param  size 要申请的内存大小
* @retval 申请到的内存指针
*/
void* GUI_VMEM_Alloc(u32 size)
{
#if(GUI_VMEM_EN)      

	u8 *p;
	
	GUI_MutexLock(mutex_vmem,5000);
	p =x_heap_alloc(&heap_vmem,size);
	GUI_MutexUnlock(mutex_vmem);
  if(p==NULL)
	{
	    GUI_ERROR("GUI_VMEM_Alloc,no enough space(for %d byte)",size);
	}
  
	return p;
#endif
}
  
/**
* @brief  释放内存
* @param  p:需要释放的内存首址
* @retval 无
*/
void GUI_VMEM_Free(void *p)
{
#if(GUI_VMEM_EN)      
	GUI_MutexLock(mutex_vmem,5000);
	x_heap_free(&heap_vmem,p);
	GUI_MutexUnlock(mutex_vmem);
#endif

}

/**
  * @brief  创建一个内存堆（用于GUI内核对象）,可参考vmem配置
  * @retval 无
  */
void	GUI_MEM_Init(void)
{
  /* 本示例中的GUI内核对象使用 pvPortMalloc ，
    它已由FreeRTOS系统初始化*/

	return ;
}

/* OS_MALLOC和 OS_FREE在gui_os_port.h中定义*/
/**
* @brief  动态内存申请(用于GUI内核对象)
* @param  size 要申请的内存大小
* @retval 申请到的内存指针
*/
void*	GUI_MEM_Alloc(U32 size)
{
	void *p=NULL;
	p =OS_MALLOC(size);
	if(p==NULL)
	{
	    GUI_ERROR("GUI_MEM_Alloc,no enough space(for %d byte)",size);
	}

	return p;
}

/*===================================================================================*/

/**
* @brief  释放内存(用于GUI内核对象)
* @param  p:需要释放的内存首址
* @retval 无
*/
void	GUI_MEM_Free(void *p)
{
	OS_FREE(p);
}

/*===================================================================================*/

/**
* @brief  显示动态内存申请(用于GUI显示器缓存)
* @param  size 要申请的内存大小
* @retval 申请到的内存指针
*/
void*	GUI_GRAM_Alloc(U32 size)
{
	return GUI_VMEM_Alloc(size);
}

/*===================================================================================*/

/**
* @brief  显示动态内存申请(用于GUI显示器缓存)
* @param  p:需要释放的内存首址
* @retval 无
*/
void	GUI_GRAM_Free(void *p)
{
	GUI_VMEM_Free(p);
}


void* vmalloc(int size)
{
	return GUI_VMEM_Alloc(size);
}

void vfree(void *p)
{
	GUI_VMEM_Free(p);
}

/********************************END OF FILE****************************/

