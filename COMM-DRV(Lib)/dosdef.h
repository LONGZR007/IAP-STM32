/****************************************************************/
/*		------- dosdef.h -------			*/
/*								*/
/*	Copyright (C) Egberto Willies, 1991			*/
/*	All rights reserved.					*/
/*								*/
/*%Z% MOD-%M% SID-%I% DATE-%G% TIME-%U%				*/
/*								*/
/*Author							*/
/*	Egberto Willies 04-APR-1991				*/
/****************************************************************/
#ifndef	DOSDEF_H
#define	DOSDEF_H

#ifdef	MSWIN32
#ifndef	CDECL
#define	CDECL	cdecl
#endif

#ifndef	FAR
#define	FAR
#endif

#ifndef FARC
#define	FARC	CDECL
#endif

#ifndef FARPAS
#define	FARPAS	PASCAL
#endif

#ifndef CDRVUINT
#define	CDRVUINT	unsigned int
#endif

#else

#ifndef	CDECL
#define	CDECL	cdecl
#endif

#ifndef	FAR
#define	FAR	far
#endif

#ifndef FARC
#define	FARC	FAR CDECL
#endif

#ifndef FARPAS
#define	FARPAS	FAR PASCAL
#endif

#ifndef CDRVUINT
#define	CDRVUINT	unsigned short
#endif

#endif


struct dos_psp
{
unsigned short	dos_pexit;		/* INT 20H (exit) instruction */
unsigned short	dos_nextseg;		/* First unused segment */
unsigned char	dos_msdospsp1;		/* Filler */
unsigned char	dos_cpmcall[5];		/* Far call to INT21 services */
void 		(FAR *dos_isv22)();	/*  */
void 		(FAR *dos_isv23)();	/*  */
void 		(FAR *dos_isv24)();	/*  */
unsigned short	dos_ppsp;		/* Parent PSP */
unsigned char	dos_handles[20];	/* Handles (Indeces into DOS's SFT) */
unsigned short	dos_envseg;		/* Segment of environment */
unsigned short FAR *dos_savstack;	/* User stack before INT21 entry */
unsigned short	dos_nhandles;		/* Number of handles */
unsigned char FAR *dos_handleptr;	/* Pointer to handles tables */
unsigned long	dos_share_psp;		/* Share's previous PSP */
unsigned char	dos_psp2[20];		/* Unused */
unsigned char	dos_unixdisp[3];	/* INT 21H/retf Unix type dispatcher */
unsigned char	dos_psp3[9];		/* Unused */
unsigned char	dos_fcb1[16];		/* FCB 1 */
unsigned char	dos_fcb2[20];		/* FCB 2 */
unsigned char	dos_arg_count;		/* Command line character count */
unsigned char	dos_psparg[127];	/* Argements */
};


struct dos_find_struct
	{
	#ifdef	MSWIN32
	#define	dos_reserved	fi
	#define	DOS_PATHLEN	256
	#define	DOS_FILELEN	256
	struct cdrv_fstruc
	{
 	HANDLE			hd;
	DWORD			attr;
	unsigned		flag;
	}			fi;
	#else
	#define	DOS_PATHLEN	127
	#define	DOS_FILELEN	13
	char 		dos_reserved[21];	/* Reserved by DOS */
	#endif
	char 		dos_attrib;		/* File attribute */
	unsigned	dos_wr_time;		/* Last write time */
	unsigned	dos_wr_date;		/* Last write date */
	long		dos_size;		/* File size */
	char		dos_name[DOS_FILELEN];	/* DOS filename */
	char		dos_dummy;		/* Place holder */
	int		dos_pathidx;		/* Index to place name */
	char		dos_fspec[DOS_PATHLEN];	/* path plus name */
	};

/****************************************************************/
/*=====Function prototypes					*/
/****************************************************************/
#ifndef	MSWIN32
extern long FAR bio_get_elapsedtime(unsigned short FAR *timblkptr);
extern int FAR bio_get_timer(unsigned short FAR *timblkptr);
extern int FAR bio_getchar(void);
extern void FAR	bio_getcursorxy(int page,int FAR *row,int FAR *col);
extern void FAR	bio_getpagemode(int FAR *page,int FAR *mode);
extern void FAR bio_getvchar(int page,char FAR *chr,char FAR *attr);
extern void FAR	bio_scrolldown(int nline,int attr,int lrow,int lcol,
	int rrow,int rcol);
