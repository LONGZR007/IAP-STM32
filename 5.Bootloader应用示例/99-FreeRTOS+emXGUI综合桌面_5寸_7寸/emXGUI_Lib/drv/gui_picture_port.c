/**
  *********************************************************************
  * @file    gui_picture_port.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   图片显示接口
  *********************************************************************
  * @attention
  * 官网    :www.emXGUI.com
  *
  **********************************************************************
  */ 

#include <string.h>
#include	"emXGUI.h"
#include  "gui_drv_cfg.h"
#include  "gui_mem_port.h"
#include  "gui_resource_port.h"
#include  "gui_picture_port.h"




/*===================================================================================*/
/**
  * @brief  从流媒体加载内容的回调函数（FLASH 非文件系统区）
  * @param  buf[out] 存储读取到的数据缓冲区
  * @param  offset 要读取的位置
  * @param  size 要读取的数据大小
  * @param  lParam 调用函数时的自定义参数（用户参数）
  * @retval 读取到的数据大小
  */
static int bmp_read_data_Res(void *buf,int offset,int size,LPARAM lParam)
{
	offset += lParam;
	RES_DevRead(buf,offset,size);
	return size;
}

 /**
  * @brief  显示文件系统中的BMP图片（FLASH 非文件系统区）
  * @param  hdc（输入）：绘图上下文
  * @param  x，y（输入）: 绘制到目标的坐标
  * @param 	lprc（输入）:要绘制的BMP图像矩形区域，如果设置该参数为NULL，则绘制整个BMP图像区域。
  * @retval FALSE:失败; TRUE:成功
  */
BOOL PIC_BMP_Draw_Res(HDC hdc, int x, int y, char *file_name, const RECT *lprc) 
{	 
    int pic_offset;
    GUI_GET_DATA  get_data;
  
    pic_offset =RES_GetOffset(file_name);
    if(pic_offset > 0)
    {    
      get_data.lParam = pic_offset;
      get_data.pfReadData = bmp_read_data_Res;

      return BMP_DrawEx(hdc,x,y,&get_data,lprc);
    }
    
    return FALSE;
}

 /**
  * @brief  获得BMP图像的信息（FLASH 非文件系统区）
  * @param  bm_info（输出）：存储得到的图像信息
  * @param  file_name（输入）: 绘制到目标的坐标
  * @retval FALSE:失败; TRUE:成功
  */
BOOL PIC_BMP_GetInfo_Res(BITMAPINFO *bm_info, char *file_name) 
{	 
    int pic_offset;
    GUI_GET_DATA  get_data;
  
    pic_offset =RES_GetOffset(file_name);
    if(pic_offset > 0)
    {    
      get_data.lParam = pic_offset;
      get_data.pfReadData = bmp_read_data_Res;

      return BMP_GetInfoEx(bm_info,&get_data);
    }
    
    return FALSE;
}

/* 文件系统接口 */
#if(GUI_PIC_FS_EN)

#include  "ff.h"

/**
  * @brief  从流媒体加载内容的回调函数(文件系统)
  * @param  buf[out] 存储读取到的数据缓冲区
  * @param  offset 要读取的位置
  * @param  size 要读取的数据大小
  * @param  lParam 调用函数时的自定义参数（用户参数）
  * @retval 读取到的数据大小
  */
static int bmp_read_data_fs(void *buf,int offset,int size,LPARAM lParam)
{
  int    rw;
  
  /* 本回调函数中lParam是对应的文件指针*/
  FIL *p_file = (FIL*)lParam;

  /* 偏移到指定位置 */
  f_lseek(p_file, offset);
	/* 读取数据到缓冲区 */
	 f_read(p_file, buf, (UINT)size, (UINT *)&rw);
	/* 返回读取到的数据大小 */
	return rw;
}



 /**
  * @brief  显示文件系统中的BMP图片(文件系统)
  * @param  hdc（输入）：绘图上下文
  * @param  x，y（输入）: 绘制到目标的坐标
  * @param 	lprc（输入）:要绘制的BMP图像矩形区域，如果设置该参数为NULL，则绘制整个BMP图像区域。
  * @retval FALSE:失败; TRUE:成功
  */
