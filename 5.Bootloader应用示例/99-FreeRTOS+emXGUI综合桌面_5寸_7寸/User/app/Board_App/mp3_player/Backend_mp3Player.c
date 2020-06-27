/*
******************************************************************************
* @file    recorder.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   WM8978放音功能测试+mp3解码
******************************************************************************
* @attention
*
* 实验平台:秉火  STM32 F429 开发板  
* 论坛    :http://www.firebbs.cn
* 淘宝    :https://fire-stm32.taobao.com
*
******************************************************************************
*/
#include <stdio.h>
#include <string.h>
#include "Bsp/usart/bsp_debug_usart.h"
//#include "Bsp/systick/bsp_SysTick.h"
#include "Bsp/wm8978/bsp_wm8978.h"
#include "ff.h" 
#include "./mp3_player/Backend_mp3Player.h"
#include "mp3dec.h"
#include "./mp3_player/Backend_musiclist.h"
#include "emXGUI.h"
#include "x_libc.h"
#include "./mp3_player/GUI_MUSICPLAYER_DIALOG.h"
#include "GUI_RECORDER_DIALOG.h"

#define Delay_ms GUI_msleep
/* 推荐使用以下格式mp3文件：
 * 采样率：44100Hz
 * 声  道：2
 * 比特率：320kbps
 */

/* 处理立体声音频数据时，输出缓冲区需要的最大大小为2304*16/8字节(16为PCM数据为16位)，
 * 这里我们定义MP3BUFFER_SIZE为2304，实际输出缓冲区为MP3BUFFER_SIZE*2个字节
 */
#define MP3BUFFER_SIZE  2304		
#define INPUTBUF_SIZE   3000	

static HMP3Decoder		Mp3Decoder;			/* mp3解码器指针	*/
static MP3FrameInfo		Mp3FrameInfo;		/* mP3帧信息  */
MP3_TYPE mp3player;/* mp3播放设备 */
static uint8_t Isread=0;           /* DMA传输完成标志 */
static uint8_t bufflag=0;          /* 数据缓存区选择标志 */
extern HFONT DEFAULT_FONT;
uint8_t inputbuf[INPUTBUF_SIZE]  __EXRAM;        /* 解码输入缓冲区，1940字节为最大MP3帧大小  */
static short outbuffer[2][MP3BUFFER_SIZE] __EXRAM;  /* 解码输出缓冲区，也是I2S输入数据，实际占用字节数：RECBUFFER_SIZE*2 */

FIL file __EXRAM;											/* file objects */
FRESULT result; 
UINT bw;            					/* File R/W count */
/*wav播放器*/
REC_TYPE Recorder;          /* 录音设备 */
uint16_t buffer0[RECBUFFER_SIZE] __EXRAM;  /* 数据缓存区1 ，实际占用字节数：RECBUFFER_SIZE*2 */
uint16_t buffer1[RECBUFFER_SIZE] __EXRAM;  /* 数据缓存区2 ，实际占用字节数：RECBUFFER_SIZE*2 */
static WavHead rec_wav;            /* WAV设备  */


uint32_t g_FmtList[FMT_COUNT][3] =
{
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_8k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_16k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_22k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_44k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_96k},
	{I2S_Standard_Phillips, I2S_DataFormat_16b, I2S_AudioFreq_192k},
};
RECT rc_MusicTimes = {285, 404,240,72};//歌曲时长
RECT rc_cli = {0, 380, 800, 20};//进度条
RECT rc_musicname = {180,4,400,72};//歌曲文字


const uint16_t recplaybuf[4]={0X0000,0X0000};//2个16位数据,用于录音时I2S Master发送.循环发送0.
/* 仅允许本文件内调用的函数声明 */
void MP3Player_I2S_DMA_TX_Callback(void);
extern int enter_flag;
//extern unsigned char music_lcdlist[MUSIC_MAX_NUM][MUSIC_NAME_LEN];
/**
  * @brief  获取MP3ID3V2文件头的大小
  * @param  输入MP3文件开头的数据，至少10个字节
  * @retval ID3V2的大小
  */
