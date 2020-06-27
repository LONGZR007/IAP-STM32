/*
******************************************************************************
* @file    recorder.c
* @author  fire
* @version V1.0
* @date    2015-xx-xx
* @brief   WM8978录音功能测试
******************************************************************************
* @attention
*
* 实验平台:野火  STM32 F429 开发板  
* 论坛    :http://www.firebbs.cn
* 淘宝    :https://fire-stm32.taobao.com
*
******************************************************************************
*/
#include "Bsp/usart/bsp_debug_usart.h"
#include "bsp/key/bsp_key.h" 
#include "Bsp/wm8978/bsp_wm8978.h"
#include "ff.h" 
#include "Backend_Recorder.h"
#include "./mp3_player/Backend_mp3Player.h"


/* 音频格式切换列表(可以自定义) */
#define FMT_COUNT	6		/* 音频格式数组元素个数 */

/* 录音文件路径全称：初始化为rec001.wav */
char recfilename[25]={"0:/recorder/rec001.wav"};   
extern REC_TYPE Recorder;          /* 录音设备 */
WavHead rec_wav;            /* WAV设备  */
uint8_t Isread=0;           /* DMA传输完成标志 */
uint8_t bufflag=0;          /* 数据缓存区选择标志 */
uint32_t wavsize=0;         /* wav音频数据大小 */
uint16_t record_buffer0[RECBUFFER_SIZE] __EXRAM;  /* 数据缓存区1 ，实际占用字节数：RECBUFFER_SIZE*2 */
uint16_t record_buffer1[RECBUFFER_SIZE] __EXRAM;  /* 数据缓存区2 ，实际占用字节数：RECBUFFER_SIZE*2 */

FIL record_file __EXRAM;			/* file objects */
extern FRESULT result; 
extern UINT bw;            					/* File R/W count */

extern uint32_t g_FmtList[FMT_COUNT][3];

extern const uint16_t recplaybuf[4];//2个16位数据,用于录音时I2S Master发送.循环发送0.

/* 仅允许本文件内调用的函数声明 */
void Recorder_I2S_DMA_RX_Callback(void);