BOOL PIC_BMP_Draw_FS(HDC hdc, int x, int y, char *file_name, const RECT *lprc) 
{	 	
    /* file objects */
    FIL     *file;												
    FRESULT fresult;  
    BOOL res = TRUE;
    GUI_GET_DATA  get_data;

    file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));
      
    /* 打开文件 */		
    fresult = f_open(file, file_name, FA_OPEN_EXISTING | FA_READ );
    if (fresult != FR_OK)
    {      
      GUI_ERROR("Open Pic failed!");
      GUI_VMEM_Free(file);
      return FALSE;
    }    
  
    /* 把文件指针作为lParam参数*/
    get_data.lParam = (LPARAM)file;
    /* 读取数据的回调函数 */
    get_data.pfReadData = bmp_read_data_fs;
    /* 显示图片 */
    res = BMP_DrawEx(hdc,x,y,&get_data,lprc);
    
    /* 关闭文件 */
    f_close(file);
    
    /* 释放空间 */
    GUI_VMEM_Free(file);
    
    return res;
}

 /**
  * @brief  获得BMP图像的信息(文件系统)
  * @param  bm_info（输出）：存储得到的图像信息
  * @param  file_name（输入）: 绘制到目标的坐标
  * @retval FALSE:失败; TRUE:成功
  */
BOOL PIC_BMP_GetInfo_FS(BITMAPINFO *bm_info, char *file_name) 
{	 
    /* file objects */
    FIL     *file;												
    FRESULT fresult;  
    BOOL res = TRUE;
    GUI_GET_DATA  get_data;

    file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));
      
    /* 打开文件 */		
    fresult = f_open(file, file_name, FA_OPEN_EXISTING | FA_READ );
    if (fresult != FR_OK)
    {   
      GUI_ERROR("Open Pic failed!");      
      GUI_VMEM_Free(file);
      return FALSE;
    }      
    /* 把文件指针作为lParam参数*/
    get_data.lParam = (LPARAM)file;
    /* 读取数据的回调函数 */
    get_data.pfReadData = bmp_read_data_fs;
    /* 获取图片信息 */
    res = BMP_GetInfoEx(bm_info,&get_data);
    f_close(file);
    
    /* 释放空间 */
    GUI_VMEM_Free(file);
    
    return res;
}

#endif /* GUI_PIC_FS_EN */


#if(GUI_PIC_CAPTURE_SCREEN_EN)

#pragma pack(1)
struct  tagBMP_HEADER  /*   位图文件信息头结构   */
{

	U16 bfType; /*   文件类型,   必须为   "BM "   (0x4D42)   */
	U32 bfSize; /*   文件的大小(字节)   */
	U16 bfRsvd1; /*   保留,   必须为   0   */
	U16 bfRsvd2; /*   保留,   必须为   0   */
	U32 bfOffBits; /*   位图阵列相对于文件头的偏移量(字节)   */

	U32 biSize; /*   size   of   BITMAPINFOHEADER   */
	U32 biWidth; /*   位图宽度(像素)   */
	U32 biHeight; /*   位图高度(像素)   */
	U16 biPlanes; /*   目标设备的位平面数,   必须置为1   */
	U16 biBitCount; /*   每个像素的位数,   1,4,8或24   */
	U32 biCompress; /*   位图阵列的压缩方法,0=不压缩   */
	U32 biSizeImage; /*   图像大小(字节)   */
	U32 biXPelsPerMeter; /*   目标设备水平每米像素个数   */
	U32 biYPelsPerMeter; /*   目标设备垂直每米像素个数   */
	U32 biColorUsed; /*   位图实际使用的颜色表的颜色数   */
	U32 biColorImportant; /*   重要颜色索引的个数   */
//	U32 biRedMask;
//	U32 biGreenMask;
//	U32 biBlueMask;
//	U32 biAlphaMask;

};
#pragma pack()

 /**
  * @brief  显示文件系统中的JPEG图片(文件系统)
  * @param  out_file 截图的文件名
  * @retval FALSE or TRUE
  */
