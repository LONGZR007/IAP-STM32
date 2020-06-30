#include <emXGUI.h>
#include <string.h>
#include "Widget.h"
#include "./camera/bsp_ov5640.h"
#include "x_libc.h"
#include "./camera/ov5640_AF.h"
#include "qr_decoder_user.h"
#include "GUI_AppDef.h"

extern BOOL g_dma2d_en;//DMA2D使能标志位，摄像头DMEO必须禁止
extern OV5640_IDTypeDef OV5640_Camera_ID;
extern HWND Cam_hwnd;//主窗口句柄
extern HWND SetWIN;//参数设置窗口
extern int state;//初始化摄像头状态机
//extern uint16_t *cam_buff0;
//extern uint16_t *cam_buff1;
extern GUI_SEM *cam_sem;//更新图像同步信号量（二值型）
uint8_t QR_Task = 0;
TaskHandle_t QR_Task_Handle;
//定义控件ID
enum eID
{
	eID_OK =0x1000,
	eID_DText,
  eID_QR_EXIT,
};

/* 用户自定义消息 */
enum eMSG 
{
	eMSG_QRScan_OK = WM_USER + 1,
};

void ScanCompleteDialog(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops);


/*
 * @brief  更新屏幕
 * @param  NONE
 * @retval NONE
*/
//  rt_tick_t tick1 = 0;
//  rt_tick_t tick2 = 0;

static void Update_Dialog(void *p)
{
  RECT rc;
  
  rc.x = GUI_XSIZE/2 - 320/2;
  rc.y = GUI_YSIZE/2 - 320/2;
  rc.w = 320;
  rc.h = 320;
  
	while(QR_Task) //线程已创建了
	{
    GUI_SemWait(cam_sem, 0xFFFFFFFF);
    InvalidateRect(Cam_hwnd, &rc, FALSE);
	}
  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}

static void QR_decoder_Task(void *p)
{
	while(QR_Task) //线程已创建了
	{
    char  qr_type_len=0;
    short qr_data_len=0;
    char qr_type_buf[10];
    char qr_data_buf[512];
    int addr=0;
    int i=0,j=0;
    char qr_num=0;
    qr_num = QR_decoder();
    if(qr_num)
    { 
      BEEP_ON;
      GUI_msleep(50);
      //解码的数据是按照识别条码的个数封装好的二维数组，这些数据需要
      //根据识别条码的个数，按组解包并通过串口发送到上位机串口终端
      qr_type_len = decoded_buf[i][addr++];//获取解码类型长度
       
      for(j=0;j < qr_type_len;j++)
        qr_type_buf[j]=decoded_buf[i][addr++];//获取解码类型名称
      
      qr_type_buf[j] = '\0';
       
      qr_data_len  = decoded_buf[i][addr++]<<8; //获取解码数据长度高8位
      qr_data_len |= decoded_buf[i][addr++];    //获取解码数据长度低8位
       
      for(j=0;j < qr_data_len;j++)
        qr_data_buf[j]=decoded_buf[i][addr++];//获取解码数据
      
      qr_data_buf[j] = '\0';

//      printf("类型：%s\n数据：%s\n", qr_type_buf, qr_data_buf);

      addr =0;//清零
      
      WCHAR wbuf_type[40];
      WCHAR *wbuf_data = (WCHAR *)GUI_VMEM_Alloc(512 * sizeof(WCHAR));
      x_mbstowcs_cp936(wbuf_type, qr_type_buf, sizeof(wbuf_type));
      x_mbstowcs_cp936(wbuf_data, qr_data_buf, 512 * sizeof(WCHAR));

      PostAsyncMessage(Cam_hwnd, eMSG_QRScan_OK, (WPARAM)wbuf_data, (LPARAM)wbuf_type);     // 识别完成，显示结果
      BEEP_OFF;
      
      vTaskSuspend(QR_Task_Handle);    // 挂起自己 不在执行

      QR_decoder();     // 退出前识别一次，清除上一帧
      GUI_VMEM_Free(wbuf_data);
      qr_num = 0;
    }
    
    GUI_msleep(10);
	}
  GUI_Thread_Delete(GUI_GetCurThreadHandle()); 
}

/*=========================================================================================*/
static const WCHAR *pTextInt;
static const WCHAR *pCaptionInt;
static const MSGBOX_OPTIONS *opsInt;

