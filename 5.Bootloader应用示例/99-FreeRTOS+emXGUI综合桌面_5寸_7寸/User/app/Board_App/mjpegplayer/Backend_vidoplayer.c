#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "ff.h"
#include "./mjpegplayer/Backend_avifile.h"
#include "./mjpegplayer/Backend_vidoplayer.h"
#include "./Bsp/lcd/bsp_lcd.h"
#include "./Bsp/wm8978/bsp_wm8978.h"  
#include "emXGUI.h"
#include "emXGUI_JPEG.h"
#include "x_libc.h"
#include "./mjpegplayer/GUI_AVIList_DIALOG.h"
#include "./mjpegplayer/GUI_AVIPLAYER_DIALOG.h"
FIL       fileR  __EXRAM;
UINT      BytesRD;
__align(8) uint8_t   Frame_buf[1024*30] __EXRAM;

/* 以太网使用 */
extern __IO uint32_t LocalTime; /* this variable is used to create a time reference incremented by 10ms */

static volatile uint8_t audiobufflag=0;
__align(8) uint8_t   Sound_buf[4][1024*5] __EXRAM={0};

static uint8_t   *pbuffer;

uint32_t  mid;
uint32_t  Strsize;
uint16_t  Strtype;

extern int LIST_STATE;

static volatile uint8_t timeout;
extern WAVEFORMAT*   wavinfo;
extern avih_TypeDef* avihChunk;
extern HWND avi_wnd_time;
extern int avi_chl;
void MUSIC_I2S_DMA_TX_Callback(void);
extern void mjpegdraw(uint8_t *mjpegbuffer,uint32_t size);
static void TIM3_Config(uint16_t period,uint16_t prescaler);
extern void App_DecodeMusic(HWND hwnd, const void *dat, int cbSize, JPG_DEC *dec);
extern char tiimg[];
extern unsigned int timgsize(void);
//extern HDC hdc_AVI;
extern HWND hwnd_AVI;
extern volatile int win_fps;
extern GUI_MUTEX*	AVI_JPEG_MUTEX;    // 用于确保一帧图像用后被释放完在退出线程

static volatile int frame=0;
static volatile int t0=0;
volatile int avi_fps=0;


