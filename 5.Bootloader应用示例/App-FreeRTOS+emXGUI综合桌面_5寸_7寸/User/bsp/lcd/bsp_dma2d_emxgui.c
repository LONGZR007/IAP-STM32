

//#include "BSP.h"
#include "gui_drv_cfg.h"

#if DMA2D_EN
#include "stm32f4xx_dma2d.h"
#include "./lcd/bsp_dma2d_emxgui.h"
#include "FreeRTOS.h"
/*=========================================================================================*/
 
/*=========================================================================================*/

static GUI_SEM *sem_rdy=NULL;
static volatile int dma2d_rdy=FALSE;

void DMA2D_IRQHandler(void)
{
//	uint32_t pend;
  
  BaseType_t pxHigherPriorityTaskWoken;

  /* 进入中断 */
  uint32_t ulReturn;
 /* 进入临界段，临界段可以嵌套 */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
	DMA2D_ClearFlag(DMA2D_FLAG_TC);//清除传输完成中断
	//DMA2D_ClearITPendingBit(DMA2D_IT_TC);

	dma2d_rdy=TRUE;
  
  xSemaphoreGiveFromISR(sem_rdy,&pxHigherPriorityTaskWoken);
  
  //如果需要的话进行一次任务切换，系统会判断是否需要进行切换
  portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);

  /* 离开中断 */
  /* 退出临界段 */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}


/*=========================================================================================*/

void DMA2D_DrvInit(void)
{
//	DMA2D_InitTypeDef      DMA2D_InitStruct;
//	DMA2D_FG_InitTypeDef   DMA2D_FG_InitStruct;

	sem_rdy =GUI_SemCreate(0,1);
	/* Enable the DMA2D Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);

	/* DMA2D configuration */
	DMA2D_DeInit();

#if 1
	DMA2D_ITConfig(DMA2D_IT_TC,ENABLE);

	DMA2D_ClearFlag(DMA2D_FLAG_TC);//清除传输完成中断
	DMA2D_ClearITPendingBit(DMA2D_IT_TC);

	if(1)
	{
		NVIC_InitTypeDef   nvic_init;
		nvic_init.NVIC_IRQChannel = DMA2D_IRQn;
		nvic_init.NVIC_IRQChannelPreemptionPriority =7;//抢占优先级0
		nvic_init.NVIC_IRQChannelSubPriority = 0;//子优先级1
		nvic_init.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
		NVIC_Init(&nvic_init);//配置
	}
#endif
	dma2d_rdy =TRUE;

#if 0
    /* Enable the DMA2D Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);

    /* DMA2D configuration */
    DMA2D_DeInit();

    /* Transfer mode */
    DMA2D_InitStruct.DMA2D_Mode = DMA2D_M2M;

    /* Color mode */
    DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;

    DMA2D_OutAdd = CurrentFrameBuffer + 2*(LCD_PIXEL_WIDTH*Ypos + Xpos);

    /* Output Address */
    DMA2D_InitStruct.DMA2D_OutputMemoryAdd = DMA2D_OutAdd;

    /* Number of lines : height */
    DMA2D_InitStruct.DMA2D_NumberOfLine = Height;

    /* Number of pixel per line : width */
    DMA2D_InitStruct.DMA2D_PixelPerLine = Width;

    /* Initialize the alpha and RGB values */
    DMA2D_InitStruct.DMA2D_OutputGreen = 0;
    DMA2D_InitStruct.DMA2D_OutputBlue = 0;
    DMA2D_InitStruct.DMA2D_OutputRed = 0;
    DMA2D_InitStruct.DMA2D_OutputAlpha = 0;

    /* Initialize the output offset */
    DMA2D_InitStruct.DMA2D_OutputOffset = (LCD_PIXEL_WIDTH - Width);

    /* Initialize DMA2D */
    DMA2D_Init(&DMA2D_InitStruct);
#endif
}


static inline void DMA2D_WaitRdy(void)
{
	while(dma2d_rdy==FALSE)
	{
		GUI_SemWait(sem_rdy,100);
	}
	 DMA2D_DeInit();

}

