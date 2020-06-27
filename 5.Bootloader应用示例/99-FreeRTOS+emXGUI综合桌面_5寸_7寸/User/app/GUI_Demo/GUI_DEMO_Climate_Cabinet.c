
/*
 *GUI_DEMO_MEMDC_Blt.c
 *2018年9月29日下午4:23:09
 *
 */

#include "emXGUI.h"
#include "GUI_Font_XFT.h"
#include "x_libc.h"
#include "GUI_AppDef.h"
#include "cpuusage.h"

/*============================================================================*/
extern const unsigned char gImage_0[];
extern const unsigned char gImage_1[];
extern const unsigned char gImage_5[];

extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];
extern const char ASCII_40_4BPP[];

/*============================================================================*/
#define	MEMDC_W	580  //MEMDC宽度.
#define	MEMDC_H 440  //MEMDC高度.
#define	BGCOLOR	RGB888(23,27,83)  //背景色(RGB888).


#define OBJNUM     10 //显示的对象数量.
#define	SPEED_MIN	2 //移动速度最小值.
#define	SPEED_MAX	6 //移动速度最大值.
#define RECT_SIZE_MIN 80
#define RECT_SIZE_MAX 150

//定义控件ID
enum	eID{
	ID_EXIT =0x1000,
	ID_TEMPERATURE,
  ID_DWELL,
	ID_HEATING,
  ID_START,
  
  	ID_NEXT,
  ID_DMA2D_EN,
	ID_TITLE,
  ID_ART_ACTIVE,
  
	ID_FPS,
  ID_CPU_LOAD,
  ID_INFO,

	ID_RANDOM,
	ID_BITMAP,
	ID_TEXT,
	ID_RECT,

};

/*============================================================================*/
//dma2d使能的动态开关
extern BOOL g_dma2d_en ;

static 	RECT	ObjRect[OBJNUM];
static 	u32		ObjColor[OBJNUM];
static  HFONT   ObjFont[OBJNUM];
static  BITMAP *ObjBitmap[OBJNUM];
static 	u16		ObjType[OBJNUM];
static 	s8		ObjActive[OBJNUM];
static 	s8		ObjSpeedX[OBJNUM];
static 	s8		ObjSpeedY[OBJNUM];

static HDC hdc_mem=NULL;
static BITMAP bm1,bm2,bm3;
static HFONT hFont24=NULL;
static HFONT hFont32=NULL;
static HFONT hFont40=NULL;
static int t0,frame,fps;

static int type_id=0;

static  HFONT *FontTbl[4]={

		&hFont32,
		&hFont24,
		&hFont32,
		&hFont40,

};

static  BITMAP *BitmapTbl[4]={

		&bm1,
		&bm2,
		&bm3,
		&bm1,
};

/*============================================================================*/

static void BitmapInit(void)
{
	bm1.Format	= BM_ARGB8888;
	bm1.Width = 92;
	bm1.Height = 184;
	bm1.WidthBytes =bm1.Width*4;
	bm1.LUT =NULL;
	bm1.Bits =(void*)gImage_0;

	bm2.Format	= BM_ARGB8888;
	bm2.Width = 130;
	bm2.Height = 260;
	bm2.WidthBytes =bm2.Width*4;
	bm2.LUT =NULL;
	bm2.Bits =(void*)gImage_1;

	bm3.Format = BM_ARGB8888;
	bm3.Width = 92;
	bm3.Height = 92;
	bm3.WidthBytes =bm3.Width*4;
	bm3.LUT =NULL;
	bm3.Bits =(void*)gImage_5;

}

