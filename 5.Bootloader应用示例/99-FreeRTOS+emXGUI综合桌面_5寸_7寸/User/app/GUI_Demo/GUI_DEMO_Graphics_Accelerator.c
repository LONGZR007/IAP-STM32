
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

#if 0
extern const char ASCII_24_4BPP[];
extern const char ASCII_32_4BPP[];
extern const char ASCII_40_4BPP[];
#else

#define ASCII_32_4BPP "ASCII_40_4BPP.xft"
#define ASCII_40_4BPP "ASCII_40_4BPP.xft"

#endif

/*============================================================================*/
#define	MEMDC_W	580  //MEMDC宽度.
#define	MEMDC_H 440  //MEMDC高度.
#define	BGCOLOR	RGB888(0,0,0)  //背景色(RGB888).


#define OBJNUM     10 //显示的对象数量.
#define	SPEED_MIN	2 //移动速度最小值.
#define	SPEED_MAX	6 //移动速度最大值.
#define RECT_SIZE_MIN 80
#define RECT_SIZE_MAX 150

//定义控件ID
enum	eID{
	ID_EXIT =0x1000,
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
static HDC hdc_bk=NULL;
static BITMAP bm1,bm2,bm3,bm4;
static HFONT GA_hFont24=NULL;
static HFONT GA_hFont32=NULL;
static HFONT GA_hFont40=NULL;
static int t0,frame,fps;

static int type_id=0;

static  HDC blue_fish_hdc;
static  HDC red_fish_hdc;
static  HDC crocodile_hdc;
static  HDC Okami_hdc;

extern const char ASCII_24_4BPP[];

static  HFONT *FontTbl[4]={

		&GA_hFont32,
		&GA_hFont24,
		&GA_hFont32,
		&GA_hFont40,

};

static  BITMAP *BitmapTbl[5]={

		&bm1,
		&bm2,
		&bm3,
    &bm4,
		&bm1,
};

/*============================================================================*/
#define BLUE_FISH_PIC "blue_fish_ARGB8888.bmp"
#define RED_FISH_PIC  "red_fish_ARGB8888.bmp"
#define CROCODILE_PIC "crocodile_ARGB8888.bmp"
#define Okami_PIC     "Okami.bmp"

static void BitmapInit(void)
{
#if 0
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
#else

  /* 创建蓝鱼的memdc */
  blue_fish_hdc = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888,92,184); 
  /* 清空背景为透明 */
  ClrDisplay(blue_fish_hdc,NULL,0);
  /* 绘制bmp到hdc */
  PIC_BMP_Draw_Res(blue_fish_hdc,0,0,BLUE_FISH_PIC, NULL);
  /* 转换成bitmap */
  DCtoBitmap(blue_fish_hdc,&bm1);
  
  red_fish_hdc = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888,92,92);
  ClrDisplay(red_fish_hdc,NULL,0);  
  PIC_BMP_Draw_Res(red_fish_hdc,0,0,RED_FISH_PIC, NULL);
  DCtoBitmap(red_fish_hdc,&bm2);

#if 1
  crocodile_hdc = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888,130,260);  
  ClrDisplay(crocodile_hdc,NULL,0);
  PIC_BMP_Draw_Res(crocodile_hdc,0,0,CROCODILE_PIC, NULL);
  DCtoBitmap(crocodile_hdc,&bm3);
#else
  crocodile_hdc = CreateMemoryDC(COLOR_FORMAT_RGB565,130,130);  
  ClrDisplay(crocodile_hdc,NULL,250);
  PIC_BMP_Draw_Res(crocodile_hdc,0,0,Okami_PIC, NULL);
  DCtoBitmap(crocodile_hdc,&bm3);
#endif
  Okami_hdc = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888,130,130);  
  ClrDisplay(Okami_hdc,NULL,0);
  PIC_BMP_Draw_Res(Okami_hdc,0,0,Okami_PIC, NULL);
  DCtoBitmap(Okami_hdc,&bm4);
  