static u32 alltime = 0;		//总时长 
u32 cur_time; 		//当前播放时间 
uint8_t temp11=0;	
u32 pos;//文件指针位置
s32 time_sum = 0;
void AVI_play(char *filename, HWND hwnd, int vol, int vol_horn)
{
  FRESULT  res;
  uint32_t offset;
  uint16_t audiosize;
  uint8_t avires=0;
  uint8_t audiosavebuf;

  pbuffer=Frame_buf;
  res=f_open(&fileR,filename,FA_READ);
  if(res!=FR_OK)
  {
    return;    
  }
  


  res=f_read(&fileR,pbuffer,20480,&BytesRD);


  avires=AVI_Parser(pbuffer);//解析AVI文件格式
  if(avires)
  {
    return;    
  }
  
  avires=Avih_Parser(pbuffer+32);//解析avih数据块
  if(avires)
  {
    return;    
  }
  //strl列表
  avires=Strl_Parser(pbuffer+88);//解析strh数据块
  if(avires)
  {
    return;    
  }
  
  avires=Strf_Parser(pbuffer+164);//解析strf数据块
  if(res!=FR_OK)
  {
    return;    
  }
  
  mid=Search_Movi(pbuffer);//寻找movi ID	（数据块）	
  if(mid==0)
  {
    return;    
  }
  
  Strtype=MAKEWORD(pbuffer+mid+6);//流类型（movi后面有两个字符）
  Strsize=MAKEDWORD(pbuffer+mid+8);//流大小
  if(Strsize%2)Strsize++;//奇数加1
  f_lseek(&fileR,mid+12);//跳过标志ID  
  
  offset=Search_Auds(pbuffer);
  if(offset==0)
  {
    return;    
  }  
  audiosize=*(uint8_t *)(pbuffer+offset+4)+256*(*(uint8_t *)(pbuffer+offset+5));
  if(audiosize==0)
  {
    offset=(uint32_t)pbuffer+offset+4;
    mid=Search_Auds((uint8_t *)offset);
    if(mid==0)
    {
      return;    
    }
    audiosize=*(uint8_t *)(mid+offset+4)+256*(*(uint8_t *)(mid+offset+5));
  }
  
   I2S_Stop();			/* 停止I2S录音和放音 */
	wm8978_Reset();		/* 复位WM8978到复位状态 */	
  	/* 配置WM8978芯片，输入为DAC，输出为耳机 */
	// wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
    WCHAR wbuf1[3];
   HWND  wnd = GetDlgItem(hwnd, ID_BUTTON_Bugle);
   
   GetWindowText(wnd, wbuf1, 3);
   if (wbuf1[0] == L'P')    // 判断当前
   {
      wm8978_CfgAudioPath(DAC_ON, SPK_ON);                        // 配置为扬声器输出
   }
   else
   {
      wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);    // 配置为耳机输出
   }
	/* 调节耳机音量，左右相同音量 */
	wm8978_SetOUT1Volume(vol);
  wm8978_SetOUT2Volume(vol_horn);    // 设置喇叭音量
   if(vol == 0)
      wm8978_OutMute(1);//静音
   else
      wm8978_OutMute(0);
	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
  I2S_GPIO_Config();
  I2Sx_Mode_Config(I2S_Standard_Phillips, I2S_DataFormat_16b, wavinfo->SampleRate);
  I2S_DMA_TX_Callback=MUSIC_I2S_DMA_TX_Callback;			//回调函数指wav_i2s_dma_callback
  I2S_Play_Stop();
  I2Sx_TX_DMA_Init((uint16_t *)Sound_buf[1],(uint16_t *)Sound_buf[2],audiosize/2);
  audiobufflag=0;	    
  timeout=0;
  audiosavebuf=0;
  audiobufflag=0;
  TIM3_Config((avihChunk->SecPerFrame/100)-1,9000-1);
  I2S_Play_Start();  
	
	t0= GUI_GetTickCount();

   //歌曲总长度=每一帧需要的时间（s）*帧总数
   alltime=(avihChunk->SecPerFrame/1000)*avihChunk->TotalFrame;
   alltime/=1000;//单位是秒
  WCHAR buff[128];
  //char *str = NULL;
 // RECT rc0 = {0, 367,120,30};//当前时间
  x_wsprintf(buff, L"分辨率：%d*%d", _img_w, _img_h);
  SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB2), buff);

