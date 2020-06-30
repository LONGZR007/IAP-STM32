#include <emXGUI.h>
#include <string.h>
#include <stdio.h>
#include "ff.h"
#include "Widget.h"
#include "x_libc.h"
#include "GUI_PicViewr_Dialog.h"
#include "GUI_AppDef.h"
#include "emXGUI_JPEG.h"
#include "emxgui_png.h"
#include  "gui_picture_port.h"
static char pic_path[100]="0:";//文件根目录
PicViewer_Dialog_Typedef s_PicViewer_Dialog;


icon_S GUI_PicViewer_Icon[12] = 
{
  {"Pic_Name",           {100,0,600,35},       FALSE},
  {"Pic_MSGBOX",         {200,240,400,70},     FALSE},
  {"Pic_Res",            {130,35,100,35},      FALSE},
  {"Pic_Res_Value",      {230,35,100,35},      FALSE},
  {"Pic_Time",           {330,35,70,35},       FALSE}, 
  {"Pic_Time_Value",     {400,35,90,35},       FALSE},
  {"Pic_FPS",            {490,35,70,35},       FALSE}, 
  {"Pic_FPS_Value",      {560,35,90,35},       FALSE},  
  {"In_Flash",           {0,420,250,60},       FALSE},
  {"Ex_Flash",           {250,420,250,60},     FALSE}, 
  {"SD_Card",            {500,420,300,60},     FALSE}, 
  {"piay",               {375,415,64,64},     FALSE}, 
  
};

static uint8_t viewr_flag = 0;


#if 1
//退出按钮重绘制
static void PicViewer_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;

	hdc = ds->hDC;   
	rc = ds->rc; 

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));      //设置画笔色
	}

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -1);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }

}
//透明文本
static void PicViewer_TBOX_OwnerDraw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
	HWND hwnd;
	HDC hdc, hdc_mem;
	RECT rc,rc_tmp;
	WCHAR wbuf[128];
	hwnd = ds->hwnd; //button的窗口句柄.
	hdc = ds->hDC;   //button的绘图上下文句柄.
  
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换   
  
  hdc_mem = GetDC(s_PicViewer_Dialog.PicView_Handle);//CreateMemoryDC(SURF_ARGB4444,rc.w,rc.h); 
  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, rc_tmp.x, rc_tmp.y, SRCCOPY);

  DeleteDC(hdc_mem);

  SetTextColor(hdc, MapRGB(hdc, 255, 255, 255));


  GetWindowText(hwnd, wbuf, 128); //获得按钮控件的文字

  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
}

static void PicView_Button_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc, hdc_mem; //控件窗口HDC
  HWND hwnd; //控件句柄 
  RECT rc, rc_tmp;//控件的位置大小矩形
  WCHAR wbuf[128];
  hwnd = ds->hwnd;
  hdc = ds->hDC; 
  
  
  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
  GetClientRect(hwnd, &rc);//得到控件的位置
  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换   
  
  hdc_mem = GetDC(s_PicViewer_Dialog.PicView_Handle);//CreateMemoryDC(SURF_ARGB4444,rc.w,rc.h); 
  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_mem, rc_tmp.x, rc_tmp.y, SRCCOPY);

  DeleteDC(hdc_mem);
	
  GetWindowText(ds->hwnd,wbuf,128); //获得按钮控件的文字  
   //设置按键的颜色
  SetTextColor(hdc, MapRGB(hdc,250,250,250));
   //NEXT键、BACK键和LIST键按下时，改变颜色
	if((ds->State & BST_PUSHED) )
	{ //按钮是按下状态
		SetTextColor(hdc, MapRGB(hdc, 105,105,105));      //设置文字色     
	}
  if(!IsWindowEnabled(hwnd))
  {
    SetTextColor(hdc, MapRGB(hdc, 105,105,105));
  }

   DrawText(hdc, wbuf,-1,&rc,DT_VCENTER|DT_CENTER);//绘制文字(居中对齐方式)
    
}
#endif

