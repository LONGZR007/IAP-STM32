//***********************************************************************
//*					----- Example2.cpp -----
//*
//*Author-
//*	Egberto Willies 10-31-2001
//*
//*Description-
//*	This example is a simple terminal program. Anything typed at
//*	the keyboard is sent over the serial port. Data that comes into
//*	the serial port is displayed in the terminal window.
//*
//*	Callbacks from COMM-DRV/Lib are used to effect the retrieval of
//*	the data.
//***********************************************************************

#include <afxwin.h>
#include <afxdlgs.h>
#include <comm.h>
#include <CdrvLDlgs.h>
#include "Example2.h"
#include <resource.h>


//***********************************************************************
//*=====Global variables
//***********************************************************************
CExample2App	App;
int				InputFlag;


//=====Default port characteristics
int	ComPort = 0;			//COM port number
UINT	Baud=38400;
int	Parity=PAR_NONE;
int	Length=LENGTH_8;
int	Stopbit=STOPBIT_1;
int	Protocol=PROT_NONNON;
//***********************************************************************

//***********************************************************************
//*=====InitInstance
//***********************************************************************
BOOL CExample2App::InitInstance()
{
m_pMainWnd = new CExample2Window;

m_pMainWnd->ShowWindow(m_nCmdShow);
m_pMainWnd->UpdateWindow();
return(TRUE);
}
//***********************************************************************


//***********************************************************************
//*=====CExample2Window
//*Description-
//*	Class constructor.
//*	1. Creates client window.
//***********************************************************************
CExample2Window::CExample2Window()
{

// Load the arrow cursor and the I-beam cursor and save their handles.
m_hCursorArrow = AfxGetApp ()->LoadStandardCursor (IDC_ARROW);
m_hCursorIBeam = AfxGetApp ()->LoadStandardCursor (IDC_IBEAM);

// Must specifically register class since derived from CWnd as opposed to
// CFrameWnd which inherently registers class.
CString strWndClass = AfxRegisterWndClass
			(
			0,NULL,
			(HBRUSH) (COLOR_3DFACE + 1),
			AfxGetApp ()->LoadIcon ("IDI_ICON1")
			);

// Create a window.
CreateEx (0, strWndClass,
	  _T ("The WCSC COMM-DRV/Lib"),
	  WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
	  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	  NULL, NULL);
}
//***********************************************************************


//***********************************************************************
//*=====Message Map
//***********************************************************************
BEGIN_MESSAGE_MAP(CExample2Window,CWnd)
ON_WM_CHAR ()
ON_WM_CLOSE ()
ON_WM_CREATE ()
ON_WM_KILLFOCUS()
ON_WM_PAINT()
ON_WM_SETFOCUS()
ON_MESSAGE(WM_RCVDATA,OnSerialDataIn)
ON_COMMAND(ID_REOPENPORT,OnReOpenPort)
ON_COMMAND(ID_SENDFILE_ASCII,OnSendfileASCII)
ON_COMMAND(ID_SENDFILE_XMODEM,OnSendfileXModem)
ON_COMMAND(ID_SENDFILE_XMODEMCHECKSUM,OnSendfileXModemChecksum)
ON_COMMAND(ID_SENDFILE_YMODEM,OnSendfileYModem)
ON_COMMAND(ID_SENDFILE_ZMODEM,OnSendfileZModem)
ON_COMMAND(ID_GETFILE_ASCII,OnGetfileASCII)
ON_COMMAND(ID_GETFILE_XMODEM,OnGetfileXModem)
ON_COMMAND(ID_GETFILE_XMODEMCHECKSUM,OnGetfileXModemChecksum)
ON_COMMAND(ID_GETFILE_YMODEM,OnGetfileYModem)
ON_COMMAND(ID_GETFILE_ZMODEM,OnGetfileZModem)
END_MESSAGE_MAP()
//***********************************************************************