#if 0
static void DrawHandler(HDC hdc,int Width,int Height)
{
	int i;
  RECT rc;
  

	ClrDisplay(hdc,NULL,MapRGB888(hdc,BGCOLOR));
  
    for(i=0; i<OBJNUM; i++)
    {
	      if(   ObjRect[i].x <= -40
              || ObjRect[i].x >=Width
              || ObjRect[i].y <= 0
              || ObjRect[i].y >=Height )
	      {
		        ObjActive[i] = 0;
	      }

          if(ObjActive[i] != 0)
	      {

        	  int x,y;

        	  x =ObjRect[i].x;
        	  y =ObjRect[i].y;

		       switch (ObjType[i])
	  	       {

              case 0:
              {
                WCHAR wbuf[20];
                //SetFont(hdc,hFont32);
                SetFont(hdc,ObjFont[i]);
                SetTextColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
                x_wsprintf(wbuf,L"FPS:%d",fps);
                TextOut(hdc,x,y,wbuf,-1);
              }
              break;
              ////

              case 1:
              {
                WCHAR wbuf[20];
                //SetFont(hdc,hFont32);
                SetFont(hdc,ObjFont[i]);
                SetTextColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
                x_wsprintf(wbuf,L"X:%04d:Y:%04d",x,y);
                TextOut(hdc,x,y,wbuf,-1);
              }
              break;
              ////
              case 2:
              {
//                WCHAR wbuf[20];
                //SetFont(hdc,hFont40);
                SetFont(hdc,ObjFont[i]);
                SetTextColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
                TextOut(hdc,x,y,L"emXGUI",-1);
              }
              break;
              ////

              case 3:
              {
                SetBrushColor(hdc,MapXRGB8888(hdc,ObjColor[i]));
                FillRect(hdc,&ObjRect[i]);
              }
              break;
              ////
              case 4:
              default:
                DrawBitmap(hdc,x,y,ObjBitmap[i],NULL);
                //DrawBitmap(hdc,x,y,&bm3,NULL);
                break;

	  	         }

		       ObjRect[i].x += ObjSpeedX[i];
		       ObjRect[i].y += ObjSpeedY[i];
	      }

          if (ObjActive[i]==0 )
          {
        	  	u32 rand_val;

        	  	rand_val =x_rand();

        	    ObjRect[i].x =rand_val%Width;
        	    ObjRect[i].y =1;
              ObjRect[i].w =RECT_SIZE_MIN + ((rand_val>>4)%RECT_SIZE_MAX);
              ObjRect[i].h =RECT_SIZE_MIN + ((rand_val>>8)%RECT_SIZE_MAX);


              ObjSpeedX[i] =(int)((int)x_rand()%SPEED_MAX)-(int)(x_rand()%SPEED_MAX);
              ObjSpeedY[i] =SPEED_MIN + ((rand_val>>6)%SPEED_MAX);

              switch(type_id)
              {
                case 0:
                  ObjType[i] =2;
                  break;

                case 1:
                  ObjType[i] =3;
                  break;

                case 2:
                  ObjType[i] =4;
                  break;

                default:
                  ObjType[i] = (rand_val>>2)%8;
                  break;
              }

              rand_val =x_rand();
              ObjColor[i]   = rand_val;
              ObjFont[i]    = *FontTbl[(rand_val>>4)%ARR_SIZE(FontTbl)];
              ObjBitmap[i]  = BitmapTbl[(rand_val>>8)%ARR_SIZE(BitmapTbl)];
              ObjActive[i]  = 1;

          }
    }
    
      //      			GetClientRect(hdc,&rc);
      rc.x = 0;
      rc.y = 0;
      rc.w = Width-1;
      rc.h = Height-1;
      
      SetPenColor(hdc,MapRGB(hdc,255,255,255));
      DrawRect(hdc,&rc);

      InflateRect(&rc,-1,-1);
      DrawRect(hdc,&rc);
    
      SetFont(hdc, defaultFont);
      SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
    	TextOut(hdc,rc.x+10,rc.h-30,L"刷图区分辨率：580x440",-1); 
}
#endif


static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc0,rc;
	HWND wnd;
  static   uint8_t cpu_load_major,cpu_load_minor;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
