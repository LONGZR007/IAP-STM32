
#include "emxgui_png.h"
#include <stddef.h>
#include <stdint.h>

#include	"x_libc.h"
//#include	"x_file.h"
//#include	"Kernel.h"
#include	"emXGUI.h"

#include 	"lodepng.h"

/* 是否使用 */
#define USE_FILE_SYSTEM     0


/*============================================================================*/
extern void *GUI_VMEM_Alloc(u32 size);
extern void GUI_VMEM_Free(void *ptr);
/*============================================================================*/

void* lodepng_malloc(size_t size)
{
	return GUI_VMEM_Alloc(size);
}

void lodepng_free(void *p)
{
	GUI_VMEM_Free(p);
}

void* lodepng_realloc(void *p,size_t size)
{
	void *p_new;

	if(p==NULL)
	{
		return GUI_VMEM_Alloc(size);
	}

	if(size==0)
	{
		GUI_VMEM_Free(p);
		return NULL;
	}

	p_new =GUI_VMEM_Alloc(size);
	if(p_new!=NULL)
	{
		memcpy(p_new,p,size);
		GUI_VMEM_Free(p);

	}
	return p_new;
}

/*============================================================================*/

static void ABGR2ARGB(u8 *in,int size)
{
	u8 a,b,*end;

	end =&in[size];

	while(in < end)
	{
		a =in[0];
		b =in[2];

		in[0] =b;
		in[2] =a;

		in += 4;
	}
}

/*============================================================================*/

#pragma pack(1)
struct  tagBMP_HEADER  /*   浣嶅浘鏂囦欢淇℃伅澶寸粨鏋�  */
{

	U16 bfType; /*   鏂囦欢绫诲瀷,   蹇呴』涓�  "BM "   (0x4D42)   */
	U32 bfSize; /*   鏂囦欢鐨勫ぇ灏�瀛楄妭)   */
	U16 bfRsvd1; /*   淇濈暀,   蹇呴』涓�  0   */
	U16 bfRsvd2; /*   淇濈暀,   蹇呴』涓�  0   */
	U32 bfOffBits; /*   浣嶅浘闃靛垪鐩稿浜庢枃浠跺ご鐨勫亸绉婚噺(瀛楄妭)   */

	U32 biSize; /*   size   of   BITMAPINFOHEADER   */
	U32 biWidth; /*   浣嶅浘瀹藉害(鍍忕礌)   */
	U32 biHeight; /*   浣嶅浘楂樺害(鍍忕礌)   */
	U16 biPlanes; /*   鐩爣璁惧鐨勪綅骞抽潰鏁�   蹇呴』缃负1   */
	U16 biBitCount; /*   姣忎釜鍍忕礌鐨勪綅鏁�   1,4,8鎴�4   */
	U32 biCompress; /*   浣嶅浘闃靛垪鐨勫帇缂╂柟娉�0=涓嶅帇缂�  */
	U32 biSizeImage; /*   鍥惧儚澶у皬(瀛楄妭)   */
	U32 biXPelsPerMeter; /*   鐩爣璁惧姘村钩姣忕背鍍忕礌涓暟   */
	U32 biYPelsPerMeter; /*   鐩爣璁惧鍨傜洿姣忕背鍍忕礌涓暟   */
	U32 biColorUsed; /*   浣嶅浘瀹為檯浣跨敤鐨勯鑹茶〃鐨勯鑹叉暟   */
	U32 biColorImportant; /*   閲嶈棰滆壊绱㈠紩鐨勪釜鏁�  */
	U32 biRedMask;
	U32 biGreenMask;
	U32 biBlueMask;
	U32 biAlphaMask;

};
#pragma pack()