//***********************************************************************
//*===== OnCreate
//*Description-
//*	Called when window is about to be created.
//***********************************************************************
int CExample2Window::OpenSerialPort()
{
int	Status;

if (CdrvLInitDialog(ComPort,Baud,Parity,Length,Stopbit,Protocol) != IDOK)
	{
	return(-1);
	}

if ((m_Port=OpenComPort(ComPort+1,2048,2048)) < 0)
	{
	CString		msg;

	msg.Format("Error # %d on OpenComPort()",-m_Port);
	MessageBox(msg,"OpenPort Error",MB_ICONERROR);
	return(-1);
	}

if ((Status=SetPortCharacteristics(m_Port,CdrvIntToBaudIndex(m_Port,Baud),Parity,Length,
	Stopbit,Protocol)) != RS232ERR_NONE)
		{
		CString	Str;
		Str.Format("SetPortCharacteristics() Error # %d",Status);
		MessageBox(Str);
		CloseComPort(m_Port);
		return(-1);
		}

CdrvSetContext(m_Port,0,(unsigned long)m_hWnd);
	

if ((Status=SetCallbackFunction(m_Port,INTFUNC_VIBFICNT,1,Callback)) != RS232ERR_NONE)
	{
	CString	Str;
	Str.Format("SetCallbackFunction() Error # %d",Status);
	MessageBox(Str);
	CloseComPort(m_Port);
	return(-1);
	}

FlushReceiveBuffer(m_Port);
InputFlag = 0;

return(0);
}
//***********************************************************************


//***********************************************************************
//*===== OnCreate
//*Description-
//*	Called when window is about to be created.
//***********************************************************************
int CExample2Window::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
//Call the MFC default creation code
if (CWnd::OnCreate (lpCreateStruct) == -1)
	return -1;

CMenu Menu;
Menu.LoadMenu(IDR_MENU);
SetMenu(&Menu);

//Get charater metrics
CClientDC	dc(this);
TEXTMETRIC	tm;

dc.SelectStockObject(SYSTEM_FIXED_FONT);
dc.GetTextMetrics (&tm);
m_CharAvgWidth = tm.tmAveCharWidth;
m_CharMaxWidth = tm.tmMaxCharWidth;
m_CharHeight = tm.tmHeight;
m_LineSpacing = tm.tmHeight + tm.tmExternalLeading;

//Initialize screen buffer
m_Scr = new char[1][SCR_ROW][SCR_COL];
m_ScrCol=0;
m_ScrRow=0;
m_ScrScrolled;
::RtlFillMemory(m_Scr,SCR_COL*SCR_ROW,' ');

//Set initial caret position
m_ptCaretPos.x = 0;
m_ptCaretPos.y = 0;

//Size the window to a 80 char by 25 line screen + offset of menu bar
CRect rect (0, 0,SCR_COL*m_CharMaxWidth,SCR_ROW*m_CharHeight+::GetSystemMetrics(SM_CYMENU));
CalcWindowRect (&rect);
SetWindowPos(NULL, 0, 0, rect.Width(),rect.Height(),SWP_NOZORDER |
	     SWP_NOMOVE | SWP_NOREDRAW);

m_Port = -1;
if (OpenSerialPort() != 0)
	return(-1);

return 0;
}
//***********************************************************************

//***********************************************************************
//*=====PostNcDestroy
//*Description-
//*	Destroys this Window object. This is necessary as class
//*	was derived from CWnd instead of CFrameWnd. This is a virtual
//*	function that overrides the default PostNcDestroy.
//***********************************************************************
void CExample2Window::PostNcDestroy ()
{
delete this;
}
//***********************************************************************

//***********************************************************************
//*=====OnPaint
//***********************************************************************
void CExample2Window::OnPaint()
{
CPaintDC	dc(this);
CRect		rect;

GetClientRect(&rect);

dc.SelectStockObject(SYSTEM_FIXED_FONT);
dc.SelectStockObject (BLACK_PEN);
dc.SetBkMode (OPAQUE);

for (int i=0;i<SCR_ROW;i++)
	{
	dc.TextOut(0,i*m_CharHeight,(*m_Scr)[i],SCR_COL);
	}
}
//***********************************************************************

