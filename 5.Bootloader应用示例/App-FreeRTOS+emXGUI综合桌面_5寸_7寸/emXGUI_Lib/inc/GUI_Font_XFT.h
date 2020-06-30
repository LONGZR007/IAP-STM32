
#ifndef __GUI_FONT_XFT_H_
#define __GUI_FONT_XFT_H_

#ifdef	__cplusplus
extern "C"{
#endif

#include "def.h"

/*===================================================================================*/
/*
 * HDR+DAT+L1+L2+TAG2
 *
 *
 */

#define	HDR_SIZE		(64)
#define	L1_IDX_NUM		(4096)
#define	L1_TBL_SIZE		(sizeof(u16)*L1_IDX_NUM)

#define	L2_IDX_NUM		(16)
#define	L2_TBL_SIZE		(sizeof(u16)*L2_IDX_NUM)

#define	DAT_TBL_OFFSET	(HDR_SIZE)

struct	xft_head
{
	char  tag[4]; 	//"XFT\0"
	char  name[12];
	u32   tag2_offset;
	u32   L1_offset;
	u32   L2_offset;
	u32   dat_offset;
	u32   dat_slot_size;
	u16   L2_count;
	u16   dat_count;
	u16   width;
	u16   height;
	u16   gray;
	u16   rsv;
};

typedef	struct
{
	s8 x,y;
	u8 w,h;
	u8 pitch;
	u8 format;
	u8 rsv1;
	u8 rsv2;
	U8  data[4];
}xft_chr_info;


typedef	int (FN_XFT_GetData)(void *buf,int offset,int size,LONG lParam);

typedef struct __xft_obj xft_obj;

/*===================================================================================*/

const u8*	xft_get_chr_entry(const u8 *xft_dat,WCHAR wchr);
u32	xft_get_chr_offset(struct xft_head *hdr,const U16 *L1,const U16 *L2,WCHAR ch);

HFONT	XFT_CreateFont(const void *xft_dat);
HFONT	XFT_CreateFontEx(FN_XFT_GetData *pfnGetData,LONG lParam);

/*===================================================================================*/

#ifdef	__cplusplus
}
#endif


#endif /* __FONTDRV_XFT_H_ */