uint32_t mp3_GetID3V2_Size(unsigned char *buf)
{
 uint32_t ID3V2_size;
	
 if(buf[0] == 'I' && buf[1] == 'D' && buf[2] =='3')//存在ID3V2
 {
 	 ID3V2_size = (buf[6]<<21) | (buf[7]<<14) | (buf[8]<<7) | buf[9];
 }
 else//不存在ID3V2
	 ID3V2_size = 0;

 return ID3V2_size;

}

/**
  * @brief   MP3格式音频播放主程序
  * @param  mp3file：MP3文件名
  * @param  vol：音量
  * @param  hdc：屏幕绘图上下文 
  * @retval 无
  */
uint8_t NUM = 0;
static uint16_t curtime,alltime;//歌词的当前的时间以及总时间长度
void mp3PlayerDemo(HWND hwnd, const char *mp3file, uint8_t vol, uint8_t vol_horn, HDC hdc)
{
	uint8_t *read_ptr=inputbuf;
	uint32_t frames=0;//歌曲的帧数（26ms一帧）
   static uint8_t timecount = 0;
   DWORD pos;//记录文字变量
	int err=0, i=0, outputSamps=0;	
   uint32_t time_sum = 0; //计算当前已播放到的时间位置
	int	read_offset = 0;				/* 读偏移指针 */
	int	bytes_left = 0;					/* 剩余字节数 */	
	uint16_t frame_size;//MP3帧的大小
	uint32_t ID3V2_size;//MP3的ID3V2的大小
   static uint8_t lyriccount=0;//歌词index记录
   WCHAR wbuf[128];//保存文本数组
	mp3player.ucFreq=I2S_AudioFreq_Default;
	mp3player.ucStatus=STA_IDLE;
	mp3player.ucVolume = vol;//设置 WM8978的音量值
   int ooo = 0;
  NUM++;
	result=f_open(&file,mp3file,FA_READ);
	if(result!=FR_OK)
	{
		printf("Open mp3file :%s fail!!!->%d\r\n",mp3file,result);
		result = f_close (&file);
    if(time2exit == 1)
    {
      lyriccount=0;  
      GUI_SemPost(exit_sem);
    }
		return;	/* 停止播放 */
	}
	printf("当前播放文件 -> %s\n",mp3file);
	
	//初始化MP3解码器
	Mp3Decoder = MP3InitDecoder();	
	if(Mp3Decoder==0)
	{
		printf("初始化helix解码库设备\n");
		return;	/* 停止播放 */
	}
	printf("初始化中...\n");
	
	Delay_ms(10);	/* 延迟一段时间，等待I2S中断结束 */
	wm8978_Reset();		/* 复位WM8978到复位状态 */

   WCHAR wbuf1[3];
   HWND  wnd = GetDlgItem(hwnd, ID_BUTTON_BUGLE);
   
   GetWindowText(wnd, wbuf1, 3);
   if (wbuf1[0] == L'P')    // 判断当前
   {
      wm8978_CfgAudioPath(DAC_ON, SPK_ON);                        // 配置为扬声器输出
   }
   else
   {
      wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);    // 配置为耳机输出
   }
	// /* 配置WM8978芯片，输入为DAC，输出为耳机 */
	// wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* 调节音量，左右相同音量 */
	wm8978_SetOUT1Volume(mp3player.ucVolume);
   
  wm8978_SetOUT1Volume(vol_horn);    // 设置喇叭音量的值

	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
	
	/*  初始化并配置I2S  */
	I2S_Stop();
	I2S_GPIO_Config();
	I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,mp3player.ucFreq);	
	I2S_DMA_TX_Callback=MP3Player_I2S_DMA_TX_Callback;
	I2Sx_TX_DMA_Init((uint16_t *)outbuffer[0],(uint16_t *)outbuffer[1],MP3BUFFER_SIZE);	
	
	bufflag=0;
	Isread=0;
	
	mp3player.ucStatus = STA_PLAYING;		/* 放音状态 */
   result=f_read(&file,inputbuf,INPUTBUF_SIZE,&bw);
	if(result!=FR_OK)
	{
		printf("读取%s失败 -> %d\r\n",mp3file,result);
		MP3FreeDecoder(Mp3Decoder);
		return;
	}

   //获取ID3V2的大小，并偏移至该位置
	ID3V2_size = mp3_GetID3V2_Size(inputbuf);
	f_lseek(&file,ID3V2_size);	
	result=f_read(&file,inputbuf,INPUTBUF_SIZE,&bw);
	if(result!=FR_OK)
	{
		printf("读取%s失败 -> %d\r\n",mp3file,result);
		MP3FreeDecoder(Mp3Decoder);
		return;
	}   
	x_mbstowcs_cp936(wbuf, music_lcdlist[play_index], FILE_NAME_LEN);
   SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB5), wbuf);    
	read_ptr=inputbuf;
	bytes_left=bw;
   
   if(vol != 0)
   {
      wm8978_OutMute(0);
   }
   //当音量icon被按下时，设置为静音模式
   else
   {                
      wm8978_OutMute(1);//静音
   }
            
   
	/* 进入主程序循环体 */
	while(mp3player.ucStatus == STA_PLAYING)
	{
		//寻找帧同步，返回第一个同步字的位置
		read_offset = MP3FindSyncWord(read_ptr, bytes_left);			
		//没有找到同步字
		if(read_offset < 0)																		
		{
			result=f_read(&file,inputbuf,INPUTBUF_SIZE,&bw);
			if(result!=FR_OK)
			{
				printf("读取%s失败 -> %d\r\n",mp3file,result);
				break;
			}
			read_ptr=inputbuf;
			bytes_left=bw;
			continue;
		}
		
		read_ptr += read_offset;					//偏移至同步字的位置
		bytes_left -= read_offset;				//同步字之后的数据大小	
		if(bytes_left < 1024)							//补充数据
		{
			/* 注意这个地方因为采用的是DMA读取，所以一定要4字节对齐  */
			i=(uint32_t)(bytes_left)&3;									//判断多余的字节
			if(i) i=4-i;														//需要补充的字节
			memcpy(inputbuf+i, read_ptr, bytes_left);	//从对齐位置开始复制
			read_ptr = inputbuf+i;										//指向数据对齐位置
			//补充数据
			result = f_read(&file, inputbuf+bytes_left+i, INPUTBUF_SIZE-bytes_left-i, &bw);
			bytes_left += bw;										//有效数据流大小
		}
		//开始解码 参数：mp3解码结构体、输入流指针、输入流大小、输出流指针、数据格式
		err = MP3Decode(Mp3Decoder, &read_ptr, &bytes_left, outbuffer[bufflag], 0);	
      time_sum +=26;//每帧26ms      
		frames++;	
		//错误处理
		if (err != ERR_MP3_NONE)									
		{
			switch (err)
			{
				case ERR_MP3_INDATA_UNDERFLOW:
					printf("ERR_MP3_INDATA_UNDERFLOW\r\n");
					result = f_read(&file, inputbuf, INPUTBUF_SIZE, &bw);
					read_ptr = inputbuf;
					bytes_left = bw;
					break;		
				case ERR_MP3_MAINDATA_UNDERFLOW:
					/* do nothing - next call to decode will provide more mainData */
					printf("ERR_MP3_MAINDATA_UNDERFLOW\r\n");
					break;		
				default:
					printf("UNKNOWN ERROR:%d\r\n", err);		
					// 跳过此帧
					if (bytes_left > 0)
					{
						bytes_left --;
						read_ptr ++;
					}	
					break;
			}
			Isread=1;
		}
		else		//解码无错误，准备把数据输出到PCM
		{
			MP3GetLastFrameInfo(Mp3Decoder, &Mp3FrameInfo);		//获取解码信息				
			/* 输出到DAC */
			outputSamps = Mp3FrameInfo.outputSamps;							//PCM数据个数
			if (outputSamps > 0)
			{
				if (Mp3FrameInfo.nChans == 1)	//单声道
				{
					//单声道数据需要复制一份到另一个声道
					for (i = outputSamps - 1; i >= 0; i--)
					{
						outbuffer[bufflag][i * 2] = outbuffer[bufflag][i];
						outbuffer[bufflag][i * 2 + 1] = outbuffer[bufflag][i];
					}
					outputSamps *= 2;
				}//if (Mp3FrameInfo.nChans == 1)	//单声道
			}//if (outputSamps > 0)
			
			/* 根据解码信息设置采样率 */
			if (Mp3FrameInfo.samprate != mp3player.ucFreq)	//采样率 
			{
				mp3player.ucFreq = Mp3FrameInfo.samprate;
				
            //计算每帧的大小
            frame_size = (((Mp3FrameInfo.version == MPEG1)? 144:72)*Mp3FrameInfo.bitrate)/Mp3FrameInfo.samprate+Mp3FrameInfo.paddingBit;
            
            //根据公式计算歌曲时间
            alltime=(((file.fsize-ID3V2_size-128)/frame_size)*26+1000)/1000;
            //如果进入音乐列表就不显示时长
            if(enter_flag == 0){
               //获取屏幕（385，404）的颜色              
               x_wsprintf(wbuf, L"%02d:%02d",alltime/60,alltime%60);
               SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB1), wbuf);                
               x_mbstowcs_cp936(wbuf, music_lcdlist[play_index], FILE_NAME_LEN);
               SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB5), wbuf);                
               //清除rc_MusicTimes矩形的内容
               //ClrDisplay(hdc, &rc_MusicTimes, color);
               //绘制文本
               //DrawText(hdc, wbuf, -1, &rc_MusicTimes, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }
				printf(" \r\n Bitrate       %dKbps", Mp3FrameInfo.bitrate/1000);
				printf(" \r\n Samprate      %dHz", mp3player.ucFreq);
				printf(" \r\n BitsPerSample %db", Mp3FrameInfo.bitsPerSample);
				printf(" \r\n nChans        %d", Mp3FrameInfo.nChans);
				printf(" \r\n Layer         %d", Mp3FrameInfo.layer);
				printf(" \r\n Version       %d", Mp3FrameInfo.version);
				printf(" \r\n OutputSamps   %d", Mp3FrameInfo.outputSamps);
				printf("\r\n");
				//I2S_AudioFreq_Default = 2，正常的帧，每次都要改速率
				if(mp3player.ucFreq >= I2S_AudioFreq_Default)	
				{
					//根据采样率修改I2S速率
					I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,mp3player.ucFreq);
					I2Sx_TX_DMA_Init((uint16_t *)outbuffer[0],(uint16_t *)outbuffer[1],outputSamps);
				}
				I2S_Play_Start();
			}
		}//else 解码正常
		
		if(file.fptr==file.fsize) 		//mp3文件读取完成，退出
		{
         //进入切歌状态
         mp3player.ucStatus=STA_SWITCH;
         //播放曲目自增1
         play_index++;
         //printf("%d, %d\n", play_index, music_file_num);
         //设置为列表循环播放
         if(play_index >= music_file_num) play_index = 0;
         if(play_index < 0) play_index = music_file_num - 1;
         //清除重绘制进度条的值
         InvalidateRect(MusicPlayer_hwnd, &rc_cli, TRUE);
         SendMessage(music_wnd_time, SBM_SETVALUE, TRUE, 0); //设置位置值
         //清除歌词记数
         lyriccount=0;
         I2S_Stop();   
         MP3FreeDecoder(Mp3Decoder);
         f_close(&file);	
			break;
		}	

		while(Isread==0)
		{
         //进度未调整
         if(chgsch == 0)
         {
            if(timecount>=20)
            {
               //当前值
               curtime = time_sum/1000; 
               //如果进入音乐列表，则不显示
               if(enter_flag == 0)
               {
                  //清除歌曲时间显示和歌词名字的显示
                  //ClrDisplay(hdc, &rc_MusicTimes, color);
                  //ClrDisplay(hdc, &rc_musicname, color);
                  //将字符数组转换为宽字符类型
                   //x_mbstowcs_cp936(wbuf, music_lcdlist[play_index], FILE_NAME_LEN);
//                  DrawText(hdc, wbuf, -1, &rc_musicname, DT_SINGLELINE | DT_CENTER | DT_VCENTER);//绘制文字
                  //将歌曲时间格式化输出到wbuf
//                  x_wsprintf(wbuf, L"%02d:%02d",curtime/60,curtime%60);
//                  DrawText(hdc, wbuf, -1, &rc_MusicTimes, DT_SINGLELINE | DT_CENTER | DT_VCENTER);//绘制文字
                  if(ooo == 0)//确保只会刷新一次
                  {
                     x_wsprintf(wbuf, L"%02d:%02d",alltime/60,alltime%60);
                     SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB1), wbuf);                
                     x_mbstowcs_cp936(wbuf, music_lcdlist[play_index], FILE_NAME_LEN);
                     SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB5), wbuf);       
                     ooo=1;
                  }
                  x_wsprintf(wbuf, L"%02d:%02d",curtime/60,curtime%60);
                  SetWindowText(GetDlgItem(MusicPlayer_hwnd, ID_TB2), wbuf);                       
                  //更新进度条
                  SendMessage(music_wnd_time, SBM_SETVALUE, TRUE, curtime*255/alltime);
                  InvalidateRect(MusicPlayer_hwnd, &rc_cli, FALSE);   
                  
                  
                  lrc.curtime = curtime;  
                  if(lrc.flag == 1){
                     //+100是提前显示，显示需要消耗一点时间
                     if((lrc.oldtime <= lrc.curtime*100+100)&&(lrc.indexsize>7))
                     {
                        //显示当前行的歌词
                        x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount]-1], LYRIC_MAX_SIZE);
                        SetWindowText(wnd_lrc3,wbuf);
                        //显示第i-1行的歌词（前一行）
                        if(lyriccount>0)
                        {
                           x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount-1]-1], LYRIC_MAX_SIZE);
                           SetWindowText(wnd_lrc2,wbuf);
                        }
                        else
                           SetWindowText(wnd_lrc2,L" ");
                        //显示第i-2行的歌词（前两行）
                        if(lyriccount>0)
                        {
                           x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount-2]-1], LYRIC_MAX_SIZE);
                           SetWindowText(wnd_lrc1,wbuf);
                        }
                        else
                           SetWindowText(wnd_lrc1,L" ");
                        //显示第i+1行的歌词（后一行）   
                        if(lyriccount < lrc.indexsize-1)
                        {
                           x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount+1]-1], LYRIC_MAX_SIZE);
                           SetWindowText(wnd_lrc4,wbuf);                    
                        }
                        else
                           SetWindowText(wnd_lrc4,L" ");
                        //显示第i+2行的歌词（后二行）   
                        if(lyriccount < lrc.indexsize-2)
                        {
                           x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount+2]-1], LYRIC_MAX_SIZE);
                           SetWindowText(wnd_lrc5,wbuf);                    
                        }
                        else
                           SetWindowText(wnd_lrc5,L" ");
                                  
                     do{
                        lyriccount++;					
                        if(lyriccount>=lrc.indexsize)
                        {
                           lrc.oldtime=0xffffff;
                           break;
                        }
                        lrc.oldtime=lrc.time_tbl[lyriccount];
                        }while(lrc.oldtime<=(lrc.curtime*100));
                     }                  
               
                  }
                  //找不到歌词文件
                  else
                  {
                     
                     SetWindowText(wnd_lrc3,L"请在SDCard放入相应的歌词文件(*.lrc)");
                     SetWindowText(wnd_lrc1,L" ");
                     SetWindowText(wnd_lrc2,L" ");
                     SetWindowText(wnd_lrc4,L" ");
                     SetWindowText(wnd_lrc5,L" ");
                  }
                  
               }
               
               timecount=0;
            }
         }
         else
         {
           uint8_t temp=0;	
               
           //根据进度条调整播放位置				
           temp=SendMessage(music_wnd_time, SBM_GETVALUE, NULL, NULL);        
               
           //计算进度条表示的时间
           time_sum = (float)alltime/255*temp*1000;  	
           //根据时间计算文件位置并跳转至该位置
           pos = ID3V2_size + (time_sum/26)*frame_size;
           result = f_lseek(&file,pos);
           lrc.oldtime=0;
           lyriccount=0;
           chgsch=0;           
         }
		}
		Isread=0;
      
      
      timecount++;
	}
   lyriccount=0;
	I2S_Stop();   
	MP3FreeDecoder(Mp3Decoder);
	f_close(&file);	
  if(time2exit == 1)
  {
    lyriccount=0;
    I2S_Stop();   
    MP3FreeDecoder(Mp3Decoder);
    f_close(&file);	 
    GUI_SemPost(exit_sem);
  }
}

