#ifndef __GUI_PICTURE_PORT_H
#define	__GUI_PICTURE_PORT_H

#include "stm32f4xx.h"
#include "emXGUI.h"

void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ;
void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ; 
void _ShowGIFEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime);  
void _ShowGIF(uint8_t * imgBuffer, uint32_t BufferSize,uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime)  ;
void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY); 
BOOL PIC_BMP_GetInfo_FS(BITMAPINFO *bm_info, char *file_name);
BOOL PIC_BMP_Draw_FS(HDC hdc, int x, int y, char *file_name, const RECT *lprc);
BOOL PIC_BMP_Draw_Res(HDC hdc, int x, int y, char *file_name, const RECT *lprc);
BOOL PIC_Capture_Screen_To_BMP(const char *out_file);

#endif /* __GUI_PICTURE_PORT_H */