static void DialogBoxButton_OwnerDraw(DRAWITEM_HDR *ds)
{
	HDC hdc;
	RECT rc;
	WCHAR wbuf[128];

	hdc = ds->hDC;   //button的绘图上下文句柄.
	rc = ds->rc;     //button的绘制矩形区.
  
  if(ds->State&BST_PUSHED)
	{
    /* 画按键背景区 */
    SetBrushColor(hdc,MapRGB(hdc, 200, 200, 200));
	}
	else
	{
    /* 画按键背景区 */
    SetBrushColor(hdc,MapRGB(hdc, 250, 250, 250));
	}
  
  FillRoundRect(hdc, &rc, 7);
    
  SetPenColor(hdc,MapRGB(hdc, 200, 200, 200));    /* 画灰色的线 */
  HLine(hdc, rc.x, rc.y, rc.x+rc.w);
  if (ds->ID < opsInt->ButtonCount - 1)    /* 最后一个按钮不画右边的一条线 */
  {
    VLine(hdc, rc.x+rc.w-1, rc.y, rc.y+rc.h);
  }
  
  SetPenColor(hdc,MapRGB(hdc, 105, 105, 105));           // 设置画笔的颜色
  GetWindowText(ds->hwnd, wbuf, 128);                    // 获得按钮控件的文字
//  OffsetRect(&rc, 0, -3);
  DrawText(hdc, wbuf, -1, &rc, DT_VCENTER|DT_CENTER);    // 绘制文字(居中对齐方式)
}

static LRESULT ScanCompleteWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    /* 窗口创建 */
    case WM_CREATE:
    {
      RECT rc;
      RECT rc_Text;
      RECT rc_button;
//      int FontH;
//      FontH = GetFontAveHeight(defaultFont);
      GetClientRect(hwnd, &rc);

      for(uint8_t xC=0; xC<opsInt->ButtonCount; xC++)
      {
        /* 计算按键位置 （高度为窗口的四分之一） */
        rc_button.w = rc.w / opsInt->ButtonCount;    // 按钮的宽度
        rc_button.h = rc.h / 4;                      // 窗口高度的 1/4
        rc_button.x = rc_button.w * xC;
        rc_button.y = rc.h - rc_button.h;
        
        /* 创建一个按钮 */
        CreateWindow(BUTTON, opsInt->pButtonText[xC], BS_FLAT|WS_TRANSPARENT|WS_VISIBLE|WS_OWNERDRAW, //
                     rc_button.x, rc_button.y, rc_button.w, rc_button.h, hwnd, xC, NULL, NULL);
      }
      
       /* 计算消息类型的提示位置 */
      rc_Text.w = rc.w;
      rc_Text.h = rc.h / 4 * 2;
      rc_Text.y = rc.y + rc.h / 4;
      rc_Text.x = rc.x;
      
      /* 显示消息类型 */
      CreateWindow(TEXTBOX, pTextInt, WS_VISIBLE, rc_Text.x, rc_Text.y, rc_Text.w, rc_Text.h, hwnd, eID_DText, NULL, NULL);
      SendMessage(GetDlgItem(hwnd, eID_DText), TBM_SET_TEXTFLAG, 0, DT_TOP | DT_LEFT | DT_BKGND | DT_WORDBREAK);
//      DrawText(hdc, , -1, &rc_Text, DT_VCENTER|DT_CENTER);
      
      break; 
    }
    
    case WM_ERASEBKGND:
    {
      return TRUE;    // 直接返回 TRUE 则库不会画默认背景
    }

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc;
      RECT rc;
      RECT rc_Text;

      GetClientRect(hwnd,&rc);
      hdc = BeginPaint(hwnd,&ps);
      