//遍历整个SD卡
static FRESULT scan_Picfiles(char** file_list, char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn; 
  char file_name[100];	
	
#if _USE_LFN 
  static char lfn[_MAX_LFN * (0x81 ? 2 : 1) + 1]; 	//长文件名支持
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif  
  res = f_opendir(&dir, path); //打开目录
  if (res == FR_OK) 
  { 
    i = strlen(path); 
    for (;;) 
    { 
      res = f_readdir(&dir, &fno); 										//读取目录下的内容
     if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      if (*fn == '.') continue; 											//点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
        sprintf(&path[i], "/%s", fn); 							//合成完整目录名
        res = scan_Picfiles(file_list, path);											//递归遍历 
        if (res != FR_OK) 
					break; 																		//打开失败，跳出循环
        path[i] = 0; 
      } 
      else 
      {  
				//printf("%s%s\r\n", path, fn);								//输出文件名
				if(strstr(fn,".jpg")||strstr(fn,".png")||strstr(fn,".bmp")||strstr(fn,".gif"))
				{
					if ((strlen(path)+strlen(fn)<PICFILE_NAME_MAXLEN)&&
              (s_PicViewer_Dialog.m_file_nums<PICFILE_NUM_MAX))
					{
						sprintf(file_name, "%s/%s", path, fn);						
//						memcpy(PicViewer.pic_list[PicViewer.m_file_nums],file_name,strlen(file_name));//打开路径
						memcpy(file_list[s_PicViewer_Dialog.m_file_nums],file_name,strlen(file_name));						//文件名
						s_PicViewer_Dialog.m_file_nums++;//记录文件个数
					}
				}//if
      }//else
     } //for
  } 
  return res; 
}


void Draw_Pic_JPG(char *file_name)
{
  BOOL res;
//  RECT rc = {0,0,800,480};
  u8 *jpeg_buf;
  u32 jpeg_size;
  JPG_DEC *dec;
  
  HDC PicView_Hdc;
  res= FS_Load_Content(file_name, (char**)&jpeg_buf, &jpeg_size);
  if(res)
  {
    /* 根据图片数据创建JPG_DEC句柄 */
    dec = JPG_Open(jpeg_buf, jpeg_size);
    /* 读取图片文件信息 */
    JPG_GetImageSize(&s_PicViewer_Dialog.ms_jpg.m_jpg_wid, &s_PicViewer_Dialog.ms_jpg.m_jpg_high,dec);
    u16 wid = s_PicViewer_Dialog.ms_jpg.m_jpg_wid, high = s_PicViewer_Dialog.ms_jpg.m_jpg_high;
    PicView_Hdc = GetDC(s_PicViewer_Dialog.PicView_Handle);
        
    HDC hdc_tmp;
    
    hdc_tmp = CreateMemoryDC(SURF_SCREEN, GUI_XSIZE, GUI_YSIZE);
    
    JPG_Draw(hdc_tmp, 0,0, dec);
    if(high == 480)
      BitBlt(PicView_Hdc, 400-wid/2, 240 - high/2, wid, high, hdc_tmp,0,0,SRCCOPY);
    else
      BitBlt(PicView_Hdc, 400-wid/2, 275 - high/2, wid, high, hdc_tmp,0,0,SRCCOPY);
    DeleteDC(hdc_tmp);
    DeleteDC(PicView_Hdc);
    /* 关闭JPG_DEC句柄 */
    JPG_Close(dec);
  }
  /* 释放图片内容空间 */
  RES_Release_Content((char **)&jpeg_buf);  
}


void Draw_Pic_BMP(char *file_name)
{
  HDC hdc, hdc_tmp;
  RECT rc = {0,0,800,480};
  PIC_BMP_GetInfo_FS(&s_PicViewer_Dialog.ms_bmp.bm_info,file_name);
  
  hdc = GetDC(s_PicViewer_Dialog.PicView_Handle);
  
  hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480); 
  
  SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, PICVIWER_BACK_GROUND));
  FillRect(hdc_tmp, &rc);
  
  PIC_BMP_Draw_FS(hdc_tmp, 0,0, file_name, NULL); 
  if(s_PicViewer_Dialog.ms_bmp.bm_info.Height == 480)
    BitBlt(hdc,400-s_PicViewer_Dialog.ms_bmp.bm_info.Width/2,240 - s_PicViewer_Dialog.ms_bmp.bm_info.Height/2,
          s_PicViewer_Dialog.ms_bmp.bm_info.Width,s_PicViewer_Dialog.ms_bmp.bm_info.Height,hdc_tmp,0,0,SRCCOPY);  
  else  
    BitBlt(hdc,400-s_PicViewer_Dialog.ms_bmp.bm_info.Width/2,275 - s_PicViewer_Dialog.ms_bmp.bm_info.Height/2,
          s_PicViewer_Dialog.ms_bmp.bm_info.Width,s_PicViewer_Dialog.ms_bmp.bm_info.Height,hdc_tmp,0,0,SRCCOPY);
  DeleteDC(hdc);
  DeleteDC(hdc_tmp);  
}