///**
//  * @brief   WAV格式音频播放主程序
//	* @note   
//  * @param  无
//  * @retval 无
//  */
//void RecorderDemo(void)
//{
//	uint8_t i;
//	uint8_t ucRefresh;	/* 通过串口打印相关信息标志 */
//	DIR dir;
//	
//	Recorder.ucStatus=STA_IDLE;    /* 开始设置为空闲状态  */
//	Recorder.ucInput=0;            /* 缺省MIC输入  */
//	Recorder.ucFmtIdx=3;           /* 缺省飞利浦I2S标准，16bit数据长度，44K采样率  */
//	Recorder.ucVolume=35;          /* 缺省耳机音量  */
//	if(Recorder.ucInput==0) //MIC 
//	{
//		Recorder.ucGain=50;          /* 缺省MIC增益  */
//		rec_wav.wChannels=2;         /* 缺省MIC单通道 */
//	}
//	else                    //LINE
//	{
//		Recorder.ucGain=6;           /* 缺省线路输入增益 */
//		rec_wav.wChannels=2;         /* 缺省线路输入双声道 */
//	}
//	
//	rec_wav.riff=0x46464952;       /* “RIFF”; RIFF 标志 */
//	rec_wav.size_8=0;              /* 文件长度，未确定 */
//	rec_wav.wave=0x45564157;       /* “WAVE”; WAVE 标志 */ 
//	
//	rec_wav.fmt=0x20746d66;        /* “fmt ”; fmt 标志，最后一位为空 */
//	rec_wav.fmtSize=16;            /* sizeof(PCMWAVEFORMAT) */ 
//	rec_wav.wFormatTag=1;          /* 1 表示为PCM 形式的声音数据 */ 
//	/* 每样本的数据位数，表示每个声道中各个样本的数据位数。 */
//	rec_wav.wBitsPerSample=16;
//	/* 采样频率（每秒样本数） */
//	rec_wav.dwSamplesPerSec=g_FmtList[Recorder.ucFmtIdx][2];
//	/* 每秒数据量；其值为通道数×每秒数据位数×每样本的数据位数／ 8。 */
//	rec_wav.dwAvgBytesPerSec=rec_wav.wChannels*rec_wav.dwSamplesPerSec*rec_wav.wBitsPerSample/8;  
//	/* 数据块的调整数（按字节算的），其值为通道数×每样本的数据位值／8。 */
//	rec_wav.wBlockAlign=rec_wav.wChannels*rec_wav.wBitsPerSample/8; 
//	
//	rec_wav.data=0x61746164;       /* “data”; 数据标记符 */
//	rec_wav.datasize=0;            /* 语音数据大小 目前未确定*/
//	
//	/*  如果路径不存在，创建文件夹  */
//	result = f_opendir(&dir,RECORDERDIR);
//	while(result != FR_OK)
//	{
//		f_mkdir(RECORDERDIR);
//		result = f_opendir(&dir,RECORDERDIR);
//	}	
//	
//	/*  初始化并配置I2S  */
//	I2S_Stop();
//	I2S_GPIO_Config();
//	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
//	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
//	
//	I2S_DMA_TX_Callback=MusicPlayer_I2S_DMA_TX_Callback;
//	I2S_Play_Stop();
//	
//	I2S_DMA_RX_Callback=Recorder_I2S_DMA_RX_Callback;
//	I2Sxext_Recorde_Stop();
//	
//	ucRefresh = 1;
//	bufflag=0;
//	Isread=0;
//	/* 进入主程序循环体 */
//	while (1)
//	{
//		/* 如果使能串口打印标志则打印相关信息 */
//		if (ucRefresh == 1)
//		{						
//			DispStatus();		/* 显示当前状态，频率，音量等 */
//			ucRefresh = 0;
//		}
//		if(Recorder.ucStatus == STA_IDLE)
//		{				
//			/*  KEY2开始录音  */
//			if(Key_Scan(KEY2_GPIO_PORT,KEY2_PIN)==KEY_ON)
//			{
//				/* 寻找合适文件名 */
//				for(i=1;i<0xff;++i)
//				{
//					sprintf(recfilename,"0:/recorder/rec%03d.wav",i);
//					result=f_open(&record_file,(const TCHAR *)recfilename,FA_READ);
//					if(result==FR_NO_FILE)break;					
//				}
//				f_close(&record_file);
//				
//				if(i==0xff)
//				{
//					Recorder.ucStatus =STA_ERR;
//					continue;
//				}
//				/* 开始录音 */
//				StartRecord(recfilename);
//				ucRefresh = 1;
//			}
//			/*  TouchPAD开始回放录音  */
//			if(TPAD_Scan(0))
//			{			
//				/* 开始回放 */
//				StartPlay(recfilename);
//				ucRefresh = 1;				
//			}
//		}
//		else
//		{			
//			/*  KEY1停止录音或回放  */
//			if(Key_Scan(KEY1_GPIO_PORT,KEY1_PIN)==KEY_ON)
//			{
//				/* 对于录音，需要把WAV文件内容填充完整 */
//				if(Recorder.ucStatus == STA_RECORDING)
//				{
//					I2Sxext_Recorde_Stop();
//					I2S_Play_Stop();
//					rec_wav.size_8=wavsize+36;
//					rec_wav.datasize=wavsize;
//					result=f_lseek(&record_file,0);
//					result=f_write(&record_file,(const void *)&rec_wav,sizeof(rec_wav),&bw);
//					result=f_close(&record_file);
//					printf("录音结束\r\n");
//				}
//				ucRefresh = 1;
//				Recorder.ucStatus = STA_IDLE;		/* 待机状态 */
//				I2S_Stop();		/* 停止I2S录音和放音 */
//				wm8978_Reset();	/* 复位WM8978到复位状态 */
//			}
//		}
//		/* DMA传输完成 */
//		if(Isread==1)
//		{
//			Isread=0;
//			switch (Recorder.ucStatus)
//			{
//				case STA_RECORDING:  // 录音功能，写入数据到文件
//						if(bufflag==0)
//							result=f_write(&record_file,record_buffer0,RECBUFFER_SIZE*2,(UINT*)&bw);//写入文件							
//						else
//							result=f_write(&record_file,record_buffer1,RECBUFFER_SIZE*2,(UINT*)&bw);//写入文件
//						wavsize+=RECBUFFER_SIZE*2;	
//					break;
//				case STA_PLAYING:   // 回放功能，读取数据到播放缓冲区
//						if(bufflag==0)
//							result = f_read(&record_file,buffer0,RECBUFFER_SIZE*2,&bw);	
//						else
//							result = f_read(&record_file,buffer1,RECBUFFER_SIZE*2,&bw);
//						/* 播放完成或读取出错停止工作 */
//						if((result!=FR_OK)||(record_file.fptr==record_file.fsize))
//						{
//							printf("播放完或者读取出错退出...\r\n");
//							I2S_Play_Stop();
//							record_file.fptr=0;
//							f_close(&record_file);
//							Recorder.ucStatus = STA_IDLE;		/* 待机状态 */
//							I2S_Stop();		/* 停止I2S录音和放音 */
//							wm8978_Reset();	/* 复位WM8978到复位状态 */							
//						}		
//					break;
//			}			
//		}
//		
//	}
//}