//      SetBrushColor(hdc,MapRGB(hdc,50,0,0));
//      FillRect(hdc, &rc);
      SetBrushColor(hdc, MapRGB(hdc,250,250,250));
      FillRoundRect(hdc, &rc, 7);
      
      /* 计算消息类型的提示位置 */
      rc_Text.w = rc.w;
      rc_Text.h = rc.h / 4;
      rc_Text.x = rc.x;
      rc_Text.y = rc.y;
      
      /* 显示消息类型 */
      DrawText(hdc, pCaptionInt, -1, &rc_Text, DT_VCENTER|DT_CENTER);
      
      EndPaint(hwnd,&ps);
      break;
    }
    
    case WM_NOTIFY: 
    {
      u16 code;
      u16 id;
			id =LOWORD(wParam);
      code=HIWORD(wParam);//获取消息的类型

      if(code == BN_CLICKED)
      {
        if (id == 0)
          PostCloseMessage(hwnd);
      }
      break;
     }
    
     case	WM_CTLCOLOR:
		{
			/* 控件在绘制前，会发送 WM_CTLCOLOR到父窗口.
			 * wParam参数指明了发送该消息的控件ID;lParam参数指向一个CTLCOLOR的结构体指针.
			 * 用户可以通过这个结构体改变控件的颜色值.用户修改颜色参数后，需返回TRUE，否则，系统
			 * 将忽略本次操作，继续使用默认的颜色进行绘制.
			 *
			 */
			u16 id;
			id =LOWORD(wParam);
      /* 文本框背景文字颜色设置 */
			if(id== eID_DText)
			{
				CTLCOLOR *cr;
				cr =(CTLCOLOR*)lParam;
        cr->TextColor =RGB888(0,0,0);//背景颜色（RGB888颜色格式)
				cr->BackColor =RGB888(250,250,250);//背景颜色（RGB888颜色格式)
				return TRUE;
			}

        return FALSE;
		}
    
     case WM_DRAWITEM:
     {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       
       DialogBoxButton_OwnerDraw(ds);
       
       return TRUE;
     }
    
    case WM_DESTROY:
    {
      return PostQuitMessage(hwnd);
    }
    
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  
  return WM_NULL;
}

//退出按钮重绘制
static void QR_ExitButton_OwnerDraw(DRAWITEM_HDR *ds)
{
  HDC hdc;
  RECT rc;
//  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
//  hwnd = ds->hwnd;

//  GetClientRect(hwnd, &rc_tmp);//得到控件的位置
//  WindowToScreen(hwnd, (POINT *)&rc_tmp, 1);//坐标转换

//  BitBlt(hdc, rc.x, rc.y, rc.w, rc.h, hdc_bk, rc_tmp.x, rc_tmp.y, SRCCOPY);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //设置画笔色
	}
	else
	{ //按钮是弹起状态
		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -1);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }

}

void ScanCompleteDialog(HWND hwndParent, RECT rc,const WCHAR *pText,const WCHAR *pCaption,const MSGBOX_OPTIONS *ops)
{
  WNDCLASS	wcex;

  if(hwndParent==NULL)
	{
		hwndParent =GetDesktopWindow();
	}
  
  /* 初始化内部参数 */
  pTextInt = pText;
  pCaptionInt = pCaption;
  opsInt = ops;
  
	wcex.Tag = WNDCLASS_TAG;  
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ScanCompleteWinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	DialogWindow(	&wcex,	pCaption,
//							WS_OVERLAPPED|WS_BORDER|WS_DLGFRAME|WS_CLIPCHILDREN,
                WS_CLIPCHILDREN,//WS_CAPTIONWS_BORDER|
							rc.x, rc.y, rc.w, rc.h,
							hwndParent, 0x0000, NULL, NULL);//hInst

// 	//显示主窗口
// 	ShowWindow(hwnd, SW_SHOW);
}

/*============================================================================*/
/*
 * @brief  初始化参数配置
 * @param  NONE
 * @retval NONE
*/
static void Camera_ReConfig(void)
{
  cam_mode.frame_rate = FRAME_RATE_15FPS;	
	
	//ISP窗口
	cam_mode.cam_isp_sx = 0;
	cam_mode.cam_isp_sy = 0;	
	
	cam_mode.cam_isp_width = 1920;
	cam_mode.cam_isp_height = 1080;
	
	//输出窗口
	cam_mode.scaling = 1;     //使能自动缩放
	cam_mode.cam_out_sx = 16;	//使能自动缩放后，一般配置成16即可
	cam_mode.cam_out_sy = 4;	  //使能自动缩放后，一般配置成4即可
	cam_mode.cam_out_width = 800;
	cam_mode.cam_out_height = 480;
	
	//LCD位置
	cam_mode.lcd_sx = 0;
	cam_mode.lcd_sy = 0;
	cam_mode.lcd_scan = 5; //LCD扫描模式，本横屏配置可用1、3、5、7模式
	
	//以下可根据自己的需要调整，参数范围见结构体类型定义	
	cam_mode.light_mode = 0;//自动光照模式
	cam_mode.saturation = 0;	
	cam_mode.brightness = 0;
	cam_mode.contrast = 0;
	cam_mode.effect = 0;		//正常模式
	cam_mode.exposure = 0;		

	cam_mode.auto_focus = 1;
}