//  char *ss;
//  int length1=strlen(filename);
//  int length2=strlen(File_Path);
//  if(strncpy(filename,File_Path,length2))//比较前n个字符串，类似strcpy
//  {
//    ss = filename + length2;
//  }
  x_mbstowcs_cp936(buff, lcdlist_wnd[Play_index], 200);
  SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB1), buff);
  
  x_wsprintf(buff, L"%02d:%02d:%02d",
             alltime/3600,(alltime%3600)/60,alltime%60);
  SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB4), buff);
  
  while(1&&!sw_flag)//播放循环
  {					
		int t1;
     if(!avi_chl){

        
        
   //fptr存放着文件指针的位置，fsize是文件的总大小，两者之间的比例和当前时间与总时长的比例相同（fptr/fsize = cur/all）     
   cur_time=((double)fileR.fptr/fileR.fsize)*alltime;
   //更新进度条
   if(!LIST_STATE)
    InvalidateRect(avi_wnd_time, NULL, FALSE);  
   if(!LIST_STATE)   
    SendMessage(avi_wnd_time, SBM_SETVALUE, TRUE, cur_time*255/alltime);    
   
    x_wsprintf(buff, L"%02d:%02d:%02d",///%02d:%02d:%02d alltime/3600,(alltime%3600)/60,alltime%60
             cur_time/3600,(cur_time%3600)/60,cur_time%60); 		
   
	 if(Strtype==T_vids)//显示帧
    {    	
			frame++;
			t1 =GUI_GetTickCount();
			if((t1 - t0) >= 1000)
			{
				
				avi_fps =frame;
				t0 =t1;
				frame =0;
			}

      //HDC hdc_mem,hdc;
      pbuffer=Frame_buf;
      AVI_DEBUG("S\n"); 
      f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//读入整帧+下一数据流ID信息
      AVI_DEBUG("E\n");   
			timeout=0;
		
			if(frame&!LIST_STATE)
			{	
#if 1		//直接写到窗口方式.	
				HDC hdc;
        GUI_MutexLock(AVI_JPEG_MUTEX,0xFFFFFFFF);    // 获取互斥量
//				printf("1\n");
				hdc =GetDC(NULL);    // 得到屏幕的HDC
//        hdc_AVI = GetDC(hwnd);
				JPEG_Out(hdc,160,89,Frame_buf,BytesRD);
        ReleaseDC(NULL,hdc);
//        printf("2\n");
        
        
//            ClrDisplay(hdc, &rc0, MapRGB(hdc, 0,0,0));
//            SetTextColor(hdc, MapRGB(hdc,255,255,255));
//            DrawText(hdc, buff,-1,&rc0,DT_VCENTER|DT_CENTER);
            
//       if(!LIST_STATE)
        SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB5), buff);
        x_wsprintf(buff, L"帧率：%dFPS/s", avi_fps);
//        if(!LIST_STATE)
        SetWindowText(GetDlgItem(VideoPlayer_hwnd, ID_TB3), buff);
        GUI_MutexUnlock(AVI_JPEG_MUTEX);              // 解锁互斥量

#endif
			}
			
      while(timeout==0)
      {   
				//rt_thread_delay(1); //不要死等，最好用信号量.				
        GUI_msleep(1);
      }      
      
      //DeleteDC(hdc_mem);
      //ReleaseDC(hwnd, hdc);
      timeout=0;
    }//显示帧
    else if(Strtype==T_auds)//音频输出
    { 
      uint8_t i;
      audiosavebuf++;
      if(audiosavebuf>3)
			{
				audiosavebuf=0;
			}	
      do
      {
				//rt_thread_delay(1); 
        i=audiobufflag;
        if(i)
					i--;
        else 
					i=3; 

      }while(audiobufflag==i);
      AVI_DEBUG("S\n");
      f_read(&fileR,Sound_buf[audiosavebuf],Strsize+8,&BytesRD);//读入整帧+下一数据流ID信息
      AVI_DEBUG("E\n");
      pbuffer=Sound_buf[audiosavebuf];      
    }
    else 
    {
      GUI_INFO("帧错误\n");
//      GUI_INFO("%lX    %X    %d    %lX",fileR.fptr,Strtype,Strsize,&fileR.fptr-Strsize);
      break;
    }
					   	
  }
  else
  {
         pos = fileR.fptr;
//         //根据进度条调整播放位置				
         temp11=SendMessage(avi_wnd_time, SBM_GETVALUE, NULL, NULL); 
         time_sum = fileR.fsize/alltime*(temp11*alltime/255-cur_time);//跳过多少数据 计算公式：文件总大小/需要跳过的数据量 = 总时间/当前的时间
         //如果当前文件指针未到最后
        	if(pos<fileR.fsize)pos+=time_sum; 
         //如果文件指针到了最后30K内容
          if(pos>(fileR.fsize-1024*30))
          {
            pos=fileR.fsize-1024*30;
          }
         
          f_lseek(&fileR,pos);
      
      #if 0
         if(pos == 0)
            mid=Search_Movi(Frame_buf);//寻找movi ID  判断自己是不是还在数据段
         else 
            mid = 0;  
        int iiii= 0;//计算偏移量
         while(1)
         {
            //每次读512个字节，直到找到数据帧的帧头
            u16 temptt = 0;//计算数据帧的位置
            AVI_DEBUG("S\n");

            f_read(&fileR,Frame_buf,512,&BytesRD);
            AVI_DEBUG("E\n");

            temptt = Search_Fram(Frame_buf,BytesRD);
            iiii++;
            if(temptt)
            {            
               AVI_DEBUG("S temptt =%d\n",temptt);
               AVI_DEBUG("S Frame_buf[temptt] =%c %c %c %c\n",
                                      Frame_buf[temptt],
                                      Frame_buf[temptt+1],
                                      Frame_buf[temptt+2],
                                      Frame_buf[temptt+3]);
               /* 多读取512数据，防止标志在边界时出错 */
               f_read(&fileR,(u8*)Frame_buf+BytesRD,512,&BytesRD);
               AVI_DEBUG("E\n");
                pbuffer = Frame_buf;
               Strtype=MAKEWORD(pbuffer+temptt+2);//流类型
               Strsize=MAKEDWORD(pbuffer+temptt+4);//流大小
               mid += temptt + 512*iiii-512;//加上偏移量
//               if(temptt == 16)
//                  continue;
               break;
            }

         }
         #else
         f_read(&fileR,Frame_buf,1024*30,&BytesRD);
         AVI_DEBUG("E\n");
         if(pos == 0)
            mid=Search_Movi(Frame_buf);//寻找movi ID
         else 
            mid = 0;
         mid += Search_Fram(Frame_buf,1024*30);
         pbuffer = Frame_buf;
         Strtype=MAKEWORD(pbuffer+mid+2);//流类型
         Strsize=MAKEDWORD(pbuffer+mid+4);//流大小
         #endif
         
         if(Strsize%2)Strsize++;//奇数加1
         f_lseek(&fileR,pos+mid+8);//跳过标志ID  
         AVI_DEBUG("S Strsize=%d\n",Strsize);

         f_read(&fileR,Frame_buf,Strsize+8,&BytesRD);//读入整帧+下一数据流ID信息 
         AVI_DEBUG("E\n");
         
         avi_chl = 0;    
     }
     
    
         //判断下一帧的帧内容 
         Strtype=MAKEWORD(pbuffer+Strsize+2);//流类型
         Strsize=MAKEDWORD(pbuffer+Strsize+4);//流大小									
         if(Strsize%2)Strsize++;//奇数加1		  