/**
  * @brief  配置WM8978和STM32的I2S开始录音。
  * @param  无
  * @retval 无
  */
void StartRecord(const char *filename)
{
	uint8_t ucRefresh;	/* 通过串口打印相关信息标志 */
//	DIR dir;
	
	Recorder.ucStatus=STA_IDLE;    /* 开始设置为空闲状态  */
	Recorder.ucInput=0;            /* 缺省MIC输入  */
	Recorder.ucFmtIdx=3;           /* 缺省飞利浦I2S标准，16bit数据长度，44K采样率  */
	Recorder.ucVolume=35;          /* 缺省耳机音量  */
	if(Recorder.ucInput==0) //MIC 
	{
		Recorder.ucGain=50;          /* 缺省MIC增益  */
		rec_wav.wChannels=2;         /* 缺省MIC单通道 */
	}
	else                    //LINE
	{
		Recorder.ucGain=6;           /* 缺省线路输入增益 */
		rec_wav.wChannels=2;         /* 缺省线路输入双声道 */
	}
	
	rec_wav.riff=0x46464952;       /* “RIFF”; RIFF 标志 */
	rec_wav.size_8=0;              /* 文件长度，未确定 */
	rec_wav.wave=0x45564157;       /* “WAVE”; WAVE 标志 */ 
	
	rec_wav.fmt=0x20746d66;        /* “fmt ”; fmt 标志，最后一位为空 */
	rec_wav.fmtSize=16;            /* sizeof(PCMWAVEFORMAT) */ 
	rec_wav.wFormatTag=1;          /* 1 表示为PCM 形式的声音数据 */ 
	/* 每样本的数据位数，表示每个声道中各个样本的数据位数。 */
	rec_wav.wBitsPerSample=16;
	/* 采样频率（每秒样本数） */
	rec_wav.dwSamplesPerSec=g_FmtList[Recorder.ucFmtIdx][2];
	/* 每秒数据量；其值为通道数×每秒数据位数×每样本的数据位数／ 8。 */
	rec_wav.dwAvgBytesPerSec=rec_wav.wChannels*rec_wav.dwSamplesPerSec*rec_wav.wBitsPerSample/8;  
	/* 数据块的调整数（按字节算的），其值为通道数×每样本的数据位值／8。 */
	rec_wav.wBlockAlign=rec_wav.wChannels*rec_wav.wBitsPerSample/8; 
	
	rec_wav.data=0x61746164;       /* “data”; 数据标记符 */
	rec_wav.datasize=0;            /* 语音数据大小 目前未确定*/
	
	/*  初始化并配置I2S  */
	I2S_Stop();
	I2S_GPIO_Config();
	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
	I2S_DMA_RX_Callback=Recorder_I2S_DMA_RX_Callback;
	I2Sxext_Recorde_Stop();
	
	ucRefresh = 1;
	bufflag=0;
	Isread=0;
  
	printf("当前录音文件 -> %s\n",filename);
	result=f_open(&record_file,filename,FA_CREATE_ALWAYS|FA_WRITE);
	if(result!=FR_OK)
	{
		printf("Open wavfile fail!!!->%d\r\n",result);
		result = f_close (&record_file);
		Recorder.ucStatus = STA_ERR;
		return;
	}
	
	// 写入WAV文件头，这里必须写入写入后文件指针自动偏移到sizeof(rec_wav)位置，
	// 接下来写入音频数据才符合格式要求。
	result=f_write(&record_file,(const void *)&rec_wav,sizeof(rec_wav),&bw);
	
	GUI_msleep(10);		/* 延迟一段时间，等待I2S中断结束 */
	I2S_Stop();			/* 停止I2S录音和放音 */
	wm8978_Reset();		/* 复位WM8978到复位状态 */

	Recorder.ucStatus = STA_RECORDING;		/* 录音状态 */
		
	/* 调节放音音量，左右相同音量 */
	wm8978_SetOUT1Volume(Recorder.ucVolume);

	if(Recorder.ucInput == 1)   /* 线输入 */
	{
		/* 配置WM8978芯片，输入为线输入，输出为耳机 */
		wm8978_CfgAudioPath(LINE_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
		wm8978_SetLineGain(Recorder.ucGain);
	}
	else   /* MIC输入 */
	{
		/* 配置WM8978芯片，输入为Mic，输出为耳机 */
		//wm8978_CfgAudioPath(MIC_LEFT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
		//wm8978_CfgAudioPath(MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
		wm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);	
		wm8978_SetMicGain(Recorder.ucGain);	
	}
		
	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);

	I2Sx_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	I2Sxext_Mode_Config(g_FmtList[Recorder.ucFmtIdx][0],g_FmtList[Recorder.ucFmtIdx][1],g_FmtList[Recorder.ucFmtIdx][2]);
	
	I2Sx_TX_DMA_Init(&recplaybuf[0],&recplaybuf[1],1);
	DMA_ITConfig(I2Sx_TX_DMA_STREAM,DMA_IT_TC,DISABLE);//开启传输完成中断
	
	I2S_DMA_RX_Callback=Recorder_I2S_DMA_RX_Callback;
	I2Sxext_RX_DMA_Init(record_buffer0,record_buffer1,RECBUFFER_SIZE);
  	
	I2S_Play_Start();
	I2Sxext_Recorde_Start();
}