#endif
}
static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{  
  HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //设置画笔色
	}

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -1);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }
}

static void GA_BUTTON_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc,hdc_mem;
	RECT rc, rc_tmp;
	WCHAR wbuf[128];
  
  
	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  
  
  
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  hdc_mem = CreateMemoryDC(SURF_SCREEN, rc.w, rc.h);
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

  BitBlt(hdc_mem, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);
  if(ds->ID == ID_ART_ACTIVE)
    SetTextColor(hdc_mem, MapRGB(hdc_mem, 0, 255, 0));
  else
    SetTextColor(hdc_mem, MapRGB(hdc_mem, 255, 255, 255));
	

	  /* 使用控制图标字体 */
	
	//  SetTextColor(hdc,MapRGB(hdc,255,255,255));

	GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字

	DrawText(hdc_mem, wbuf, -1, &rc, DT_VCENTER);//绘制文字(居中对齐方式)

  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, rc.x, rc.y, SRCCOPY);
  DeleteDC(hdc_mem);

}
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
                //SetFont(hdc,GA_hFont32);
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
                //SetFont(hdc,GA_hFont32);
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
                //SetFont(hdc,GA_hFont40);
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

      SetPenColor(hdc,MapRGB(hdc,100,100,150));
      InflateRect(&rc,-1,-1);
      DrawRect(hdc,&rc);
    
      SetPenColor(hdc,MapRGB(hdc,100,100,150));
      InflateRect(&rc,-1,-1);
      DrawRect(hdc,&rc);
    
      SetFont(hdc, defaultFont);
      SetTextColor(hdc,MapRGB(hdc,255,255,255)); 
    	TextOut(hdc,rc.x+10,rc.h-30,L"刷图区分辨率：580x440",-1); 
}



static LRESULT	WinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	RECT rc0,rc;
	HWND wnd;
  static   uint8_t cpu_load_major,cpu_load_minor;
  static   u8 *font_32,*font_40;

	switch(msg)
	{
		case WM_CREATE: //窗口创建时,会自动产生该消息,在这里做一些初始化的操作或创建子窗口.
		{
//      HDC hdc;

			GetClientRect(hwnd,&rc0); //获得窗口的客户区矩形.

			BitmapInit(); //初始化位图数据
#if 1
			GA_hFont24 =XFT_CreateFont(ASCII_24_4BPP);
			GA_hFont32 =XFT_CreateFont(ASCII_32_4BPP);
			GA_hFont40 =XFT_CreateFont(ASCII_40_4BPP);
#else
			GA_hFont24 =defaultFont;
			GA_hFont32 =GUI_Init_Extern_Font_2RAM(ASCII_32_4BPP,&font_32);         
			GA_hFont40 =GUI_Init_Extern_Font_2RAM(ASCII_40_4BPP,&font_40); 
      
      /* 创建不成功可能是内存不够 */
      if(GA_hFont32 == NULL )
      {
        /* 使用流方式加载 */
        GA_hFont32 =GUI_Init_Extern_Font(ASCII_32_4BPP);
        if(GA_hFont32 == NULL)
        {
            /* 依然失败就用默认字体 */
            GA_hFont32 = defaultFont;
        }
      }
      
      /* 创建不成功可能是内存不够 */
      if(GA_hFont40 == NULL )
      {
        /* 使用流方式加载 */
        GA_hFont40 =GUI_Init_Extern_Font(ASCII_40_4BPP);
        if(GA_hFont40 == NULL)
        {
            /* 依然失败就用默认字体 */
            GA_hFont40 = defaultFont;
        }
      }
#endif      

			type_id =3;

			frame=0;
			fps =0;
			t0 =GUI_GetTickCount();
			hdc_mem =CreateMemoryDC(SURF_SCREEN,MEMDC_W,MEMDC_H);

			rc.w =130;
			rc.h =50;
			rc.x = MEMDC_W + (rc0.w - MEMDC_W - rc.w)/2;
			rc.y =50;
   
      
//      hdc =BeginPaint(hwnd,&ps);

      /* Home按钮 */    
			wnd=CreateWindow(BUTTON,L"O",	WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 740, 17, 36, 36,hwnd,ID_EXIT,NULL,NULL); //创建一个按钮.
			SetWindowFont(wnd,controlFont_64); //设置控件窗口字体.

//      SetFont(hdc, defaultFont);
//      SetTextColor(hdc,MapRGB(hdc,255,255,255)); 

//			OffsetRect(&rc,0,rc.h+10);
//			TextOut(hdc,rc.x,rc.y,L"帧率",-1); 
      wnd=CreateWindow(BUTTON,L"帧率",TBS_FLAT|WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w-40,rc.h,hwnd,ID_INFO,NULL,NULL); //创建一个文字框.
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.


			OffsetRect(&rc,0,rc.h);
			wnd=CreateWindow(TEXTBOX,L"---FPS",TBS_FLAT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_FPS,NULL,NULL); //创建一个文字框.
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h+5);
//			TextOut(hdc,rc.x,rc.y,L"CPU使用率",-1); 
      wnd=CreateWindow(BUTTON,L"CPU使用率",TBS_FLAT|WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_INFO,NULL,NULL); //创建一个文字框.
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h);
			wnd=CreateWindow(TEXTBOX,L"---%",TBS_FLAT|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_CPU_LOAD,NULL,NULL); //创建一个文字框.
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h+5);
//			TextOut(hdc,rc.x,rc.y,L"显示内容",-1); 
      wnd=CreateWindow(BUTTON,L"显示内容",TBS_FLAT|WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_INFO,NULL,NULL); //创建一个文字框.
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h);
			wnd=CreateWindow(BUTTON,L"下一种",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_NEXT,NULL,NULL);
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.

			OffsetRect(&rc,0,rc.h+5);