/*
 * @brief  摄像头窗口回调函数
*/
extern int SelectDialogBox(HWND hwndParent, RECT rc, const WCHAR *pText, const WCHAR *pCaption, const MSGBOX_OPTIONS *ops);
static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
//  static HDC hdc_mem_temp;
  
  switch(msg)
  {
    case WM_CREATE:
    {
      /* 初始化摄像头GPIO及IIC */
      OV5640_HW_Init();  
      /* 读取摄像头芯片ID，确定摄像头正常连接 */
      OV5640_ReadID(&OV5640_Camera_ID);
      
       CreateWindow(BUTTON, L"O", WS_TRANSPARENT|BS_FLAT | BS_NOTIFY |WS_OWNERDRAW|WS_VISIBLE,
                  740, 17, 36, 36, hwnd, eID_QR_EXIT, NULL, NULL); 

      if(OV5640_Camera_ID.PIDH  == 0x56)
      {
        SetTimer(hwnd, 1, 400, TMR_START | TMR_SINGLE, NULL);  
        GUI_DEBUG("OV5640 ID:%x %x",OV5640_Camera_ID.PIDH ,OV5640_Camera_ID.PIDL);
      }
      else
      {
        // MSGBOX_OPTIONS ops;
        // //const WCHAR *btn[]={L"确定"};
        // int x,y,w,h;

        // ops.Flag =MB_ICONERROR;
        // //ops.pButtonText =btn;
        // ops.ButtonCount =0;
        // w =500;
        // h =200;
        // x =(GUI_XSIZE-w)>>1;
        // y =(GUI_YSIZE-h)>>1;
        // MessageBox(hwnd,x,y,w,h,L"没有检测到OV5640摄像头，\n请重新检查连接。",L"错误",&ops); 
        // PostCloseMessage(hwnd);
        SetTimer(hwnd, 3, 3, TMR_START | TMR_SINGLE, NULL);      // 初始化出错启动提示
      }
      cam_sem = GUI_SemCreate(0,1);//同步摄像头图像

      QR_Task = 1;
      
      xTaskCreate((TaskFunction_t )Update_Dialog,   /* 任务入口函数 */
                            (const char*    )"Update_Dialog",       /* 任务名字 */
                            (uint16_t       )1024/4,                 /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )NULL,                  /* 任务入口函数参数 */
                            (UBaseType_t    )5,                     /* 任务的优先级 */
                            (TaskHandle_t  )NULL);                  /* 任务控制块指针 */
                            
      xTaskCreate((TaskFunction_t )QR_decoder_Task,  /* 任务入口函数 */
                            (const char*    )"QR decoder Task",     /* 任务名字 */
                            (uint16_t       )1024*5,              /* 任务栈大小FreeRTOS的任务栈以字为单位 */
                            (void*          )NULL,                  /* 任务入口函数参数 */
                            (UBaseType_t    )4,                     /* 任务的优先级 */
                            (TaskHandle_t  )&QR_Task_Handle);        /* 任务控制块指针 */

//      HDC hdc_mem_320;
//      hdc_mem_temp = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, GUI_XSIZE, GUI_YSIZE);
//      ClrDisplay(hdc_mem_temp, NULL, ARGB8888(100, 0, 0, 0));
//        
//      hdc_mem_320 = CreateMemoryDC((SURF_FORMAT)COLOR_FORMAT_ARGB8888, 320, 320);
//      ClrDisplay(hdc_mem_320, NULL,  ARGB8888(1, 0, 0, 0));
//        
//      BitBlt(hdc_mem_temp, GUI_XSIZE/2 - 320/2, GUI_YSIZE/2 - 320/2, 320, 320, hdc_mem_320, 0 , 0, SRCCOPY);
//      DeleteDC(hdc_mem_320);
      break;  
    }

    case WM_LBUTTONDOWN://点击屏幕，关闭窗口
    {

//      PostCloseMessage(hwnd);
  

      break;
    }
    case WM_TIMER://摄像头状态机
    {
      u16 id;
      
      id = LOWORD(wParam); //获得产生该消息的控件ID.     
      if(id == 1)
      {
        switch(state)
        {
          case 0:
          {
            OV5640_Init();  
            OV5640_RGB565Config();
            OV5640_USER_Config();
            OV5640_FOCUS_AD5820_Init();

            if(cam_mode.auto_focus ==1)
            {
              OV5640_FOCUS_AD5820_Constant_Focus();
//                focus_status = 1;
            }
            //使能DCMI采集数据
            DCMI_Cmd(ENABLE); 
            DCMI_CaptureCmd(ENABLE); 

            state = 1;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
          }
        }
      }
      else if(id == 3)
      {
        RECT RC;
        MSGBOX_OPTIONS ops;
        const WCHAR *btn[] = { L"确认",L"取消" };      //对话框内按钮的文字

        ops.Flag = MB_ICONERROR;
        ops.pButtonText = btn;
        ops.ButtonCount = 2;
        RC.w = 300;
        RC.h = 200;
        RC.x = (GUI_XSIZE - RC.w) >> 1;
        RC.y = (GUI_YSIZE - RC.h) >> 1;
        SelectDialogBox(hwnd, RC, L"没有检测到OV5640模块\n请重新检查连接。", L"错误", &ops);    // 显示错误提示框
        PostCloseMessage(hwnd);
        break; 
      }
      break;
    }
    
    case WM_ERASEBKGND:
    {
      RECT rc = {0, 0, GUI_XSIZE, GUI_YSIZE};
      HDC hdc =(HDC)wParam;
      
      SetBrushColor(hdc,MapRGB(hdc,220,220,220));
      FillRect(hdc, &rc);
      
      rc.x = GUI_XSIZE/2 - 320/2;
      rc.y = GUI_YSIZE/2 - 320/2 - 55;
      rc.w = 320;
      rc.h = 30;
      SetTextColor(hdc,MapRGB(hdc,50,50,50));
      DrawText(hdc, L"二维码识别", -1, &rc, DT_VCENTER|DT_CENTER);
      return TRUE;
    }
    
    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      SURFACE *pSurf;
      HDC hdc_mem;
      HDC hdc;
      RECT rc;
