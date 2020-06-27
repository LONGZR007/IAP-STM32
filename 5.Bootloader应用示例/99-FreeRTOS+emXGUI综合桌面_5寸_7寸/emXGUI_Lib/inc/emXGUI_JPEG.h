/*
 * emXGUI_JPEG.h
 *
 *  Created on: 2018年8月21日
 *      Author: liuwei
 */

#ifndef __EMXGUI_JPEG_H__
#define __EMXGUI_JPEG_H__


#ifdef	__cplusplus
extern "C"{
#endif

#include "emXGUI.h"

typedef	struct	__jpg_dec JPG_DEC;

/**
  * @brief  获取JPG_DEC句柄
  * @param  dat[in]图片所在的缓冲区
  * @param  cbSize图片文件大小
  * @retval 生成的JPG_DEC句柄
  */  
JPG_DEC*	JPG_Open(const void *dat,int cbSize);

/**
* @brief  获取图片的宽高
* @param  width[out] height[out] 图片的宽高
* @param  JPG_DEC 图片句柄
* @retval 是否正常
*/  
BOOL 	JPG_GetImageSize(U16 *width,U16 *height,JPG_DEC* jdec);

/**
* @brief  绘制图片
* @param  hdc 绘图上下文
* @param  x y 显示的位置
* @param  JPG_DEC 图片句柄
* @retval 是否正常
*/  
BOOL	JPG_Draw(HDC hdc,int x,int y,JPG_DEC *jdec);

/**
* @brief  绘制图片(高速)
* @param  hdc 绘图上下文
* @param  x y 显示的位置
* @param  mjpegbuffer 图片内容
* @param  size 图片大小
* @retval 是否正常
*/  
void JPEG_Out(HDC hdc,int x,int y,u8 *mjpegbuffer,s32 size);

/**
* @brief  释放绘图句柄
* @param  JPG_DEC 图片句柄
* @retval 无
*/  
void	JPG_Close(JPG_DEC *jdec);



#ifdef	__cplusplus
}
#endif


#endif /* __EMXGUI_JPEG_H__ */
