//***********************************************************************
//*					----- Example2.h -----
//*
//*Author-
//*	Egberto Willies 10-31-2001
//*
//***********************************************************************

#define	WM_RCVDATA	WM_USER+1

int Callback(int port,int event,unsigned long fncval);

#define	SCR_COL	132
#define	SCR_ROW	50


//Application object
class CExample2App : public CWinApp
{
public:
virtual BOOL InitInstance();
};

//Main Window object
class CExample2Window : public CWnd
{
protected:
HCURSOR m_hCursorArrow;		//Handle of arrow cursor
HCURSOR m_hCursorIBeam;		//Handle of I-beam cursor

int m_CharAvgWidth;		//Average character width
int m_CharMaxWidth;		//Max char width
int m_CharHeight;		//Character height
int m_LineSpacing;		//Vertical line spacing in message box

CPoint m_ptCaretPos;		//Current caret position

char (*m_Scr)[SCR_ROW][SCR_COL];	//Pointer to screen buffer
int m_ScrRow;				//Current character X position
int m_ScrCol;				//Current Character Y Position
int m_ScrScrolled;			//# of lines scrolledup

int	m_Port;			//Port
CString	Filename;

public:
CExample2Window();
void Display(char *Msg,DWORD val,char *Msg1="",DWORD val1=0,char *Msg2="",DWORD val2=0,char *Msg3="",DWORD val3=0);


protected:
virtual void PostNcDestroy ();
afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
afx_msg void OnClose();
afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
afx_msg void OnKillFocus(CWnd* pWnd);
afx_msg void OnPaint();
afx_msg void OnSetFocus(CWnd* pWnd);
afx_msg LRESULT OnSerialDataIn(WPARAM,LPARAM);
afx_msg void OnReOpenPort();
afx_msg int OpenSerialPort();
afx_msg int OnSendfile();
afx_msg void OnSendfileASCII();
afx_msg void OnSendfileXModemChecksum();
afx_msg void OnSendfileXModem();
afx_msg void OnSendfileYModem();
afx_msg void OnSendfileZModem();
afx_msg int OnGetfile();
afx_msg void OnGetfileASCII();
afx_msg void OnGetfileXModemChecksum();
afx_msg void OnGetfileXModem();
afx_msg void OnGetfileYModem();
afx_msg void OnGetfileZModem();
DECLARE_MESSAGE_MAP()

};


