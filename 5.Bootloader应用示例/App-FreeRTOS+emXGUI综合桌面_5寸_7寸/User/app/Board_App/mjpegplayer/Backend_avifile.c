#include "./mjpegplayer/Backend_avifile.h"
#include <stdio.h>
#include <string.h>

AVI_TypeDef   AVI_file;
avih_TypeDef* avihChunk;
strh_TypeDef* strhChunk;
BMPINFO*   bmpinfo;
WAVEFORMAT*   wavinfo;
uint32_t      temp=0x00;
uint8_t       vids_ID;
uint8_t       auds_ID;

uint8_t AVI_Parser(uint8_t *buffer)
{
	temp=ReadUnit(buffer,0,4,1);//读"RIFF"
	if(temp!=RIFF_ID)return 1;
	AVI_file.RIFFchunksize=ReadUnit(buffer,4,4,1);//RIFF数据块长度
	temp=ReadUnit(buffer,8,4,1);//读"AVI "
	if(temp!=AVI_ID)return 2;
	temp=ReadUnit(buffer,12,4,1);//读"LIST"
	if(temp!=LIST_ID)return 3;
	AVI_file.LISTchunksize=ReadUnit(buffer,16,4,1);//LIST数据块长度
	temp=ReadUnit(buffer,20,4,1);//读"hdrl"
	if(temp!=hdrl_ID)return 4;
	temp=ReadUnit(buffer,24,4,1);//读"avih"
	if(temp!=avih_ID)return 5;
	AVI_file.avihsize=ReadUnit(buffer,28,4,1);//avih数据块长度	
	return 0;				
}

uint8_t Avih_Parser(uint8_t *buffer)
{
	avihChunk=(avih_TypeDef*)buffer;
#if 0
	printf("\r\navih数据块信息:");
	printf("\r\nSecPerFrame:%ld",avihChunk->SecPerFrame);//显示每一帧所需的时间us，
	printf("\r\nMaxByteSec:%ld",avihChunk->MaxByteSec);//最大的数据传输率
	printf("\r\nChunkBase:%ld",avihChunk->ChunkBase);//
	printf("\r\nSpecProp:%ld",avihChunk->SpecProp);//
	printf("\r\nTotalFrame:%ld",avihChunk->TotalFrame);//文件中的总帧数
	printf("\r\nInitFrames:%ld",avihChunk->InitFrames);//在开始播放前需要多少帧
	printf("\r\nStreams:%ld",avihChunk->Streams);//数据流个数（视频流和音乐流）
	printf("\r\nRefBufSize:%ld",avihChunk->RefBufSize);//缓冲区的大小
	printf("\r\nWidth:%ld",avihChunk->Width);//图片的宽度
	printf("\r\nHeight:%ld\n",avihChunk->Height);//图片的高度
#endif
	if((avihChunk->Width>800)||(avihChunk->Height>480))return 1;//视频尺寸不支持
	if(avihChunk->Streams!=2)return 2;//视频流数不支持
	return 0;
}