//      GUI_INFO("%lX    %X    %X    %lX    %X    %X",fileR.fptr,Strtype,Strsize,fileR.fptr-Strsize,Strsize+8,BytesRD);
     }
  
 

  sw_flag = 0;
  I2S_Play_Stop();
  I2S_Stop();		/* 停止I2S录音和放音 */
	wm8978_Reset();	/* 复位WM8978到复位状态 */
  TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,DISABLE); //使能定时器3
  f_close(&fileR);
}

void MUSIC_I2S_DMA_TX_Callback(void)
{
  audiobufflag++;
  if(audiobufflag>3)
	{
		audiobufflag=0;
	}
	
	if(DMA1_Stream4->CR&(1<<19)) //当前读取Memory1数据
	{
		DMA_MemoryTargetConfig(DMA1_Stream4,(uint32_t)Sound_buf[audiobufflag], DMA_Memory_0);
	}
	else
	{
		DMA_MemoryTargetConfig(DMA1_Stream4,(uint32_t)Sound_buf[audiobufflag], DMA_Memory_1); 
	}
}

/**
  * @brief  通用定时器3中断初始化
  * @param  period : 自动重装值。
  * @param  prescaler : 时钟预分频数
  * @retval 无
  * @note   定时器溢出时间计算方法:Tout=((period+1)*(prescaler+1))/Ft us.
  *          Ft=定时器工作频率,为SystemCoreClock/2=90,单位:Mhz
  */
static void TIM3_Config(uint16_t period,uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=prescaler;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=period;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  定时器3中断服务函数
  * @param  无
  * @retval 无
  */
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{    
    timeout=1;
    LocalTime+=10;//10ms增量
	}
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位	
}
