/************************************************************************/
/*                              ----- comm.h -----                      */
/*                                                                      */
/*              Willies' Computer Software Company                      */
/*              Copyright (C) 1989-1992 Egberto Willies, 1989           */
/*              All rights reserved.                                    */
/*                                                                      */
/*Author                                                                */
/*      Egberto Willies 06-FEB-1989                                     */
/*                                                                      */
/*Description-                                                          */
/*      Include for communicatons package.                              */
/************************************************************************/
/* $Header: o:/projects/commdrv/work/inc/rcs/comm.h 1.9 1996/12/14 18:31:01 ewillies Exp ewillies $ */
/************************************************************************/
#ifndef FILE_COMM_H
#define FILE_COMM_H             1
/************************************************************************/

/************************************************************************/
/*===== Compatibility Equates                                           */
/************************************************************************/
#ifdef	_WINDOWS
#ifndef	MSWIN
#define	MSWIN
#endif
#ifndef	MSWINDLL
#define	MSWINDLL
#endif
#endif

#ifdef WIN32
#ifndef MSWIN32
#define MSWIN32
#endif
#endif

#ifndef MSNTDRV
#ifdef MSWIN
#ifndef _INC_WINDOWS
#include <windows.h>
#endif
#endif
#else
//#define HANDLE  unsigned int
#define DWORD   unsigned long
#define WINAPI
#define HWND    unsigned int
#define WPARAM  unsigned int
#define LPARAM  unsigned long
#endif

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#ifdef  MSWIN32
#ifndef CDECL
#define CDECL   cdecl
#endif

#ifndef FAR
#define FAR
#endif

#ifndef FARC
#define FARC    CDECL
#endif

#ifndef FARPAS
#define FARPAS  WINAPI
#endif

#ifndef CDRVUINT
#define CDRVUINT        unsigned int
#endif

#ifndef CDRVIFNC
#define CDRVIFNC        unsigned long
#endif

#define FP_SEG(s)       ((CDRVUINT)s)
#define FP_OFF(s)       ((CDRVUINT)s)

#else

#ifndef CDECL
#define CDECL   cdecl
#endif

#ifndef FAR
#define FAR     far
#endif

#ifndef FARC
#define FARC    FAR CDECL
#endif

#ifndef FARPAS
#define FARPAS  FAR PASCAL
#endif

#ifndef CDRVUINT
#define CDRVUINT        unsigned short
#endif

#ifndef CDRVIFNC
#define CDRVIFNC        short
#endif

#endif

#ifndef DOSDEF_H
#include <dosdef.h>
#endif

/************************************************************************/
/*======Version information                                             */
/************************************************************************/
#define COMMDRV_COMPANY         "WCSC(www.wcscnet.com)"

#define COMMDRV_VERSION         "COMM-DRV "
#define COMMDRV_VERSION1        "v18.2  "
#define COMMDRV_VERSION2        "Copr (C) 1989-2002 WCSC"
#define COMMDRV_VERSION3        18,1,0,0
#define COMMDRV_VERSION_MAJOR   18
#define COMMDRV_VERSION_MINOR   1

//COMM-DRV/Lib versioning
#define	CDRVLIB_PRODUCT_STR	"COMM-DRV/Lib"
#define	CDRVLIB_VERSION_STRNUM	"21,0,0,1"
#define	CDRVLIB_VERSION_MAJOR	21
#define CDRVLIB_VERSION_MINOR	0
#define	CDRVLIB_VERSION_BUILD	0
#define CDRVLIB_VERSION_STR	"v21.0 "
#define	CDRVLIB_TRADEMARK_STR	CDRVLIB_PRODUCT_STR "\256 is a trademark of " " " COMMDRV_COMPANY
#define	CDRVLIB_COPR_YEAR_STR	"1989-2008"
#define	CDRVLIB_COPR_STR		"Copr \251 " CDRVLIB_COPR_YEAR_STR " " COMMDRV_COMPANY
#define	CDRVLIB_COPR_STR_DOS	"Copr (C) " CDRVLIB_COPR_YEAR_STR " " COMMDRV_COMPANY

//COMM-DRV/NT versioning
#define	CDRVNT_PRODUCT_STR	"COMM-DRV/NT"
#define CDRVNT_VERSION_STR	"v18.2 "
#define	CDRVNT_TRADEMARK_STR	CDRVNT_PRODUCT_STR "\256 is a trademark of " " " COMMDRV_COMPANY
#define	CDRVNT_COPR_YEAR_STR	"1989-2002"
#define	CDRVNT_COPR_STR		"Copr \251 " CDRVNT_COPR_YEAR_STR " " COMMDRV_COMPANY
#define	CDRVNT_COPR_STR_DOS	"Copr (C) " CDRVNT_COPR_YEAR_STR " " COMMDRV_COMPANY
#define	CDRVNT_VERSION_MAJOR	18
#define CDRVNT_VERSION_MINOR	2
#define	CDRVNT_VERSION_BUILD	1

/************************************************************************/
/*======General Symbols                                                 */
/************************************************************************/
#define CDRVPATHLEN             64      /* COMMDRV path array length */

#define CDRV_ON         1
#define CDRV_OFF        0
#define CDRV_TRUE       1
#define CDRV_FALSE      0

/************************************************************************/
/*======These values may be changed to reduce the amount of space used  */
/*      for port/irq/other resources. If any of these items change,     */
/*      then the corresponding item in COMM.INC must be changed. The    */
/*      libraries must then be reassembled and recompiled.              */
/************************************************************************/
#define MAX_IRQ         16      /*Maximum IRQ #*/
#define MAX_IRQS        17      /*Max # of IRQs active concurrently*/
#define MAX_PORT        31      /*Maximum port #*/
#define MAX_PORTS       32      /*Max # of ports active concurrently*/
#define MAX_DRIVERS     10      /*Max # of sub-device drivers*/
#define MAX_TIMERS      32      /*Max # of Timer resources*/
#define MAX_ASYNCS      32      /*Max # of async callback resources*/

/************************************************************************/
/*=====Argument limits                                                  */
/************************************************************************/
#define MAX_BAUD        21      /*Max baud*/
#define MAX_BREAK       1       /*Maximum break*/
#define MAX_LENGTH      3       /*Max char length*/
#define MAX_PARITY      4       /*Max parity*/
#define MAX_PROTOCOL    15      /*Maximum protocol*/
#define MAX_STOPBIT     1       /*Maximum stop bit*/

/************************************************************************/
/*=====Defaults                                                         */
/************************************************************************/
#define DEF_AUXADDR     0               /*Default auxiliary address*/
#define DEF_BASE        0x3f8           /*Default port*/
#define DEF_BAUD        BAUD9600        /*Default baud rate (9600 baud)*/
#define DEF_BLOCK       0x0             /*Default xmit/recv block time*/
#define DEF_BREAK       BREAK_OFF       /*Default break(OFF)*/
#define DEF_CARD        CARD_NORMAL     /*Default card type */
#define DEF_CSEG        0               /*Default card segment*/
#define DEF_INLEN       1024            /*Input buffer length*/
#define DEF_IRQ         4               /*Default IRQ*/
#define DEF_ITIMEOUT    0               /*Default input timeout*/
#define DEF_LENGTH      LENGTH_8        /*Default word length(8 bits)*/
#define DEF_OTIMEOUT    0               /*Default output timeout*/
#define DEF_OUTLEN      256             /*Output buffer length*/
#define DEF_PARITY      PAR_NONE        /*Default parity setting (NONE)*/
#define DEF_PORT        0               /*Default port number*/
#define DEF_PORTTYPE    0               /*Default port type(Normal)*/
#define DEF_PROTOCOL    PROT_NONNON     /*Default protocol*/
#define DEF_STOPBIT     STOPBIT_1       /*Default stopbit (1 stop bit)*/
#define DEF_THHIGH      1               /*Default High Threshold*/
#define DEF_THLOW       0               /*Default low threshold*/

/************************************************************************/
/*=====Flow control characters                                          */
/************************************************************************/
#define XOFF            0x13    /*ASCII code for XOFF */
#define XON             0x11    /*ASCII code for XON  */

/************************************************************************/
/*=====INT14 Bits and flags                                             */
/************************************************************************/
#define SER_INT14_ERROR_BIT     0x80    /* Error bit */
#define SER_INT14_ACTIVE        0x1     /* Port can be initted via INT14 */
#define SER_INT14_FOSSIL        0x2     /* Fossil driver flag */
#define SER_INT14_INTERRUPT     0x14    /* Interrupt */

/************************************************************************/
/*=====8250 Asynchronous adapter offsets and bits                       */
/************************************************************************/
#define BAUD_LOW        0       /*Offset to low byte of baud divisor*/
#define DATABUF         0       /*Offset to rcv/xmt buffer*/

#define BAUD_HIGH       1       /*Offset to high byte of baud divisor*/
#define IER             1       /*Interrupt Enable Register*/
#define IER_RDA         0x001   /*Receive Data Available int bit*/
#define IER_THRE        0x002   /*Transmitter Hold Reg. Empty int bit*/
#define IER_RLS         0x004   /*Receive Line Status int bit*/
#define IER_MS          0x008   /*Modem Status int bit*/

#define IIR             2       /*Interrupt Identification Register*/
#define IIR_RLS         0x6     /**equal* if Receiver Line Stat int*/
#define IIR_RDA         0x4     /**equal* to if character ready*/
#define IIR_THRE        0x2     /**equal* to if TX buffer empty*/
#define IIR_PEND        0x1     /* zero if any interrupt pending*/
#define IIR_MS          0x0     /**equal* to if Modem Status int*/
#define IIR_FIFOL       0x40    /*FIFO ACTIVE*/
#define IIR_FIFOH       0x80    /*FIFO ACTIVE*/

#define FCR             2       /*FIFO Control Register*/
#define FCR_ENAB        0x01    /*FIFO Enable*/
#define FCR_RRESET      0x02    /*Receiver FIFO Reset*/
#define FCR_XRESET      0x04    /*Xmitter FIFO Reset*/
#define FCR_DMASEL      0x08    /*DMA Select*/
#define FCR_TTRGLSB     0x10    /*Xmitter Trigger LSB*/
#define FCR_TTRGMSB     0x20    /*Xmitter Trigger MSB*/
#define FCR_RTRGLSB     0x40    /*Receiver Trigger LSB*/
#define FCR_RTRGMSB     0x80    /*Receiver Trigger MSB*/

#define EFR		2	/*Enhanced Feature Register*/
#define EFR_RCVX1ONOFF	0x2	/*Receiver compares Xon1/Xoff1*/
#define EFR_XMTX2ONOFF	0x4	/*Xmitter compares Xon2/Xoff2*/
#define EFR_ENABLE	0x10	/*Enable certain bits in IER,ISR,FCR,MCR*/
#define EFR_SPECCHAR	0x20	/*Special character select*/
#define EFR_AUTORTS	0x40	/*Enable Auto RTS*/
#define EFR_AUTOCTS	0x80	/*Enable Auto CTS*/

#define LCR             3       /*Line Control Register*/
#define LCR_WLS0        0       /*Word Length Select Bit 0*/
#define LCR_WLS1        0x1     /*Word Length Select Bit 1*/
#define LCR_STOPBITS1   0x4     /*Number of stop bits*/
#define LCR_PARITYEN    0x8     /*Enab Parity(see SPARITY & EPARITY)*/
#define LCR_EPARITY     0x10    /*Even Parity Bit*/
#define LCR_SPARITY     0x20    /*Stick Parity*/
#define LCR_BREAK       0x40    /*set if break desired*/
#define LCR_DLAB        0x80    /*Divisor Latch Access Bit*/

#define MCR             4       /*Modem Control Register*/
#define MCR_DTR         0x1     /*Data Terminal Ready*/
#define MCR_RTS         0x2     /*Request To Send*/
#define MCR_OUT1        0x4     /*Output 1 (unused)*/
#define MCR_OUT2        0x8     /*Output 2 (External Int Enable)*/
#define MCR_LOOP        0x10    /*Loopback enable*/

#define	XON1R		4	/*XON1 reg when LCR=0xBF*/

#define LSR             5       /*Line Status Register*/
#define LSR_DATA        0x1     /*Data Ready bit*/
#define LSR_OVERRUN     0x2     /*Overrun error bit*/
#define LSR_PARITY      0x4     /*Parity error bit*/
#define LSR_FRAMING     0x8     /*Framing error bit*/
#define LSR_BREAK       0x10    /*Break Detect*/
#define LSR_THRE        0x20    /*Transmit Holding Register Empty*/
#define LSR_TSRE        0x40    /*Transmit Shift Register Empty*/
#define LSR_FERR	0x80	/*Error in FIFO */

#define	XON2R		5	/*XON2 reg when LCR=0xBF*/

#define MSR             6       /*Modem Status Register*/
#define MSR_DEL_CTS     0x1     /*Delta Clear To Send*/
#define MSR_DEL_DSR     0x2     /*Delta Data Set Ready*/
#define MSR_EDGE_RI     0x4     /*Trailing Edge of Ring Indicator*/
#define MSR_DEL_SIGD    0x8     /*Delta Receive Line Signal Detect*/
#define MSR_CTS         0x10    /*Clear To Send*/
#define MSR_DSR         0x20    /*Data Set Ready*/
#define MSR_RI          0x40    /*Ring Indicator - during entire ring*/
#define MSR_DCD         0x80    /*Data Carrier Detect - on line*/

#define	XOFF1R		6	/*XOFF1 reg when LCR=0xBF*/

#define SCR             7       /*Scratch Register*/
#define	XOFF2R		7	/*XOFF2 reg when LCR=0xBF*/

/************************************************************************/
/*=====8259 registers Addresses                                         */
/************************************************************************/
#define PORT_8259_REG1  0x20    /*8259 reg1*/
#define PORT_8259_REG2  0x21    /*8259 reg2*/
#define EOI_8259        0x20    /*End of int instruction to 8259*/

/************************************************************************/
/*=====Baud Rate Indeces                                                */
/************************************************************************/
#define BAUD110         0       /* 110 baud */
#define BAUD150         1       /* 150 baud */
#define BAUD300         2       /* 300 baud */
#define BAUD600         3       /* 600 baud */
#define BAUD1200        4       /* 1200 baud */
#define BAUD2400        5       /* 2400 baud */
#define BAUD4800        6       /* 4800 baud */
#define BAUD9600        7       /* 9600 baud */
#define BAUD19200       8       /* 19200 baud */
#define BAUD38400       9       /* 38400 baud */
#define BAUD57600       10      /* 57600 baud */
#define BAUD115200      11      /* 115.2 kbaud */
#define BAUDUSER00      12      /* User baud rate */
#define BAUD14400       12      /* 14400 baud */
#define BAUDUSER01      13      /* User baud */
#define BAUD28800       13      /* 28800 baud */
#define BAUDUSER02      14      /* User baud rate */
#define BAUDUSER03      15      /* User baud rate */
#define BAUDUSER04      16      /* User baud rate */
#define BAUDUSER05      17      /* User baud rate */
#define BAUDUSER06      18      /* User baud rate */
#define BAUDUSER07      19      /* User baud rate */
#define BAUDUSER08      20      /* User baud rate */
#define BAUDUSER09      21      /* User baud rate */

/************************************************************************/
/*=====Parity Indeces                                                   */
/************************************************************************/
#define PAR_NONE        0       /* No parity */
#define PAR_ODD         1       /* ODD parity */
#define PAR_EVEN        2       /* Even parity */
#define PAR_SODD        3       /* Sticky ODD parity */
#define PAR_SEVEN       4       /* Sticky Even parity */

#ifndef MSWIN32
#define PARITY_NONE     0       /* No parity */
#define PARITY_ODD      1       /* ODD parity */
#define PARITY_EVEN     2       /* Even parity */
#define PARITY_SODD     3       /* Sticky ODD parity */
#define PARITY_SEVEN    4       /* Sticky Even parity */
#endif

/************************************************************************/
/*=====Length Indeces                                                   */
/************************************************************************/
#define LENGTH_5        0       /* 5 bits */
#define LENGTH_6        1       /* 6 bits */
#define LENGTH_7        2       /* 7 bits */
#define LENGTH_8        3       /* 8 bits */

/************************************************************************/
/*=====Stopbit Indeces                                                  */
/************************************************************************/
#define STOPBIT_1       0       /* 1 Stop bit */
#define STOPBIT_2       1       /* 2 Stop bit */

/************************************************************************/
/*=====Break Indeces                                                    */
/************************************************************************/
#define BREAK_OFF       0       /* Break off */
#define BREAK_ON        1       /* Break on */

/************************************************************************/
/*=====Protocol Indeces                                                 */
/************************************************************************/
#define PROT_RTSRTS     0       /*Local CTS/RTS-Remote CTS/RTS */
#define PROT_RTSXON     1       /*Local CTS/RTS-Remote XON/XOFF*/
#define PROT_RTSDTR     2       /*Local CTS/RTS-Remote DSR/DTR*/
#define PROT_RTSNON     3       /*Local CTS/RTS-Remote no prot*/
#define PROT_NONNON     4       /*Local no prot-Remote no prot*/
#define PROT_NONXON     5       /*Local no  protocol-Remote XOFF/XON*/
#define PROT_XONNON     6       /*Local XOFF/XON-Remote no prot*/
#define PROT_XONXON     7       /*Local XON/XOFF-Remote XON/XOFF*/
#define PROT_DTRNON     8       /*Local  DSR/DTR-Remote no protocol*/
#define PROT_DTRRTS     9       /*Local  DSR/DTR-Remote CTS/RTS*/
#define PROT_DTRDTR     10      /*Local  DSR/DTR-Remote DSR/DTR */
#define PROT_DTRXON     11      /*Local  DSR/DTR-Remote DSR/DTR*/
#define PROT_NONRTS     12      /*Local no  prot-Remote CTS/RTS prot*/
#define PROT_NONDTR     13      /*Local no  prot-Remote DSR/DTR prot*/
#define PROT_XONRTS     14      /*Local XOFF/XON- Remote CTS/RTS*/
#define PROT_XONDTR     15      /*Local XOFF/XON-Remote DSR/DTR*/