uint8_t Strl_Parser(uint8_t *buffer)
{
	temp=ReadUnit(buffer,0,4,1);//读"LIST"
	if(temp!=LIST_ID)return 1;
	AVI_file.strlsize=ReadUnit(buffer,4,4,1);//strl数据块长度
	temp=ReadUnit(buffer,8,4,1);//读"strl"
	if(temp!=strl_ID)return 2;
	temp=ReadUnit(buffer,12,4,1);//读"strh"
	if(temp!=strh_ID)return 3;
	AVI_file.strhsize=ReadUnit(buffer,16,4,1);//strh数据块长度
	strhChunk=(strh_TypeDef*)(buffer+20);		 //108
#if 0
	printf("\r\nstrh数据块信息:");	
	printf("\r\nStreamType:%s",strhChunk->StreamType);//数据流类型（视频数据流or音频数据流）
	printf("\r\nHandler:%s",strhChunk->Handler);//编码类型MJPEG
	printf("\r\nStreamFlag:%ld",strhChunk->StreamFlag);//数据流属性
	printf("\r\nPriority:%d",strhChunk->Priority);//数据流的播放优先级
	printf("\r\nLanguage:%d",strhChunk->Language);//语言
	printf("\r\nInitFrames:%ld",strhChunk->InitFrames);//开始播放前需要多少帧
	printf("\r\nScale:%ld",strhChunk->Scale);//数据量，每一帧数据的大小或者是音频的采样大小
	printf("\r\nRate:%ld",strhChunk->Rate);//每秒的采样率
	printf("\r\nStart:%ld",strhChunk->Start);//数据流开始播放的位置
	printf("\r\nLength:%ld",strhChunk->Length);//数据流的数据量
	printf("\r\nRefBufSize:%ld",strhChunk->RefBufSize);//缓冲区的大小
	printf("\r\nQuality:%ld",strhChunk->Quality);//解压缩质量参数，值越大，质量越好
	printf("\r\nSampleSize:%ld",strhChunk->SampleSize);//音频的采样大小
   //视频图像所占的矩形
	printf("\r\nFrameLeft:%d",strhChunk->Frame.Left);
	printf("\r\nFrameTop:%d",strhChunk->Frame.Top);
	printf("\r\nFrameRight:%d",strhChunk->Frame.Right);
	printf("\r\nFrameBottom:%d\n",strhChunk->Frame.Bottom);
#endif
	if(strhChunk->Handler[0]!='M')return 4;
	return 0;
}
int _img_w, _img_h;
uint8_t Strf_Parser(uint8_t *buffer)
{
	temp=ReadUnit(buffer,0,4,1);//读"strf"
	if(temp!=strf_ID)return 1;
	if(strhChunk->StreamType[0]=='v')//第一个流为视频流
	{
		vids_ID='0';
		auds_ID='1';
		bmpinfo=(BMPINFO*)(buffer+8);
		wavinfo=(WAVEFORMAT*)(buffer+4332);
	}
	else if(strhChunk->StreamType[0]=='a')//第一个流为音频流
	{
		vids_ID='1';
		auds_ID='0';
		wavinfo=(WAVEFORMAT*)(buffer+8);
		bmpinfo=(BMPINFO*)(buffer+4332);
	}
   
   _img_w = bmpinfo->bmiHeader.Width;
   _img_h = bmpinfo->bmiHeader.Height;
#if 0		
	printf("\r\nstrf数据块信息(视频流):");		
	printf("\r\n本结构体大小:%ld",bmpinfo->bmiHeader.Size);
	printf("\r\n图像宽:%ld",bmpinfo->bmiHeader.Width);
	printf("\r\n图像高:%ld",bmpinfo->bmiHeader.Height);
	printf("\r\n平面数:%d",bmpinfo->bmiHeader.Planes);
	printf("\r\n像素位数:%d",bmpinfo->bmiHeader.BitCount);
	printf("\r\n压缩类型:%s",bmpinfo->bmiHeader.Compression);
	printf("\r\n图像大小:%ld",bmpinfo->bmiHeader.SizeImage);
	printf("\r\n水平分辨率:%ld",bmpinfo->bmiHeader.XpixPerMeter);
	printf("\r\n垂直分辨率:%ld",bmpinfo->bmiHeader.YpixPerMeter);
	printf("\r\n使用调色板颜色数:%ld",bmpinfo->bmiHeader.ClrUsed);
	printf("\r\n重要颜色:%ld",bmpinfo->bmiHeader.ClrImportant);

	printf("\r\nstrf数据块信息(音频流):\n");
	printf("\r\n格式标志:%d",wavinfo->FormatTag);
	printf("\r\n声道数:%d",wavinfo->Channels);
	printf("\r\n采样率:%ld",wavinfo->SampleRate);
	printf("\r\n波特率:%ld",wavinfo->BaudRate);
	printf("\r\n块对齐:%d",wavinfo->BlockAlign);
	printf("\r\n本结构体大小:%d\n",wavinfo->Size);
#endif
	return 0;
}

uint16_t Search_Movi(uint8_t* buffer)
{
	uint16_t i;
	for(i=0;i<20480;i++)
	{
	   	if(buffer[i]==0x6d)
			if(buffer[i+1]==0x6f)
				if(buffer[i+2]==0x76)	
					if(buffer[i+3]==0x69)return i;//找到"movi"	
	}
	return 0;		
}

uint16_t Search_Fram(uint8_t* buffer,uint32_t size)
{
	uint16_t i;
	for(i=0;i<size;i++)
	{
	   	if(buffer[i]=='0')
			if(buffer[i+1]==vids_ID)
				if(buffer[i+2]=='d')	
					if(buffer[i+3]=='c')
                  return i;//找到"xxdc"	
	}
	return 0;		
}
uint16_t Search_Auds(uint8_t* buffer)
{
	uint16_t i;
	for(i=0;i<20480;i++)
	{
	   	if(buffer[i]=='0')
			if(buffer[i+1]==auds_ID)
				if(buffer[i+2]=='w')	
					if(buffer[i+3]=='b')return i;//找到"xxwb"	
	}
	return 0;		
}

uint32_t ReadUnit(uint8_t *buffer,uint8_t index,uint8_t Bytes,uint8_t Format)//1:大端模式;0:小端模式
{
  	uint8_t off=0;
  	uint32_t unit=0;  
  	for(off=0;off<Bytes;off++)unit|=buffer[off+index]<<(off*8);
  	if(Format)unit=__REV(unit);//大端模式
  	return unit;
}