//      RECT rc_title = {100, 0, 600, 70};

      hdc = BeginPaint(hwnd,&ps);
      GetClientRect(hwnd,&rc);
      if(state==0)
      {
        SetTextColor(hdc,MapRGB(hdc,50,50,50));
        SetBrushColor(hdc,MapRGB(hdc,220,220,220));
        SetPenColor(hdc,MapRGB(hdc,250,0,0));
        
        DrawText(hdc,L"正在初始化摄像头\r\n\n请等待...",-1,&rc,DT_VCENTER|DT_CENTER|DT_BKGND);
      }  
      else if (state==1)
      {   

        pSurf =CreateSurface(SURF_RGB565,cam_mode.cam_out_width, cam_mode.cam_out_height, 0, (U16*)cam_buff00);     
        
        hdc_mem =CreateDC(pSurf,NULL);
        
        SetPenColor(hdc_mem, MapRGB(hdc,250,0,0));
        rc.x = GUI_XSIZE/2 - 320/2;
        rc.y = GUI_YSIZE/2 - 320/2;
        rc.w = 320;
        rc.h = 320;
        DrawRect(hdc_mem, &rc);

//        BitBlt(hdc_mem, 0, 0, GUI_XSIZE, GUI_YSIZE, hdc_mem_temp, 0 , 0, SRCCOPY);
        
//        BitBlt(hdc,  cam_mode.lcd_sx , cam_mode.lcd_sy, cam_mode.cam_out_width,  
//               cam_mode.cam_out_height, hdc_mem, rc.x, rc.y, SRCCOPY);
        BitBlt(hdc,  rc.x, rc.y, rc.w, rc.h, hdc_mem, rc.x, rc.y, SRCCOPY);

        // SetTextColor(hdc,MapRGB(hdc,10,10,10));
        // DrawText(hdc, L"二维码识别", -1, &rc_title, DT_CENTER|DT_VCENTER); 

        DeleteSurface(pSurf);
        DeleteDC(hdc_mem);
      }
      
      EndPaint(hwnd,&ps);
      
        
      OV5640_Capture_Control(ENABLE);//关闭摄像头采集图像
      DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE); //关闭DMA中断
      DCMI_Cmd(ENABLE);                               //DCMI失能
      DCMI_CaptureCmd(ENABLE); 
      DCMI_Stop();
      HAL_DCMI_Start_DMA((uint32_t)cam_buff00,
                        cam_mode.cam_out_height*cam_mode.cam_out_width/2);
      DCMI_Start();
      break;
    }
    
    case eMSG_QRScan_OK:
    {
      WCHAR *wbuf_type = (WCHAR *)lParam; 
      WCHAR *wbuf_data = (WCHAR *)wParam; 
      
      RECT RC;
      MSGBOX_OPTIONS ops;
      const WCHAR *btn[] ={L"确认"};      //对话框内按钮的文字

      ops.pButtonText =btn;
      ops.ButtonCount =1;
      RC.w = 400;
      RC.h = 320;
      RC.x = (GUI_XSIZE - RC.w) >> 1;
      RC.y = (GUI_YSIZE - RC.h) >> 1;
      
      ScanCompleteDialog(Cam_hwnd, RC, wbuf_data, wbuf_type, &ops);    // 显示识别的内容
      vTaskResume(QR_Task_Handle);    // 恢复识别任务
      
      break;
    }
    
    case WM_DESTROY:
    {
      state = 0;
//      DeleteDC(hdc_mem_temp);
      OV5640_Reset();//复位摄像头
      OV5640_Capture_Control(DISABLE);//关闭摄像头采集图像
      DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,DISABLE); //关闭DMA中断
      DCMI_Cmd(DISABLE); //DCMI失能
      DCMI_CaptureCmd(DISABLE); 
      if (QR_Task)
      {
        GUI_SemDelete(cam_sem);
      }
      QR_Task=0;