/************************************************************************/
/*=====Cardtype Indeces                                                 */
/************************************************************************/
#define CARD_NORMAL     0       /* Normal cards */
#define CARD_INTELH     1       /* Intel HUB card(No Longer supported) */
#define CARD_CYCLOMY    1       /* Cyclades ISA & PCI Yx Cards */
#define CARD_DIGCXI     2       /* Digiboard COMXI */
#define CARD_ARNETSPLUS 3       /* Arnet Smartplus */
#define CARD_BOCA1610   4       /* BOCA 1610  */
#define CARD_DIGPCX     5       /* Digiboard PCX  */
#define CARD_GTEK       6       /* GTEK board  */
#define CARD_INT14      7       /* 3rd party INT14H board  */
#define CARD_WCSCVXD    8       /* WCSC high speed VxD for 8250 family */
#define CARD_WINAPI     9       /* Uses Windows API */

/************************************************************************/
/*=====Flag bit settings(System Flag(pcb.flag))                         */
/************************************************************************/
#define ACTIVE_FLG      0x0001  /* Set = port is active. */
#define XMTOFF_STATE    0x0002  /* Set = Remote host in XOFF state. */
#define RCVOFF_STATE    0x0004  /* Set = Local host in XOFF state. */
#define INTFUNC_MODEM   0x0008  /* Set = Exec modem intrpt func */
#define INTFUNC_STAT    0x0010  /* Set = Exec status intrpt func */
#define INTFUNC_RCV     0x0020  /* Set = Exec receive intrpt func */
#define INTFUNC_XMIT    0x0040  /* Set = Exec xmit interpt func */
#define INTFUNC_AVAIL   0x0080  /* Set =  */
#define INTFUNC_RCVCHR  0x0100  /* Set = Exec specific char rcv intrpt func */
#define INTFUNC_IBFDCNT 0x0200  /* Set = Exec inbuf dec to val intrpt func */
#define INTFUNC_IBFICNT 0x0400  /* Set = Exec inbuf inc to val intrpt func */
#define INTFUNC_OBFDCNT 0x0800  /* Set = Exec outbuf dec to val intrpt func */
#define INTFUNC_OBFICNT 0x1000  /* Set = Exec outbuf inc to val intrpt func */
#define INTFUNC_XMTCHR  0x2000  /* Set = Exec if about to xmit chr intrpt fnc */
#define INTFUNC_X       0x4000  /* Set = Use extended masks  */
#define INTFUNC_OTHER   0x8000  /* Set = Exec by other conditions */
#define INTFUNC_X_BREAK 0x0001  /* Set = Exec on Break intrpt func */
#define INTFUNC_X_FRAME 0x0002  /* Set = Exec on Frame intrpt func */
#define INTFUNC_X_OVRUN 0x0004  /* Set = Exec on Parity intrpt func */
#define INTFUNC_X_PARER 0x0008  /* Set = Exec on Frame intrpt func */
#define INTFUNC_X_CTS   0x0010  /* Set = Exec on CTS intrpt func */
#define INTFUNC_X_DSR   0x0020  /* Set = Exec on DSR intrpt func */
#define INTFUNC_X_CD    0x0040  /* Set = Exec on CD intrpt func */
#define INTFUNC_X_RING  0x0080  /* Set = Exec on RING intrpt func */

/************************************************************************/
/*=====Flag2 bit settings(User flag(pcb.flag2))                         */
/************************************************************************/
#define STOREALL                0x0001  /*Set = Store LSR/MSR/PORT/DATA*/
#define DIFFXBUF                0x0002  /*Set = Different seg for xbuf */
#define CHAIN_INT               0x0004  /*Set = Chain interrupts */
#define LEAVE_DTR               0x0008  /*Set = DTR unchanged on cleanup */
#define LEAVE_RTS               0x0010  /*Set = RTS unchanged on cleanup */
#define IGNORE_16550            0x0020  /*Set = Do not enable 16550 */
#define DISABLE_MODEMSIG        0x0040  /*Set = Disable RTS/DTR on setup */
#define NOCHANGE_MODEMSIG       0x0080  /*Set = On setup leave RTS/DTR as is */
#define STORESOME               0x0100  /*Set = Store PORT/DATA only*/
#define LOCKBAUD                0x0200  /*Set = Lock baud at initial setup */
#define POLLONSTATUS            0x0400  /*Set = call uoprime&uiprime */
#define PARENTPORT              0x0800  /*Set = VxD will use pport for parent */
#define CUSTOMFLAG1             0x1000  /*Custom usage explained elsewhere */
#define	PCIFLAG			0x1000	/*Set = PCI card/Adr=VenID Seg=DevID*/
#define CUSTOMFLAG2             0x2000  /*Custom usage explained elsewhere */
#define TIMESTAMPBYTES          0x2000  /*Timestamp bytes in stream  */
#define CUSTOMFLAG3             0x4000  /*Custom usage explained elsewhere */
#define NINEBITPROTOCOL         0x4000  /*Use Nine Bit protocol*/
#define CUSTOMFLAG4             0x8000  /*Custom usage explained elsewhere */
#define MULTIDROP               0x8000  /*Use multidrop protocol*/

/************************************************************************/
/*=====Values returned to intfunc() handler                             */
/************************************************************************/
#define INTFUNC_VMODEM          0x0000  /* Modem intrpt */
#define INTFUNC_VSTAT           0x0001  /* Status intrpt */
#define INTFUNC_VRCV            0x0002  /* Receive intrpt */
#define INTFUNC_VXMIT           0x0003  /* Xmit interpt */
#define INTFUNC_VAVAIL          0x0004  /*  */
#define INTFUNC_VRCVCHR         0x0005  /* Specific char rcv intrpt */
#define INTFUNC_VIBFDCNT        0x0006  /* Inbuf dec to val intrpt */
#define INTFUNC_VIBFICNT        0x0007  /* Inbuf inc to val intrpt */
#define INTFUNC_VOBFDCNT        0x0008  /* Outbuf dec to val intrpt */
#define INTFUNC_VOBFICNT        0x0009  /* Outbuf inc to val intrpt */
#define INTFUNC_VXMTCHR         0x000a  /* XMIT specific char int */
#define INTFUNC_VWAITCDON       0x000b  /* Waitfor CD on int */
#define INTFUNC_VWAITCDOFF      0x000c  /* Waitfor CD off int */
#define INTFUNC_VWAITCTSON      0x000d  /* Waitfor CTS on int */
#define INTFUNC_VWAITCTSOFF     0x000e  /* Waitfor CTS off int */
#define INTFUNC_VWAITDSRON      0x000f  /* Waitfor DSR on int */
#define INTFUNC_VWAITDSROFF     0x0010  /* Waitfor DSR off int */
#define INTFUNC_VWAITRION       0x0011  /* Waitfor RI on int */
#define INTFUNC_VWAITRIOFF      0x0012  /* Waitfor RI off int */
#define INTFUNC_VWAITXEMPTY     0x0013  /* Waitfor xmit empty int */
#define INTFUNC_VWAITXNOTEMPTY  0x0014  /* Waitfor xmit not empty int */
#define INTFUNC_VWAITREMPTY     0x0015  /* Waitfor receive empty int */
#define INTFUNC_VWAITRNOTEMPTY  0x0016  /* Waitfor receive not empty int */
#define INTFUNC_VWAITTIME       0x0017  /* Waitfor time int */
#define INTFUNC_VX_BREAK        0x0018  /* Break intrpt */
#define INTFUNC_VX_FRAME        0x0019  /* Frame intrpt */
#define INTFUNC_VX_OVRUN        0x001a  /* Overrun intrpt */
#define INTFUNC_VX_PARER        0x001b  /* Parity intrpt */
#define INTFUNC_VX_CTS          0x001c  /* CTS intrpt */
#define INTFUNC_VX_DSR          0x001d  /* DSR intrpt */
#define INTFUNC_VX_CD           0x001e  /* CD intrpt */
#define INTFUNC_VX_RING         0x001f  /* RING intrpt */
#define INTFUNC_VWAITRCOUNT     0x0020  /* RCV Count intrpt */
#define INTFUNC_VWAITXCOUNT     0x0021  /* XMT Count intrpt */
#define INTFUNC_VSENDNOWDONE    0x0022  /* Send now done */

/************************************************************************/
/*=====ser_rs232_misc_func() commands                                   */
/************************************************************************/
#define WAITCDON        0x0000  /* Waitfor CD on */
#define WAITCDOFF       0x0001  /* Waitfor CD off */
#define WAITCTSON       0x0002  /* Waitfor RTS on */
#define WAITCTSOFF      0x0003  /* Waitfor RTS off */
#define WAITDSRON       0x0004  /* Waitfor DSR on  */
#define WAITDSROFF      0x0005  /* Waitfor DSR off */
#define WAITRION        0x0006  /* Waitfor RI on */
#define WAITRIOFF       0x0007  /* Waitfor RI off */
#define WAITXEMPTY      0x0008  /* Waitfor xmit buf empty */
#define WAITXNOTEMPTY   0x0009  /* Waitfor xmit buf not empty */
#define WAITREMPTY      0x000a  /* Waitfor receive buf empty */
#define WAITRNOTEMPTY   0x000b  /* Waitfor receive buf not empty */
#define SETRCVCHR       0x000c  /* Set receive character comparator */
#define SETXMTCHR       0x000d  /* Set transmit character comparator */
#define SETIBFDCNT      0x000e  /* Set input buffer down count threshold */
#define SETIBFICNT      0x000f  /* Set input buffer up count threshold */
#define SETOBFDCNT      0x0010  /* Set output buffer down count threshold */
#define SETOBFICNT      0x0011  /* Set output buffer up count threshold */
#define WAITTIME        0x0012  /* Waitfor time */
#define WAITRCOUNT      0x0013  /* Waitfor at least RCV buffer count */
#define WAITXCOUNT      0x0014  /* Waitfor at least XMT buffer count */
#define SETRCOUNT       0x0015  /* Waitfor at least RCV buffer count */
#define SETXCOUNT       0x0016  /* Waitfor at least XMT buffer count */
#define SENDNOW         0x0017  /* Send a string ahead of buffered data */

/************************************************************************/
/*=====IRQ Flag bit settings(icb.irq_flag)                              */
/************************************************************************/
#define IRQ_ACTIVE      0x0004          /* IRQ active */
#define IRQ_CHAIN_INT   0x0008          /* Chain interrupt */

/************************************************************************/
/*=====Return error codes                                               */
/************************************************************************/
#define RS232ERR_NONE           0       /*RS232 no error*/
#define RS232ERR_BUFFER         1       /*RS232 buffer not set or buf changed*/
#define RS232ERR_ACTIVE         2       /*RS232 port not active*/
#define RS232ERR_XMTBUF         3       /*RS232 xmit buffer full*/
#define RS232ERR_RCVBUF         4       /*RS232 recv buffer empty*/
#define RS232ERR_SYNTAX         5       /*RS232 port syntax  error*/
#define RS232ERR_BUFSIZ         6       /*RS232 invalid buffer size*/
#define RS232ERR_PORT           7       /*RS232 invalid port*/
#define RS232ERR_HANDLR         8       /*RS232 handler changed*/
#define RS232ERR_BAUD           9       /*RS232 invalid baud rate*/
#define RS232ERR_PARITY         10      /*RS232 invalid parity*/
#define RS232ERR_LNGTH          11      /*RS232 invalid data length*/
#define RS232ERR_STOPBIT        12      /*RS232 invalid # stopbits*/
#define RS232ERR_PROTOCOL       13      /*RS232 invalid protocol*/
#define RS232ERR_IRQCHANGED     14      /*RS232 IRQ changed*/
#define RS232ERR_PORTCHANGED    15      /*RS232 port changed*/
#define RS232ERR_THRESHOLD      16      /*RS232 invalid threshold*/
#define RS232ERR_INVIRQ         17      /*RS232 invalid IRQ*/
#define RS232ERR_INTOFF         18      /*RS232 interrupts not enabled*/
#define RS232ERR_BREAK          19      /*RS232 invalid break syntax*/
#define RS232ERR_FATAL          20      /*RS232 fatal error*/
#define RS232ERR_DSR            21      /*RS232 CTS error*/
#define RS232ERR_INVADR         22      /*RS232 Invalid RS232 address*/
#define RS232ERR_ENVIRON        23      /*Environment variable not set */
#define RS232ERR_IOCTL          24      /*Error issuing IOCTL call */
#define RS232ERR_ATEXIT         25      /*Error issuing atexit cleanup */
#define RS232ERR_DEVINIT        26      /*Error mapping device for dir calls*/
#define RS232ERR_DOSOPEN        27      /*Error opening Device */
#define RS232ERR_MALLOC         28      /*Error allocating memory */
#define RS232ERR_EXTMICRO       29      /*Error on external micro*/
#define RS232ERR_CARDCHANGED    30      /*Card changed error */
#define RS232ERR_CARDTYPE       31      /*Card type error */
#define RS232ERR_NOSUPPORT      32      /*Not supported */
#define RS232ERR_CMDBUFFULL     33      /*Card command buffer full*/
#define RS232ERR_PPORT          34      /*Invalid parent PCB*/
#define RS232ERR_NODEVICE       35      /*No device for this port*/
#define RS232ERR_UNKNOWN        36      /*Unknow error*/
#define RS232ERR_BUSY           37      /*Busy*/
#define RS232ERR_NOTIMER        38      /*No more timers available*/
#define RS232ERR_INT14VEC       39      /*INT 14H vector changed*/
#define RS232ERR_INT1CVEC       40      /*Timer vector changed*/
#define RS232ERR_DPMI           41      /*DPMI error*/
#define RS232ERR_WINBUF         42      /*No windows buffer or too small*/
#define RS232ERR_NOASYNCRES     43      /*No asynchronous resources left */
#define RS232ERR_NOTIMERRES     44      /*No timer resources left */
#define RS232ERR_NOOTHERES      45      /*Out of other resources */
#define RS232ERR_FILEIO         46      /*File I/O error */
#define RS232ERR_HMEMG64        47      /*Hardware memory exceeded 64K */
#define RS232ERR_MAPVXD         48      /*VXD/Kernel Driver not loaded */
#define RS232ERR_THREAD         49      /*Could not start a thread */
#define RS232ERR_MAPVDD         50      /*NT VDD CDRVVDD.DLL not loaded */
#define	RS232ERR_PCIBIOS	51	/*No PCI BIOS present */
#define	RS232ERR_TSRNOTLOADED	52	/*TSR not loaded */
#define	RS232ERR_TIMEOUT	53	/*Timeout */

/************************************************************************/
/*======Offsets to callbacks from sub-device functions to COMM-DRV      */
/************************************************************************/
#define CALLBACK_MSR            0       /* Called on every MSR change int. */
#define CALLBACK_XMT            4       /* Called on every XMIT interrupt. */
#define CALLBACK_RCV            8       /* Called on every RCV interrupt. */
#define CALLBACK_LSR            12      /* Called on every LSR interrupt. */
#define CALLBACK_SETTIM         16      /* Called to set a timer. */
#define CALLBACK_RSTTIM         20      /* Called to reset a timer. */
#define CALLBACK_GETPATH        24      /* Called to reset a timer. */
#define CALLBACK_GETDATA        28      /* Called to get data pointer. */

/************************************************************************/
/*======Offsets to sub-device functions                                 */
/************************************************************************/
#define CALLDEV_CALLBACK        0       /* IDX for adr of ptr to c'back funcs */
#define CALLDEV_UINTHDR         4       /* IDX for sub-dev interrupt handler */
#define CALLDEV_UDTROFF         8       /* IDX for sub-dev dtr_off function */
#define CALLDEV_UDTRON          12      /* IDX for sub-dev dtr_on function */
#define CALLDEV_UGETREG         16      /* IDX for sub-dev  get uart reg func */
#define CALLDEV_UIPRIME         20      /* IDX for sub-dev input primer */
#define CALLDEV_UOPRIME         24      /* IDX for sub-dev output primer */
#define CALLDEV_UPUTREG         28      /* IDX for sub-dev put uart reg func */
#define CALLDEV_URESET          32      /* IDX for sub-dev reset/cleanup */
#define CALLDEV_URTSOFF         36      /* IDX for sub-dev rts_off */
#define CALLDEV_URTSON          40      /* IDX for sub-dev rts_on */
#define CALLDEV_USETUP          44      /* IDX for sub-dev init/setup */
#define CALLDEV_UXMIT           48      /* IDX for sub-dev xmit trigger */
#define CALLDEV_UKXMIT          52      /* IDX for sub-dev kill xmitter */
#define CALLDEV_UIXMIT          56      /* IDX for sub-dev immediate xmit */
#define CALLDEV_UBAUDIV         60      /* IDX for sub-dev baud rate divisor */
#define CALLDEV_INTENTER        64      /* IDX for sub-dev enter isr */
#define CALLDEV_INTEXIT         68      /* IDX for sub-dev exit isr */
#define CALLDEV_MPX             72      /* IDX for multiplex functions */