//***********************************************************************
//*=====OnSetFocus
//***********************************************************************
void CExample2Window::OnSetFocus (CWnd* pWnd)
{
CreateSolidCaret(max (2,::GetSystemMetrics(SM_CXBORDER)),m_CharHeight);
SetCaretPos (m_ptCaretPos);
ShowCaret ();
}
//***********************************************************************

//***********************************************************************
//*=====OnKillFocus
//***********************************************************************
void CExample2Window::OnKillFocus (CWnd* pWnd)
{
//Hide caret when Focus lost
HideCaret ();
m_ptCaretPos = GetCaretPos();
::DestroyCaret ();
}
//***********************************************************************


//***********************************************************************
//*=====OnChar
//***********************************************************************
void CExample2Window::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
PutByte(m_Port,nChar);
}
//***********************************************************************

//***********************************************************************
//*=====OnClose
//***********************************************************************
void CExample2Window::OnClose()
{
CloseComPort(m_Port);
CWnd::OnClose();
}
//***********************************************************************

//***********************************************************************
//*===== OnSerialDataIn
//***********************************************************************
LRESULT CExample2Window::OnSerialDataIn(WPARAM,LPARAM)
{
char		Buf[100];
int		ByteCount;
int		ScrolledLines=0;

CRect rect;
GetClientRect (&rect);

CClientDC dc (this);
dc.SelectStockObject(SYSTEM_FIXED_FONT);
dc.SelectStockObject (BLACK_PEN);
dc.SetBkMode (OPAQUE);

HideCaret();

while(1)
	{
	//Read data from serial port
	if ((ByteCount=GetPacket(m_Port,sizeof(Buf),Buf))<=0)
		{
		m_ptCaretPos.x = m_ScrCol*m_CharMaxWidth;
		m_ptCaretPos.y = m_ScrRow*m_CharHeight;
		SetCaretPos (m_ptCaretPos);
		ShowCaret();

		return 0;
		}

	//Output characters to client area. If characters are not printable
	//ASCII then replace with ".".
	for (int i=0;i<ByteCount;i++)
		{
		switch(Buf[i])
			{
			//Carriage return
			case	0x0d:
			m_ScrCol = 0;
			break;

			//Line feed
			case	0x0a:
			m_ScrRow++;
			if (m_ScrRow >= SCR_ROW)
				{
				m_ScrRow=SCR_ROW-1;
				::RtlMoveMemory(&((*m_Scr)[0][0]),&((*m_Scr)[1][0]),(SCR_ROW-1)*SCR_COL);
				::RtlFillMemory(&((*m_Scr)[m_ScrRow][0]),SCR_COL,' ');
				ScrollWindow(0,-m_CharHeight,NULL,NULL);
				ValidateRect(&rect);
				dc.TextOut(0,m_ScrRow*m_CharHeight,&((*m_Scr)[m_ScrRow][0]),SCR_COL);
				}
			break;

			default:
			if ((Buf[i] < ' ') || (Buf[i] > '~'))
				Buf[i] = '.';
			(*m_Scr)[m_ScrRow][m_ScrCol] = Buf[i];
			dc.TextOut(m_ScrCol*m_CharMaxWidth,m_ScrRow*m_CharHeight,(char *)&Buf[i],1);
			if (++m_ScrCol >= SCR_COL)
				{
				m_ScrCol = 0;
				m_ScrRow++;
				if (m_ScrRow >= SCR_ROW)
					{
					m_ScrRow=SCR_ROW-1;
					::RtlMoveMemory(&((*m_Scr)[0][0]),&((*m_Scr)[1][0]),(SCR_ROW-1)*SCR_COL);
					::RtlFillMemory(&((*m_Scr)[m_ScrRow][0]),SCR_COL,' ');
					ScrollWindow(0,-m_CharHeight,NULL,NULL);
					ValidateRect(&rect);
					dc.TextOut(0,m_ScrRow*m_CharHeight,&((*m_Scr)[m_ScrRow][0]),SCR_COL);
					}
				}
			}
		}
	}
}
//***********************************************************************