void Draw_Pic_PNG(char *file_name)
{
  BOOL res;
  
  u8 *png_buf;
  u32 png_size;
  PNG_DEC *png_dec;
  HDC hdc, hdc_tmp;
  RECT rc = {0,0,800,480};
  res= FS_Load_Content(file_name, (char**)&png_buf, &png_size);
  if(res)
  {
    png_dec = PNG_Open(png_buf);
    
    PNG_GetBitmap(png_dec, &s_PicViewer_Dialog.ms_png.png_bm);
    
    
    hdc = GetDC(s_PicViewer_Dialog.PicView_Handle);
    
    hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480); 
    SetBrushColor(hdc_tmp, MapRGB(hdc_tmp, PICVIWER_BACK_GROUND));
    FillRect(hdc_tmp, &rc);  
    
    DrawBitmap(hdc_tmp, 0,0, &s_PicViewer_Dialog.ms_png.png_bm, NULL);
    
    if(s_PicViewer_Dialog.ms_png.png_bm.Height == 480)
      BitBlt(hdc,400-s_PicViewer_Dialog.ms_png.png_bm.Width/2,240 - s_PicViewer_Dialog.ms_png.png_bm.Height/2,
            s_PicViewer_Dialog.ms_png.png_bm.Width,s_PicViewer_Dialog.ms_png.png_bm.Height,hdc_tmp,0,0,SRCCOPY);  
    else
      BitBlt(hdc,400-s_PicViewer_Dialog.ms_png.png_bm.Width/2,275 - s_PicViewer_Dialog.ms_png.png_bm.Height/2,
            s_PicViewer_Dialog.ms_png.png_bm.Width,s_PicViewer_Dialog.ms_png.png_bm.Height,hdc_tmp,0,0,SRCCOPY);    
  
    DeleteDC(hdc_tmp);
    DeleteDC(hdc);
    PNG_Close(png_dec);
  }
  RES_Release_Content((char **)&png_buf);
}


 
void Draw_Pic_GIF(char *file_name)
{
  RECT rc = {0,0,800,480};
  HDC hdc;
  //GUI_DEBUG("%s", file_name);
//  if(res)
  {
    switch(s_PicViewer_Dialog.ms_gif.m_gif_state)
    {
      case 0:
      {
        s_PicViewer_Dialog.ms_gif.m_gif_curnums = 0;//清除计数
        s_PicViewer_Dialog.ms_gif.m_gif_state = 1;

        FS_Load_Content(file_name, (char**)&s_PicViewer_Dialog.ms_gif.gif_buf, &s_PicViewer_Dialog.ms_gif.gif_size);
        s_PicViewer_Dialog.ms_gif.m_hgif = GIF_Open(s_PicViewer_Dialog.ms_gif.gif_buf);
        GIF_GetInfo(s_PicViewer_Dialog.ms_gif.m_hgif,&(s_PicViewer_Dialog.ms_gif.img_info));
        s_PicViewer_Dialog.ms_gif.frame_num = GIF_GetFrameCount(s_PicViewer_Dialog.ms_gif.m_hgif); 

//        PicViewer.pic_width = s_PicViewer_Dialog.ms_gif.img_info.Width;
//        PicViewer.pic_height = s_PicViewer_Dialog.ms_gif.img_info.Height;
           
        RES_Release_Content((char **)&s_PicViewer_Dialog.ms_gif.gif_buf);
      }
      case 1:
      {
        if(s_PicViewer_Dialog.ms_gif.m_gif_curnums >= s_PicViewer_Dialog.ms_gif.frame_num)
        {
           s_PicViewer_Dialog.ms_gif.m_gif_curnums = 0;
        }  
        hdc = GetDC(s_PicViewer_Dialog.PicView_Handle);
          
        HDC hdc_tmp;
          
        hdc_tmp = CreateMemoryDC(SURF_SCREEN, 800, 480);
        SetBrushColor(hdc_tmp, MapRGB(hdc_tmp,PICVIWER_BACK_GROUND));
        FillRect(hdc_tmp, &rc);     
        
        s_PicViewer_Dialog.ms_gif.m_delay = GIF_DrawFrame(hdc_tmp,0,0,MapRGB(hdc,PICVIWER_BACK_GROUND),
                                                          s_PicViewer_Dialog.ms_gif.m_hgif,
                                                          s_PicViewer_Dialog.ms_gif.m_gif_curnums); 
        
        BitBlt( hdc,400-s_PicViewer_Dialog.ms_gif.img_info.Width/2,
                275 - s_PicViewer_Dialog.ms_gif.img_info.Width/2,
                s_PicViewer_Dialog.ms_gif.img_info.Width,
                s_PicViewer_Dialog.ms_gif.img_info.Width,
                hdc_tmp,0,0,SRCCOPY);     
        DeleteDC(hdc_tmp);
        DeleteDC(hdc);
        s_PicViewer_Dialog.ms_gif.m_gif_curnums++;
        break;
      }
    } 
  }
}