extern void FAR	bio_scrollup(int nline,int attr,int lrow,int lcol,
	int rrow,int rcol);
extern void FAR	bio_setcursorxy(int page,int row,int col);
extern void FAR	bio_setmode(int mode);
extern void FAR	bio_setpage(int page);
extern void FAR	bio_putchar(int str,int page,int attr,int count);
extern void FAR	bio_putstring(char FAR *str,int page,int attr);
extern int FAR bio_set_timer(long count,unsigned short FAR *timblkptr);
extern void FAR bio_set_timerresolution(long val);
extern void FAR	bio_vputchar(int str,int page,int attr,int count);
extern void FAR	bio_vputstring(char FAR *str,int page,int attr);
extern unsigned FAR dos_allocatemem(unsigned parlen,int strategy,
	unsigned FAR *adr);
extern int FAR dos_changedir(char FAR *dir);
extern int FAR dos_close(int handle);
extern int FAR dos_commit(int handle);
extern int FAR dos_create(char FAR *fname,unsigned attrib);
extern int FAR dos_createdir(char FAR *dir);
extern int FAR dos_delete(char FAR *fname);
extern int FAR dos_deletedir(char FAR *dir);
extern void FAR * FAR dos_find_tsr(int mpxid);
extern int  FAR dos_find_tsr_by_name(char FAR *tsrname);
extern int FAR dos_getdir(char FAR *dir);
extern int FAR dos_getfdate(int hd,unsigned short FAR *dat,
        unsigned short FAR *tim);
extern int FAR dos_getfileattribute(char FAR *fname,unsigned FAR *attr);
extern int FAR dos_getfiledatetime(char FAR *fname,unsigned FAR *date,
	unsigned FAR *tim);
extern int FAR dos_getfirstfile(struct dos_find_struct FAR *fptr,
       unsigned attrib,char FAR *fname);
extern int  FAR dos_get_free_mpxid(char FAR *tsrname);
extern long FAR * FAR dos_get_varaddress_tsr(int mpxid);
extern long FAR dos_getvariable_tsr(int mid);
extern int FAR dos_install_fixedtsr(int mpxid,void FAR *func,int FAR *tail);
extern int FAR dos_install_tsr(int mpxid,void FAR *func);
extern int FAR dos_link_tsr(int mpxid);
extern void FAR dos_getdate(int FAR *year,int FAR *month,int FAR *day,
	int FAR *wday);
extern int FAR dos_getnextfile(struct dos_find_struct FAR *fptr);
extern void FAR dos_gettime(int FAR *hour,int FAR *minute,int FAR *second,
	int FAR *hundredth);
extern void FAR * FAR dos_getvector(int vecnum);
extern int FAR dos_open(char FAR *fname,unsigned attrib);
extern int FAR dos_read(int handle,void FAR *bufptr,int count);
extern int dos_raw(int handle);
extern unsigned FAR dos_releasemem(unsigned para);
extern int FAR dos_remove_tsr(int mpxid);
extern int FAR dos_rename(char FAR *oldfname,char FAR *newfname);
extern unsigned FAR dos_resizemem(unsigned para,unsigned len);
extern unsigned long FAR dos_seek(int handle,int method,long offset);
extern void FAR dos_setdate(int year,int month,int day,int wday);
extern int FAR dos_setfdate(int hd,unsigned short dat,unsigned short tim);
extern int FAR dos_setfileattribute(char FAR *fname,unsigned attr);
extern int FAR dos_setfiledatetime(char FAR *fname,unsigned date,unsigned tim);
extern void FAR dos_settime(int hour,int minute,int second,int hundredth);
extern void FAR dos_setvariable_tsr(int mid,long var);
extern void FAR dos_setvector(int vecnum,void FAR *vect);
extern void FAR dos_shrinkenv(unsigned thispsp,char FAR *fname);
extern int FAR dos_unlink_tsr(void);
extern int FAR dos_write(int handle,void FAR *bufptr,int count);
extern void FAR msc_find_mtasker(void);
extern void FAR msc_release_tslice(void);

#else