//      GUI_VMEM_Free(cam_buff1);
//      GUI_VMEM_Free(cam_buff0);
      //复位摄像头配置参数
      Camera_ReConfig();
      cur_index = 0;
      LCD_LayerCamInit((uint32_t)LCD_FRAME_BUFFER,800, 480);
      return PostQuitMessage(hwnd);	
    }    
    
    case WM_DRAWITEM:
    {
       DRAWITEM_HDR *ds;
       ds = (DRAWITEM_HDR*)lParam;
       switch(ds->ID)
       {
          case eID_QR_EXIT:
          {
            QR_ExitButton_OwnerDraw(ds);
            return TRUE;             
          }
       }

       break;
    }
    
    case WM_NOTIFY: //WM_NOTIFY消息:wParam低16位为发送该消息的控件ID,高16位为通知码;lParam指向了一个NMHDR结构体.
    {
      u16 code,id;
      code =HIWORD(wParam); //获得通知码类型.
      id   =LOWORD(wParam); //获得产生该消息的控件ID.

      if(id==eID_QR_EXIT && code==BN_CLICKED)//退出窗口
      {
        PostCloseMessage(hwnd);
      }
      break;  
    }
    
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return WM_NULL;
}


void	GUI_Camera_QRCode_DIALOG(void)
{	
	WNDCLASS	wcex;
	MSG msg;

   g_dma2d_en = TRUE;
	wcex.Tag = WNDCLASS_TAG;  
  
  
//  cam_buff0 = (uint16_t *)GUI_VMEM_Alloc(LCD_XSIZE*LCD_YSIZE*2);
//  cam_buff1 = (uint16_t *)GUI_VMEM_Alloc(LCD_XSIZE*LCD_YSIZE*2);

  
  
	wcex.Style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WinProc; //设置主窗口消息处理的回调函数.
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = NULL;//hInst;
	wcex.hIcon = NULL;//LoadIcon(hInstance, (LPCTSTR)IDI_WIN32_APP_TEST);
	wcex.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);

	//创建主窗口
	Cam_hwnd = CreateWindowEx(WS_EX_NOFOCUS,
                                    &wcex,
                                    L"GUI_Camera_QRCode_Dialog",
                                    WS_VISIBLE|WS_CLIPCHILDREN|WS_OVERLAPPED,
                                    0, 0, GUI_XSIZE, GUI_YSIZE,
									NULL, NULL, NULL, NULL);

	//显示主窗口
	ShowWindow(Cam_hwnd, SW_SHOW);

	//开始窗口消息循环(窗口关闭并销毁时,GetMessage将返回FALSE,退出本消息循环)。
	while (GetMessage(&msg, Cam_hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
  }


}