//			TextOut(hdc,rc.x,rc.y,L"Chrome加速器",-1); 
      wnd=CreateWindow(BUTTON,L"Chrom加速器",TBS_FLAT|WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE,rc.x,rc.y,rc.w+40,rc.h,hwnd,ID_INFO,NULL,NULL); //创建一个文字框.
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.
      
			OffsetRect(&rc,0,rc.h);
			wnd=CreateWindow(BUTTON,L"开启/关闭",BS_FLAT|BS_NOTIFY|WS_VISIBLE,rc.x,rc.y,rc.w,rc.h,hwnd,ID_DMA2D_EN,NULL,NULL);
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.


			wnd=CreateWindow(BUTTON,L"随机图案",TBS_FLAT|WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,0,0,100,35,hwnd,ID_TITLE,NULL,NULL); //创建一个文字框.
			SetWindowFont(wnd,defaultFont); //设置控件窗口字体.
      
      /* Chrom-ART 激活 */      
      wnd=CreateWindow(BUTTON,L"Chrom-ART 激活",TBS_FLAT|WS_OWNERDRAW|WS_TRANSPARENT|WS_VISIBLE,400,0,200,35,hwnd,ID_ART_ACTIVE,NULL,NULL); //创建一个文字框.
      SetWindowFont(wnd,defaultFont); //设置控件窗口字体.
      g_dma2d_en = TRUE;