#define COMMDRV_SUBDEV_FLAG     96      /* Offset to subdevice flags */

/************************************************************************/
/*======Auxiliary flag bits(auxpcb.aux_flag)                            */
/************************************************************************/
#define AUXFLAG_BRKERR          0x00000001L     /* Set=Break error occurred */
#define AUXFLAG_FRAMERR         0x00000002L     /* Set=Framing error occurred */
#define AUXFLAG_IOVRERR         0x00000004L     /* Set=Input overrun occurred */
#define AUXFLAG_OVRERR          0x00000008L     /* Set=UART overrun occurred */
#define AUXFLAG_PARERR          0x00000010L     /* Set=Parity error occurred */
#define AUXFLAG_RING            0x00000020L     /* Set=Ring detected on UART */
#define AUXFLAG_XFERDLG         0x00000040L     /* Set=Show File Xfer Dialog */
#define AUXFLAG_XFERCAN         0x00000080L     /* Set=Cancel File Xfer */
#define AUXFLAG_NOMSGLOOP       0x00000100L     /* Set=No to 2ndary Msg Loop */
#define AUXFLAG_NOTIMER         0x00000200L     /* Set=No to timer routine */
#define AUXFLAG_SWITCHONDELAY   0x00000400L     /* Set=Force context on del */
#define AUXFLAG_9BITTOGGLE1     0x00000800L     /*  Nine bit toggle*/
#define AUXFLAG_9BITTOGGLE2     0x00001000L     /*  Nine bit toggle*/
#define AUXFLAG_SLEEPONDELAY	0x00001000L     /* Set=Force Sleep on delay */

/************************************************************************/
/*======Async Event Resource                                            */
/************************************************************************/
struct async_res
{
#ifndef MSWIN32
CDRVUINT                async_ptr;      /* Pointer to next async resource */
CDRVUINT                async_tim;      /* Timer */
CDRVUINT                async_val;      /* Value to pass to intfunc */
CDRVUINT                async_pcb;      /* Port PCB */
CDRVUINT                async_fnc;      /* Testing sub-function */
#else
struct async_res        *async_ptr;     /* Pointer to next async resource */
unsigned long           async_tim;      /* Timer */
short                   async_val;      /* Value to pass to intfunc */
struct port_param       *async_pcb;     /* Port PCB */
int                     async_fnc;      /* Testing sub-function index */
#endif
};

/************************************************************************/
/*======Timed Event Resource                                            */
/************************************************************************/
struct timer_res
{
CDRVUINT        timer_ptr;      /* Pointer to next timer resource */
CDRVUINT        timer_tim;      /* Timer */
void FAR        *timer_fnc;     /* user function */
CDRVUINT        timer_rtm;      /* Timer reload */
};

/************************************************************************/
/*======Modem Information structure.                                    */
/************************************************************************/
struct modem_param
{
int     Pacetime;       /* Modem pacetime(def=>1)                       */
int     PlusPacetime;   /* +++ Pacetime(def=>2)                         */
int     Waitresponse;   /* Timeout to wait for modem(def=>1*18)         */
int     Waitconnect;    /* Timeout for connect(def=>30*18)              */
int     Waitecho;       /* Timeout to wait for echo(def=>1*18)          */
int     Waitcommand;    /* Timeout to wait for command(def=>1*18)       */
int     Waitdtr;        /* DTR flash time(def=>2*18)                    */
int     RingInterval;   /* Interval between rings(def=>10*18)           */
char    CmdPref[4];     /* AT command prefix (def=>"AT")                */
char    Init[80];       /* Initialization string (def=>"V1Q0")          */
char    ToneDial[4];    /* Tone Dial (def=>"DT")                        */
char    PulseDial[4];   /* Pulse Dial(def=>"DP")                        */
char    Reset[4];       /* Reset modem string (def=>"Z")                */
char    Escape[12];     /* Escape to command mode(def=>"+++")           */
char    Hangup[4];      /* Hangup string (def=>"H")                     */
char    CmdSufx[4];     /* AT command suffix (def=>"\r")                */
char    Ok[10];         /* AT command acknowledge (def=>"OK")           */
char    Error[10];      /* AT command failure (def=>"ERROR")            */
char    Connect[15];    /* AT command connect (def=>"CONNECT")          */
char    NoCarrier[15];  /* AT command carrier (def=>"NO CARRIER")       */
char    Busy[10];       /* AT command connect (def=>"BUSY")             */
char    NoTone[15];     /* AT command connect (def=>"NO DIALTONE")      */
char    Verbose[10];    /* AT Verbose mode (def=>"V1Q0")                */
char    Answer[10];     /* AT Verbose mode (def=>"S0=")                 */
char    Carrier[15];    /* AT command connect (def=>"CARRIER")          */
char    Ring[10];       /* AT Ringing (def=>"RING")                     */
char    AnswerCmd[4];   /* AT Answering (def=>"A")                      */
char    Online[4];      /* AT Go Online (def=>"O")                      */
char    Offhook[4];     /* AT Go Offhook (def=>"H1")                    */
char    SpeakerOff[4];  /* AT Speaker Off(def=>"M0")                    */
char    SpeakerOn[4];   /* AT Speaker On(def=>"M1")                     */
char    VolumeLow[4];   /* AT Volume Low(def=>"L1")                     */
char    VolumeMedium[4];/* AT Volume Medium(def=>"L2")                  */
char    VolumeHigh[4];  /* AT Volume High(def=>"L3"                     */
};

/************************************************************************/
/*======Auxiliary port control block                                    */
/************************************************************************/
#define SENDNOWBUFSIZE  10      /* Size of send now buffer */
#ifdef	MSWIN32
#define	CDRVMAXCONTEXT	20	/* */
#else
#define	CDRVMAXCONTEXT	5	/* */
#endif

struct aux_pparam
{
unsigned long   aux_itimer[3];  /* Input timer block */
unsigned long   aux_otimer[3];  /* Output timer */
unsigned long   aux_rcvint;     /* Receiver interrupts */
unsigned long   aux_timint;     /* Timeout Receiver interrupts */
unsigned long   aux_xmtint;     /* Transmit interrupts */
unsigned long   aux_lsrint;     /* LSR interrupts */
unsigned long   aux_msrint;     /* MSR interrupts */
unsigned long   aux_ovrint;     /* Overrun error */
unsigned long   aux_parint;     /* Parity error */
unsigned long   aux_frmint;     /* Framing error */
unsigned long   aux_brkint;     /* Break error */
unsigned long   aux_rcvxon;     /*XONs received*/
unsigned long   aux_rcvxoff;    /*XOFFs received*/
unsigned long   aux_xmtxon;     /*XONs xmitted*/
unsigned long   aux_xmtxoff;    /*XOFFs xmitted*/
unsigned long   aux_ctson;      /*CTS off to on transitions*/
unsigned long   aux_dsron;      /*DSR off to on transitions*/
unsigned long   aux_rtson;      /*RTS off to on transitions*/
unsigned long   aux_dtron;      /*DTR off to on transitions*/
unsigned int    aux_iblk[5];    /* Input buffer control block */
unsigned int    aux_oblk[5];    /* Output buffer control block */
unsigned short  aux_PCIAdr;     /* Uart address on PCI Card */
unsigned short  aux_PCIIrq;     /* Uart IRQ on PCI card */
CDRVUINT        aux_spare11;    /* Spare */
unsigned char   aux_spare01[2]; /* */
CDRVUINT        aux_ibfdcnt;    /* Input buffer dec flag point(intfunc) */
CDRVUINT        aux_ibficnt;    /* Input buffer inc flag point(intfunc) */
CDRVUINT        aux_obfdcnt;    /* Output buffer dec flag point(intfunc) */
CDRVUINT        aux_obficnt;    /* Output buffer inc flag point(intfunc) */
unsigned char   aux_rcvchr;     /* Receive char flag point(intfunc) */
unsigned char   aux_xmtchr;     /* Transmit char flag point(intfunc) */
unsigned char   aux_cmask;      /* Character mask */
unsigned char   aux_flag2;	/* flag */
CDRVUINT        aux_rcount;     /* RCV Buffer Count Event thresh */
CDRVUINT        aux_xcount;     /* XMT Buffer Count Event thresh */
int             aux_wport;      /* Windows driver port for this PCB*/
CDRVUINT                aux_sendnowcnt;         /* Send now count */
unsigned char FAR       *aux_sendnowptr;        /* Pointer to data */
char            aux_sendnowbuf[SENDNOWBUFSIZE]; /* Send now buffer size */
long            aux_sendnowtot; /* #of sendnow characters sent. */
long            aux_icharflush; /* #of input characters flushed */
long            aux_ocharflush; /* #of output characters flushed */
long            aux_charnotbuf; /* #of chars disposed by "RCV" intfunc */
long            aux_charnotsent;/* #of chars not sent because of"XMT"intfunc*/
CDRVUINT        aux_user[5];            /* For user purposes */
long            aux_spare10;            /* Spare */
CDRVIFNC (FARC *(FAR *aux_callback))(short c,struct port_param FAR *p);
int             aux_pacetime;   /* Character pacing time interval */
int             aux_ftimeout;   /* Function timeout */
int (FARC *aux_tfunc)(int port);        /* Fnc set w/CdrvSetTimeoutFunction() */
int             aux_timerres;           /* Timer resolution */
long            aux_flag;               /* Flag used by high level functions */
struct modem_param FAR  *aux_modem;     /* Modem Information */
long            aux_carrierspeed;       /* Modem carrier speed */
long            aux_connectspeed;       /* Modem connect speed */
void (FARC *aux_dsfnc)(int p,int c, char FAR *h); /* Data stream fnc  */
int             aux_delimcnt;           /* # of characters in delimiter list */
char FAR        *aux_delimptr;          /* Pointer to delimiter list */
unsigned long	aux_context[CDRVMAXCONTEXT]; /* Context data for use by (ser_rs232_set_context)*/
					/*0=Used by COMM-DRV++ for C++ context */
					/*1=Used by Callbacks(Cdecl Calls) 2=WaitForSetString*/
					/*3=Used for multiple file xfer spec */
					/*4=Used for XferFiles() */
					/*5=Abort flag 6=Callbacks(Winapi calls)*/
					/*10-19=Used by CdrvSetContext()*/
};

/************************************************************************/
/*======Interrupt Control Block structure (ICB)                         */
/************************************************************************/
struct irq_param
{
CDRVUINT        irq_hdr;        /* Offset to IRQ handler */
CDRVUINT        irq_lnk;        /* Head of all PCB belonging to this IRQ */
void    (FARC *irq_oldvec)();   /* Old IRQ vector for this IRQ */
CDRVUINT        irq_num;        /* IRQ number */
CDRVUINT        irq_flag;       /* Flags/Subdevices may use upper 8 bits */
unsigned long   irq_pass;       /* Total interupts passed to next handler */
unsigned long   irq_int;        /* Interrupt instances processed */
unsigned long   irq_ign;        /* Interrupts ignored */
unsigned long   irq_oldvec2[2]; /* Old IRQ vector for this IRQ */
unsigned long   irq_spare[3];   /* Spares */
};

/************************************************************************/
/*======Port Control Block structure (PCB)                              */
/************************************************************************/
struct port_param
{
CDRVUINT        ser_rs232_base; /*Base address of port*/
unsigned short  irq;            /*Interrupt number*/
CDRVUINT        baud;           /*baud rate index*/
unsigned short  parity;         /*parity index*/
unsigned short  lngth;          /*length index*/
unsigned short  stopbit;        /*stopbit index*/
unsigned short  brk;            /*Break*/
unsigned short  protocol;       /*protocol index*/
unsigned char   block[2];       /*# of 1/18.2 secs count*/
CDRVUINT        inbuf_low;      /*Low threshold of recv buffer*/
CDRVUINT        inbuf_high;     /*High threshold of xmit buffer*/
CDRVUINT        buffer_seg;     /*Segment to start of buffer*/
CDRVUINT        buffer_off;     /*Offset to start of buffer*/
CDRVUINT        inbuf_len;      /*Input buffer length*/
CDRVUINT        outbuf_len;     /*Output buffer length*/
unsigned char   aux_addr1;      /*Auxiliary Address*/
unsigned char   sflag;          /*System flag*/
CDRVUINT        cardseg;        /*Memory card segment*/
unsigned short  cardtype;       /*Card Type*/
CDRVIFNC (FAR *(FAR *intfunc))(short c,struct port_param FAR *p);
CDRVUINT        error;          /* Error flag*/
CDRVUINT        flag;           /* general purpose flag*/
CDRVUINT        inbuf_count;    /* # bytes in input buf*/
CDRVUINT        outbuf_count;   /* # of bytes in ouput*/
unsigned long   lost_ichar;     /* Lost inputted bytes*/
unsigned long   lost_ochar;     /* Lost outputted bytes*/
unsigned long   total_ichar;    /* Total attempted bytes in*/
unsigned long   total_ochar;    /* Total attempted bytes out*/
void FAR *   FAR *devfuncs;     /* Pointer to table of pointer to dev funcs  */
unsigned char   msr_reg;        /* Current modem status register*/
unsigned char   lsr_reg;        /* Current line status register*/
CDRVUINT        ddata;          /* Data that sub-device driver may use */
CDRVUINT        inbuf_start;    /* Input  buffer control block*/
CDRVUINT        outbuf_start;   /* Output buffer control block*/
struct irq_param FAR *irqp;     /* Pointer to irq parameter block */
CDRVUINT        port;           /* Port number */
CDRVUINT        flag2;          /* Secondary flag */
CDRVUINT        obufoff;        /* Output buf off if DIFFXBUF set */
CDRVUINT        obufseg;        /* Output buf seg if DIFFXBUF set */
CDRVUINT        pport;          /* Parent port */
CDRVUINT        ppcb;           /* Parent pcb */
CDRVUINT        timer_iblk;     /*Timer block.*/
CDRVUINT        timer_oblk;     /*Timer block.*/
unsigned char   hblock[2];      /* Hi byte of input/output timeouts */
unsigned char   char_xon;       /* XON character */
unsigned char   char_xoff;      /* XOFF character */
unsigned char   char_xxon;      /* XON xmit character */
unsigned char   char_xxoff;     /* XOFF xmit character */
struct aux_pparam FAR *auxpcb;  /* Pointer to auxiliary PCB*/
unsigned int    prot_local;     /* Local protocol */
unsigned int    prot_remote;    /* Remote protocol */
struct port_param FAR *opcb;    /* Pointer to this PCB*/
CDRVUINT        dflag;          /* Subdevice specific flag */
unsigned char   o_baud_reg[2];  /* Old baud rate*/
unsigned char   o_lcr_reg;      /* Old line control register*/
unsigned char   o_mcr_reg;      /* Old modem control register*/
unsigned char   o_ier_reg;      /* Old interrupt enable register*/
unsigned char   tuart;          /* Type of UART*/
CDRVUINT        next_pcb;       /* Pointer to next */
};

/************************************************************************/
/*=====System Data                                                      */
/************************************************************************/
struct ser_rs232_sdata
{
CDRVUINT        dfunc;          /* Offset to table of driver functions  */
unsigned short  drvmn;          /* Maximum number of drivers            */
unsigned short  drvna;          /* Number of active drivers             */
unsigned short  icbmn;          /* Highest IRQ allowed                  */
unsigned short  icbmx;          /* Max num of concurrently active IRQs  */
CDRVUINT        icbs;           /* Offset pointer to pool of icbs       */
CDRVUINT        icbss;          /* Offset to pool of icbs               */
unsigned short  icbna;          /* Number of ICBs Active                */
unsigned short  pcbmn;          /* Highest port number allowed          */
unsigned short  pcbmx;          /* Max num of concurrently active ports */
CDRVUINT        pcbs;           /* Offset pointer to pool of pcbs       */
CDRVUINT        pcbss;          /* Offset to pool of pcbs               */
unsigned short  pcbna;          /* Number of PCBs Active                */
unsigned short  timmx;          /* Maximum Number of timers             */
CDRVUINT        timss;          /* Start of timers                      */
CDRVUINT        timact;         /* Start of timers active               */
CDRVUINT  timfre;               /* Start of free timers                 */
unsigned short  asyncmx;        /* Maximum Number of async resources    */
CDRVUINT        asyncss;        /* Start of async resources             */
CDRVUINT        asyncact;       /* Start of async active                */
CDRVUINT  asyncfre;     /* Start of free asyncs                 */
void FAR        *wbuf;          /* Pointer Windows shared buffer        */
CDRVUINT        wbufsz;         /* Windows buffer size                  */
CDRVUINT        wbuffg;         /* Windows shared buffer flag           */
void FAR        *wintfunc;      /* DPMI function to call Windows func   */
CDRVUINT        irqign;         /* Ints ignored during COMM-DRV init    */
void FAR        *old_thandler;  /* Old timer handler                    */
void FAR        *callback[10];  /* Array of pointers to callbacks       */
unsigned short  FAR *dev_head;  /* Start of MS-DOS/COMMDRV device chain */
unsigned short  dev_count;      /* Number of MSDOS/COMMDRV devices      */
char    cdrvpath[CDRVPATHLEN];  /* Path to COMMDRV                      */
unsigned char   baudremap[22];  /* Baud rate remapping                  */
struct port_param FAR * (FARC *getpcbfptr)(int port);   /* Get PCB fnc  */
int             multitasker;    /* 0=NONE 1=MTASK 2=Desqview 3=WindowsE */
int             devpolltime;    /* Device poll time                     */
void (FARC *begincritical)(void);       /* Begin critical section       */
void (FARC *endcritical)(void);         /* End critical section         */
void (FARC *ibegincritical)(void);      /* Begin int critical section   */
void (FARC *iendcritical)(void);        /* End int critical section     */
long            critdenied;             /* Critical section denied      */
long            icritdenied;            /* Crit sectione denied from int*/
};

