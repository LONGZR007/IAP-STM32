#ifndef __RECORD_H__
#define __RECORD_H__

#include <inttypes.h>
#include "Backend_mp3Player.h"

/* RECBUFFER_SIZE决定两个缓存区大小，实际每个缓存区占用RAM空间字节数为：RECBUFFER_SIZE*2
 * 对于选择I2S_AudioFreq_16k采样率使用RECBUFFER_SIZE为1024*4，就可保证录音及放音效果流畅，
 * 如果选择其他更高采样率需要适当调整用RECBUFFER_SIZE大小，比如当选择择I2S_AudioFreq_44k
 * 是设置RECBUFFER_SIZE为1024*8
 * 另外，SD卡本身的读写速度也在一定性能上影响录音效果
 */
#define RECBUFFER_SIZE  1024*8

/* 录音文件存放路径 */
#define RECORDERDIR	"0:/recorder"

///* 录音机状态 */
//enum
//{
//	STA_IDLE = 0,  	/* 待机状态 */
//	STA_RECORDING,	/* 录音状态 */
//	STA_PLAYING,	  /* 放音状态 */
//	STA_ERR,		  	/*  error  */
//};

//typedef struct
//{
//	uint8_t ucInput;			/* 输入源：0:MIC, 1:线输入 */
//	uint8_t ucFmtIdx;			/* 音频格式：标准、位长、采样频率 */
//	uint8_t ucVolume;			/* 当前放音音量 */
//	uint8_t ucGain;			  /* 当前增益 */	
//	uint8_t ucStatus;			/* 录音机状态，0表示待机，1表示录音中，2表示播放中 */	
//}REC_TYPE;	

///* WAV文件头格式 */
//typedef __packed struct
//{ 
//	uint32_t	riff;							/* = "RIFF"	0x46464952*/
//	uint32_t	size_8;						/* 从下个地址开始到文件尾的总字节数	*/
//	uint32_t	wave;							/* = "WAVE" 0x45564157*/
//	
//	uint32_t	fmt;							/* = "fmt " 0x20746d66*/
//	uint32_t	fmtSize;					/* 下一个结构体的大小(一般为16) */
//	uint16_t	wFormatTag;				/* 编码方式,一般为1	*/
//	uint16_t	wChannels;				/* 通道数，单声道为1，立体声为2 */
//	uint32_t	dwSamplesPerSec;	/* 采样率 */
//	uint32_t	dwAvgBytesPerSec;	/* 每秒字节数(= 采样率 × 每个采样点字节数) */
//	uint16_t	wBlockAlign;			/* 每个采样点字节数(=量化比特数/8*通道数) */
//	uint16_t	wBitsPerSample;		/* 量化比特数(每个采样需要的bit数) */
//																			  
//	uint32_t	data;							/* = "data" 0x61746164*/
//	uint32_t	datasize;					/* 纯数据长度 */
//} WavHead;	

void RecorderDemo(void);
void StartRecord(const char *filename);

#endif  /* __RECORD_H__   */