///**
//  * @brief  显示当前状态
//  * @param  无
//  * @retval 无
//  */
//static void DispStatus(void)
//{
//	char buf[100];

//	printf("\n    按下KEY2键开始录音    \n");
//	printf(  "   按下电容按键开始回放    \n");	
//	printf(  "      按下KEY1键结束    \n\r");
//	/* 显示当前音频格式 */
//	sprintf(buf, "音频格式: Philips,16Bit,%d.%dkHz     ",
//		g_FmtList[Recorder.ucFmtIdx][2]/1000,
//		(g_FmtList[Recorder.ucFmtIdx][2]%1000)/100
//		);
//	printf("%s\n",buf);

//	if (Recorder.ucStatus == STA_IDLE)
//	{
//		printf("状态 = 空闲    ");
//	}
//	else if (Recorder.ucStatus == STA_RECORDING)
//	{
//		printf("状态 = 正在录音");
//	}
//	else if (Recorder.ucStatus == STA_PLAYING)
//	{
//		printf("状态 = 正在回放");
//	}

//	sprintf(buf, "  增益 = %d ", Recorder.ucGain);
//	printf("%s\n",buf);

//	sprintf(buf, "耳机音量 = %d \r", Recorder.ucVolume);
//	printf("%s\n",buf);
//}

/* DMA接收完成中断回调函数 */
/* 录音数据已经填充满了一个缓冲区，需要切换缓冲区，
   同时可以把已满的缓冲区内容写入到文件中 */
void Recorder_I2S_DMA_RX_Callback(void)
{
	if(Recorder.ucStatus == STA_RECORDING)
	{	
		if(I2Sxext_RX_DMA_STREAM->CR&(1<<19)) //当前使用Memory1数据
		{
			bufflag=0;
		}
		else                                 //当前使用Memory0数据
		{
			bufflag=1;
		}
		Isread=1;                            // DMA传输完成标志
	}
}


/***************************** (END OF FILE) *********************************/