/************************************************************************/
/* Structure of Ring Buffer Control Block                               */
/************************************************************************/
struct RingCtl
{
int                     RingTop;
int                     RingBottom;
int                     RingInput;
int                     RingOutput;
int                     Size;
int                     Cnt;
unsigned char FAR       *Buf;
};

/************************************************************************/
/*=====WaitFor Control Structure					*/
/************************************************************************/
struct WaitForCtl
	{
	int    		WaitForCount;
	int		*WaitForLength;
	unsigned char	**WaitForString;
	};

/************************************************************************/
/*=====IOCTL Commands for COMM-DRV/Dos                                  */
/************************************************************************/
#define SER_IOCTL_OUT_SET       0       /* Set communication parameters */
#define SER_IOCTL_OUT_PUTREG    1       /* Change register */
#define SER_IOCTL_OUT_BLOCK     2       /* Set blocking */
#define SER_IOCTL_OUT_FLUSH     3       /* Flush buffers */
#define SER_IOCTL_OUT_DTR_ON    4       /* Turn DTR on */
#define SER_IOCTL_OUT_DTR_OFF   5       /* Turn DTR off */
#define SER_IOCTL_OUT_RTS_ON    6       /* Turn RTS on */
#define SER_IOCTL_OUT_RTS_OFF   7       /* Turn RTS off */
#define SER_IOCTL_OUT_INTFUNC   8       /* User Interrupt Function */
#define SER_IOCTL_OUT_HBLOCK    9       /* Set blocking 16bit */
#define SER_IOCTL_OUT_ASET      10      /* Set advance comm parameters */
#define SER_IOCTL_OUT_ORAW      11      /* Set raw mode */

/************************************************************************/
/*=====Structure for Initializing COM port through IOCTL                */
/************************************************************************/
union ser_ioctl_out
{
struct ser_ioctl_out_sets
	{
	unsigned char   type;           /*Function type*/
	unsigned char   baud;           /*baud rate index*/
	unsigned char   parity;         /*parity index*/
	unsigned char   lngth;          /*length index*/
	unsigned char   stopbit;        /*stopbit index*/
	unsigned char   brk;            /*Break*/
	unsigned char   protocol;       /*protocol index*/
	unsigned char   block[2];       /*# of 1/18.2 secs count*/
	} set;
struct ser_ioctl_out_putregs
	{
	unsigned char   type;           /*Function type*/
	unsigned char   reg;            /*Register to change*/
	unsigned char   value;          /*Value to stuf */
	} putreg;
struct ser_ioctl_out_blks
	{
	unsigned char   type;           /*Function type*/
	unsigned char   block[2];       /*Blocking counts*/
	} blk;
struct ser_ioctl_out_flshs
	{
	unsigned char   type;           /*Function type*/
	unsigned char   flush_buf;      /*Buffer to flush */
	} flsh;
struct ser_ioctl_out_dtron
	{
	unsigned char   type;           /*Function type*/
	} dtr_on;
struct ser_ioctl_out_dtroff
	{
	unsigned char   type;           /*Function type*/
	} dtr_off;
struct ser_ioctl_out_rtson
	{
	unsigned char   type;           /*Function type*/
	} rts_on;
struct ser_ioctl_out_rtsoff
	{
	unsigned char   type;           /*Function type*/
	} rts_off;
struct ser_ioctl_out_intfunc
	{
	unsigned char   type;           /*Function type*/
	unsigned char   filler;
	CDRVIFNC (FAR *(FAR *func))(short val,struct port_param FAR *p);
	short   imask;
	} intfunc;
struct ser_ioctl_out_hblks
	{
	unsigned char   type;           /*Function type*/
	unsigned char   filler;
	unsigned int    block[2];       /*Blocking counts*/
	} hblk;
struct ser_ioctl_out_asets
	{
	unsigned char   type;           /*Function type*/
	unsigned char   subtype;        /* =0 */
	unsigned char   baud;           /*baud rate index*/
	unsigned char   parity;         /*parity index*/
	unsigned char   lngth;          /*length index*/
	unsigned char   stopbit;        /*stopbit index*/
	unsigned char   brk;            /*Break*/
	unsigned char   protocol;       /*protocol index*/
	unsigned short  block[2];       /*# of 1/18.2 secs count*/
	unsigned char   xon;            /* XON */
	unsigned char   xoff;           /* XOFF */
	unsigned char   xxon;           /* XON xmit */
	unsigned char   xxoff;          /* XOFF xmit */
	unsigned short  spare[19];      /* Spares */
	} aset;
struct ser_ioctl_out_oraw
	{
	unsigned char   type;           /*Function type*/
	unsigned char   flag;           /* Flag */
	} oraw;
};

/************************************************************************/
/*=====Structure for reading status of COM port through IOCTL           */
/************************************************************************/
union ser_ioctl_in
{
struct ser_rs232_routines       FAR *ser_rs232; /*Ptr to routine ptrs*/
struct port_param               port;           /* Port param block */
};

/************************************************************************/
/*=====Offsets to functions in direct address mapping table of ptrs     */
/************************************************************************/
#define CLEANUP         0
#define GETBYTE         4
#define GETPORT         8
#define GETREGISTER     12
#define GETSTATUS       16
#define PUTBYTE         20
#define PUTREGISTER     24
#define MAXPORT         28
#define SETUP           32
#define PUTPACKET       36
#define GETPACKET       40
#define FLUSH           44
#define BLCK            48
#define DTR_ON          52
#define DTR_OFF         56
#define RTS_ON          60
#define RTS_OFF         64
#define SET_INTFUNC     68
#define GET_SDATA       72
#define MISC_FUNC       76
#define SETBAUDDIV      80
#define VIEWPACKET      84

/************************************************************************/
/*=====Structure of ptrs to serial routines in COMMTSR                  */
/************************************************************************/
struct ser_rs232_routines
{
int (FAR *cleanup)(int port);
int (FAR *getbyte)(int port,unsigned char FAR *ch);
int (FAR *getport)(int port,struct port_param FAR *pcb);
int (FAR *getregister)(int port,int offset,int FAR *value);
int (FAR *getstatus)(int port,unsigned char FAR *stat);
int (FAR *putbyte)(int port,unsigned char FAR *ch);
int (FAR *putregister)(int port,int offset,int value);
int (FAR *maxport)(void);
int (FAR *setup)(int port,struct port_param FAR *pcb);
int (FAR *putpacket)(int port,int count,unsigned char FAR *ch);
int (FAR *getpacket)(int port,int count,unsigned char FAR *ch);
int (FAR *flush)(int port,int arg);
int (FAR *block)(int port,int input,int output);
int (FAR *dtr_on)(int port);
int (FAR *dtr_off)(int port);
int (FAR *rts_on)(int port);
int (FAR *rts_off)(int port);
int (FAR *set_intfunc)(int port,
CDRVIFNC (FAR *(FAR *func))(short val,struct port_param FAR *p),
short imask);
struct ser_rs232_sdata FAR * (FAR *get_sdata)(void);
int (FAR *misc_func)(int port,unsigned short cmd,unsigned long val);
int (FAR *setbauddiv)(int cardtype,int baudidx,unsigned bauddiv);
int (FAR *viewpacket)(int port,int count,unsigned char FAR *ch);
};

/************************************************************************/
/*=====INT14 PCB                                                        */
/************************************************************************/
struct ser_int14_pcb
{
int     ser_int14_flag;         /* Flags */
};

/************************************************************************/
/*======Int14 handler information record.                               */
/************************************************************************/
struct ser_int14_info
{
char                            hdrname[9+1];   /* Name of handler */
struct ser_rs232_routines FAR   *ser_rs232;     /*Ptr to routine ptrs*/
long                            oldvec;         /* Old INT14H vector */
};

/************************************************************************/
/*=====Control block used by ser1prim.c.                                */
/************************************************************************/
struct commdrv_ctl
{
unsigned int    flag;                           /* Flag */
#define COMMDRV_PORT_OPEN       0x0001          /* Port was opened */
int             h;                              /* Handle */
unsigned char   *buf;                           /* Pointer to buf for data */
char            *dev;                           /* Pointer to device name */
};

/************************************************************************/
/*=====Communication Device header structure(COMM-DRV/Dos)              */
/************************************************************************/
struct  dev_head
{
unsigned FAR    *next_dev;              /* Pointer to next device */
unsigned        attribute;              /* Type of device */
unsigned        strategy;               /* Pointer to strategy routine */
unsigned        interrpt;               /* Pointer to interrupt routine */
char            dev_name[8];            /* Device name */
unsigned        com_id;                 /* Port id */
unsigned        rh_seg;                 /* Request Header segment */
unsigned        rh_off;                 /* Request Header offset */
unsigned        device_flag;            /* Local device flag */
unsigned char   ahead_byte;             /* Look ahead byte */
unsigned char   dev_flag;               /*  */
};

/************************************************************************/
/*=====File transfer definitions                                        */
/************************************************************************/
#ifdef MSWIN32
#define	XFRTRES(T55)	(((T55)*55)/bio_get_timerresolution())
#else
#define	XFRTRES(T55)	(T55)
#endif

/* Definitions */
#define CDRVXFER_SOH    '\001'
#define CDRVXFER_STX    '\002'
#define CDRVXFER_EOT    '\004'
#define CDRVXFER_ACK    '\006'
#define CDRVXFER_NAK    '\025'
#define CDRVXFER_CAN    '\030'
#define CDRVXFER_CC     '\103'
#define CDRVXFER_G      '\107'
#define CDRVXFER_XON    '\021'
#define CDRVXFER_XOFF   '\023'
#define CDRVXFER_DEL    '\177'

/* Zmodem special packet characters */
#define CDRVXFER_ZPAD   '*'             /* 052 Pad character begins frames */
#define CDRVXFER_ZDLE   0x18            /* Ctrl-X Zmodem escape */
#define CDRVXFER_ZDLEE  (0x18^0x40)     /* Escaped ZDLE as transmitted */
#define CDRVXFER_ZBIN   'A'             /* Binary frame indicator */
#define CDRVXFER_ZHEX   'B'             /* HEX frame indicator */
#define CDRVXFER_ZBIN32 'C'             /* Binary frame with 32 bit FCS */

/* ZDLE escape pairs */
#define CDRVXFER_ZCRCE 'h'      /* CRC next, frame ends, header pkt follows */
#define CDRVXFER_ZCRCG 'i'      /* CRC next, frame continues nonstop */
#define CDRVXFER_ZCRCQ 'j'      /* CRC next, frame continues, ZACK expected */
#define CDRVXFER_ZCRCW 'k'      /* CRC next, ZACK expected, end of frame */
#define CDRVXFER_ZRUB0 'l'      /* Translate to rubout 0177 */
#define CDRVXFER_ZRUB1 'm'      /* Translate to rubout 0377 */

/* Bit Masks for ZRINIT first flag byte(flag0=offset+3,flag1=offset+2,...  */
#define CDRVXFER_CANFDX         01      /* Rx can snd and rcv true FDX */
#define CDRVXFER_CANOVIO        02      /* Rx can rcv data during disk I/O */
#define CDRVXFER_CANBRK         04      /* Rx can snd a break signal */
#define CDRVXFER_CANCRY         010     /* Rx can decrypt */
#define CDRVXFER_CANLZW         020     /* Rx can uncompress */
#define CDRVXFER_CANFC32        040     /* Rx can use 32 bit Frame Check */
#define CDRVXFER_ESCCTL         0100    /* Rx expects ctl chars to be escaped */
#define CDRVXFER_ESC8           0200    /* Rx expects 8th bit to be escaped */

/* Byte positions within header array */
#define CDRVXFER_ZF0    3               /* First flags byte */
#define CDRVXFER_ZF1    2
#define CDRVXFER_ZF2    1
#define CDRVXFER_ZF3    0
#define CDRVXFER_ZP0    0               /* Low order 8 bits of position */
#define CDRVXFER_ZP1    1
#define CDRVXFER_ZP2    2
#define CDRVXFER_ZP3    3               /* High order 8 bits of file position */

/* Parameters for ZSINIT frame */
#define CDRVXFER_ZATTNLEN 32    /* Max length of attention string */
/* Bit Masks for ZSINIT flags byte ZF0 */
#define CDRVXFER_TESCCTL 0100   /* Trans expects ctl chars to be escaped */
#define CDRVXFER_TESC8   0200   /* Trans expects 8th bit to be escaped */

/* Parameters for ZFILE frame */
/* Conversion options one of these in ZF0 */
#define CDRVXFER_ZCBIN          1       /* Binary xfer - inhibit conversion */
#define CDRVXFER_ZCNL           2       /* Convert NL to local eol convention */
#define CDRVXFER_ZCRESUM        3       /* Resume interrupted file transfer */
/* Management include options, one of these ored in ZF1 */
#define CDRVXFER_ZMSKNOLOC      0200    /* Skip file if not present at rx */
/* Management options, one of these ored in ZF1 */
#define CDRVXFER_ZMMASK         037     /* Mask for the choices below */
#define CDRVXFER_ZMNEWL         1       /* Xfer if source newer or longer */
#define CDRVXFER_ZMCRC          2       /* Xfer if diff file CRC or length */
#define CDRVXFER_ZMAPND         3       /* Append contents to existing file */
#define CDRVXFER_ZMCLOB         4       /* Replace existing file */
#define CDRVXFER_ZMNEW          5       /* Transfer if source newer */
/* Number 5 is alive ... */
#define CDRVXFER_ZMDIFF         6       /* Xfer if dates or lengths different */
#define CDRVXFER_ZMPROT         7       /* Protect destination file */
/* Transport options, one of these in ZF2 */
#define CDRVXFER_ZTLZW          1       /* Lempel-Ziv compression */
#define CDRVXFER_ZTCRYPT        2       /* Encryption */
#define CDRVXFER_ZTRLE          3       /* Run Length encoding */
/* Extended options for ZF3, bit encoded */
#define CDRVXFER_ZXSPARS        64      /* Encod for sparse file operations */

/* Parameters for ZCOMMAND frame ZF0 (otherwise 0) */
#define CDRVXFER_ZCACK1         1       /* Acknowledge, then do command */

/* Zmodem frame types */
#define CDRVXFER_ZRQINIT        0       /* Request receive init */
#define CDRVXFER_ZRINIT         1       /* Receive init */
#define CDRVXFER_ZSINIT         2       /* Send init sequence (optional) */
#define CDRVXFER_ZACK           3       /* ACK to above */
#define CDRVXFER_ZFILE          4       /* File name from sender */
#define CDRVXFER_ZSKIP          5       /* To sender: skip this file */
#define CDRVXFER_ZNAK           6       /* Last packet was garbled */
#define CDRVXFER_ZABORT         7       /* Abort batch transfers */
#define CDRVXFER_ZFIN           8       /* Finish session */
#define CDRVXFER_ZRPOS          9       /* Resume data trans at this position */
#define CDRVXFER_ZDATA          10      /* Data packet(s) follow */
#define CDRVXFER_ZEOF           11      /* End of file */
#define CDRVXFER_ZFERR          12      /* Fatal Read or Write error Detected */
#define CDRVXFER_ZCRC           13      /* Request for file CRC and response */
#define CDRVXFER_ZCHALLENGE     14      /* Receiver's Challenge */
#define CDRVXFER_ZCOMPL         15      /* Request is complete */
#define CDRVXFER_ZCAN           16      /* Other end canned session w/CAN*5 */
#define CDRVXFER_ZFREECNT       17      /* Req for free bytes on filesystem */
#define CDRVXFER_ZCOMMAND       18      /* Command from sending program */
#define CDRVXFER_ZSTDERR        19      /* Output to stdd error, data follows */

/* Lengths */
#ifdef	MSWIN32
#define LEN_FSPEC       992
#else
#define LEN_FSPEC       255
#endif

/* States */
#define CDRVXFER_RSTATE_INIT            0
#define CDRVXFER_RSTATE_BEGIN           1
#define CDRVXFER_RSTATE_SENDSYNC        2
#define CDRVXFER_RSTATE_GETCHAR         3
#define CDRVXFER_RSTATE_SENDACK         4
#define CDRVXFER_RSTATE_SENDNAK         5
#define CDRVXFER_RSTATE_ZRINIT          6
#define CDRVXFER_RSTATE_SENDHEADER      7
#define CDRVXFER_RSTATE_WZPKT01         8

/* Sub-States */
#define CDRVXFER_RSUBSTATE_GETFRAME     20
#define CDRVXFER_RSUBSTATE_GETBLKBYT1   21
#define CDRVXFER_RSUBSTATE_GETBLKBYT2   22
#define CDRVXFER_RSUBSTATE_GETDATA      23
#define CDRVXFER_RSUBSTATE_GETCRC1      24
#define CDRVXFER_RSUBSTATE_GETCRC2      25
#define CDRVXFER_RSUBSTATE_SENDHDR      26
#define CDRVXFER_RSUBSTATE_SENDDATA     27

