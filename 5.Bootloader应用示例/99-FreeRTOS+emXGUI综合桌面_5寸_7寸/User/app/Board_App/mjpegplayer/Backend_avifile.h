#ifndef __AVIFILE_H
#define __AVIFILE_H

#include "stm32f4xx.h"
#include "ff.h"

//#define DEBUGINFO	//信息打印开关

#define RIFF_ID		0x52494646
#define AVI_ID		0x41564920
#define LIST_ID		0x4c495354
#define hdrl_ID		0x6864726c
#define avih_ID		0x61766968
#define strl_ID		0x7374726c
#define strh_ID		0x73747268
#define strf_ID		0x73747266
#define movi_ID		0x6d6f7669

typedef struct
{	
  DWORD  RIFFchunksize;
  DWORD  LISTchunksize;
	DWORD  avihsize;
	DWORD  strlsize;
	DWORD  strhsize;
}AVI_TypeDef;

typedef struct
{	
	DWORD  SecPerFrame;	//显示每帧的时间
	DWORD  MaxByteSec; 	//最大数据传输率
	DWORD  ChunkBase; 	//块长度为此值的倍数，通常为2048
	DWORD  SpecProp;	  //AVI文件的特殊属性
	DWORD  TotalFrame;	//文件总帧数
	DWORD  InitFrames;  //开始播放前的初始化帧数
	DWORD  Streams;		  //包含的数据流种类
	DWORD  RefBufSize;  //建议使用的缓冲区大小，通常为存储一帧图像与同步音频数据之和
	DWORD  Width;		    //图像宽
	DWORD  Height;		  //图像高
	DWORD  Reserved;	  //保留
}avih_TypeDef;

typedef struct
{	
	uint8_t StreamType[4];//数据流种类，vids(0x73646976):视频,auds:音频
	uint8_t Handler[4];	  //数据解压缩的驱动程序代号
	DWORD   StreamFlag;   //数据流属性
	WORD    Priority;	    //此数据流播放的优先级
	WORD    Language;	    //音频的语言代号
	DWORD   InitFrames;   //开始播放前的初始化帧数
	DWORD   Scale;		    //视频每帧的大小或音频的采样率
	DWORD   Rate; 		    //Scale/Rate=每秒采样数
	DWORD   Start;		    //数据流开始播放的位置，单位为Scale
	DWORD   Length;		    //数据流的数据量，单位为Scale
 	DWORD   RefBufSize;   //建议使用的缓冲区大小
  DWORD   Quality;		  //解压缩质量参数，值越大，质量越好
	DWORD   SampleSize;	  //音频的样本大小
	struct{				
	  short int  Left;
		short int  Top;
		short int  Right;
		short int  Bottom;
	}Frame;				        //视频帧所占的矩形 
}strh_TypeDef;

typedef struct tagBMPHEADER
{
  DWORD  	Size;		      //本结构所占用字节数
	long   	Width;		    //图像宽
	long   	Height;		    //图像高
	WORD   	Planes;		    //平面数，必须为1
	WORD   	BitCount;	    //像素位数
	u8  	Compression[4];	//压缩类型，MJPEG
	DWORD  	SizeImage;	  //图像大小
	long   	XpixPerMeter; //水平分辨率
	long   	YpixPerMeter; //垂直分辨率
	DWORD  	ClrUsed;		  //实际使用了调色板中的颜色数,压缩格式中不使用
	DWORD  	ClrImportant; //重要的颜色
}BMPHEADER;             //该结构占用40字节

typedef struct tagRGBQUAD 
{
	BYTE  rgbBlue;	      //蓝色的亮度(值范围为0-255)
	BYTE  rgbGreen;       //绿色的亮度(值范围为0-255)
	BYTE  rgbRed; 	      //红色的亮度(值范围为0-255)
	BYTE  rgbReserved;    //保留，必须为0
}RGBQUAD;               //颜色表

typedef struct tagBMPINFO 
{
	BMPHEADER  bmiHeader;  //位图信息头
	RGBQUAD    bmColors[1];//颜色表
}BMPINFO;  

typedef struct 
{
  WORD  FormatTag;
	WORD  Channels;	  //声道数
	DWORD SampleRate; //采样率
	DWORD BaudRate;   //波特率
	WORD  BlockAlign; //数据块对齐标志
	WORD  Size;       //该结构大小
}WAVEFORMAT;

#define	 MAKEWORD(ptr)	(WORD)(((WORD)*((BYTE*)(ptr))<<8)|(WORD)*(BYTE*)((ptr)+1))
#define  MAKEDWORD(ptr)	(DWORD)(((WORD)*(BYTE*)(ptr)|(((WORD)*(BYTE*)(ptr+1))<<8)|\
						(((WORD)*(BYTE*)(ptr+2))<<16)|(((WORD)*(BYTE*)(ptr+3))<<24))) 


uint8_t  AVI_Parser(uint8_t *buffer);
uint8_t  Avih_Parser(uint8_t *buffer);
uint8_t  Strl_Parser(uint8_t *buffer);
uint8_t  Strf_Parser(uint8_t *buffer);
uint16_t Search_Movi(uint8_t* buffer);
uint16_t Search_Fram(uint8_t* buffer,uint32_t size);
uint16_t Search_Auds(uint8_t* buffer);
uint32_t ReadUnit(uint8_t *buffer,uint8_t index,uint8_t Bytes,uint8_t Format);
extern int _img_w, _img_h;
#endif