//      HDC hdc;

			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.

			BitmapInit(); //初始化位图数据

			hFont24 =XFT_CreateFont(ASCII_24_4BPP);
			hFont32 =XFT_CreateFont(ASCII_32_4BPP);
			hFont40 =XFT_CreateFont(ASCII_40_4BPP);

			type_id =3;

			frame=0;
			fps =0;
			t0 =GUI_GetTickCount();
			hdc_mem =CreateMemoryDC(SURF_SCREEN,MEMDC_W,MEMDC_H);

			rc.w =180;
			rc.h =100;
			rc.x = 20;
			rc.y =0;      

      /* Home按钮 */  
			wnd=CreateWindow(BUTTON,L"C",	BS_FLAT|WS_VISIBLE,rc0.w - 60,0,60,60,hwnd,ID_EXIT,NULL,NULL); //创建一个按钮.
			SetWindowFont(wnd,controlFont); //设置控件窗口字体.

			OffsetRect(&rc,0,10);
			wnd=CreateWindow(BUTTON,L"温湿度",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_TEMPERATURE,NULL,NULL);
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.
      
			OffsetRect(&rc,0,rc.h+20);
			wnd=CreateWindow(BUTTON,L"保持时间",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_DWELL,NULL,NULL);
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.
      
      OffsetRect(&rc,0,rc.h+20);
			wnd=CreateWindow(BUTTON,L"加热/冷却时间",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_HEATING,NULL,NULL);
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.
      
      OffsetRect(&rc,0,rc.h+20);
			wnd=CreateWindow(BUTTON,L"开始",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_START,NULL,NULL);
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.

		}
		return TRUE;
		////

		case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
		{
			u16 code,id;

			code =HIWORD(wParam); //获得通知码类型.
			id   =LOWORD(wParam); //获得产生该消息的控件ID.

			if(id ==ID_NEXT)
			{
				if(code == BN_CLICKED)
				{
          type_id++;
          type_id %=4;
//          GUI_DEBUG("type_id =%d",type_id);
             
          wnd =GetDlgItem(hwnd,ID_TITLE);

          switch(type_id)
          {
            case 0:      
               SetWindowText(wnd,L"文字显示");
              break;
            case 1:
              SetWindowText(wnd,L"复制矩形"); 
              break;

            case 2:  
              SetWindowText(wnd,L"复制Bitmap");       
              break;

             case 3:             
              SetWindowText(wnd,L"随机图案");
              break;
             
             default:
               break;
          } 
				}
			}
      
      if(id == ID_DMA2D_EN)
      {
        if(code == BN_CLICKED)
				{
          g_dma2d_en =!g_dma2d_en;
          
          if(g_dma2d_en)
          {
            wnd =GetDlgItem(hwnd,ID_ART_ACTIVE);
            SetWindowText(wnd,L"Chrom-ART 激活");

//            ShowWindow(GetDlgItem(hwnd, ID_ART_ACTIVE), SW_SHOW);
          }
          else
          {
            wnd =GetDlgItem(hwnd,ID_ART_ACTIVE);
            SetWindowText(wnd,L"");

//            ShowWindow(GetDlgItem(hwnd, ID_ART_ACTIVE), SW_HIDE);
          }
        }
      }

			if(id==ID_EXIT && code==BN_CLICKED) // 按钮“单击”了.
			{
				PostCloseMessage(hwnd); //使产生WM_CLOSE消息关闭窗口.
			}
		}
		break;
		////

		case	WM_ERASEBKGND:
		{
			HDC hdc=(HDC)wParam;
			RECT rc;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc,MapRGB888(hdc,BGCOLOR));
			FillRect(hdc,&rc);
			return TRUE;
		}