/* Sending States */
#define CDRVXFER_SSTATE_INIT            40
#define CDRVXFER_SSTATE_BEGIN           41
#define CDRVXFER_SSTATE_RECVSYNC        42
#define CDRVXFER_SSTATE_SENDPACKET      43
#define CDRVXFER_SSTATE_SENDFINFO       44
#define CDRVXFER_SSTATE_SENDHEADER      45
#define CDRVXFER_SSTATE_READBLOCK       46
#define CDRVXFER_SSTATE_REPLY           47
#define CDRVXFER_SSTATE_EOT             48
#define CDRVXFER_SSTATE_ZRQINIT         49
#define CDRVXFER_SSTATE_WZPKT01         50
#define CDRVXFER_SSTATE_STARTSPACKET    51
#define CDRVXFER_SSTATE_SENDBFRAME      52
#define CDRVXFER_SSTATE_WZPKT02         53

/* Substates */
#define CDRVXFER_SSUBSTATE_PUTFRAME     60
#define CDRVXFER_SSUBSTATE_PUTBLKBYT1   61
#define CDRVXFER_SSUBSTATE_PUTBLKBYT2   62
#define CDRVXFER_SSUBSTATE_PACKETSENT   63
#define CDRVXFER_SSUBSTATE_EOTSENT      64

/* Error code */
#define CDRVXFER_ERR_OK         0       /* Complete without errors */
#define CDRVXFER_ERR_NONE       0       /* Complete without errors */
#define CDRVXFER_ERR_SERIAL     1       /* Some serial I/O error */
#define CDRVXFER_ERR_OS         2       /* MSDOS Error */
#define CDRVXFER_ERR_PROTOCOL   3       /* Protocol Error */
#define CDRVXFER_ERR_BUSY       4       /* Protocol engine still busy */
#define CDRVXFER_ERR_CANCEL     5       /* Cancel terminated */
#define CDRVXFER_ERR_INVPORT    6       /* Invalid Port */
#define CDRVXFER_ERR_NOPCB      7       /* No more PCBs */
#define CDRVXFER_ERR_MTASK      8       /* MTASK error */
#define CDRVXFER_ERR_ACTIVE     9       /* XFER on port still active */
#define CDRVXFER_ERR_FNAME      10      /* Invalid filename */
#define CDRVXFER_ERR_KILLED     11      /* Transfer killed */
#define CDRVXFER_ERR_MEMORY     12      /* Memory allocation error */
#define CDRVXFER_ERR_NOCD       13      /* No Carrier Detect */
#define CDRVXFER_ERR_TIMEOUT    14      /* Timeout receiving file */


/* X(Y)MODEM Definitions */
#define CDRVXFER_MAXGRETRY      2
#define CDRVXFER_MAXCRETRY      5
#define CDRVXFER_MAXRETRY       10
#define CDRVXFER_MAXBRETRY      10
#define CDRVXFER_MAXTIMEOUT     180
#define CDRVXFER_MAXPAUSE       90

/* ZMODEM Definitions */
#define CDRVXFER_MAXORETRY      10      /* Max number of char output retries */
#define CDRVXFER_MAXOTIMOUT     180     /* Max time for char output */
#define CDRVXFER_MAXIRETRY      10      /* Max number of char input retries */
#define CDRVXFER_MAXITIMOUT     540     /* Max time for char input */
#define CDRVXFER_MAXZRETRY      10

/************************************************************************/
/*=====File find structure                                              */
/************************************************************************/
struct cdrvxfer_fileinfo
{
unsigned                time;
unsigned                date;
long                    size;
char FAR                *fspec;
struct dos_find_struct  fbuf;
};


/************************************************************************/
/*=====File transfer progress structure                                 */
/************************************************************************/
struct cdrvxfer_info 
{
char	*fname;
int	curblk;
long	numbytes;
long	offsetbytes;
int	totalerr;
int     currenterr;
long	filesize;
};


/************************************************************************/
/*=====File transfer control structure                                  */
/************************************************************************/
struct cdrvxfer_ftpcb
{
/* User Provided */
char                    fspec[LEN_FSPEC+1];
int                     port;
int                     state;
unsigned char FAR       *buf;
unsigned int            bufsiz;
int                     retrycnt;
int                     rcvtimeout;
int                     xmttimeout;

/* Xfer Progress Info */
int                     curblk;
int                     blksiz;
long                    numbytes;
long                    offsetbytes;
int                     error;
int                     exterror;
int                     totalerr;
int                     currenterr;

/* System Usage */
int                     fhandle;
int                     saveerror;
int                     deltaerr;
int                     substate;
int                     blkidx;
int                     retry;
int                     crcretry;
int                     gretry;
int                     bretry;
#define CDRVXFER_FLAG_NOTCRC    0x00000001L
#define CDRVXFER_FLAG_SYNCED    0x00000002L
#define CDRVXFER_FLAG_YMODEM    0x00000004L
#define CDRVXFER_FLAG_YMODEMG   0x00000008L
#define CDRVXFER_FLAG_SFIRST    0x00000010L
#define CDRVXFER_FLAG_SCNVHEX   0x00000020L
#define CDRVXFER_FLAG_SDOCRC    0x00000040L
#define CDRVXFER_FLAG_SCRC32    0x00000080L
#define CDRVXFER_FLAG_SCRC      0x00000100L
#define CDRVXFER_FLAG_RFIRST    0x00000200L
#define CDRVXFER_FLAG_RNHEX     0x00000400L
#define CDRVXFER_FLAG_RNOREAD   0x00000800L
#define CDRVXFER_FLAG_RCRC      0x00001000L
#define CDRVXFER_FLAG_RZDLE     0x00002000L
#define CDRVXFER_FLAG_RESC      0x00004000L
#define CDRVXFER_FLAG_NEXTFILE  0x00008000L
#define CDRVXFER_FLAG_NORHEAD   0x00010000L
#define CDRVXFER_FLAG_SENDING   0x00020000L
#define CDRVXFER_FLAG_SESC      0x00040000L
#define CDRVXFER_FLAG_SHDX      0x00080000L
#define CDRVXFER_FLAG_SCANDO32  0x00100000L
#define CDRVXFER_FLAG_RECOVER   0x00200000L
#define CDRVXFER_FLAG_OVERWRITE 0x00400000L
#define CDRVXFER_FLAG_RENAME    0x00800000L
#define CDRVXFER_FLAG_SKIPSAME  0x01000000L
#define CDRVXFER_FLAG_RCVACK    0x02000000L
#define CDRVXFER_FLAG_ZRPOS	0x40000000L
#define CDRVXFER_FLAG_FILEDONE  0x80000000L
unsigned long                   flag;
unsigned short                  itimer_buf[6];
unsigned short                  otimer_buf[6];
unsigned short                  ftimer_buf[6];
long                            elapsed;
unsigned char                   blockn[2];
unsigned char                   crc[2];
unsigned short                  ccrc;
unsigned int                    flag2;
unsigned int                    zconv;
unsigned int                    zmanag;
unsigned int                    ztrans;
unsigned char                   zfe;
unsigned char                   tzfe;
unsigned long                   ocrc32;
unsigned int                    xmtcnt;
unsigned int                    xmtidx;
unsigned char FAR               *xmtdat;
unsigned char                   xmtopkt[2];
unsigned int                    xmtocnt;
unsigned int                    xmtoidx;
unsigned char                   xmtpkt[20];
unsigned int                    xmtpcnt;
unsigned int                    xmtpidx;
unsigned char FAR               *xmtpdat;
unsigned int                    xmtncnt;
unsigned int                    xmtnidx;
unsigned char FAR               *xmtndat;
unsigned int                    txmtcnt;
unsigned int                    txmtidx;
unsigned char                   txmtpkt[20];
unsigned long                   icrc32;
unsigned int                    crcstat;
unsigned int                    lasthdr;
unsigned int                    rcvstate;
unsigned int                    rcvsubstate;
unsigned int                    rcvcancel;
unsigned int                    rcvhtype;
unsigned int                    rcvcnt;
unsigned char FAR               *rcvdat;
unsigned int                    rcvidx;
unsigned char                   rcvpkt[20];
unsigned int                    trcvidx;
unsigned int                    xbufcnt;
unsigned int                    xbufidx;
unsigned char                   xbuf[50];
unsigned int                    rbufcnt;
unsigned int                    rbufidx;
unsigned char                   rbuf[50];
struct cdrvxfer_fileinfo        f;
struct port_param               pcb;
};

/************************************************************************/
/*=====Definition for file transfer dialog box(FileTransferDialog).     */
/************************************************************************/
#define CDRVXFER_NAME           0x651
#define CDRVXFER_BYTECOUNT      0x652
#define CDRVXFER_ERRORCOUNT     0x653
#define CDRVXFER_RATE           0x654
#define CDRVXFER_FSIZ           0x655

/************************************************************************/
/*=====Definitions for the SetXferParameters() function                 */
/************************************************************************/
#define CDRVXFER_RCVFILEOPTIONS 0
#define CDRVXFER_PARM_RECOVER   CDRVXFER_FLAG_RECOVER
#define CDRVXFER_PARM_OVERWRITE CDRVXFER_FLAG_OVERWRITE
#define CDRVXFER_PARM_RENAME    CDRVXFER_FLAG_RENAME
#define CDRVXFER_PARM_SKIPSAME  CDRVXFER_FLAG_SKIPSAME
#define CDRVXFER_RCVRETRYCNT    1
#define CDRVXFER_RCVITIMER      2
#define CDRVXFER_RCVOTIMER      3
#define CDRVXFER_XMTFILEOPTIONS 4
#define CDRVXFER_XMTRETRYCNT    5
#define CDRVXFER_XMTITIMER      6
#define CDRVXFER_XMTOTIMER      7
#define CDRVXFER_MONITORCD      8

/************************************************************************/
/*=====Definitions for the SetSpecialBehavior() function                */
/************************************************************************/
#define SB_SetGetStringDelimiter        0       /* Set String Delimiters */
#define SB_DoNotCall2ndMsgLoop          1       /* No Secondary loop */
#define SB_DisableTiming                2       /* Don't use timer */
#define SB_SwitchOnDelay                3       /* Context switch on delay */
#define SB_SleepOnDelay			4       /* Sleep on delay */

/************************************************************************/
/*======File Custom file transfer definitions                           */
/************************************************************************/
#define SER2XFER_ERR_NONE       0       /* No error */
#define SER2XFER_ERR_SERIAL     -1      /* Error opening serial port */
#define SER2XFER_ERR_RFOPEN     -2      /* Remote file open error */
#define SER2XFER_ERR_LFOPEN     -3      /* Local file open error */
#define SER2XFER_ERR_RFREAD     -4      /* Remote file read error */
#define SER2XFER_ERR_LFREAD     -5      /* Local file read error */
#define SER2XFER_ERR_RFWRITE    -6      /* Remote file write error */
#define SER2XFER_ERR_LFWRITE    -7      /* Local file write error */
#define SER2XFER_ERR_TIMEOUT    -8      /* Remote timeout */
#define SER2XFER_ERR_CRC        -9      /* CRC error */
#define SER2XFER_ERR_SYNTAX     -10     /* Syntax error */
#define SER2XFER_ERR_CLEANUP    -11     /* Serial port cleanup failed */
#define SER2XFER_ERR_RFCLOSE    -12     /* Remote file close error */
#define SER2XFER_ERR_LFCLOSE    -13     /* Local file close error */
#define SER2XFER_ERR_BUFOVR     -14     /* Buffer overrun */
#define SER2XFER_ERR_NOFILE     -15     /* No more files */
#define SER2XFER_ERR_DRIVER     -16     /* Error initializing driver */
#define SER2XFER_ERR_SEQ        -17     /* Out of sequence error */

#define SER2XFER_RETRY          3
#define SER2XFER_PACKET_LEN     256-sizeof(PacketH)     /* Packet length */

/************************************************************************/
/*=====Custom Xfer packet structure                                     */
/************************************************************************/
typedef struct
	{
	unsigned short  crc;            /* CRC of packet starting at "len" */
	unsigned short  len;            /* Length of data portion of packet */
	unsigned short  seq;            /* Packet sequence number */
	unsigned short  date;           /* File date  */
	unsigned short  time;           /* File time  */
#define SER2XFER_OPEN_READ      0       /* Open file for read slave command */
#define SER2XFER_OPEN_WRITE     1       /* Open file for write slave command */
#define SER2XFER_CLOSE          2       /* Close file slave command */
#define SER2XFER_READ           3       /* Read file slave command */
#define SER2XFER_WRITE          4       /* Write file slave command */
#define SER2XFER_FINDFIRST      5       /* Find first matching file slave cmd*/
#define SER2XFER_FINDNEXT       6       /* Find next matching file slave cmd*/
	unsigned short  cmd;            /* Slave command variable */
	short           stat;           /* Status of slave command */
	} PacketH;

typedef struct
{
PacketH         head;                           /* Packet header */
unsigned char   data[SER2XFER_PACKET_LEN];      /* Packet data buffer */
} Packet;

/************************************************************************/
/*=====COMMDRV screen sub-function equates                              */
/************************************************************************/
#define COMMDRVW_CHARSCREENINIT         0
#define COMMDRVW_CHARSCREENRESIZE       1
#define COMMDRVW_CHARSCREENWRITE        2
#define COMMDRVW_CHARSCREENPAINT        3
#define COMMDRVW_CHARSCREENVSCROLL      4
#define COMMDRVW_CHARSCREENHSCROLL      5
#define COMMDRVW_CHARSCREENSETFOCUS     6
#define COMMDRVW_CHARSCREENKILLFOCUS    7
#define COMMDRVW_CHARSCREENCLEANUP      8

#define COMMDRVW_MAXROW                 25
#define COMMDRVW_MAXCOL                 80

#ifdef  MSWIN
/****************************************************************/
/*=====CDRVRMAP & WINCDRV definitions                           */
/****************************************************************/
/* Number of ports reserverd for old comm.drv */
#define OLDCNT  4

/* Menu items */
#define CDRVRMAP_MENU_ABOUT     (WM_USER+1)
#define CDRVRMAP_MENU_REMAP     (WM_USER+2)

#define CDRVRMAP_CID_UP         (WM_USER+50)
#define CDRVRMAP_CID_DOWN       (WM_USER+51)
#define CDRVRMAP_CID_TEXT       (WM_USER+52)
#define CDRVRMAP_DOIT           (WM_USER+53)
#define CDRVRMAP_PORT_UP        (WM_USER+54)
#define CDRVRMAP_PORT_DOWN      (WM_USER+55)
#define CDRVRMAP_PORT_TEXT      (WM_USER+56)
/****************************************************************/
#endif

/****************************************************************/
/****************************************************************/
/*=====COMMDRV Specific VxD(WCSCCDRV.386)                       */
/****************************************************************/
/****************************************************************/
#define WCSCCDRV_Dev_ID         0x3229          /* Device ID */

#define VACTIVE_FLG             0x00000001L     /* */
#define VDISABLE_MODEMSIG       0x00000002L     /* */
#define VIGNORE_16550           0x00000004L     /* */
#define VNOCHANGE_MODEMSIG      0x00000008L     /* */
#define VLEAVE_RTS              0x00000010L     /* */
#define VLEAVE_DTR              0x00000020L     /* */
#define VXMTOFF_STATE           0x00000040L     /* */
#define VRCVOFF_STATE           0x00000080L     /* */
#define VSEND_XON               0x00000100L     /* */
#define VSEND_XOFF              0x00000200L     /* */
#define VSTOREALL               0x00000400L     /* */
#define VPARENTPORT             0x00000800L     /* */
#define VNINEBITTOGGLE          0x00001000L     /* */
#define VNINEBITTOGGLE2         0x00002000L     /* */
#define VNINEBITPROTOCOL        0x00004000L     /* */
#define VMULTIDROP              0x00008000L     /* */
#define VTIMESTAMPBYTES         0x00010000L     /* */
#define VLSRINT                 0x00020000L     /* */
#define VRCVINT                 0x00040000L     /* */
#define VXMTINT                 0x00080000L     /* */
#define VMSRINT                 0x00100000L     /* */
#define VPCIFLAG                0x00200000L     /* */

/************************************************************************/
/*=====VxD/Kernel Driver Events						*/
/************************************************************************/
#define VXD_MAX_EVENT	  	13	/* Max event number */
#define VXD_EVENT_MODEM		0	/* Modem intrpt */
#define VXD_EVENT_LSTAT		1	/* Status intrpt */
#define VXD_EVENT_RCV		2	/* Receive intrpt */
#define VXD_EVENT_XMT		3	/* Xmit interpt */
#define VXD_EVENT_RCVCHR	4	/* Receive special char */
#define VXD_EVENT_CTSON		5	/* CTS On */
#define VXD_EVENT_CTSOFF	6	/* CTS Off */
#define VXD_EVENT_DSRON		7	/* DSR On */
#define VXD_EVENT_DSROFF	8	/* DSR Off */
#define	VXD_EVENT_CDON		9	/* Carrier Detect ON */
#define	VXD_EVENT_CDOFF		10	/* Carrier Detect OFF */
#define	VXD_EVENT_RION		11	/* Ring Indicator ON */
#define	VXD_EVENT_RIOFF		12	/* Ring Indicator OFF */
#define	VXD_EVENT_BREAK		13	/* Break detected */