static inline void DMA2D_WaitDone(void)
{
	while(dma2d_rdy==FALSE)
	{
		GUI_SemWait(sem_rdy,100);
	}
	//DMA2D->CR |= DMA2D_CR_SUSP;

}

static void DMA2D_Execu(void)
{
	dma2d_rdy =FALSE;
	DMA2D->CR |= DMA2D_CR_TCIE|DMA2D_CR_START;
}


#if 1
void DMA2D_M2M_Config(const SURFACE *pSurf,uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height,const u8 *bits)
{
    DMA2D_InitTypeDef      DMA2D_InitStruct;
    DMA2D_FG_InitTypeDef   DMA2D_FG_InitStruct;


    /* Transfer mode */
    DMA2D_InitStruct.DMA2D_Mode = DMA2D_M2M;

    /* Color mode */
    DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;

  //  DMA2D_OutAdd = CurrentFrameBuffer + 2*(pSurf->Width*Ypos + Xpos);

    /* Output Address */
    DMA2D_InitStruct.DMA2D_OutputMemoryAdd = (u32)pSurf->GL->SetPos(pSurf,Xpos,Ypos);

    /* Number of lines : height */
    DMA2D_InitStruct.DMA2D_NumberOfLine = Height;

    /* Number of pixel per line : width */
    DMA2D_InitStruct.DMA2D_PixelPerLine = Width;

    /* Initialize the alpha and RGB values */
    DMA2D_InitStruct.DMA2D_OutputGreen = 0;
    DMA2D_InitStruct.DMA2D_OutputBlue = 0;
    DMA2D_InitStruct.DMA2D_OutputRed = 0;
    DMA2D_InitStruct.DMA2D_OutputAlpha = 0;

    /* Initialize the output offset */
    DMA2D_InitStruct.DMA2D_OutputOffset = (pSurf->Width - Width);

    /* Initialize DMA2D */
    DMA2D_Init(&DMA2D_InitStruct);

    /* Configure default values for foreground */
    DMA2D_FG_StructInit(&DMA2D_FG_InitStruct);

    /* Configure DMA2D foreground color mode */
    DMA2D_FG_InitStruct.DMA2D_FGCM = DMA2D_RGB565;

    /* Configure Input Address */
    DMA2D_FG_InitStruct.DMA2D_FGMA = (u32)bits;//CurrentFrameBuffer+BUFFER_OFFSET;

    /* Initialize foreground */
    DMA2D_FGConfig(&DMA2D_FG_InitStruct);

    /* Start Transfer */
    DMA2D_StartTransfer();

    /* Wait for CTC Flag activation */
    while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
    {
    }
}
#endif


BOOL DMA2D_FillRect(const SURFACE *pSurf,int x,int y,U16 w,U16 h,U32 color)
{
	if((w*h) < (32*32))
	{
		return FALSE;
	}

    DMA2D_WaitRdy();

    switch(pSurf->Format)
    {
	case SURF_RGB565:
		DMA2D->OPFCCR = DMA2D_RGB565;
		break;
	case SURF_ARGB4444:
		DMA2D->OPFCCR = DMA2D_ARGB4444;
		break;
	case SURF_XRGB8888:
		DMA2D->OPFCCR = DMA2D_RGB888;
		break;
	case SURF_ARGB8888:
		DMA2D->OPFCCR = DMA2D_ARGB8888;
		break;
	default:
		return FALSE;
    }


    DMA2D->CR     = DMA2D_R2M;
    //DMA2D->OPFCCR = DMA2D_RGB565;
    DMA2D->OCOLR  = color;
    DMA2D->OMAR   = (u32)pSurf->GL->SetPos(pSurf,x,y);
    DMA2D->OOR    = pSurf->Width-w;
    DMA2D->NLR    = (w<<16)|h;

    ////
    DMA2D_Execu();
    DMA2D_WaitDone();
	return TRUE;

}