//		break;
		////
		case WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);

			if(id == ID_TITLE ||id == ID_INFO) //ID_TEXT控件的颜色.
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

				cr->TextColor =RGB888(250,250,250);
				cr->BackColor =BGCOLOR;
				cr->BorderColor =RGB888(50,50,50);
				return TRUE;
			}
			else if( id == ID_ART_ACTIVE ) //ID_TEXT控件的颜色.
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

				cr->TextColor =RGB888(0,255,0);
				cr->BackColor =BGCOLOR;
				cr->BorderColor =RGB888(50,50,50);
				return TRUE;
			}
			else if(id == ID_FPS  ) //ID_FPS控件的颜色.
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;

				cr->TextColor =RGB888(255,255,0);
				cr->BackColor =RGB888(0,50,70);
				cr->BorderColor =RGB888(50,50,50);
				return TRUE;
			}
      else if(id == ID_CPU_LOAD ) //ID_CPU_LOAD控件的颜色.
			{
//        uint8_t major,minor;  
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
        
//        cpu_usage_get(&major,&minor);
        
        if(cpu_load_major<=25)
          cr->TextColor =RGB888(0,255,0);
				else if(cpu_load_major<=50)
          cr->TextColor =RGB888(200,255,0);
        else if(cpu_load_major<=75)
          cr->TextColor =RGB888(255,255,0);
        else if(cpu_load_major<=100)
          cr->TextColor =RGB888(255,0,0);

        cr->BackColor =RGB888(0,50,70);
				cr->BorderColor =RGB888(50,50,50);
        
				return TRUE;
			}
      else if(id == ID_NEXT  || id == ID_DMA2D_EN)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;

        /* 按钮按下 */
        if(	SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_PUSHED)
        {
          cr->TextColor =RGB888(0,0,0);
          cr->BackColor =RGB888(179,238,58);
          cr->BorderColor =RGB888(50,50,50);
        }          
        else
        {
          cr->TextColor =RGB888(0,0,0);
          cr->BackColor =RGB888(255,255,205);
          cr->BorderColor =RGB888(50,50,50);
        }
				return TRUE;
      }
      else if( id == ID_EXIT)
      {
        CTLCOLOR *cr;
        cr =(CTLCOLOR*)lParam;

        if(	SendMessage(GetDlgItem(hwnd,id),BM_GETSTATE,0,0)&BST_PUSHED)
        {
          cr->TextColor =RGB888(255,255,255);
          cr->BackColor =RGB888_COLOR_INVALID;
          cr->BorderColor =RGB888(50,50,50);
        }          
        else
        {
          cr->TextColor =RGB888(255,255,255);
          cr->BackColor =RGB888_COLOR_DESKTOP_BACK_GROUND;
          cr->BorderColor =RGB888(50,50,50);
        }
				return TRUE;
      }
			else
			{
				//其它按钮使用系统默认的颜色进行绘制，所以直接返回FALSE.
				return FALSE;
			}
		}
//		break;
		////

		case WM_PAINT: //窗口需要绘制时，会自动产生该消息.
		{
			PAINTSTRUCT ps;
			HDC hdc;
			int t;
			WCHAR wbuf[20];

			t =GUI_GetTickCount();
			if((t-t0) >= 1000)
			{
				fps =frame;

				t0 =t;
				frame=0;

				wnd =GetDlgItem(hwnd,ID_FPS);
				x_wsprintf(wbuf,L"%d FPS",fps);
				SetWindowText(wnd,wbuf);
        
        cpu_usage_get(&cpu_load_major,&cpu_load_minor);

        wnd =GetDlgItem(hwnd,ID_CPU_LOAD);
        if(cpu_load_major != 0)
          x_wsprintf(wbuf,L"%d %%",cpu_load_major);
        else
          x_wsprintf(wbuf,L"0.%d %%",cpu_load_minor);

				SetWindowText(wnd,wbuf);

			}
			else
			{
				frame++;
			}

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
//			DrawHandler(hdc_mem,MEMDC_W,MEMDC_H);

//			BitBlt(hdc,0,40,MEMDC_W,MEMDC_H,hdc_mem,0,0,SRCCOPY);
      
			EndPaint(hwnd,&ps); //结束绘图

			////
			InvalidateRect(hwnd,NULL,FALSE); //触发下一个WM_PAINT消息.
			GUI_msleep(2); //释放一下CPU,否则将全占CPU进行绘制.
		}
		break;
		////

		case WM_CLOSE: //窗口关闭时，会自动产生该消息.
		{

			DeleteDC(hdc_mem);
			DeleteFont(hFont24);
			DeleteFont(hFont32);
			DeleteFont(hFont40);
			return DestroyWindow(hwnd); //调用DestroyWindow函数销毁窗口，该函数会使主窗口结束并退出消息循环;否则窗口将继续运行.
		}
//		break;
		////

		default: //用户不关心的消息,由系统处理.
		{
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

	}

	return WM_NULL;
}

/*============================================================================*/

void	GUI_DEMO_Climate_Cabinet(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////

	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS,
                            &wcex,
                            L"GUI_DEMO: MEMDC Blt", //窗口名称
                            WS_CLIPCHILDREN,
                            0,0,GUI_XSIZE,GUI_YSIZE,    //窗口位置和大小
                            NULL,NULL,NULL,NULL);

	//显示主窗口
	ShowWindow(hwnd,SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*============================================================================*/
