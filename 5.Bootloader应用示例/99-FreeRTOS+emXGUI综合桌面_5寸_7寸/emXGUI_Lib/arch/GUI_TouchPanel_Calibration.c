
#include <emXGUI.h>
#include	"GUI_Drv.h"

#if 1/*(GUI_TOUCHSCREEN_CALIBRATE > 0)*/

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

static  int x_abs(int a)
{
	if(a<0)
	{
		a =-a;
	}
	return a;
}

static	void	clr_cursor(HDC hdc,int x,int y,COLORREF color)
{
	RECT rc;

	rc.x = x-20;
	rc.y = y-20;
	rc.w = 20*2;
	rc.h = 20*2;
	
	FillRect(hdc,&rc);
	
}

static	void	draw_cursor(HDC hdc,int x,int y,COLORREF color)
{	
	SetPenColor(hdc,color);
	
	VLine(hdc,x,y,y-20+1);	//上
	VLine(hdc,x,y,y+20-1);	//下
	
	HLine(hdc,x,y,x+20-1);	//右
	HLine(hdc,x,y,x-20+1);	//左
	
	DrawCircle(hdc,x,y,6);	//中心
		
}


static BOOL	_TouchPanel_Calibrate(HDC hdc,RECT *lpRect)
{
		
		COLORREF bk_color;
		int times,failed_times,step,pt_count;
		POINT pt,cal_pt[8];
		RECT rt;
		////
		
		if(!TouchPanel_CalibrationStart())
		{
			goto exit;
		}	
				
		pt_count=TouchPanel_GetCalibrationPoints(cal_pt);
		
		CopyRect(&rt,lpRect);
		
		failed_times =0;
		bk_color     =MapRGB(hdc,200,200,200);
				
		SetTextColor(hdc,MapRGB(hdc,0,0,200));

cal_st:	
	
		times	=0;
		step	=0;
		FillRect(hdc,&rt);
		while(step < pt_count)
		{
			DrawText(hdc,L"TouchScreen Calibrate",-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
			
			if(step >= 1)
			{
				clr_cursor(hdc,cal_pt[step-1].x,cal_pt[step-1].y,bk_color);
			}
			
			draw_cursor(hdc,cal_pt[step].x,cal_pt[step].y,MapRGB(hdc,255,0,0));
				
			GUI_msleep(50);
			while(TouchPanel_IsPenDown())
			{
				GUI_msleep(50);
			}
			////
			
			while(1)
			{
				GUI_msleep(100);
				if(TouchPanel_IsPenDown())
				{	
					times++;
				}
				else
				{
					times=0;
				}
				
				if(times>5)
				{
					draw_cursor(hdc,cal_pt[step].x,cal_pt[step].y,MapRGB(hdc,255,0,0));
					TouchDev_GetSample(&pt);
					TouchPanel_SetCalibrationSample(&pt,step);
					break;
				}
								
			}
			////
			
			step++;
		}
		////////
		TouchPanel_CalibrationFinish();
		clr_cursor(hdc,cal_pt[pt_count-1].x,cal_pt[pt_count-1].y,bk_color);
		draw_cursor(hdc,GUI_XSIZE>>1,GUI_YSIZE>>1,MapRGB(hdc,255,0,0));
		GUI_msleep(50);
		while(TouchPanel_IsPenDown())
		{
			GUI_msleep(50);
		}	
		////	
		while(1)
		{
			
			if(TouchPanel_IsPenDown())
			{	
				times++;
			}
			else
			{
				times=0;
			}
				
			if(times > 2)
			{
				TouchDev_GetSample(&pt);
				TouchPanel_TranslatePoint(GUI_XSIZE,GUI_YSIZE,&pt);
				
			//	if(x_abs(pt.x-(GUI_XSIZE>>1))>40)	goto	cal_failed;
			//	if(x_abs(pt.y-(GUI_YSIZE>>1))>40)	goto	cal_failed;
				goto	cal_ok;
				
			}
			
			GUI_msleep(100);
		
		}
		////
		
cal_failed:

	if(failed_times++ < 3)	goto	cal_st;	//小于3次
	FillRect(hdc,&rt);
	TouchPanel_CalibrationFailed();
	DrawText(hdc,L"Touch Screen Calibrate",-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	OffsetRect(&rt,0,32);
	DrawText(hdc,L" ** Failed **",-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	GUI_msleep(2000);
	goto exit;		

	////
	
cal_ok:

	TouchPanel_CalibrationSucceed();
				
	FillRect(hdc,&rt);
	DrawText(hdc,L"Touch Screen Calibrate",-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	OffsetRect(&rt,0,32);
	DrawText(hdc,L"** Success **",-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	
	while(TouchPanel_IsPenDown())
	{
		GUI_msleep(50);
	}
	
	OffsetRect(&rt,0,32);	
	DrawText(hdc,L"Tap Screen Exit",-1,&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE);	
	////
				
	for(times=0;times<(5000/50);times++)
	{
		GUI_msleep(50);
		if(TouchPanel_IsPenDown())	break;
	}

	//goto exit;
	////////
	
exit:
	FillRect(hdc,&rt);
	GUI_Printf("TouchScreen Calibrate Exit!\n");
	return 1;
	
}

/*============================================================================*/
BOOL _TouchPanel_Calibrate(HDC hdc, RECT *lpRect);

BOOL TouchScreenCalibrate(void)
{
	HDC hdc;
	RECT rc;

	hdc = GetDC(NULL);
	if (hdc == NULL )
	{
		return FALSE;
	}

	ShowWindow(GetDesktopWindow(), SW_HIDE); //隐藏桌面
	SetRectArea(&rc, 0, 0, GUI_XSIZE, GUI_YSIZE);

	_TouchPanel_Calibrate(hdc, &rc);

	ReleaseDC(NULL, hdc);
	ShowWindow(GetDesktopWindow(), SW_SHOW); //恢复桌面
	return TRUE;
}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/

#endif /*(GUI_TOUCHSCREEN_CALIBRATE)*/