/************************************************************************/
/*=====VxD Port Control Structure                                       */
/*      (VPCBIdx - VTotalIn) == Portion returned for partial            */
/*      structure read.                                                 */
/************************************************************************/
#ifdef MSNTDRV
#pragma pack(push)
#pragma pack(1)
#endif
struct vxd_port_param
{
unsigned long   VTotalIn;       /*Number of characters received.        */
unsigned long   VTotalOut;      /*Number of characters transmitted.     */
unsigned long   VInBufCnt;      /*Input buffer count                    */
unsigned long   VOutBufCnt;     /*Output buffer count                   */
unsigned long   VIRingSize;     /*Size of input buffer                  */
unsigned long   VORingSize;     /*Size of output buffer                 */
unsigned long   VMSRIntCnt;     /*Number of MSR ints                    */
unsigned long   VLSRIntCnt;     /*Number of LSR ints                    */
unsigned long   VXmtIntCnt;     /*Number of XMT ints                    */
unsigned long   VRcvIntCnt;     /*Number of RCV ints                    */
unsigned long   VRcvLostCnt;    /*Number of received characters lost.   */
unsigned long   VXmtLostCnt;    /*Number of transmit characters lost.   */
unsigned long   VOFlushCnt;     /*Output flush count                    */
unsigned long   VIFlushCnt;     /*Input flush count                     */
unsigned char   VMSRReg;        /*Current modem status register         */
unsigned char   VLSRReg;        /*Current line status register          */
unsigned char   VTypeUART;      /*Type of UART                          */
unsigned char   VFillerb1;      /*Filler                                */
unsigned short  VPort;          /*COMM-DRV port number                  */
unsigned short  VBase;          /*Base address                          */
unsigned short  VIrq;           /*IRQ                                   */
unsigned short  VBaud;          /*Baud rate                             */
unsigned short  VBreak;         /*Break                                 */
unsigned short  VLength;        /*Length                                */
unsigned short  VParity;        /*Parity                                */
unsigned short  VStopbit;       /*Stop bits                             */
unsigned short  VProtocol;      /*Protocol                              */
unsigned short  VCardseg;       /*Card segment                          */
unsigned char   VCharXon;       /*XON character to expect from remote   */
unsigned char   VCharXoff;      /*XOFF character to expect from remote  */
unsigned char   VCharXxon;      /*XON character to send to remote       */
unsigned char   VCharXxoff;     /*XOFF character to send to remote      */
unsigned long   VFlag;          /*Flag                                  */
unsigned long   VCurOverrunCnt; /*Ovrrun err cnt                        */
unsigned long   VCurParityCnt;  /*Parity err cnt                        */
unsigned long   VCurFramingCnt; /*Framing err cnt                       */
unsigned long   VCurBreakCnt;   /*Break err cnt                         */
unsigned long   VBytesRead;     /*Number of bytes read from VxD         */
unsigned long   VBytesWrite;    /*Number of bytes written to VxD        */
unsigned long   VXonIn;         /*Number of XON received                */
unsigned long   VXoffIn;        /*Number of XOFF received               */
unsigned long   VXonOut;        /*Number of XON sent                    */
unsigned long   VXoffOut;       /*Number of XOFF sent                   */
unsigned short	VDevBase;	/*Device Relative Base Address entered  */
unsigned short	VDevIrq;	/*Device RelativeIRQ Entered            */
unsigned short	VDData;		/*Actual DData entered                  */
unsigned short	VDFlag;		/*Actual DFlag Entered                  */
unsigned short	VSubPort;	/*Actual Subport Entered		*/
unsigned short	VDevBus;	/*Device Bus                            */
unsigned long	VIrqPCB;	/*Pointer to IRQ PCB                    */
unsigned long   VDevSlot;	/*Device Slot                           */
unsigned short	VDevLevel;	/*Irq level                             */
unsigned short	VFillerb2a;	/*Room for growth                       */
unsigned long   VFillerb2[4];	/*Room for growth                       */

unsigned short  VPCBIdx;        /*Index to this PCB                     */
unsigned short  VXmtCnt;        /*Number of bytes to xmit at once       */
unsigned long   VORingTop;      /*Start of output buffer ring           */
unsigned long   VORingBottom;   /*Bottom of output buffer ring+1        */
unsigned long   VORingInput;    /*Current input pointer to output buf   */
unsigned long   VORingOutput;   /*Current output pointer to output buf  */
unsigned long   VIRingTop;      /*Start of input buffer ring            */
unsigned long   VIRingBottom;   /*Bottom of input buffer ring+1         */
unsigned long   VIRingInput;    /*Current input pointer to input buf    */
unsigned long   VIRingOutput;   /*Current output pointer to input buf   */
unsigned long   VProtLocal;     /*Local protocol index                  */
unsigned long   VProtRemote;    /*Remote protocol index                 */
unsigned long   VInbufHigh;     /*Input buffer high threshold           */
unsigned long   VInbufLow;      /*Input buffer high threshold           */
unsigned long   VNextPCBPtr;    /*Pointer to next PCB                   */
unsigned char   VOldBaudReg[2]; /*Old Baud Rate Register                */
unsigned char   VOldMCRReg;     /*Old Modem Control Register            */
unsigned char   VOldLCRReg;     /*Old Line Control Register             */
unsigned char   VOldFCRReg;     /*Old FIFO Control Register             */
unsigned char   VOldIERReg;     /*Old Interrupt Enable Register         */
unsigned char   VFillerb3[2];   /*Filler(Complete 32bit boundary)       */
unsigned long   VCtsOn;         /*# of Cts On transition                */
unsigned long   VCtsOff;        /*# of Cts Off transition               */
unsigned long   VCdOn;          /*# of Cd On transition                 */
unsigned long   VCdOff;         /*# of Cd Off transition                */
unsigned long   VRiOn;          /*# of Ri On transition                 */
unsigned long   VRiOff;         /*# of Ri Off transition                */
unsigned long   VDsrOn;         /*# of Dsr On transition                */
unsigned long   VDsrOff;        /*# of Dsr Off transition               */
unsigned long   VFillerb4[2];   /*Room for growth                       */
unsigned long   VPPCB;          /*Parent PCB                            */
};
#ifdef MSNTDRV
#pragma pack(pop)
#endif

/************************************************************************/
/*=====Index for WCSCCDRV.386 API functions                             */
/************************************************************************/
#define VXDAPI_NT_IOCTL_OFFSET  0x800   /* */
#define VXDAPI_Version          0       /* */
#define VXDAPI_Setup            1       /* */
#define VXDAPI_GetPacket        2       /* */
#define VXDAPI_PutPacket        3       /* */
#define VXDAPI_Cleanup          4       /* */
#define VXDAPI_ViewPacket       5       /* */
#define VXDAPI_Flush            6       /* */
#define VXDAPI_GetPort          7       /* */
#define VXDAPI_GetMetric        8       /* */
#define VXDAPI_ModemCtl         9       /* */
#define VXDAPI_BaudDiv          10      /* */
#define VXDAPI_UARTReg          11      /* */
#define VXDAPI_GetPCIInfo	12      /* */
#define VXDAPI_WaitForEvent	13      /* */
#define VXDAPI_DeleteEvent	14      /* */

#ifdef  MSWIN32
#define ICTL1   FILE_DEVICE_UNKNOWN
#define ICTL2   VXDAPI_NT_IOCTL_OFFSET
#define ICTL3   METHOD_BUFFERED
#define ICTL4   FILE_ANY_ACCESS
#define ICTL(i) CTL_CODE((ICTL1),(ICTL2+i),(ICTL3),(ICTL4))

#define VXDAPI_Version_NT	CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_Setup_NT         CTL_CODE(FILE_DEVICE_UNKNOWN,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_GetPacket_NT     CTL_CODE(FILE_DEVICE_UNKNOWN,0x802,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_PutPacket_NT     CTL_CODE(FILE_DEVICE_UNKNOWN,0x803,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_Cleanup_NT       CTL_CODE(FILE_DEVICE_UNKNOWN,0x804,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_ViewPacket_NT    CTL_CODE(FILE_DEVICE_UNKNOWN,0x805,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_Flush_NT         CTL_CODE(FILE_DEVICE_UNKNOWN,0x806,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_GetPort_NT       CTL_CODE(FILE_DEVICE_UNKNOWN,0x807,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_GetMetric_NT     CTL_CODE(FILE_DEVICE_UNKNOWN,0x808,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_ModemCtl_NT      CTL_CODE(FILE_DEVICE_UNKNOWN,0x809,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_BaudDiv_NT       CTL_CODE(FILE_DEVICE_UNKNOWN,0x80a,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_UARTReg_NT       CTL_CODE(FILE_DEVICE_UNKNOWN,0x80b,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_GetPCIInfo_NT	CTL_CODE(FILE_DEVICE_UNKNOWN,0x80c,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_WaitForEvent_NT	CTL_CODE(FILE_DEVICE_UNKNOWN,0x80d,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define VXDAPI_DeleteEvent_NT	CTL_CODE(FILE_DEVICE_UNKNOWN,0x80e,METHOD_BUFFERED,FILE_ANY_ACCESS)
#endif

/************************************************************************/
/************************************************************************/
/*=====External Declarations                                            */
/************************************************************************/
/************************************************************************/
#ifndef MSNTDRV
#ifdef  MSWIN
extern HINSTANCE        cdrvxf_hInst;
#endif

#ifdef  MSWIN32
extern CRITICAL_SECTION CSection;
#endif
#endif

/* COMM-DRV System Software Externals */
extern struct ser_rs232_routines FAR    *ser_rs232;
extern struct commdrv_ctl               *commdrv_ctl;
extern char                             commdrv_version[];
extern char FAR                         cdrvpath[];

/* File Transfer Externals */
extern unsigned char    char_ack[];
extern unsigned char    char_c[];
extern unsigned char    char_can[];
extern unsigned char    char_eot[];
extern unsigned char    char_g[];
extern unsigned char    char_nak[];
extern unsigned char    *cdrvxfer_htab;
extern int              cdrvxfer_debug;
extern int              ser2xfer_sflag;
extern unsigned long    cdrvxfer_rcvflag;
extern int              cdrvxfer_rcvretrycnt;
extern int              cdrvxfer_rcvrcvtimeout;
extern int              cdrvxfer_rcvxmttimeout;
extern unsigned long    cdrvxfer_xmtflag;
extern int              cdrvxfer_xmtretrycnt;
extern int              cdrvxfer_xmtrcvtimeout;
extern int              cdrvxfer_xmtxmttimeout;
extern int              cdrvxfer_monitorcd;

/* High Level Function Externals */
extern int      modem_pacetime;
extern int      modem_pluspacetime;
extern int      modem_waitresponse;
extern int      modem_waitconnect;
extern int      modem_waitecho;
extern int      modem_waitcommand;
extern int      modem_waitdtr;
extern char     *modem_cmdstate;
extern char     *modem_attention;
extern char     *modem_hangup;
extern char     *modem_echoon;
extern char     *modem_dialpulse;
extern char     *modem_dialtone;
extern char     *modem_ok;

#ifdef  CDRVDYNA
#define AUXPCBPTR(pcb)  (((struct aux_pparam FAR *)((((long)(FP_SEG((pcb))))<<16)+FP_OFF((pcb)->auxpcb))))
#else
#define AUXPCBPTR(pcb)  (pcb->auxpcb)
#endif

/************************************************************************/
/************************************************************************/
/*=====Function prototypes                                              */
/************************************************************************/
/************************************************************************/

/************************************************************************/
/*=====Function prototypes of the COMM-DRV low level functions.         */
/************************************************************************/
/*     If dynamically linking to a WINDOWS DLL then                     */
/*     MSWIN & MSWINDLL must be defined before COMM.H is                */
/*     included for WIN32 apps MSWIN, MSWINDLL, and MSWIN32             */
/*      must be defined before COMM.H.                                  */
/*                                                                      */
/*      If dynamically linking to the MS-DOS TSR then                   */
/*      COMMDRV_DRIVER must be defined before COMM.H is                 */
/*      included.                                                       */
/************************************************************************/
#ifdef  COMMDRV_DRIVER
extern int CDECL ser_rs232_init(void);
#define ser_rs232_cleanup(p)            ser_rs232->cleanup((p))
#define ser_rs232_getbyte(p,c)          ser_rs232->getbyte((p),(c))
#define ser_rs232_getport(p,pc)         ser_rs232->getport((p),(pc))
#define ser_rs232_getregister(p,o,v)    ser_rs232->getregister((p),(o),(v))
#define ser_rs232_getstatus(p,s)        ser_rs232->getstatus((p),(s))
#define ser_rs232_putbyte(p,c)          ser_rs232->putbyte((p),(c))
#define ser_rs232_putregister(p,o,v)    ser_rs232->putregister((p),(o),(v))
#define ser_rs232_maxport()             ser_rs232->maxport()
#define ser_rs232_setup(p,pc)           ser_rs232->setup((p),(pc))
#define ser_rs232_putpacket(p,c,ch)     ser_rs232->putpacket((p),(c),(ch))
#define ser_rs232_getpacket(p,c,ch)     ser_rs232->getpacket((p),(c),(ch))
#define ser_rs232_flush(p,a)            ser_rs232->flush((p),(a))
#define ser_rs232_block(p,i,o)          ser_rs232->block((p),(i),(o))
#define ser_rs232_dtr_off(p)            ser_rs232->dtr_off((p))
#define ser_rs232_dtr_on(p)             ser_rs232->dtr_on((p))
#define ser_rs232_rts_off(p)            ser_rs232->rts_off((p))
#define ser_rs232_rts_on(p)             ser_rs232->rts_on((p))
#define ser_rs232_set_intfunc(p,q,r)    ser_rs232->set_intfunc((p),(q),(r))
#define ser_rs232_get_sdata()           ser_rs232->get_sdata()
#define ser_rs232_misc_func(p,c,v)      ser_rs232->misc_func((p),(c),(v))
#define ser_rs232_setbauddiv(p,q,r)     ser_rs232->setbauddiv((p),(q),(r))
#define ser_rs232_viewpacket(p,q,r)     ser_rs232->viewpacket((p),(q),(r))
#else
#ifdef  COMMDRV_ALL
extern int CDECL ser_rs232_init(void);
#define ser_rs232_cleanup(p)            ser_cleanup((p))
#define ser_rs232_getbyte(p,c)          ser_getbyte((p),(c))
#define ser_rs232_getport(p,pc)         ser_getport((p),(pc))
#define ser_rs232_getregister(p,o,v)    ser_getregister((p),(o),(v))
#define ser_rs232_getstatus(p,s)        ser_getstatus((p),(s))
#define ser_rs232_putbyte(p,c)          ser_putbyte((p),(c))
#define ser_rs232_putregister(p,o,v)    ser_putregister((p),(o),(v))
#define ser_rs232_maxport()             ser_maxport()
#define ser_rs232_setup(p,pc)           ser_setup((p),(pc))
#define ser_rs232_putpacket(p,c,ch)     ser_putpacket((p),(c),(ch))
#define ser_rs232_getpacket(p,c,ch)     ser_getpacket((p),(c),(ch))
#define ser_rs232_flush(p,a)            ser_flush((p),(a))
#define ser_rs232_block(p,i,o)          ser_block((p),(i),(o))
#define ser_rs232_dtr_off(p)            ser_dtr_off((p))
#define ser_rs232_dtr_on(p)             ser_dtr_on((p))
#define ser_rs232_rts_off(p)            ser_rts_off((p))
#define ser_rs232_rts_on(p)             ser_rts_on((p))
#define ser_rs232_set_intfunc(p,q,r)    ser_set_intfunc((p),(q),(r))
#define ser_rs232_get_sdata()           ser_get_sdata()
#define ser_rs232_misc_func(p,c,v)      ser_misc_func((p),(c),(v))
#define ser_rs232_setbauddiv(p,q,r)     ser_setbauddiv((p),(q),(r))
#define ser_rs232_viewpacket(p,q,r)     ser_viewpacket((p),(q),(r))
#else
#ifdef  MSWINDLL
int CDECL ser_rs232_init(void);
int FARPAS ser_rs232_cleanup(int port);
int FARPAS ser_rs232_del_t_event(int port,unsigned short event);
int FARPAS ser_rs232_getbyte(int port,unsigned char FAR *ch);
unsigned long FARPAS ser_rs232_getcontext(int port,int location);
int FARPAS ser_rs232_getport(int port,struct port_param FAR *pcb);
int FARPAS ser_rs232_getregister(int port,int offset,int FAR *value);
int FARPAS ser_rs232_getstatus(int port,unsigned char FAR *stat);
int FARPAS ser_rs232_putbyte(int port,unsigned char FAR *ch);
int FARPAS ser_rs232_putregister(int port,int offset,int value);
int FARPAS ser_rs232_maxport(void);
int FARPAS ser_rs232_setup(int port,struct port_param FAR *pcb);
int FARPAS ser_rs232_putpacket(int port,int count,
		 unsigned char FAR *ch);
int FARPAS ser_rs232_getpacket(int port,int count,
		 unsigned char FAR *ch);
int FARPAS ser_rs232_flush(int port,int arg);
int FARPAS ser_rs232_block(int port,int input,int output);
int FARPAS ser_rs232_dtr_off(int port);
int FARPAS ser_rs232_dtr_on(int port);
int FARPAS ser_rs232_rts_off(int port);
int FARPAS ser_rs232_rts_on(int port);
int FARPAS ser_rs232_set_intfunc(int port,
	       CDRVIFNC (FAR *(FAR * func))(short val,
	       struct port_param FAR *p),short imask);
int FARPAS ser_rs232_setcontext(int port,int location,unsigned long context);
struct ser_rs232_sdata FAR * FARPAS ser_rs232_get_sdata(void);
int FARPAS ser_rs232_misc_func(int port,unsigned short cmd,
	       unsigned long val);