static PicTypeDef Judge_FileType(char* fliename)
{
  if(strstr(fliename,".jpg"))
    return Type_JPG;
  else if(strstr(fliename,".png"))
    return Type_PNG;
  else if(strstr(fliename,".gif"))
    return Type_GIF;
  else if(strstr(fliename,".bmp"))
    return Type_BMP;
  return Type_None;
}

static uint32_t Get_FlieNames(char *file_path)
{
  uint32_t pos[10];
  uint32_t i = 0;
  uint32_t count = 0;
  char *p = file_path;
  do
  {
    i++;
    if(*p == '/')
    {
      pos[count] = i;
      count++;
      
    } 
  p++;
  }while(*p != '\0');
  
  return pos[count-1];
}

static LRESULT	PicViewer_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CREATE:
    {
      RECT rc;
      GetClientRect(hwnd, &rc); 
      CreateWindow(BUTTON, L"图片浏览器", WS_TRANSPARENT|WS_OWNERDRAW|WS_VISIBLE, 100,0,600,35,          
                   hwnd, eID_Pic_Name, NULL, NULL);
//      CreateWindow(BUTTON, L"支持jpg、bmp、png、gif格式", WS_TRANSPARENT|WS_OWNERDRAW  |WS_VISIBLE, 100,35,600,35,          
//                   hwnd, eID_Pic_Def, NULL, NULL);
      
      CreateWindow(BUTTON, L"L", WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW |WS_VISIBLE,
                   0, rc.h * 1 / 2-70/2, 70, 70, hwnd, eID_Pic_PREV, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_PREV), controlFont_64); 
      
      
      CreateWindow(BUTTON, L"K", WS_TRANSPARENT | BS_FLAT | BS_NOTIFY | WS_OWNERDRAW | WS_VISIBLE,
                   rc.w - 65, rc.h * 1 / 2-70/2, 70, 70, hwnd, eID_Pic_NEXT, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_NEXT), controlFont_64);
            
      CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                 740, 18, 36, 36, hwnd, eID_Pic_EXIT, NULL, NULL); 

      CreateWindow(BUTTON, L"分辨率：", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[2].rc.x, GUI_PicViewer_Icon[2].rc.y, 
                   GUI_PicViewer_Icon[2].rc.w, GUI_PicViewer_Icon[2].rc.h,          
                   hwnd, eID_Pic_Res, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[3].rc.x, GUI_PicViewer_Icon[3].rc.y, 
                   GUI_PicViewer_Icon[3].rc.w, GUI_PicViewer_Icon[3].rc.h,          
                   hwnd, eID_Pic_Res_Value, NULL, NULL);
                   
      CreateWindow(BUTTON, L"时间：", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[4].rc.x, GUI_PicViewer_Icon[4].rc.y, 
                   GUI_PicViewer_Icon[4].rc.w, GUI_PicViewer_Icon[4].rc.h,          
                   hwnd, eID_Pic_Time, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[5].rc.x, GUI_PicViewer_Icon[5].rc.y, 
                   GUI_PicViewer_Icon[5].rc.w, GUI_PicViewer_Icon[5].rc.h,          
                   hwnd, eID_Pic_Time_Value, NULL, NULL);
    
      CreateWindow(BUTTON, L"帧率：", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[6].rc.x, GUI_PicViewer_Icon[6].rc.y, 
                   GUI_PicViewer_Icon[6].rc.w, GUI_PicViewer_Icon[6].rc.h,          
                   hwnd, eID_Pic_FPS, NULL, NULL);

      CreateWindow(BUTTON, L" ", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[7].rc.x, GUI_PicViewer_Icon[7].rc.y, 
                   GUI_PicViewer_Icon[7].rc.w, GUI_PicViewer_Icon[7].rc.h,          
                   hwnd, eID_Pic_FPS_Value, NULL, NULL);

      CreateWindow(BUTTON, L"T", WS_OWNERDRAW|WS_VISIBLE|WS_TRANSPARENT, 
                   GUI_PicViewer_Icon[11].rc.x, GUI_PicViewer_Icon[11].rc.y, 
                   GUI_PicViewer_Icon[11].rc.w, GUI_PicViewer_Icon[11].rc.h,          
                   hwnd, eID_Pic_PLAY, NULL, NULL);
      SetWindowFont(GetDlgItem(hwnd,eID_Pic_PLAY), controlFont_64);
      
      SetTimer(hwnd,2,50,TMR_SINGLE|TMR_START,NULL);
      
      //Malloc for file list
      s_PicViewer_Dialog.mp_file_list = (char **)GUI_VMEM_Alloc(sizeof(char *) * PICFILE_NUM_MAX);
      for(int i = 0; i < PICFILE_NUM_MAX; i++)
      {
        s_PicViewer_Dialog.mp_file_list[i] = (char *)GUI_VMEM_Alloc(sizeof(char) * PICFILE_NAME_MAXLEN);	
        memset(s_PicViewer_Dialog.mp_file_list[i],0,PICFILE_NAME_MAXLEN);
      }	           
      scan_Picfiles(s_PicViewer_Dialog.mp_file_list, pic_path); 
      //SendMessage(hwnd, UpdatePicInfo, NULL, NULL);
      break;
    } 
    case WM_TIMER:
    {
      int tmr_id;
       
      tmr_id = wParam;

      if (tmr_id == 2)
        InvalidateRect(hwnd,NULL,TRUE);
      else if (tmr_id == 3)
      {
        s_PicViewer_Dialog.m_file_index++;
        if (s_PicViewer_Dialog.m_file_index >= s_PicViewer_Dialog.m_file_nums)
          s_PicViewer_Dialog.m_file_index=0;

        if(s_PicViewer_Dialog.ms_gif.m_gif_state == 1)
          SendMessage(hwnd, CloseGif, NULL, NULL);
        //SendMessage(hwnd, UpdateButtonState, (WPARAM)eID_Pic_NEXT, NULL);
        InvalidateRect(hwnd,NULL, TRUE);
      }
      break;
    }
    case WM_ERASEBKGND:
    {
      TickType_t tick_record = xTaskGetTickCount();
      HDC hdc =(HDC)wParam;
      RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};//*(RECT*)lParam;