BOOL DMA2D_DrawBitmap_RGB565(const SURFACE *pSurf,int x,int y,U16 w,U16 h,int width_bytes,const U8 *bits)
{

	if((w*h) < (32*32))
	{
		return FALSE;
	}


	if((U32)bits & 0x0000000F)
	{
		return FALSE;
	}

    DMA2D_WaitRdy();

    switch(pSurf->Format)
    {
	case SURF_RGB565:
		DMA2D->OPFCCR = DMA2D_RGB565;
		break;
	case SURF_ARGB4444:
		DMA2D->OPFCCR = DMA2D_ARGB4444;
		break;
	case SURF_XRGB8888:
		DMA2D->OPFCCR = DMA2D_RGB888;
		break;
	case SURF_ARGB8888:
		DMA2D->OPFCCR = DMA2D_ARGB8888;
		break;
	default:
		return FALSE;
    }

    DMA2D->CR = DMA2D_M2M;

    DMA2D->OMAR   = (U32)pSurf->GL->SetPos(pSurf,x,y);
    DMA2D->OOR    = pSurf->Width-w;

    DMA2D->NLR    = (w<<16)|h;

    DMA2D->FGPFCCR = CM_RGB565;
    DMA2D->FGMAR  = (U32)bits;
    DMA2D->FGOR   = (width_bytes>>1)-w;

    ////

    DMA2D_Execu();
    DMA2D_WaitDone();
	return TRUE;

}

BOOL DMA2D_DrawBitmap_ARGB(const SURFACE *pSurf,int x,int y,U16 w,U16 h,int width_bytes,const U8 *bits,U32 color_format)
{

	if((w*h) < (20*20))
	{
		return FALSE;
	}


	if((U32)bits & 0x0000000F)
	{
		return FALSE;
	}

    DMA2D_WaitRdy();

    switch(pSurf->Format)
    {
	case SURF_RGB565:
		DMA2D->OPFCCR = DMA2D_RGB565;
		DMA2D->BGPFCCR =CM_RGB565;
		break;
	case SURF_ARGB4444:
		DMA2D->OPFCCR = DMA2D_ARGB4444;
		DMA2D->BGPFCCR =CM_ARGB4444;
		break;
	case SURF_XRGB8888:
		DMA2D->OPFCCR = DMA2D_RGB888;
		DMA2D->BGPFCCR =CM_RGB888;
		break;
	case SURF_ARGB8888:
		DMA2D->OPFCCR = DMA2D_ARGB8888;
		DMA2D->BGPFCCR =CM_ARGB8888;
		break;
	default:
		return FALSE;
    }

    DMA2D->CR     = DMA2D_M2M_BLEND;
    DMA2D->OMAR   = (U32)pSurf->GL->SetPos(pSurf,x,y);
    DMA2D->OOR    = pSurf->Width-w;

    DMA2D->NLR    = (w<<16)|h;

    //DMA2D->BGPFCCR =CM_RGB565;
    DMA2D->BGMAR =DMA2D->OMAR;
    DMA2D->BGOR  =DMA2D->OOR;

    DMA2D->FGPFCCR = NO_MODIF_ALPHA_VALUE|color_format;
    DMA2D->FGMAR = (U32)bits;
   // DMA2D->FGOR   = (width_bytes>>1)-w;

    switch(color_format)
    {
    	case CM_ARGB4444:
    		DMA2D->FGOR   = (width_bytes>>1)-w;
    		break;
    	case CM_ARGB8888:
    		DMA2D->FGOR   = (width_bytes>>2)-w;
    		break;
    	default:
    		return FALSE;
    }

    ////

    DMA2D_Execu();
    DMA2D_WaitDone();
	return TRUE;

}

/*=========================================================================================*/
#if 0