long WINAPI bio_get_elapsedtime(unsigned short *timblkptr);
int WINAPI bio_get_timer(unsigned short *timblkptr);
int bio_getchar(void);
void WINAPI	bio_getcursorxy(int page,int *row,int *col);
void WINAPI	bio_getpagemode(int *page,int *mode);
void WINAPI bio_getvchar(int page,char *chr,char *attr);
long WINAPI bio_get_timerresolution(void);
void WINAPI	bio_scrolldown(int nline,int attr,int lrow,int lcol,
	int rrow,int rcol);
void WINAPI	bio_scrollup(int nline,int attr,int lrow,int lcol,
	int rrow,int rcol);
void WINAPI	bio_setcursorxy(int page,int row,int col);
void WINAPI	bio_setmode(int mode);
void WINAPI	bio_setpage(int page);
void WINAPI	bio_putchar(int str,int page,int attr,int count);
void WINAPI	bio_putstring(char *str,int page,int attr);
int WINAPI bio_set_timer(long count,unsigned short *timblkptr);
void WINAPI bio_set_timerresolution(long val);
void WINAPI	bio_vputchar(int str,int page,int attr,int count);
void WINAPI	bio_vputstring(char *str,int page,int attr);
unsigned WINAPI dos_allocatemem(unsigned parlen,int strategy,
	unsigned *adr);
int WINAPI dos_changedir(char *dir);
int WINAPI dos_close(int handle);
int WINAPI dos_commit(int handle);
int WINAPI dos_create(char *fname,unsigned attrib);
int WINAPI dos_createdir(char *dir);
int WINAPI dos_delete(char *fname);
int WINAPI dos_deletedir(char *dir);
void * WINAPI dos_find_tsr(int mpxid);
int  WINAPI dos_find_tsr_by_name(char *tsrname);
int WINAPI dos_getdir(char *dir);
int WINAPI dos_getfdate(int hd,unsigned short *dat,
       unsigned short *tim);
int WINAPI dos_getfileattribute(char *fname,unsigned *attr);
int WINAPI dos_getfiledatetime(char *fname,unsigned *date,
unsigned *tim);
int WINAPI dos_getfirstfile(struct dos_find_struct *fptr,
      unsigned attrib,char *fname);
int  WINAPI dos_get_free_mpxid(char *tsrname);
long * WINAPI dos_get_varaddress_tsr(int mpxid);
long WINAPI dos_getvariable_tsr(int mid);
int WINAPI dos_install_fixedtsr(int mpxid,void *func,int *tail);
int WINAPI dos_install_tsr(int mpxid,void *func);
int WINAPI dos_link_tsr(int mpxid);
void WINAPI dos_getdate(int *year,int *month,int *day,
int *wday);
int WINAPI dos_getnextfile(struct dos_find_struct *fptr);
void WINAPI dos_gettime(int *hour,int *minute,int *second,
int *hundredth);
void * WINAPI dos_getvector(int vecnum);
int WINAPI dos_open(char *fname,unsigned attrib);
int WINAPI dos_read(int handle,void *bufptr,int count);
int dos_raw(int handle);
unsigned WINAPI dos_releasemem(unsigned para);
int WINAPI dos_remove_tsr(int mpxid);
int WINAPI dos_rename(char *oldfname,char *newfname);
unsigned WINAPI dos_resizemem(unsigned para,unsigned len);
unsigned long WINAPI dos_seek(int handle,int method,long offset);
void WINAPI dos_setdate(int year,int month,int day,int wday);
int WINAPI dos_setfdate(int hd,unsigned short dat,unsigned short tim);
int WINAPI dos_setfileattribute(char *fname,unsigned attr);
int WINAPI dos_setfiledatetime(char *fname,unsigned date,unsigned tim);
void WINAPI dos_settime(int hour,int minute,int second,int hundredth);
void WINAPI dos_setvariable_tsr(int mid,long var);
void WINAPI dos_setvector(int vecnum,void *vect);
void WINAPI dos_shrinkenv(unsigned thispsp,char *fname);
int WINAPI dos_unlink_tsr(void);
int WINAPI dos_write(int handle,void *bufptr,int count);
void WINAPI msc_find_mtasker(void);
void WINAPI msc_release_tslice(void);
#endif
/****************************************************************/
#endif