//      static PicTypeDef e_pictype_old = Type_None;
      PicTypeDef e_pictype = Type_None;
      TickType_t tick;
      float time=0.0f;
      SetBrushColor(hdc, MapRGB(hdc, 0, 0, 0));
      FillRect(hdc, &rc);       
      e_pictype = Judge_FileType(s_PicViewer_Dialog.mp_file_list[s_PicViewer_Dialog.m_file_index]);
//      if(e_pictype_old == Type_GIF && e_pictype != Type_GIF)
//      {
//        s_PicViewer_Dialog.ms_gif.m_gif_state = 0;
//        ResetTimer(hwnd,2,s_PicViewer_Dialog.ms_gif.m_delay,TMR_SINGLE,NULL);
//        GIF_Close(s_PicViewer_Dialog.ms_gif.m_hgif);
//      }
      
      switch(e_pictype)
      {
        case Type_JPG:
        {
          tick = xTaskGetTickCount();
          Draw_Pic_JPG(s_PicViewer_Dialog.mp_file_list[s_PicViewer_Dialog.m_file_index]);
          time = (float)(xTaskGetTickCount() - tick)/1000;
          break;
        }
        case Type_PNG:
        {
          tick = xTaskGetTickCount();
          Draw_Pic_PNG(s_PicViewer_Dialog.mp_file_list[s_PicViewer_Dialog.m_file_index]);
          time = (float)(xTaskGetTickCount() - tick)/1000;
          break;
        }
        case Type_GIF:
        {
          tick = xTaskGetTickCount();
          Draw_Pic_GIF(s_PicViewer_Dialog.mp_file_list[s_PicViewer_Dialog.m_file_index]);
          time = (float)(xTaskGetTickCount() - tick)/1000;
          ResetTimer(hwnd,2,s_PicViewer_Dialog.ms_gif.m_delay,TMR_SINGLE|TMR_START,NULL);
          break;
        }
        case Type_BMP:
        {
          tick = xTaskGetTickCount();
          Draw_Pic_BMP(s_PicViewer_Dialog.mp_file_list[s_PicViewer_Dialog.m_file_index]);
          time = (float)(xTaskGetTickCount() - tick)/1000;
          break;
        }          
      
      }