BOOL PIC_Capture_Screen_To_BMP(const char *out_file)
{
	HDC hdc,hdc_mem;
	int i,w,h,fsize,line_bytes;
	struct  tagBMP_HEADER *bmp;
	FIL *file;
  FRESULT fresult;  
  BOOL res = TRUE;
  UINT bw;
  
	u8 *buf;
	////////

	w =GUI_XSIZE;
	h =GUI_YSIZE;
  
  /* 文件句柄空间 */
  file =(FIL*)GUI_VMEM_Alloc(sizeof(FIL));

	hdc_mem =CreateMemoryDC(SURF_RGB565,w,h); //创建一个和屏幕大小相同的内存型DC，XRGB8888格式的.

	if(hdc_mem!=NULL)
	{
		hdc =GetDC(NULL); //获得屏幕DC。
		if(hdc!=NULL)
		{
			BitBlt(hdc_mem,0,0,w,h,hdc,0,0,SRCCOPY); //将屏幕DC的内容复制到内存DC中.
			ReleaseDC(NULL,hdc); //释放屏幕DC。

			line_bytes =w*3;
			fsize =54+(h*line_bytes); //计算BMP文件大小(54B 文件头+位图数据)

			buf=(u8*)GUI_VMEM_Alloc(fsize);  //申请一片内存作为文件缓冲区。
			if(buf!=NULL)
			{
				u8 *dst;
				int x,y;

				bmp=(struct tagBMP_HEADER*)buf;
				memset(bmp,0,128);

				bmp->bfType    =0x4D42;
				bmp->bfSize    =54 + h*line_bytes;
				bmp->bfOffBits =54;

				bmp->biSize   =40;
				bmp->biWidth  =w;
				bmp->biHeight =h;
				bmp->biPlanes =1;
				bmp->biBitCount =24;
				bmp->biCompress =0;
				bmp->biSizeImage =h*line_bytes;
				bmp->biColorUsed =0;
				bmp->biColorImportant =0;

				dst =buf + bmp->bfOffBits;
				//src =(u8*)out+(h*line_bytes)-line_bytes;

				for(y=h-1;y>=0;y--)
				{
					i=0;
					for(x=0;x<w;x++)
					{
						U32 c;
						u8 r,g,b;
            
//            u8 a;
//						a=0xFF;

						c=GetPixel(hdc_mem,x,y);     //丛内存DC里读像素值。
						GetRGB(hdc_mem,c,&r,&g,&b);  //将像素值转换成8位的R,G,B份量，用于写入BMP文件位图数据区.
						dst[i++] =b;
						dst[i++] =g;
						dst[i++] =r;
						//dst[i++] =a;
					}

					dst += line_bytes;
				}

        fresult = f_open(file, out_file, FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
				if(fresult == FR_OK)
				{
					u8 *p;
					int a;

					i=fsize;
					p=buf;
					while(i > 0)
					{
						a=MIN(i,(64*1024));
						fresult = f_write(file,p,a,&bw);
            if(fresult != FR_OK)
            { 
              res = FALSE;
              break;
            }              
            
						i -= a;
						p += a;
					}
					f_close(file);
				}
        else
          res = FALSE;
        
				GUI_VMEM_Free(file);
        GUI_VMEM_Free(buf);
			}
      else
        res = FALSE;
		}
    else 
      res = FALSE;
    
		DeleteDC(hdc_mem); //删除内存DC。
	}
  else 
    res = FALSE;
  
  return res;
}

#endif /* GUI_PIC_CAPTURE_SCREEN_EN */



/********************************END OF FILE****************************/
