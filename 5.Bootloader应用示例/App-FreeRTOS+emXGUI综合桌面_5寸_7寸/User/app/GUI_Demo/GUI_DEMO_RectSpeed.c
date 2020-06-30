
#include <emXGUI.h>
#include	"x_libc.h"

#ifdef	WIN32

//#include <wchar.h>

//#define x_wsprintf	wsprintfW
//#define	x_rand		rand
	
#endif

/*===================================================================================*/
/*===================================================================================*/
extern U32 GUI_GetTickCount(void);

/*===================================================================================*/

static  COLORREF _aColor[8] = {
  0x000000, 
  0x0000FF, 
  0x00FF00, 
  0x00FFFF, 
  0xFF0000, 
  0xFF00FF, 
  0xFFFF00, 
  0xFFFFFF
};

void GUI_DEMO_RectSpeed(void)
{

  WCHAR buf[128];
  HDC hdc;
  HWND hwnd;
  int t,t1;
  int i;
  int XSize;
  int YSize;
  unsigned int NumPixels;
  RECT rc;

  hwnd	=GetDesktopWindow();
  hdc	=GetDC(hwnd);
  
  XSize =GUI_XSIZE;
  YSize =GUI_YSIZE;
  NumPixels=0;

  
    _aColor[0] = MapRGB(hdc,0,0,0);
    _aColor[1] = MapRGB(hdc,0,0,255);
    _aColor[2] = MapRGB(hdc,0,255,0);
    _aColor[3] = MapRGB(hdc,0,255,255);
    _aColor[4] = MapRGB(hdc,255,0,0);
    _aColor[5] = MapRGB(hdc,255,0,255);
    _aColor[6] = MapRGB(hdc,255,255,0);
    _aColor[7] = MapRGB(hdc,255,255,255);
    
	//GUI_Printf("GUIDEMO_RectSpeed\r\n"); 
 
   

	t = GUI_GetTickCount();	
	i = 0;
	while(1)
	{
		RECT r;
		
		t1 =GUI_GetTickCount();
		if((t1-t) >= 5000)	break;
			
		SetBrushColor(hdc,_aColor[i&7]);
		/* Calculate random positions */
		
	//	r.x = 8;//rand() % XSize - XSize / 2;
	//	r.y = 4;//rand() % YSize - YSize / 2;
	//	r.w = 80;//r.x + rand() % XSize;
	//	r.h = 60;//r.y + rand() % YSize;
	   
		r.x = x_rand() % XSize - XSize / 2;
		r.y = x_rand() % YSize - YSize / 2;
		r.w = x_rand() % XSize;
		r.h = x_rand() % YSize;
		
		FillRect(hdc,&r);
		/* Clip rectangle to visible area and add the number of pixels (for speed computation) */
		if (r.w >= XSize)
		  r.w = XSize - 1;
		if (r.h >= YSize)
		  r.h = YSize - 1;
		if (r.x < 0 )
		  r.x = 0;
		if (r.y < 0)
		  r.y = 0;
			
		NumPixels += (r.w) * (r.h);
			i++;
		/* Allow short breaks so we do not use all available CPU time ... */
	}
	

	t = (t1 - t) / 100;
	ClrDisplay(hdc,NULL,MapRGB(hdc,0,100,200));
	GUI_msleep(200);

	
	if(t==0)	t++;

	SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetClientRect(hwnd,&rc);

	x_wsprintf(buf,L"Pixels/sec: %d",10 * (NumPixels / t));
	DrawText(hdc,buf,-1,&rc,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	ReleaseDC(hwnd,hdc);

    GUI_msleep(3000);
	InvalidateRect(hwnd,NULL,TRUE);
	GUI_msleep(100);
}

/*===================================================================================*/