//      e_pictype_old = e_pictype;
      
      PostMessage(hwnd, UpdatePicInfo, (WPARAM)e_pictype, (LPARAM)(time*1000));
      //DeleteDC(s_PicViewer_Dialog.PicView_Hdc);
      //GUI_DEBUG("擦除背景共耗时 %d", xTaskGetTickCount() - tick_record);
      
      return FALSE;
    }   
    case WM_PAINT:
    {
      TickType_t tick_record = xTaskGetTickCount();
       HDC hdc, hdc_mem;
       PAINTSTRUCT ps;
       RECT rc = {0,0,800,70};

       hdc = BeginPaint(hwnd, &ps);

      if (!viewr_flag)
       {
        hdc_mem = CreateMemoryDC(SURF_ARGB4444, 800,70);
        SetBrushColor(hdc_mem, MapARGB(hdc_mem,100,105, 105, 105));
        FillRect(hdc_mem, &rc);
        
        BitBlt(hdc, 0, 0, 800, 70, hdc_mem,0,0,SRCCOPY);
        BitBlt(hdc, 0, GUI_YSIZE-70, 800, 70, hdc_mem,0,0,SRCCOPY);
        DeleteDC(hdc_mem);
       }

       EndPaint(hwnd, &ps);
       //GUI_DEBUG("重绘背景共耗时 %d", xTaskGetTickCount() - tick_record);
       break;
    } 
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_Pic_EXIT:
          {
            PicViewer_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }
//          case eID_Pic_Def: 
          case eID_Pic_Time: 
          case eID_Pic_Time_Value:
          case eID_Pic_FPS:
          case eID_Pic_FPS_Value:
          case eID_Pic_Res:
          case eID_Pic_Res_Value:
          case eID_Pic_Name:
          {
            PicViewer_TBOX_OwnerDraw(ds);
            return TRUE;  
          }   
          case eID_Pic_PLAY:
          case eID_Pic_PREV:
          case eID_Pic_NEXT:
          {
            PicView_Button_OwnerDraw(ds);
            return TRUE;  
          }
           
       }
       break;
    }
    case WM_NOTIFY:
    {
      u16 code, id;
      id  =LOWORD(wParam);//获取消息的ID码
      code=HIWORD(wParam);//获取消息的类型    
      if(code == BN_CLICKED && id == eID_Pic_EXIT)
      {
        PostCloseMessage(hwnd);
        break;
      }
      if(code == BN_CLICKED && id == eID_Pic_NEXT)
      {
        //PostCloseMessage(hwnd);
        s_PicViewer_Dialog.m_file_index++;
        if(s_PicViewer_Dialog.ms_gif.m_gif_state == 1)
          SendMessage(hwnd, CloseGif, NULL, NULL);
        SendMessage(hwnd, UpdateButtonState, (WPARAM)eID_Pic_NEXT, NULL);
        InvalidateRect(hwnd,NULL, TRUE);
        
        break;
      }  
      if(code == BN_CLICKED && id == eID_Pic_PREV)
      {        
        
        s_PicViewer_Dialog.m_file_index--;
       
        InvalidateRect(hwnd,NULL, TRUE);
        //切换图片时，如果当前图片为GIF类型，则需要释放GIF句柄
        if(s_PicViewer_Dialog.ms_gif.m_gif_state == 1)
          SendMessage(hwnd, CloseGif, NULL, NULL);
        SendMessage(hwnd, UpdateButtonState, (WPARAM)eID_Pic_PREV, NULL);
        
        
        break;
      }

      if(code == BN_CLICKED && id == eID_Pic_PLAY)
      { 
        static uint8_t time_flag = 0;
        time_flag = !time_flag;
        if (time_flag)
        {
          SetWindowText(GetDlgItem(hwnd, eID_Pic_PLAY), L"U");
          SetTimer(hwnd, 3, 3000, TMR_START, NULL);
        }
        else
        {
          SetWindowText(GetDlgItem(hwnd, eID_Pic_PLAY), L"T");
          KillTimer(hwnd, 3);
        }
        
        break;
      }
      
      break;
    } 
    case CloseGif:
    {
      s_PicViewer_Dialog.ms_gif.m_gif_state = 0;
      s_PicViewer_Dialog.ms_gif.frame_num = 0;
      s_PicViewer_Dialog.ms_gif.m_gif_curnums = 0;
      s_PicViewer_Dialog.ms_gif.gif_size = 0;
      s_PicViewer_Dialog.ms_gif.m_delay = 0;
      GIF_Close(s_PicViewer_Dialog.ms_gif.m_hgif);
      break;
    }
    case UpdatePicInfo:
    {      
      WCHAR wbuf[128];
      char  cbuf[128];
      PicTypeDef pic_info = (PicTypeDef)wParam;
      float load_time = (float)lParam/1000;
      char* file_name = s_PicViewer_Dialog.mp_file_list[s_PicViewer_Dialog.m_file_index];
      file_name += Get_FlieNames(file_name); 
      GetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf, 128);
      x_wcstombs_cp936(cbuf, wbuf, PICFILE_NAME_MAXLEN);
      if (strstr(file_name, cbuf) == NULL)     // 为空时不是一张图片更新信息，否则不更新
      {
        x_mbstowcs_cp936(wbuf, file_name, PICFILE_NAME_MAXLEN);
        SetWindowText(GetDlgItem(hwnd, eID_Pic_Name), wbuf);
        switch(pic_info)
        {
          case Type_JPG:
          {
            x_wsprintf(wbuf, L"%d*%d", s_PicViewer_Dialog.ms_jpg.m_jpg_wid, s_PicViewer_Dialog.ms_jpg.m_jpg_high);
            SetWindowText(GetDlgItem(hwnd, eID_Pic_Res_Value), wbuf);           
            break;
          }
          case Type_PNG:
          {
            x_wsprintf(wbuf, L"%d*%d", s_PicViewer_Dialog.ms_png.png_bm.Width, s_PicViewer_Dialog.ms_png.png_bm.Height);
            SetWindowText(GetDlgItem(hwnd, eID_Pic_Res_Value), wbuf);           
            break;
          }
          case Type_GIF:
          {
            x_wsprintf(wbuf, L"%d*%d",s_PicViewer_Dialog.ms_gif.img_info.Width, s_PicViewer_Dialog.ms_gif.img_info.Height);
            SetWindowText(GetDlgItem(hwnd, eID_Pic_Res_Value), wbuf);
            break;
          }
          case Type_BMP:
          {
            x_wsprintf(wbuf, L"%d*%d",s_PicViewer_Dialog.ms_bmp.bm_info.Width,s_PicViewer_Dialog.ms_bmp.bm_info.Height);
            SetWindowText(GetDlgItem(hwnd, eID_Pic_Res_Value), wbuf);
            break;
          } 
        }        
        x_wsprintf(wbuf, L"%.2fs", load_time);
        SetWindowText(GetDlgItem(hwnd, eID_Pic_Time_Value), wbuf);
        x_wsprintf(wbuf, L"%.1fFPS", 1/load_time);
        SetWindowText(GetDlgItem(hwnd, eID_Pic_FPS_Value), wbuf);       
      }
      break;
    }
    case UpdateButtonState:
    {
      uint16_t id = wParam;
      EnableWindow(GetDlgItem(hwnd, eID_Pic_PREV), TRUE);
      EnableWindow(GetDlgItem(hwnd, eID_Pic_NEXT), TRUE);
      switch(id)
      {
        case eID_Pic_PREV:
        {
          if(s_PicViewer_Dialog.m_file_index == 255)
          {
            s_PicViewer_Dialog.m_file_index = 0;
            EnableWindow(GetDlgItem(hwnd, eID_Pic_PREV), FALSE);
          }       
          break;
        }
        case eID_Pic_NEXT:
        {
          if(s_PicViewer_Dialog.m_file_index < s_PicViewer_Dialog.m_file_nums)
          {
            //EnableWindow(GetDlgItem(hwnd, eID_Pic_NEXT), TRUE);
          }
          else
          {
            s_PicViewer_Dialog.m_file_index--;
            EnableWindow(GetDlgItem(hwnd, eID_Pic_NEXT), FALSE);
          }           
          break;
        }
      }
      break;
    }