int FARPAS ser_rs232_setbauddiv(int cardtype,int baudidx,
		 unsigned bauddiv);
int FARPAS ser_rs232_viewpacket(int port,int count,
		 unsigned char FAR *ch);


/* WINCDRV.DRV Specific */
int FARPAS GetCOMMDRVPort(int cid);
int FARPAS GetCOMMDRVPorts(int idx);
int FARPAS SetCOMMDRVPort(int cid,int port);
#else
int CDECL ser_rs232_init(void);
int FARC ser_rs232_cleanup(int port);
int FARC ser_rs232_getbyte(int port,unsigned char FAR *ch);
int FARC ser_rs232_getport(int port,struct port_param FAR *pcb);
int FARC ser_rs232_getregister(int port,int offset,int FAR *value);
int FARC ser_rs232_getstatus(int port,unsigned char FAR *stat);
int FARC ser_rs232_putbyte(int port,unsigned char FAR *ch);
int FARC ser_rs232_putregister(int port,int offset,int value);
int FARC ser_rs232_maxport(void);
int FARC ser_rs232_setup(int port,struct port_param FAR *pcb);
int FARC ser_rs232_putpacket(int port,int count,unsigned char FAR *ch);
int FARC ser_rs232_getpacket(int port,int count,unsigned char FAR *ch);
int FARC ser_rs232_flush(int port,int arg);
int FARC ser_rs232_block(int port,int input,int output);
int FARC ser_rs232_dtr_off(int port);
int FARC ser_rs232_dtr_on(int port);
int FARC ser_rs232_rts_off(int port);
int FARC ser_rs232_rts_on(int port);
int FARC ser_rs232_set_intfunc(int port,
		CDRVIFNC (FAR *(FAR *func))(short val,
		struct port_param FAR *p),short imask);
struct ser_rs232_sdata FAR * FARC ser_rs232_get_sdata(void);
int FARC ser_rs232_misc_func(int port,unsigned short cmd,unsigned long val);
int FARC ser_rs232_setbauddiv(int cardtype,int baudidx,unsigned bauddiv);
int FARC ser_rs232_viewpacket(int port,int count,unsigned char FAR *ch);
#endif
#endif
#endif

/************************************************************************/
/*=====Interrupt routines prototypes                                    */
/************************************************************************/
int FAR ser_rs232_ifnc_getpacket(struct port_param FAR *p,
char FAR *buf,int cnt);
int FAR ser_rs232_ifnc_putpacket(struct port_param FAR *p,
char FAR *buf,int cnt);
int FAR ser_rs232_ifnc_dtr_off(struct port_param FAR *p);
int FAR ser_rs232_ifnc_dtr_on(struct port_param FAR *p);
int FAR ser_rs232_ifnc_rts_off(struct port_param FAR *p);
int FAR ser_rs232_ifnc_rts_on(struct port_param FAR *p);

/************************************************************************/
/*=====High level COMM-DRV function prototypes                          */
/************************************************************************/
#ifdef  MSWINDLL
int FARPAS BytesInReceiveBuffer(int port);
int FARPAS BytesInTransmitBuffer(int port);
void FARPAS CdrvAbort(int port);
int FARPAS CdrvCheckTime(unsigned short FAR *timerblk);
void FARPAS CdrvClearAbort(int port);
unsigned short FARPAS CdrvCrc16(int count,char FAR *buf);
unsigned long FARPAS CdrvCrc32(int count,char FAR *buf);
int FARPAS CdrvDelay(int port,int timeout);
unsigned long FARPAS CdrvGetContext(int port,int idx);
struct port_param FAR * FARPAS CdrvGetPcb(int port);
int FARPAS CdrvIntToBaudIndex(int Port,long BaudRate);
unsigned long FARPAS CdrvReturnStringAddress(void FAR * adr);
int FARPAS CdrvSetContext(int port,int idx,unsigned long Context);
int FARPAS CdrvSetTime(int port,int timeout,unsigned short FAR *timerblk);
int FARPAS CdrvSetTimeoutFunction(int port,int (FAR *func)(int port));
int FARPAS CdrvSetTimerResolution(int port,int resolution);
int CDECL CdrvTimeoutFunction(int port);
int FARPAS cdrvxfer_files(int port,int direction,int protocol,
	       char FAR *fspec,int (FARC *fnc)(int chgflg,char FAR *fname,
	       long bytcnt,int curblk,int errcnt,unsigned rate));
void FARPAS cdrvxfer_gclose(struct cdrvxfer_ftpcb FAR *p);
int FARPAS cdrvxfer_getfiles(int mode,struct cdrvxfer_ftpcb FAR *p);
int FARPAS cdrvxfer_GetTestFtpcb(struct cdrvxfer_ftpcb FAR *p);
int FARPAS cdrvxfer_sendfiles(int mode,struct cdrvxfer_ftpcb FAR *p);
int FARPAS cdrvxfer_sfiles(int port,int direction,int protocol,
			       char FAR *fspec,int (FAR *fnc)(int chgflg,
			       char FAR *fname,long bytcnt,int curblk,
			       int errcnt,unsigned rate,
			       struct cdrvxfer_ftpcb FAR *ftpcb));
int FARPAS CdrvXferClearFileSpec(int port);
int FARPAS CdrvXferCreateDialog(struct cdrvxfer_ftpcb FAR *p);
int FARPAS CdrvXferDestroyDialog(struct cdrvxfer_ftpcb FAR *p);
int FARPAS CdrvXferSetFileSpec(int port,char FAR *fspec);
int FARPAS CdrvXferUpdateDialog(struct cdrvxfer_ftpcb FAR *p);
int FARPAS CloseComPort(int port);
int FARPAS DataStreamGetByte(int port,unsigned char FAR *ch);
int FARPAS DataStreamGetPacket(int port,int count,unsigned char FAR *ch);
int FARPAS Dial(int port,int mode,char FAR *telephone);
int FARPAS DtrOff(int port);
int FARPAS DtrOn(int port);
int FARPAS FileTransferDialog(int port,int mode);
int FARPAS FlushReceiveBuffer(int port);
int FARPAS FlushTransmitBuffer(int port);
int FARPAS GetByte(int port);
int FARPAS GetNumber(int port,int count,char FAR *str);
unsigned int FARPAS GetPaceTime(int port);
int FARPAS GetPacket(int port,int len,char FAR *pkt);
int FARPAS GetString(int port,int len,char FAR *str);
unsigned int FARPAS GetTimeout(int port);
int FARPAS InitializePort(int port,int subport,unsigned short addr,
	       unsigned short irq,unsigned short cardtype,
	       unsigned short cardseg,CDRVUINT inbuflen,
	       CDRVUINT outbuflen,CDRVUINT flag);
int FARPAS IsAllDataOut(int port);
int FARPAS IsBreak(int port);
int FARPAS IsCarrierDetect(int port);
int FARPAS IsCts(int port);
int FARPAS IsDsr(int port);
int FARPAS IsFramingError(int port);
int FARPAS IsInputOverrun(int port);
int FARPAS IsOverrunError(int port);
int FARPAS IsParityError(int port);
int FARPAS IsPortAvailable(int port);
int FARPAS IsReceiveBufferEmpty(int port);
int FARPAS IsRing(int port);
int FARPAS IsTransmitBufferEmpty(int port);
int FARPAS ModemAnswerMode(int port,int count);
int FARPAS ModemAttention(int port);
int FARPAS ModemCommandState(int port);
int FARPAS ModemConnect(int port);
int FARPAS ModemForceAnswer(int port);
long FARPAS ModemGetCarrierSpeed(int port);
long FARPAS ModemGetConnectSpeed(int port);
int FARPAS ModemGetSRegister(int port,int SReg);
char FAR * FARPAS ModemGetString(int port,int code);
long FARPAS ModemGetValue(int port,int code);
int FARPAS ModemHangup(int port);
int FARPAS ModemInit(int port);
int FARPAS ModemModifyString(int port,int code,char FAR *str);
int FARPAS ModemModifyValue(int port,int code,long val);
int FARPAS ModemOffHook(int port);
int FARPAS ModemOnline(int port);
int FARPAS ModemSendCommand(int port,char FAR *cmd);
int FARPAS ModemSetSRegister(int port,int SReg, int val);
int FARPAS ModemSpeaker(int port, int OnOff);
int FARPAS ModemVolume(int port, int lev);
int FARPAS ModemWaitForCall(int port,int RingCnt,int Mode,
			    int (FARC *fnc)(int port));
int FARPAS ModemWaitForConnect(int port,int RingCnt,int Mode);
int FARPAS ModemWaitForRing(int port, int cnt,int timeout);
int FARPAS OpenComPort(int ComPort, long inbuflen, long outbuflen);
int FARPAS PeekChar(int port);
int FARPAS PutByte(int port,char ch);
int FARPAS PutPacket(int port,int len,char FAR *pkt);
int FARPAS PutString(int port,char FAR *str);
int FARPAS ReceiveBufferSize(int port);
int FARPAS RtsOff(int port);
int FARPAS RtsOn(int port);
int FARPAS SendBreak(int port,int timeval);
int FARPAS SetBaud(int port,int baud);
int FARPAS SetCustomBaud(int Port,int BaudIdx,long BaudRate);
int FARPAS SetCallbackFunction(int port,int event,unsigned long val,int (*callback)(int port,int event,unsigned long val));
int FARPAS SetCallbackFunctionWinAPI(int port,int event,unsigned long val,int (WINAPI *callback)(int port,int event,unsigned long val));
int FARPAS SetDataStreamFunction(int port,void  (FARC *fnc)(int p,
				 int c, char FAR *h));
int FARPAS SetFlowControlCharacters(int port,int xoff,int xon,
	       int xxon,int xxoff);
int FARPAS SetFlowControlThreshold(int port,int inbuf_low,int inbuf_high);
int FARPAS SetPaceTime(int port,int timeval);
int FARPAS SetPortCharacteristics(int port,CDRVUINT baud,
	       unsigned short parity,unsigned short length,
	       unsigned short stopbit, unsigned short protocol);
int FARPAS SetSpecialBehavior(int port,int cmd,unsigned long val1,
			      unsigned long val2);
int FARPAS SetTimeout(int port,int timeout);
int FARPAS SetXferParameters(unsigned int command,unsigned long value);
int FARPAS SpaceInReceiveBuffer(int port);
int FARPAS SpaceInTransmitBuffer(int port);
int FARPAS SpaceTransmitBuffer(int port);
int FARPAS TransferFiles(int cmd,int port,int direction,int protocol,
		char FAR *fspec, struct cdrvxfer_ftpcb FAR *p);
int FARPAS TransmitBufferSize(int port);
int FARPAS UnInitializePort(int port);
int FARPAS WaitFor(int port,int timeout,char FAR *out,char FAR *in);
int FARPAS WaitForData(int port,int Timeout);
int FARPAS WaitForDataPeek(int port,int Timeout);
int FARPAS WaitForDataSetString(int port,unsigned char *ch,int len);
int FARPAS WaitForDataClearStrings(int port);
int FARPAS WaitForFixed(int port,int timeout,char FAR *out,
			char FAR *in,int olen,int ilen);
int FARPAS WaitForPeek(int port,int timeout,char FAR *out,char FAR *in);
int FARPAS WaitForPeekFixed(int port,int timeout,char FAR *out,
			    char FAR *in,int olen,int ilen);
int FARPAS WaitForPeekTable(int port,int timeout,char FAR *out,
		char FAR * FAR *in);
int FARPAS WaitForPeekTableFixed(int port,int timeout,char FAR *out,
				 char FAR * FAR *in,int olen,int FAR *ilen);
int FARPAS WaitForTable(int port,int timeout,char FAR *out,char FAR * FAR *in);
int FARPAS WaitForTableFixed(int port,int timeout,char FAR *out,
			     char FAR * FAR *in,int olen,int FAR *ilen);
int FARPAS XferFiles(int cmd,int port,int direction,int protocol,
		char FAR *fspec, struct cdrvxfer_info *p);
#else
int CDECL BytesInReceiveBuffer(int port);
int CDECL BytesInTransmitBuffer(int port);
int CDECL CdrvCheckTime(unsigned short FAR *timerblk);
unsigned short CDECL CdrvCrc16(int count,char FAR *buf);
unsigned long CDECL CdrvCrc32(int count,char FAR *buf);
int CDECL CdrvDelay(int port,int timeout);
struct port_param FAR * CDECL CdrvGetPcb(int port);
int CDECL CdrvSetTime(int port,int timeout,unsigned short FAR *timerblk);
int CDECL CdrvSetTimeoutFunction(int port,int (FAR *func)(int port));
int CDECL CdrvSetTimerResolution(int port,int resolution);
void CDECL cdrvxfer_gclose(struct cdrvxfer_ftpcb FAR *p);
int CDECL cdrvxfer_getfiles(int mode,struct cdrvxfer_ftpcb FAR *p);
int CDECL cdrvxfer_files(int port,int direction,int protocol,
    char FAR *fspec,int (FAR *fnc)(int chgflg,char FAR *fname,long bytcnt,
    int curblk,int errcnt,unsigned rate));
int CDECL cdrvxfer_sendfiles(int mode,struct cdrvxfer_ftpcb FAR *p);
int CDECL cdrvxfer_sfiles(int port,int direction,int protocol,char FAR *fspec,
		   int (FAR *fnc)(int chgflg,char FAR *fname,long bytcnt,
		   int curblk,int errcnt,unsigned rate,
		   struct cdrvxfer_ftpcb FAR *ftpcb));
int CDECL CdrvXferClearFileSpec(int port);
int CDECL CdrvXferCreateDialog(struct cdrvxfer_ftpcb FAR *p);
int CDECL CdrvXferDestroyDialog(struct cdrvxfer_ftpcb FAR *p);
int CDECL CdrvXferSetFileSpec(int port,char far *fspec);
int CDECL CdrvXferUpdateDialog(struct cdrvxfer_ftpcb FAR *p);
int CDECL CloseComPort(int port);
int CDECL DataStreamGetByte(int port,unsigned char FAR *ch);
int CDECL DataStreamGetPacket(int port,int count,unsigned char FAR *ch);
int CDECL Dial(int port,int mode,char FAR *telephone);
int CDECL DtrOff(int port);
int CDECL DtrOn(int port);
int CDECL FileTransferDialog(int port,int mode);
int CDECL FlushReceiveBuffer(int port);
int CDECL FlushTransmitBuffer(int port);
int CDECL GetByte(int port);
int CDECL GetNumber(int port,int count,char FAR *str);
unsigned int CDECL GetPaceTime(int port);
int CDECL GetPacket(int port,int len,char FAR *pkt);
int CDECL GetString(int port,int len,char FAR *str);
unsigned int GetTimeout(int port);
int CDECL InitializePort(int port,int subport,unsigned short addr,
    unsigned short irq,unsigned short cardtype,unsigned short cardseg,
    CDRVUINT inbuflen,CDRVUINT outbuflen,CDRVUINT flag);
int CDECL IsAllDataOut(int port);
int CDECL IsBreak(int port);
int CDECL IsCarrierDetect(int port);
int CDECL IsCts(int port);
int CDECL IsDsr(int port);
int CDECL IsFramingError(int port);
int CDECL IsInputOverrun(int port);
int CDECL IsOverrunError(int port);
int CDECL IsParityError(int port);
int CDECL IsPortAvailable(int port);
int CDECL IsReceiveBufferEmpty(int port);
int CDECL IsRing(int port);
int CDECL IsTransmitBufferEmpty(int port);
int CDECL ModemAttention(int port);
int CDECL ModemAnswerMode(int port,int count);
int CDECL ModemCommandState(int port);
int CDECL ModemConnect(int port);
long CDECL ModemGetCarrierSpeed(int port);
long CDECL ModemGetConnectSpeed(int port);
int CDECL ModemForceAnswer(int port);
int CDECL ModemGetSRegister(int port,int SReg);
char FAR * CDECL ModemGetString(int port,int code);
long CDECL ModemGetValue(int port,int code);
int CDECL ModemHangup(int port);
int CDECL ModemInit(int port);
int CDECL ModemModifyString(int port,int code,char FAR *str);
int CDECL ModemModifyValue(int port,int code,long val);
int CDECL ModemOffHook(int port);
int CDECL ModemOnline(int port);
int CDECL ModemSendCommand(int port,char FAR *cmd);
int CDECL ModemSetSRegister(int port,int SReg, int val);
int CDECL ModemSpeaker(int port, int OnOff);
int CDECL ModemVolume(int port, int lev);
int CDECL ModemWaitForCall(int port,int RingCnt,int Mode,
int (FARC *fnc)(int port));
int CDECL ModemWaitForRing(int port, int cnt,int timeout);
int CDECL OpenComPort(int ComPort, long inbuflen, long outbuflen);
int CDECL PeekChar(int port);
int CDECL PutByte(int port,char ch);
int CDECL PutPacket(int port,int len,char FAR *pkt);
int CDECL PutString(int port,char FAR *str);
int CDECL ReceiveBufferSize(int port);
int CDECL RtsOff(int port);
int CDECL RtsOn(int port);
int CDECL SendBreak(int port,int timeval);
int CDECL SetBaud(int port,int baud);
int CDECL SetDataStreamFunction(int port,void   (FARC *fnc)(int p,
				 int c, char FAR *h));
int CDECL SetFlowControlCharacters(int port,int xoff,int xon,
	int xxon,int xxoff);
