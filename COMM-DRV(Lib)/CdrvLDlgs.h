//***********************************************************************
//			----- CdrvLDlgs ------
//Description-
//	Definitions for COMM-DRV/Lib dialog boxes.
//***********************************************************************
//#ifdef	DLLFunctionDefintion
//#define	DllFunction	__declspec(dllexport)
//#else
//#define	DllFunction	__declspec(dllimport)
//#endif




//*Function Prototypes
extern "C"
{
int WINAPI CdrvLInitDialog(
	int &Subport,UINT &Baud,int &Parity,
	int &Length,int &Stopbit,int &Protocol);
int WINAPI CdrvLInitDialog2(
	int &Subport,UINT &Baud,int &Parity,
	int &Length,int &Stopbit,int &Protocol, int &Flag);
}