#if Slide_Enable 
    case WM_LBUTTONDOWN:
    {
      
      break;
      

    }
    case WM_MOUSEMOVE:
    {
      int x;
//      static int times = 0;
      static int x_move;
      x = LOWORD(lParam);
      //GUI_DEBUG("x = %d",x);
      s_PicViewer_Dialog.x_off = x - x_move;
      
      s_PicViewer_Dialog.m_touch_times++;
      if(s_PicViewer_Dialog.m_touch_times > 3)
      {
        s_PicViewer_Dialog.m_touch_times = 0;
        s_PicViewer_Dialog.m_slide_staus = 1;
      }
      x_move = x;
      
//        pApp->OnMouseMove(hwnd, x, y);
      break;
    }
    case WM_LBUTTONUP:
    {
//      POINT pt;
//      RECT rc_prev, rc_next, rc_exit;
//      pt.x = LOWORD(lParam);
//      pt.y = HIWORD(lParam);
//      
//      GetClientRect(GetDlgItem(hwnd,eID_Pic_PREV), &rc_prev);  
//      GetClientRect(GetDlgItem(hwnd,eID_Pic_NEXT), &rc_next); 
//      GetClientRect(GetDlgItem(hwnd,eID_Pic_EXIT), &rc_exit);       
////      if((PtInRect(&rc_prev, &pt) || PtInRect(&rc_next, &pt) || PtInRect(&rc_exit, &pt)))
////        break;
      
      if(s_PicViewer_Dialog.m_slide_staus != 1)
      {
        viewr_flag = !viewr_flag;
        for (uint32_t xC=eID_Pic_Name; xC<=eID_Pic_FPS_Value; xC++)
        {
          ShowWindow(GetDlgItem(hwnd, xC), viewr_flag ? SW_HIDE : SW_SHOW);
        }
        InvalidateRect(hwnd,NULL, TRUE);
        //GUI_DEBUG("viewr_flag = %d",viewr_flag);
        s_PicViewer_Dialog.m_touch_times = 0;
        break;
      }
      s_PicViewer_Dialog.m_slide_staus = 0;
      if(s_PicViewer_Dialog.x_off == 0)
        break;
      if(s_PicViewer_Dialog.x_off > 0)
      {
        s_PicViewer_Dialog.m_file_index--;
        InvalidateRect(hwnd,NULL, TRUE);
        if(s_PicViewer_Dialog.ms_gif.m_gif_state == 1)
          SendMessage(hwnd, CloseGif, NULL, NULL);        
        SendMessage(hwnd, UpdateButtonState, (WPARAM)eID_Pic_PREV, NULL);
        //SendMessage(hwnd, UpdatePicInfo, NULL, NULL);        
      }
      else
      {
        s_PicViewer_Dialog.m_file_index++;

        SendMessage(hwnd, UpdateButtonState, (WPARAM)eID_Pic_NEXT, NULL);
        InvalidateRect(hwnd,NULL, TRUE);
        if(s_PicViewer_Dialog.ms_gif.m_gif_state == 1)
          SendMessage(hwnd, CloseGif, NULL, NULL);        
        //SendMessage(hwnd, UpdatePicInfo, NULL, NULL);
      }
      break;
    } 
#endif    
    case WM_DESTROY:
    {
      for(int i = 0; i < PICFILE_NUM_MAX; i++)
        GUI_VMEM_Free(s_PicViewer_Dialog.mp_file_list[i]);
      GUI_VMEM_Free(s_PicViewer_Dialog.mp_file_list);
      s_PicViewer_Dialog.m_file_nums = 0;
      s_PicViewer_Dialog.m_file_index = 0;
      
      
      return PostQuitMessage(hwnd);	
    } 
    default:
      return	DefWindowProc(hwnd, msg, wParam, lParam);   
  }
  
  return WM_NULL;
  
}

void GUI_PicViewer_Dialog(void)
{
	
	WNDCLASS	wcex;
	MSG msg;


	wcex.Tag = WNDCLASS_TAG;

	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = PicViewer_proc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
   
	//创建主窗口
	s_PicViewer_Dialog.PicView_Handle = CreateWindowEx(WS_EX_NOFOCUS|WS_EX_FRAMEBUFFER,
                                                    &wcex,
                                                    L"GUI_LED_DIALOG",
                                                    WS_VISIBLE|WS_CLIPCHILDREN,
                                                    0, 0, GUI_XSIZE, GUI_YSIZE,
                                                    NULL, NULL, NULL, NULL);
   //显示主窗口
	ShowWindow(s_PicViewer_Dialog.PicView_Handle, SW_SHOW);
	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, s_PicViewer_Dialog.PicView_Handle))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}  
}


