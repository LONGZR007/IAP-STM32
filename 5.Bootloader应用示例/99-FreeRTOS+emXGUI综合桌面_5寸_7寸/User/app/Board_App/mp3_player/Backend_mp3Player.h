#ifndef __MP3PLAYER_H__
#define __MP3PLAYER_H__

#include <inttypes.h>
#include "emXGUI.h"


#define RECBUFFER_SIZE  1024*8
#define FMT_COUNT	6		/* 音频格式数组元素个数 */
/* 状态 */
enum
{
	STA_IDLE = 0,	    /* 待机状态 */
	STA_PLAYING,	    /* 放音状态 */
  STA_SWITCH,       /* 切歌状态 */
  STA_RECORDING,    /* 录音状态  */
	STA_ERR,          /*  error  */
};

typedef struct
{
	uint8_t ucVolume;			/* 当前放音音量 */
	uint8_t ucStatus;			/* 状态，0表示待机，1表示播放中，2 出错 */	
	uint32_t ucFreq;			/* 采样频率 */
     uint32_t ucbps;            /* 比特率 ：每秒传送多少个位 */
}MP3_TYPE;	

typedef struct
{
	uint8_t ucInput;			/* 输入源：0:MIC, 1:线输入 */
	uint8_t ucFmtIdx;			/* 音频格式：标准、位长、采样频率 */
	uint8_t ucVolume;			/* 当前放音音量 */
	uint8_t ucGain;			  /* 当前增益 */	
	uint8_t ucStatus;			/* 录音机状态，0表示待机，1表示录音中，2表示播放中 */	
}REC_TYPE;	

/* WAV文件头格式 */
typedef __packed struct
{ 
	uint32_t	riff;							/* = "RIFF"	0x46464952*///4
	uint32_t	size_8;						/* 从下个地址开始到文件尾的总字节数	*///4
	uint32_t	wave;							/* = "WAVE" 0x45564157*///4
	
	uint32_t	fmt;							/* = "fmt " 0x20746d66*///4
	uint32_t	fmtSize;					/* 下一个结构体的大小(一般为16) *///4	 
	uint16_t	wFormatTag;				/* 编码方式,一般为1	*///2
	uint16_t	wChannels;				/* 通道数，单声道为1，立体声为2 *///2
	uint32_t	dwSamplesPerSec;	/* 采样率 *///4
	uint32_t	dwAvgBytesPerSec;	/* 每秒字节数(= 采样率 × 每个采样点字节数) *///4
	uint16_t	wBlockAlign;			/* 每个采样点字节数(=量化比特数/8*通道数) *///2
	uint16_t	wBitsPerSample;		/* 量化比特数(每个采样需要的bit数) *///2
																			   
	uint32_t	data;							/* = "data" 0x61746164*///4
	uint32_t	datasize;					/* 纯数据长度 *///4
} WavHead;
extern MP3_TYPE mp3player;         /* mp3播放设备 */
void mp3PlayerDemo(HWND hwnd, const char *mp3file, uint8_t vol, uint8_t vol_horn, HDC hdc);
void wavplayer(const char *wavfile, uint8_t vol, HDC hdc, HWND hwnd);
extern int time2exit;
extern GUI_SEM *exit_sem;
#endif  /* __MP3PLAYER_H__   */