//			EndPaint(hwnd,&ps);
      hdc_bk = CreateMemoryDC(SURF_SCREEN, 800, 480);
      
		}
		return TRUE;
		////
	case	WM_DRAWITEM:
	{
		/*　当控件指定了WS_OWNERDRAW风格，则每次在绘制前都会给父窗口发送WM_DRAWITEM消息。
		 *  wParam参数指明了发送该消息的控件ID;lParam参数指向一个DRAWITEM_HDR的结构体指针，
		 *  该指针成员包含了一些控件绘制相关的参数.
		 */

		DRAWITEM_HDR *ds;

		ds = (DRAWITEM_HDR*)lParam;
      if(ds->ID == ID_EXIT)
         exit_owner_draw(ds);
      else
         GA_BUTTON_OwnerDraw(ds);
	   /* 返回TRUE表明使用重绘操作 */
		return TRUE;
	}
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
			RECT rc =*(RECT*)lParam;;

			GetClientRect(hwnd,&rc);
			SetBrushColor(hdc_bk,MapRGB888(hdc_bk,BGCOLOR));
			FillRect(hdc_bk,&rc);
      BitBlt(hdc, 0,0,800,480,hdc_bk,0,0,SRCCOPY);
			return TRUE;
		}
//		break;
		////
		case WM_CTLCOLOR:
		{
			u16 id;

			id =LOWORD(wParam);

//			if(id == ID_TITLE ||id == ID_INFO) //ID_TEXT控件的颜色.
//			{
//				CTLCOLOR *cr;
//				cr =(CTLCOLOR*)lParam;

//				cr->TextColor =RGB888(250,250,250);
//				cr->BackColor =BGCOLOR;
//				cr->BorderColor =RGB888(50,50,50);
//				return TRUE;
//			}
//			else 
//      if( id == ID_ART_ACTIVE ) //ID_TEXT控件的颜色.
//			{
//				CTLCOLOR *cr;
//				cr =(CTLCOLOR*)lParam;

//				cr->TextColor =RGB888(0,255,0);
//				cr->BackColor =BGCOLOR;
//				cr->BorderColor =RGB888(50,50,50);
//				return TRUE;
//			}
//			else 
      if(id == ID_FPS  ) //ID_FPS控件的颜色.
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
        
        cpu_usage_get((char *)&cpu_load_major);

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
 #if 0     
      if(g_dma2d_en)
      {
        wnd =GetDlgItem(hwnd,ID_ART_ACTIVE);
				SetWindowText(wnd,L"Chrom-ART 激活");

//        ShowWindow(GetDlgItem(hwnd, ID_ART_ACTIVE), SW_SHOW);
      }
      else
      {
        wnd =GetDlgItem(hwnd,ID_ART_ACTIVE);
				SetWindowText(wnd,L"");

//        ShowWindow(GetDlgItem(hwnd, ID_ART_ACTIVE), SW_HIDE);
      }
#endif

			hdc =BeginPaint(hwnd,&ps); //开始绘图

			////用户的绘制内容...
			DrawHandler(hdc_mem,MEMDC_W,MEMDC_H);

			BitBlt(hdc,0,40,MEMDC_W,MEMDC_H,hdc_mem,0,0,SRCCOPY);
      
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
      DeleteDC(blue_fish_hdc);
      DeleteDC(red_fish_hdc);
      DeleteDC(crocodile_hdc);
      DeleteDC(Okami_hdc);
      DeleteDC(hdc_bk);
      if(GA_hFont24 != defaultFont)
        DeleteFont(GA_hFont24);
      if(GA_hFont32 != defaultFont)
        DeleteFont(GA_hFont32);      
      if(GA_hFont40 != defaultFont)
        DeleteFont(GA_hFont40);  

      GUI_VMEM_Free((void*)font_32);      
      GUI_VMEM_Free((void*)font_40);      

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

void	GUI_DEMO_Graphics_Accelerator(void)
{
	HWND	hwnd;
	WNDCLASS	wcex;
	MSG msg;

	/////
  GUI_DEBUG("Enter");
	wcex.Tag 		    = WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;

	//创建主窗口
	hwnd	=CreateWindowEx(	WS_EX_LOCKPOS|WS_EX_NOFOCUS,
                            &wcex,
                            L"GUI_DEMO: MEMDC Blt", //窗口名称
                            WS_VISIBLE|WS_CLIPCHILDREN,
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