int CDECL SetFlowControlThreshold(int port,int inbuf_low,int inbuf_high);
int CDECL SetPaceTime(int port,int timeval);
int CDECL SetPortCharacteristics(int port,CDRVUINT baud,
	unsigned short parity,unsigned short length,unsigned short stopbit,
	unsigned short protocol);
int CDECL SetSpecialBehavior(int port,int cmd,unsigned long val1,
			      unsigned long val2);
int CDECL SetTimeout(int port,int timeout);
int CDECL SetXferParameters(unsigned int command,unsigned long value);
int CDECL SpaceInReceiveBuffer(int port);
int CDECL SpaceInTransmitBuffer(int port);
int CDECL SpaceTransmitBuffer(int port);
int CDECL TransferFiles(int cmd,int port,int direction,int protocol,
    char FAR *fspec, struct cdrvxfer_ftpcb FAR *p);
int CDECL TransmitBufferSize(int port);
int CDECL UnInitializePort(int port);
int CDECL WaitFor(int port,int timeout,char FAR *out,char FAR *in);
int CDECL WaitForFixed(int port,int timeout,char FAR *out,
		       char FAR *in,int olen,int ilen);
int CDECL WaitForPeek(int port,int timeout,char FAR *out,char FAR *in);
int CDECL WaitForPeekFixed(int port,int timeout,char FAR *out,
			   char FAR *in,int olen,int ilen);
int CDECL WaitForPeekTable(int port,int timeout,char FAR *out,
			   char FAR * FAR *in);
int CDECL WaitForPeekTableFixed(int port,int timeout,char FAR *out,
				char FAR * FAR *in,int olen,int FAR *ilen);
int CDECL WaitForTable(int port,int timeout,char FAR *out,char FAR * FAR *in);
int CDECL WaitForTableFixed(int port,int timeout,char FAR *out,
			    char FAR * FAR *in,int olen,int FAR *ilen);
#endif

/************************************************************************/
/*=====VxD prototypes                                                   */
/************************************************************************/
#ifdef  MSWINDLL
int FARPAS WCSCVxDBreakOff(int id);
int FARPAS WCSCVxDBreakOn(int id);
int FARPAS WCSCVxDBytesInRcvBuf(int id);
int FARPAS WCSCVxDBytesInXmtBuf(int id);
int FARPAS WCSCVxDCallbackOnEvent(int id,int EventType,unsigned long Value1,
		                unsigned long Value2,unsigned long User,
				int (*Fnc)(int id,int event,unsigned long User));
int FARPAS WCSCVxDCleanup(int id);
int FARPAS WCSCVxDDeleteEvent(int id,int EventType);
int FARPAS WCSCVxDDtrOff(int id);
int FARPAS WCSCVxDDtrOn(int id);
int FARPAS WCSCVxDFlush(int id,int flag);
long FARPAS WCSCVxDGetMetrics(int id,int code);
int FARPAS WCSCVxDGetPacket(int id,int count,char FAR *pkt);
int FARPAS WCSCVxDGetPort(int id,struct vxd_port_param FAR *vpcb,int flag);
int FARPAS WCSCVxDGetRegister(int id,int offset,int FAR *value);
int FARPAS WCSCVxDInit(void);
int FARPAS WCSCVxDInitializePort(int FAR *id,unsigned short sp,
				 unsigned short adr,unsigned short irq,
				 unsigned short ct, unsigned short cs,
				 unsigned short flag);
int FARPAS WCSCVxDIsAllDataOut(int id);
int FARPAS WCSCVxDOpenPort(int sport,unsigned short addr,unsigned short irq,
			   unsigned short ctyp,unsigned short cseg,
			   unsigned short flg,unsigned pport);
int FARPAS WCSCVxDPutPacket(int id,int count,char FAR *pkt);
int FARPAS WCSCVxDPutRegister(int id,int offset,int value);
int FARPAS WCSCVxDRtsOff(int id);
int FARPAS WCSCVxDRtsOn(int id);
int FARPAS WCSCVxDSetBaudDiv(int bidx,int bdiv);
int FARPAS WCSCVxDSetPortCharacteristics(int id,unsigned short bd,
					 unsigned short pa,unsigned short len,
					 unsigned short st,unsigned short prot);
int FARPAS WCSCVxDSetup(struct port_param FAR *vpcb,int FAR *id);
int FARPAS WCSCVxDSizeOfRcvBuf(int id);
int FARPAS WCSCVxDSizeOfXmtBuf(int id);
int FARPAS WCSCVxDSpaceInRcvBuf(int id);
int FARPAS WCSCVxDSpaceInXmtBuf(int id);
int FARPAS WCSCVxDViewPacket(int id,int count,char FAR *pkt);
int FARPAS WCSCVxDWaitForEvent(int id,int EventType,long Timeout,
		            unsigned long value1, unsigned long value2);

#else
int FAR WCSCVxDBreakOff(int id);
int FAR WCSCVxDBreakOn(int id);
int FAR WCSCVxDBytesInRcvBuf(int id);
int FAR WCSCVxDBytesInXmtBuf(int id);
int FAR WCSCVxDCallbackOnEvent(int id,int EventType,unsigned long Value1,
		                unsigned long Value2,unsigned long User,
				int (*Fnc)(int id,int event,unsigned long User));
int FAR WCSCVxDCleanup(int id);
int FAR WCSCVxDDeleteEvent(int id,int EventType);
int FAR WCSCVxDDtrOff(int id);
int FAR WCSCVxDDtrOn(int id);
int FAR WCSCVxDFlush(int id,int flag);
long FAR WCSCVxDGetMetrics(int id,int code);
int FAR WCSCVxDGetPacket(int id,int count,char FAR *pkt);
int FAR WCSCVxDGetPort(int id,struct vxd_port_param FAR *vpcb,int flag);
int FAR WCSCVxDGetRegister(int id,int offset,int FAR *value);
int FAR WCSCVxDInit(void);
int FAR WCSCVxDInitializePort(int FAR *id,unsigned short sp,
			      unsigned short adr,unsigned short irq,
			      unsigned short ct, unsigned short cs,
			      unsigned short flag);
int FAR WCSCVxDIsAllDataOut(int id);
int FAR WCSCVxDOpenPort(int sport,unsigned short addr,unsigned short irq,
unsigned short ctyp,unsigned short cseg,unsigned short flg,unsigned pport);
int FAR WCSCVxDPutPacket(int id,int count,char FAR *pkt);
int FAR WCSCVxDPutRegister(int id,int offset,int value);
int FAR WCSCVxDRtsOff(int id);
int FAR WCSCVxDRtsOn(int id);
int FAR WCSCVxDSetBaudDiv(int bidx,int bdiv);
int FAR WCSCVxDSetPortCharacteristics(int id,unsigned short bd,
				      unsigned short pa,unsigned short len,
				      unsigned short st,unsigned short prot);
int FAR WCSCVxDSetup(struct port_param FAR *vpcb,int FAR *id);
int FAR WCSCVxDSizeOfRcvBuf(int id);
int FAR WCSCVxDSizeOfXmtBuf(int id);
int FAR WCSCVxDSpaceInRcvBuf(int id);
int FAR WCSCVxDSpaceInXmtBuf(int id);
int FAR WCSCVxDViewPacket(int id,int count,char FAR *pkt);
int FAR WCSCVxDWaitForEvent(int id,int EventType,long Timeout,
		            unsigned long value1, unsigned long value2);
#endif

#ifdef  MSWIN
#ifdef  MSWINDLL
int FARPAS CdrvScrCreate(HWND hWnd,int height,int width);
int FARPAS CdrvScrDestroy(HWND hWnd);
int FARPAS CdrvScrKillFocus(HWND hWnd);
int FARPAS CdrvScrResize(HWND hWnd);
int FARPAS CdrvScrPaint(HWND hWnd);
int FARPAS CdrvScrSetFocus(HWND hWnd);
int FARPAS CdrvScrWrite(HWND hWnd,char FAR *str,int len);
int FARPAS commdrvw_char_screen(int mode,HWND hWnd,WPARAM w,LPARAM l);
#else
int CdrvScrCreate(HWND hWnd,int height,int width);
int CdrvScrDestroy(HWND hWnd);
int CdrvScrKillFocus(HWND hWnd);
int CdrvScrPaint(HWND hWnd);
int CdrvScrResize(HWND hWnd);
int CdrvScrSetFocus(HWND hWnd);
int CdrvScrWrite(HWND hWnd,char FAR *str,int len);
int commdrvw_char_screen(int mode,HWND hWnd,WPARAM w,LPARAM l);
#endif
#endif

/************************************************************************/
/*=====Internal prototyping of renamed COMM-DRV routines                */
/************************************************************************/
int FAR wWCSCVxDCleanup(int id);
int FAR wWCSCVxDFlush(int id,int flag);
long FAR wWCSCVxDGetMetric(int id,int validx);
int FAR wWCSCVxDGetPacket(int id,int count,char FAR *pkt);
int FAR wWCSCVxDGetPort(int id,struct vxd_port_param FAR *vpcb,int flag);
int FAR wWCSCVxDInit(void);
int FAR wWCSCVxDInitializePort(int FAR *id,int subport,int addr,int irq,
			     int cardtype,int cardseg,int flag);
int FAR wWCSCVxDModemCtl(int id, int command);
int FAR wWCSCVxDPutPacket(int id,int count,char FAR *pkt);
int FAR wWCSCVxDSetBaudDiv(int bidx,int bdiv);
int FAR wWCSCVxDSetup(struct port_param FAR *vpcb,int FAR *id);
int FAR wWCSCVxDUARTReg(int port,int offset,int value,int putget);
int FAR wWCSCVxDViewPacket(int id,int count,char FAR *pkt);

int FAR wser_rs232_cleanup(int port);
int FAR wser_rs232_getbyte(int port,unsigned char FAR *ch);
int FAR wser_rs232_getport(int port,struct port_param FAR *pcb);
int FAR wser_rs232_getregister(int port,int offset,int FAR *value);
int FAR wser_rs232_getstatus(int port,unsigned char FAR *stat);
int FAR wser_rs232_putbyte(int port,unsigned char FAR *ch);
int FAR wser_rs232_putregister(int port,int offset,int value);
int FAR wser_rs232_maxport(void);
int FAR wser_rs232_setup(int port,struct port_param FAR *pcb);
int FAR wser_rs232_putpacket(int port,int count,unsigned char FAR *ch);
int FAR wser_rs232_getpacket(int port,int count,unsigned char FAR *ch);
int FAR wser_rs232_flush(int port,int arg);
int FAR wser_rs232_block(int port,int input,int output);
int FAR wser_rs232_dtr_off(int port);
int FAR wser_rs232_dtr_on(int port);
int FAR wser_rs232_rts_off(int port);
int FAR wser_rs232_rts_on(int port);
int FAR wser_rs232_set_intfunc(int port,
		CDRVIFNC (FAR *(FAR *func))(short val,
		struct port_param FAR *p),short imask);
struct ser_rs232_sdata FAR * FAR wser_rs232_get_sdata(void);
int FAR wser_rs232_misc_func(int port,unsigned short cmd,unsigned long val);
int FAR wser_rs232_setbauddiv(int cardtype,int baudidx,unsigned bauddiv);
int FAR wser_rs232_viewpacket(int port,int count,unsigned char FAR *ch);
void FAR * FAR ser_rs232_get_routine_table(void);

int cdrvxfer_availfname(struct cdrvxfer_ftpcb FAR *p);
int cdrvxfer_build_fileheader(struct cdrvxfer_ftpcb FAR *p);
void cdrvxfer_cancel(struct cdrvxfer_ftpcb FAR *p);
unsigned long cdrvxfer_DFdateToXFdate(unsigned short dos_date,
unsigned short dos_time);
int cdrvxfer_fileinfo(int mode,struct cdrvxfer_fileinfo FAR *f);
void cdrvxfer_flush(struct cdrvxfer_ftpcb FAR *p,int flg);
int cdrvxfer_forceout(struct cdrvxfer_ftpcb FAR *p,char FAR *ch);
int cdrvxfer_getbyte(struct cdrvxfer_ftpcb FAR *p,char FAR *ch);
int cdrvxfer_GetFirstNextFile(struct cdrvxfer_ftpcb FAR *p);
int cdrvxfer_parse_packet(struct cdrvxfer_ftpcb FAR *p);
int cdrvxfer_putbyte(struct cdrvxfer_ftpcb FAR *p,char FAR *ch);
void cdrvxymod_getfiles(int mode,struct cdrvxfer_ftpcb FAR *p);
void cdrvxymod_sendfiles(int mode,struct cdrvxfer_ftpcb FAR *p);
void cdrvzmod_getfiles(struct cdrvxfer_ftpcb FAR *p);
void cdrvzmod_sendfiles(struct cdrvxfer_ftpcb FAR *p);
void cdrvxfer_XFdateToDFdate(unsigned long xfer_date,
unsigned short FAR *dos_date, unsigned short FAR *dos_time);

/* Ring buffer prototypes */
extern int BytesInRing(struct RingCtl FAR *ctl);
extern int CreateRing(struct RingCtl FAR *ctl,int len);
extern int DestroyRing(struct RingCtl FAR *ctl);
extern int GetByteFromRing(struct RingCtl FAR *ctl);
extern int GetPacketFromRing(struct RingCtl FAR *ctl,int len,char FAR *ch);
extern int PurgeRing(struct RingCtl FAR *ctl);
extern int PutByteInRing(struct RingCtl FAR *ctl,int ch);
extern int PutPacketInRing(struct RingCtl FAR *ctl,int len,char FAR *ch);
extern int SpaceInRing(struct RingCtl FAR *ctl);
extern int ViewPacketInRing(struct RingCtl FAR *ctl,int len,char FAR *ch);

#ifdef  MSWIN
unsigned short ser_rs232_getsel(unsigned short seg);
unsigned short ser_rs232_retsel(unsigned short sel);
#endif

int ser2xfer_close(unsigned int port);
int ser2xfer_getfiles(unsigned int port,char *filespec);
int ser2xfer_open(unsigned int port,unsigned int adr,unsigned int irq,
			  unsigned int speed,unsigned int parity,
			  unsigned int length,unsigned int stopbit);
int ser2xfer_receive(unsigned int port,char *localfile,
			     char *remotefile);
int ser2xfer_send(unsigned int port,char *localfile,char *remotefile);
int ser2xfer_sendfiles(unsigned int port,char *filespec);
int ser2xfer_slave(unsigned int port);

/*=====*/
int  ser_cleanup(int  port);
int  ser_getbyte(int  port,unsigned char FAR *ch);
int  ser_getport(int  port,struct  port_param FAR *pcb);
int  ser_getregister(int  port,int  offset,int  FAR *value);
int  ser_getstatus(int  port,unsigned char FAR *status);
int  ser_putbyte(int  port,unsigned char FAR *ch);
int  ser_putregister(int  port,int  offset,int  value);
int  ser_maxport(void);
int  ser_setup(int  port,struct  port_param FAR *pcb);
int  ser_putpacket(int  port,int  count,unsigned char  FAR *ch);
int  ser_getpacket(int  port,int  count,unsigned char  FAR *ch);
int  ser_flush(int  port,int  arg);
int  ser_block(int  port,int  inblock,int  outblock);
int  ser_dtr_on(int  port);
int  ser_dtr_off(int  port);
int  ser_rts_on(int  port);
int  ser_rts_off(int  port);
int  ser_set_intfunc(int port,
	CDRVIFNC (FAR *(FAR *func))(short val,struct port_param FAR *p),
	short imask);
struct ser_rs232_sdata FAR *ser_get_sdata(void);
int ser_misc_func(int port,unsigned short cmd,
	unsigned long val);
int ser_setbauddiv(int cardtype,int baudidx,unsigned bauddiv);
int ser_viewpacket(int port,int count,unsigned char FAR *ch);
int  ser_mode(int  port,int  baud,int  parity,int  lngth,int  stopbit,
		      int  brk,int  protocol,int  inblock,int  outblock);
int  ser_settype(unsigned int  type);


/* INT14 Routines */
int  FAR ser_int14_cleanup(int port);
int  FAR ser_int14_getflag(int port);
struct ser_int14_info  FAR * FAR ser_int14_getinfo(void);
void FAR ser_int14_install(void);
int  FAR ser_int14_remove(void);
int  FAR ser_int14_setflag(int port,int flag);

/* General */
char    FAR *CdrvRpad(char FAR *str,int len);
long fatol(char FAR *str);
char FAR *fatoo(unsigned long val,char FAR *buf);
char FAR *fltoa(long  val,char  FAR *str,int  radix);
int ffsubstr(char FAR *str,char FAR *sstr);
int fmemcmp(void FAR *ch1,void FAR *ch2,int count);
void FAR *fmemmove(void FAR *ch1,void FAR *ch2,int count);
void FAR *fmemset(void  FAR *ch,int  c,int  count);
unsigned long fotoul(char FAR *buf);
char FAR *fstrcat(char FAR *ch1,char FAR *ch2);
int fstrcmp(char  FAR *ch1,char  FAR *ch2);
char FAR *fstrcpy(char FAR *ch1,char FAR *ch2);
int fstrlen(char  FAR *ch);
char FAR *fstrupr(char  FAR *str);
int ftoupper(int  ch);
void FAR *cdrv_alloc(int siz);
int cdrv_free(void FAR *ptr);

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif  /* __cplusplus */
/************************************************************************/
#endif /* FILE_COMM_H */