#if(USE_FILE_SYSTEM)
void* png_decode_to_bmp32(const char *fpath)
{
	struct  tagBMP_HEADER *bmp=NULL;
	X_FILE *fp;
	int i,fsize,line_bytes;
	u8 *png_buf,*out,err;
	u32 w,h;

	fp =x_fopen(fpath,"rb");
	if(fp!=NULL)
	{

		fsize =x_fsize(fp);
		png_buf =(u8*)lodepng_malloc(fsize);
		x_fread(png_buf,fsize,1,fp);
		err=lodepng_decode_memory(&out,&w,&h,(const unsigned char *)png_buf,fsize, LCT_RGBA, 8);
		if(err==0)
		{
			ABGR2ARGB(out,w*h*4);

			bmp=lodepng_malloc(128+(w*h*4));
			if(bmp!=NULL)
			{
				u8 *dst,*src;
				memset(bmp,0,128);

				bmp->bfType =0x4D42;
				bmp->bfSize =128+(w*h*4);
				bmp->bfOffBits =128;

				bmp->biSize =sizeof(struct tagBMP_HEADER);
				bmp->biWidth =w;
				bmp->biHeight =h;
				bmp->biPlanes =1;
				bmp->biBitCount =32;
				bmp->biCompress =0;
				bmp->biSizeImage =w*h*4;

				line_bytes =w*4;
				dst =(u8*)bmp+128;
				src =(u8*)out+(h*line_bytes)-line_bytes;

				for(i=0;i<h;i++)
				{
					memcpy(dst,src,line_bytes);
					dst += line_bytes;
					src -= line_bytes;
				}

			}

			lodepng_free(out);
		}

		lodepng_free(png_buf);
		x_fclose(fp);
	}

	return (void*)bmp;
}

BITMAP* png_decode_to_bitmap(const char *fpath)
{

	BITMAP *bm=NULL;
	X_FILE *fp;
	int i,fsize,line_bytes;
	u8 *png_buf,*out,err;
	u32 w,h;

	fp =x_fopen(fpath,"rb");
	if(fp!=NULL)
	{

		fsize =x_fsize(fp);
		png_buf =(u8*)lodepng_malloc(fsize);
		x_fread(png_buf,fsize,1,fp);
		err=lodepng_decode_memory(&out,&w,&h,(const unsigned char *)png_buf,fsize, LCT_RGBA, 8);
		if(err==0)
		{
			ABGR2ARGB(out,w*h*4);

			bm=lodepng_malloc(sizeof(BITMAP)+(w*h*4));
			if(bm!=NULL)
			{
				u8 *dst,*src;
				bm->Format =BM_ARGB8888;
				bm->Width =w;
				bm->Height =h;
				bm->WidthBytes =w*4;
				bm->Bits =(u8*)bm+sizeof(BITMAP);
				bm->LUT =NULL;

				memcpy(bm->Bits,out,bm->WidthBytes*bm->Height);
			}

			lodepng_free(out);
		}

		lodepng_free(png_buf);
		x_fclose(fp);
	}

	return bm;
}
#endif

void png_decode_free(BITMAP *de_bm)
{
	lodepng_free(de_bm);
}

/*============================================================================*/


struct __PNG_DEC{
	BITMAP bm;
	U32 magic;
};

PNG_DEC* PNG_Open(const u8 *png_dat,int png_size)
{
	PNG_DEC *dec;
	u32 w,h;
	u8 *out,err;

	dec =(PNG_DEC*)lodepng_malloc(sizeof(PNG_DEC));
	if(dec!=NULL)
	{
		err=lodepng_decode_memory(&out,&w,&h,(const unsigned char *)png_dat,png_size, LCT_RGBA, 8);
    /* 格式转换 */
    ABGR2ARGB(out,w*h*4);

		dec->bm.Format =BM_ARGB8888;
		dec->bm.Width =w;
		dec->bm.Height =h;
		dec->bm.WidthBytes =w*4;
		dec->bm.Bits =out;
		dec->bm.LUT =NULL;
		dec->magic =0x12345678;
	}

	return dec;
}