/* DMA发送完成中断回调函数 */
/* 缓冲区内容已经播放完成，需要切换缓冲区，进行新缓冲区内容播放 
   同时读取WAV文件数据填充到已播缓冲区  */
void MP3Player_I2S_DMA_TX_Callback(void)
{
	if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //当前使用Memory1数据
	{
		bufflag=0;                       //可以将数据读取到缓冲区0
	}
	else                               //当前使用Memory0数据
	{
		bufflag=1;                       //可以将数据读取到缓冲区1
	}
	Isread=1;                          // DMA传输完成标志
}





/**
  * @brief  配置WM8978和STM32的I2S开始放音。
  * @param  无
  * @retval 无
  */

/**
  * @brief   WAV格式音频播放主程序
	* @note   
  * @param  无
  * @retval 无
  */
void wavplayer(const char *wavfile, uint8_t vol, HDC hdc, HWND hwnd)
{
	static uint8_t timecount;//记录时间
  WCHAR wbuf[128];
  char ooo = 0;
	mp3player.ucStatus=STA_IDLE;    /* 开始设置为空闲状态  */
	Recorder.ucFmtIdx=3;           /* 缺省飞利浦I2S标准，16bit数据长度，44K采样率  */
	Recorder.ucVolume=vol;          /* 缺省耳机音量  */
   
  DWORD pos;//记录文字变量
  static uint8_t lyriccount=0;//歌词index记录   
   
	/*  初始化并配置I2S  */
	I2S_Stop();
	I2S_GPIO_Config();
	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
	I2S_DMA_TX_Callback=MP3Player_I2S_DMA_TX_Callback;
	I2S_Play_Stop();
	
	bufflag=0;
	Isread=0;
   if(mp3player.ucStatus == STA_IDLE)
   {						
      printf("当前播放文件 -> %s\n",wavfile);
	
      result=f_open(&file,wavfile,FA_READ);
      if(result!=FR_OK)
      {
         printf("打开音频文件失败!!!->%d\r\n",result);
         result = f_close (&file);
         Recorder.ucStatus = STA_ERR;
         return;
      }
      //读取WAV文件头
      result = f_read(&file,&rec_wav,sizeof(rec_wav),&bw);
      
      //如果进入音乐列表就不显示时长
      if(enter_flag == 0){
         //获取屏幕（385，404）的颜色             
        
         mp3player.ucFreq =  rec_wav.dwSamplesPerSec;
         mp3player.ucbps =  mp3player.ucFreq*32;   
         alltime=file.fsize*8/mp3player.ucbps;
        
//         x_wsprintf(wbuf, L"00:00 / %02d:%02d",alltime/60,alltime%60);
//         //清除rc_MusicTimes矩形的内容
///         ClrDisplay(hdc, &rc_MusicTimes, color);
//         //绘制文本
//         DrawText(hdc, wbuf, -1, &rc_MusicTimes, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
      }   
      //先读取音频数据到缓冲区
      result = f_read(&file,(uint16_t *)buffer0,RECBUFFER_SIZE*2,&bw);
      result = f_read(&file,(uint16_t *)buffer1,RECBUFFER_SIZE*2,&bw);
      
      Delay_ms(10);	/* 延迟一段时间，等待I2S中断结束 */
      I2S_Stop();			/* 停止I2S录音和放音 */
      wm8978_Reset();		/* 复位WM8978到复位状态 */	

      mp3player.ucStatus = STA_PLAYING;		/* 放音状态 */

      /* 配置WM8978芯片，输入为DAC，输出为耳机 */
      
      WCHAR wbuf[3];
      HWND  wnd = GetDlgItem(hwnd, ID_RECORD_BUGLE);
      
      GetWindowText(wnd, wbuf, 3);
      if (wbuf[0] == L'P')    // 判断当前
      {
        wm8978_CfgAudioPath(DAC_ON, SPK_ON);                        // 配置为扬声器输出
      }
      else
      {
        wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);    // 配置为耳机输出
      }
     
      /* 调节音量，左右相同音量 */
      wm8978_SetOUT1Volume(Recorder.ucVolume);
      /* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
      wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
      
      I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
      I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
      
      I2Sxext_RX_DMA_Init(&recplaybuf[0],&recplaybuf[1],1);
      DMA_ITConfig(I2Sxext_RX_DMA_STREAM,DMA_IT_TC,DISABLE);//开启传输完成中断
      I2Sxext_Recorde_Stop();
      
      I2Sx_TX_DMA_Init(buffer0,buffer1,RECBUFFER_SIZE);		
      I2S_Play_Start();
   }
   /* 进入主程序循环体 */
   while(mp3player.ucStatus == STA_PLAYING){
   /* DMA传输完成 */
      if(Isread==1)
      {
         Isread=0;
         //修改进度条
         if(chgsch==0) 
         {  
           if(timecount>=10)      
           { 
               curtime=file.fptr*8/mp3player.ucbps;                                        //获取当前播放进度(单位：s)
               if(enter_flag == 0){
                  //清除歌曲时间显示和歌词名字的显示
//                  ClrDisplay(hdc, &rc_MusicTimes, color);
//                  ClrDisplay(hdc, &rc_musicname, color);
                  //将字符数组转换为宽字符类型
//                   x_mbstowcs_cp936(wbuf, music_lcdlist[play_index], FILE_NAME_LEN);
//                  DrawText(hdc, wbuf, -1, &rc_musicname, DT_SINGLELINE | DT_CENTER | DT_VCENTER);//绘制文字
                  //将歌曲时间格式化输出到wbuf
//                  x_wsprintf(wbuf, L"%02d:%02d",curtime/60,curtime%60,alltime/60,alltime%60);
//                  DrawText(hdc, wbuf, -1, &rc_MusicTimes, DT_SINGLELINE | DT_CENTER | DT_VCENTER);//绘制文字
                  //更新进度条
                  
                  if(ooo == 0)//确保只会刷新一次
                  {
                     x_wsprintf(wbuf, L"%02d:%02d",alltime/60,alltime%60);
                     SetWindowText(GetDlgItem(hwnd, ID_TB1), wbuf);                
//                     x_mbstowcs_cp936(wbuf, music_lcdlist[play_index], FILE_NAME_LEN);
//                     SetWindowText(GetDlgItem(hwnd, ID_TB5), wbuf);       
                     ooo=1;
                  }
                  x_wsprintf(wbuf, L"%02d:%02d",curtime/60,curtime%60);
                  SetWindowText(GetDlgItem(hwnd, ID_TB2), wbuf);  
                  
                  SendMessage(music_wnd_time, SBM_SETVALUE, TRUE, curtime*255/alltime);
                  InvalidateRect(music_wnd_time, NULL, TRUE);   
                  //InvalidateRect(GetDlgItem(hwnd, ID_TB2), NULL, TRUE); 

                  lrc.curtime = curtime;  
                  if(lrc.flag == 1){
                     //+100是提前显示，显示需要消耗一点时间
                     if((lrc.oldtime <= lrc.curtime*100+100)&&(lrc.indexsize>7))
                     {
                        //显示当前行的歌词
                        x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount]-1], LYRIC_MAX_SIZE);
                        SetWindowText(wnd_lrc3,wbuf);
                        //显示第i-1行的歌词（前一行）
                        if(lyriccount>0)
                        {
                           x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount-1]-1], LYRIC_MAX_SIZE);
                           SetWindowText(wnd_lrc2,wbuf);
                        }
                        else
                           SetWindowText(wnd_lrc2,L" ");
                        //显示第i-2行的歌词（前两行）
                        if(lyriccount>0)
                        {
                           x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount-2]-1], LYRIC_MAX_SIZE);
                           SetWindowText(wnd_lrc1,wbuf);
                        }
                        else
                           SetWindowText(wnd_lrc1,L" ");
                        //显示第i+1行的歌词（后一行）   
                        if(lyriccount < lrc.indexsize-1)
                        {
                           x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount+1]-1], LYRIC_MAX_SIZE);
                           SetWindowText(wnd_lrc4,wbuf);                    
                        }
                        else
                           SetWindowText(wnd_lrc4,L" ");
                        //显示第i+2行的歌词（后二行）   
                        if(lyriccount < lrc.indexsize-2)
                        {
                           x_mbstowcs_cp936(wbuf, (const char *)&ReadBuffer1[lrc.addr_tbl[lyriccount+2]-1], LYRIC_MAX_SIZE);
                           SetWindowText(wnd_lrc5,wbuf);                    
                        }
                        else
                           SetWindowText(wnd_lrc5,L" ");
                                  
                     do{
                        lyriccount++;					
                        if(lyriccount>=lrc.indexsize)
                        {
                           lrc.oldtime=0xffffff;
                           break;
                        }
                        lrc.oldtime=lrc.time_tbl[lyriccount];
                        }while(lrc.oldtime<=(lrc.curtime*100));
                     }                  
               
                  }
                  //找不到歌词文件
                  else
                  {
                     
                     SetWindowText(wnd_lrc3,L"请在SDCard放入相应的歌词文件(*.lrc)");
                     SetWindowText(wnd_lrc1,L" ");
                     SetWindowText(wnd_lrc2,L" ");
                     SetWindowText(wnd_lrc4,L" ");
                     SetWindowText(wnd_lrc5,L" ");
                  }                  
               }   
               
               timecount=0;  
           }                              
         } 
         else
         {
           uint8_t temp=0;
          
           temp=SendMessage(music_wnd_time, SBM_GETVALUE, NULL, NULL);  
           pos=file.fsize/255*temp;
           if(pos<sizeof(WavHead))pos=sizeof(WavHead);
           if(rec_wav.wBitsPerSample==24)temp=12;
           else temp=8;
           if((pos-sizeof(WavHead))%temp)
           {
             pos+=temp-(pos-sizeof(WavHead))%temp;
           }        
           f_lseek(&file,pos);
           lrc.oldtime=0;
           lyriccount=0;
           chgsch=0;         
         }
         timecount++;
         if(bufflag==0)
            result = f_read(&file,buffer0,RECBUFFER_SIZE*2,&bw);	
         else
            result = f_read(&file,buffer1,RECBUFFER_SIZE*2,&bw);
         /* 播放完成或读取出错停止工作 */
         if((result!=FR_OK)||(file.fptr==file.fsize))
         {
            //进入切歌状态
            mp3player.ucStatus=STA_SWITCH;
            //播放曲目自增1
            play_index++;
            //printf("%d, %d\n", play_index, music_file_num);
            //设置为列表循环播放
            if(play_index >= music_file_num) play_index = 0;
            if(play_index < 0) play_index = music_file_num - 1;
            printf("播放完或者读取出错退出...\r\n");
            I2S_Play_Stop();
            file.fptr=0;
            f_close(&file);
            I2S_Stop();		/* 停止I2S录音和放音 */
            wm8978_Reset();	/* 复位WM8978到复位状态 */							
         }		    
      }
   }
			
      mp3player.ucStatus = STA_SWITCH;		/* 待机状态 */
      file.fptr=0;
      f_close(&file);
      lrc.oldtime=0;
      lyriccount=0;      
      I2S_Stop();		/* 停止I2S录音和放音 */
      wm8978_Reset();	/* 复位WM8978到复位状态 */
		
	
}
/***************************** (END OF FILE) *********************************/