#pragma pack(4)
static const u16 bm565[]=
{
		RGB565(31,0,0),RGB565(31,0,0),RGB565(31,0,0),RGB565(31,0,0),
		RGB565(0,63,0),RGB565(0,63,0),RGB565(0,63,0),RGB565(0,63,0),
		RGB565(0,0,31),RGB565(0,0,31),RGB565(0,0,31),RGB565(0,0,31),
		RGB565(31,63,31),RGB565(31,63,31),RGB565(31,63,31),RGB565(31,63,31),

		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),
		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),
		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),
		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),

		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),
		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),
		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),
		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),

		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),
		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),
		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),
		RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),RGB565(31,0,31),

};

//#pragma __pack()
#endif

#if 0
static u16 *buf16;
static u32 *buf32;
void	GUI_memset16(U16 *dst,U16 dat,int count);
void	GUI_memset32(U32 *dst,U32 dat,int count);
void DMA2D_Test(const SURFACE *pSurf)
{
	//return;

	DMA2D_DrvInit();
	buf16 =dma_mem_alloc(128*64*2);
	buf32 =dma_mem_alloc(128*64*4);
	GUI_memset16(buf16,RGB565(31,0,0),128*64);
	while(1)
	{ //测试
		/*
		DMA2D_FillRect(pSurf,30,20,300,200,RGB565(31,0,0));
		SYS_msleep(500);
		DMA2D_FillRect(pSurf,30,20,300,200,RGB565(0,63,0));
		SYS_msleep(500);
		DMA2D_FillRect(pSurf,30,20,300,200,RGB565(0,0,31));
		SYS_msleep(500);
		break;
		*/
/*
		GUI_memset16(buf16,RGB565(31,0,0),128*64);
		DMA2D_M2M_Config(pSurf,20,30,128,64,(const U8*)buf16);
		SYS_msleep(1000);
		GUI_memset16(buf16,RGB565(0,63,0),128*64);
		DMA2D_M2M_Config(pSurf,20,30,128,64,(const U8*)buf16);
		SYS_msleep(1000);
*/
		/////

		GUI_memset16(buf16,RGB565(31,0,31),128*64);
		DMA2D_DrawBitmap_RGB565(pSurf,20,30,128,64,128*2,(const U8*)buf16);
		GUI_msleep(1000);
		GUI_memset16(buf16,RGB565(0,63,0),128*64);
		DMA2D_DrawBitmap_RGB565(pSurf,20,30,128,64,128*2,(const U8*)buf16);
		GUI_msleep(1000);
/*
		GUI_memset32(buf32,ARGB8888(255,0,0,255),128*64);
		DMA2D_DrawBitmap_RGB(pSurf,20,30,128,64,128*4,(const U8*)buf32,CM_RGB888);
		SYS_msleep(1000);
		GUI_memset32(buf32,ARGB8888(255,255,255,0),128*64);
		DMA2D_DrawBitmap_RGB(pSurf,20,30,128,64,128*4,(const U8*)buf32,CM_RGB888);
		SYS_msleep(1000);
*/

/*
		GUI_memset16(buf16,ARGB4444(5,0,0,15),128*64);
		DMA2D_DrawBitmap_ARGB(pSurf,20+10,30+10,128,64,128*2,(const U8*)buf16,CM_ARGB4444);
		SYS_msleep(1000);
		GUI_memset16(buf16,ARGB4444(5,15,15,0),128*64);
		DMA2D_DrawBitmap_ARGB(pSurf,20+20,30+20,128,64,128*2,(const U8*)buf16,CM_ARGB4444);
		SYS_msleep(1000);
*/
		GUI_memset32(buf32,ARGB8888(80,0,0,255),128*64);
		DMA2D_DrawBitmap_ARGB(pSurf,20+10,30+10,128,64,128*4,(const U8*)buf32,CM_ARGB8888);
		GUI_msleep(1000);
		GUI_memset32(buf32,ARGB8888(80,255,255,0),128*64);
		DMA2D_DrawBitmap_ARGB(pSurf,20+20,30+20,128,64,128*4,(const U8*)buf32,CM_ARGB8888);
		GUI_msleep(1000);

		break;

	}

}
#endif
/*=========================================================================================*/

#endif /* DMA2D_EN*/