BOOL PNG_GetBitmap(PNG_DEC *png_dec,BITMAP *bm)
{
	if(png_dec==NULL)
	{
		return FALSE;
	}

	if(bm==NULL)
	{
		return FALSE;
	}

	memcpy(bm,&png_dec->bm,sizeof(BITMAP));
	return TRUE;
}

void PNG_Close(PNG_DEC *png_dec)
{
	if(png_dec!=NULL)
	{
		lodepng_free(png_dec->bm.Bits);
		lodepng_free(png_dec);
	}

}


/*===================================================================================================*/
/*===================================================================================================*/
/*===================================================================================================*/
/*===================================================================================================*/
/*===================================================================================================*/
/*===================================================================================================*/

static char *png_dat=NULL;
static int png_size=0;
static char *png_out=NULL;
static int time0=0;
static int time1=0;


BOOL PNG_Draw(HDC hdc,int x,int y,const u8 *png_dat,int png_size)
{
	u8 *out,err;
	u32 w,h;
	BITMAP bm;
	U32 color=0;
//	int t0,t1;

//	t0 =SYS_get_time_count(NULL);
	//err=lodepng_decode24(&out,&w,&h,(const unsigned char *)png_dat,png_size);
	err=lodepng_decode_memory(&out,&w,&h,(const unsigned char *)png_dat,png_size, LCT_RGBA, 8);
//	time0 =SYS_get_time_count(NULL)-t0;

//	t0 =SYS_get_time_count(NULL);
	ABGR2ARGB(out,w*h*4);
//	time1 =SYS_get_time_count(NULL)-t0;

	png_out =out;

	bm.Format =BM_ARGB8888;
	bm.Width =w;
	bm.Height =h;
	bm.WidthBytes =w*4;
	bm.Bits =out;
	bm.LUT =NULL;

	DrawBitmap(hdc,x,y,&bm,NULL);
	lodepng_free(out);
	return TRUE;

}

#if(USE_FILE_SYSTEM)

BOOL PNG_DrawFromFile(HDC hdc,int x,int y,const char* path)
{
	BITMAP bm;
	X_FILE *fp;
	int fsize;
	u8 *png_buf,*out;
	u32 w,h;

	fp =x_fopen(path,"rb");
	if(fp!=NULL)
	{

		fsize =x_fsize(fp);
		png_buf =(char*)lodepng_malloc(fsize);
		x_fread(png_buf,fsize,1,fp);
		lodepng_decode_memory(&out,&w,&h,(const unsigned char *)png_buf,fsize, LCT_RGBA, 8);

		ABGR2ARGB(out,w*h*4);

		bm.Format =BM_ARGB8888;
		bm.Width =w;
		bm.Height =h;
		bm.WidthBytes =w*4;
		bm.Bits =out;
		bm.LUT =NULL;

		DrawBitmap(hdc,x,y,&bm,NULL);

		lodepng_free(out);

		lodepng_free(png_buf);
		x_fclose(fp);

		return TRUE;
	}
	return FALSE;
}

void lode_png_test(void)
{
	HDC hdc;
	RECT rc;
	X_FILE *fp;
	WCHAR wbuf[100];

	fp =x_fopen("B:test1.png","rb");
	if(fp!=NULL)
	{
		png_size =x_fsize(fp);
		png_dat =lodepng_malloc(png_size);

		x_fread(png_dat,png_size,1,fp);
		x_fclose(fp);
	}

	rc.x =20;
	rc.y =30;
	rc.w =200;
	rc.h =200;


	hdc =GetDC(NULL);

	SetBrushColor(hdc,MapRGB(hdc,200,200,200));
	FillRect(hdc,&rc);
	PNG_Draw(hdc,rc.x,rc.y,(const u8*)png_dat,png_size);

	x_wsprintf(wbuf,L"png_out: %08XH,time:%dms,%dms",png_out,time0,time1);
	TextOut(hdc,rc.x+4,rc.y+180,wbuf,-1);

	ReleaseDC(NULL,hdc);

}
#endif
/*============================================================================*/