//***********************************************************************
//*=====Callback
//***********************************************************************
int Callback(int port,int event,unsigned long)
{
//Disable terminal reception if InputFlag
if (InputFlag == 1)
	return(0);

HWND	hWnd=(HWND)CdrvGetContext(port,0);

if (hWnd != NULL)
	PostMessage(hWnd,WM_RCVDATA,0,0);
return(0);
}
//***********************************************************************

//***********************************************************************
//*=====OnReOpenPort
//***********************************************************************
void CExample2Window::OnReOpenPort()
{
if (m_Port != -1)
	{
	CloseComPort(m_Port);
	m_Port = -1;
	}

if (OpenSerialPort() != 0)
	m_Port = -1;
}
//***********************************************************************


//***********************************************************************
//*=====OnSendfile
//***********************************************************************
int CExample2Window::OnSendfile()
{
CFileDialog	FileDlg(TRUE, NULL, NULL, OFN_EXPLORER | OFN_ENABLESIZING , NULL, NULL,0);

if( FileDlg.DoModal() == IDOK )
	{
		Filename = FileDlg.GetPathName();
	return(0);
	}

return(-1);
}
//***********************************************************************

//***********************************************************************
//*=====OnGetfile
//***********************************************************************
int CExample2Window::OnGetfile()
{
CFileDialog	FileDlg(TRUE, NULL, NULL, OFN_NOVALIDATE | OFN_EXPLORER | OFN_ENABLESIZING , NULL, NULL,0);

if( FileDlg.DoModal() == IDOK )
	{
		Filename = FileDlg.GetPathName();
	return(0);
	}

return(-1);
}
//***********************************************************************

//***********************************************************************
//*=====OnSendfileASCII
//***********************************************************************
void CExample2Window::OnSendfileASCII()
{
int		Status;

if (OnSendfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,0,8,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

//***********************************************************************
//*=====OnSendfileXModemChecksum
//***********************************************************************
void CExample2Window::OnSendfileXModemChecksum()
{
int		Status;

if (OnSendfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,0,7,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

//***********************************************************************
//*=====OnSendfileXModem
//***********************************************************************
void CExample2Window::OnSendfileXModem()
{
int		Status;

if (OnSendfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,0,0,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

//***********************************************************************
//*=====OnSendfileYModem
//***********************************************************************
void CExample2Window::OnSendfileYModem()
{
int		Status;

if (OnSendfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,0,3,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

//***********************************************************************
//*=====OnSendfileZModem
//***********************************************************************
void CExample2Window::OnSendfileZModem()
{
int		Status;

if (OnSendfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,0,6,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

//***********************************************************************
//*=====OnGetfileASCII
//***********************************************************************
void CExample2Window::OnGetfileASCII()
{
int		Status;

if (OnGetfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,1,8,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

//***********************************************************************
//*=====OnGetfileXModemChecksum
//***********************************************************************
void CExample2Window::OnGetfileXModemChecksum()
{
int		Status;

if (OnGetfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,1,7,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

//***********************************************************************
//*=====OnGetfileXModem
//***********************************************************************
void CExample2Window::OnGetfileXModem()
{
int		Status;

if (OnGetfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,1,0,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

//***********************************************************************
//*=====OnGetfileYModem
//***********************************************************************
void CExample2Window::OnGetfileYModem()
{
int		Status;

if (OnGetfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,1,3,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

//***********************************************************************
//*=====OnGetfileZModem
//***********************************************************************
void CExample2Window::OnGetfileZModem()
{
int		Status;

if (OnGetfile() != 0)
	return;
FileTransferDialog(m_Port,1);
InputFlag = 1;
if ((Status=cdrvxfer_files(m_Port,1,6,(char *)(LPCSTR)Filename,NULL)) != CDRVXFER_ERR_OK)
	MessageBox("Error transferring file");
InputFlag = 0;
}
//***********************************************************************

