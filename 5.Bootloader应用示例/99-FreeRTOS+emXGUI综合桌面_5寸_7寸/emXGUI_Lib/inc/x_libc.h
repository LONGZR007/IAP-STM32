
#ifndef  __X_LIBC_20130709_1415__
#define  __X_LIBC_20130709_1415__

#ifdef __cplusplus
extern "C" {
#endif
 
#include	"def.h"

/*===================================================================================*/
/*===================================================================================*/
/*============================================================================*/
/*
typedef char*	x_va_list;


#ifdef   __cplusplus
#define _ADDRESSOF(v)    ( &reinterpret_cast<const char &>(v) )
#else
#define _ADDRESSOF(v)    ( &(v) )
#endif

#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define x_va_start(ap,v)   ( ap = (x_va_list)_ADDRESSOF(v) + _INTSIZEOF(v) )
#define x_va_arg(ap,t)     ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define x_va_end(ap)       ( ap = (x_va_list)0 )
*/
 
#include <stdarg.h>
 
#define	x_va_list	va_list
#define	x_va_start	va_start
#define	x_va_arg	va_arg
#define	x_va_end	va_end


    	
/*===================================================================================*/

   
/*===================================================================================*/
/*===================================================================================*/

int x_rand(void);
void	x_srand(int seed);


int 	x_isdigit(int c);
int		x_toupper(int c);
int		x_tolower(int c);

int		x_strlen(const char *str);
char*	x_strcpy(char *dst,const char *src);
char*	x_strncpy (char *s1,const char *s2,int n);
int		x_strcmp(const char *s1,const char *s2);
int		x_strncmp (const char *s1, const char *s2, int n);
int 	x_strncasecmp(const char *str1, const char *str2, int maxlen);
char*	x_strcat (char *dest, const char *src);
char*	x_strncat(char *dest, const char *src, int count);
char 	*x_strrev(char *string);
char 	*x_strchr(const char *string, int c);
char 	*x_strrchr(const char *string, int c);
char 	*x_strstr(const char *haystack, const char *needle);

char 	*x_ltoa(long value, char *string, int radix);

unsigned long 	x_strtoul(const char *cp,char **endp,unsigned int base);  
long 	x_strtol(const char *cp,char **endp,unsigned int base);

int   	x_atoi(const char *s);
double 	x_atof(const char *s);

char*	x_ftoa(char *str,double data, int jd);
double 	x_strtod(const char *string, char **endp);

int		x_vsprintf(char *dest,const char *format,x_va_list ap);
int 	x_sprintf(char *dest, const char *format, ...);
int 	x_vscanf(const char *src,const char *format,x_va_list ap);
int  	x_sscanf(char *dest, const char *format, ...);

/*===================================================================================*/
int		x_wstrlen(const WCHAR *str);
WCHAR*	x_wstrcpy(WCHAR *dst,const WCHAR *src);
WCHAR*	x_wstrncpy (WCHAR *s1,const WCHAR *s2,int n);
int		x_wstrcmp(const WCHAR *s1,const WCHAR *s2);
int		x_wstrncmp (const WCHAR *s1, const WCHAR *s2, int n);
int 	x_wstrncasecmp(const WCHAR *str1, const WCHAR *str2, int maxlen);
WCHAR*	x_wstrcat(WCHAR *dest, const WCHAR *src);
WCHAR* 	x_wstrncat(WCHAR *dest, const WCHAR *src, int count);
WCHAR 	*x_wstrrev(WCHAR *string);
WCHAR 	*x_wstrchr(const WCHAR *string, int c);
WCHAR 	*x_wstrrchr(const WCHAR *string, int c);
WCHAR 	*x_wstrstr(const WCHAR *haystack, const WCHAR *needle);
unsigned long 	x_wstrtoul(const WCHAR *cp,WCHAR **endp,unsigned int base);
long 	x_wstrtol(const WCHAR *cp,WCHAR **endp,unsigned int base);
double 	x_wstrtod(const WCHAR *string, WCHAR **endp);

int 	x_wvsprintf(WCHAR *dest,const WCHAR *format,x_va_list ap );
int  	x_wsprintf(WCHAR *dest, const WCHAR *format, ...);
int 	x_wvscanf(const WCHAR *src,const WCHAR *format,x_va_list ap );
int 	x_wsscanf(WCHAR *dest, const WCHAR *format, ...);

/*===================================================================================*/

int	x_mbstowcs_cp437(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp437(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp720(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp720(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp737(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp737(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp775(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp775(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp850(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp850(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp852(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp852(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp855(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp855(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp857(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp857(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp858(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp858(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp862(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp862(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp866(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp866(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp874(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp874(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp1250(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp1250(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp1251(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp1251(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp1252(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp1252(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp1253(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp1253(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp1254(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp1254(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp1255(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp1255(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp1256(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp1256(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp1257(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp1257(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp1258(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp1258(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp932(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp932(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp936(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp936(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp949(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp949(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp950(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs_cp950(char *mbchar,const WCHAR *wchar,int count);

int	x_mbstowcs_cp(WCHAR *wchar,const char *mbchar,int count,int cp);
int	x_wcstombs_cp(char *mbchar,const WCHAR *wchar,int count,int cp);

int	x_mbstowcs(WCHAR *wchar,const char *mbchar,int count);
int	x_wcstombs(char *mbchar,const WCHAR *wchar,int count);

int		xStringGetChar(UINT cp,const char *str,int *count);

/*=========================================================================================*/

#define RW_SEEK_SET	0	/* Seek from the beginning of data */
#define RW_SEEK_CUR	1	/* Seek relative to current read point */
#define RW_SEEK_END	2	/* Seek relative to the end of data */

typedef	struct	__rwctx
{
	void *data;	
	int (*seek)(void *data,int offset, int whence);
	int (*read)(void *data,void *ptr, int size);
	int (*write)(void *data,const void *ptr, int size);
		
} rwctx_t;

u32	rw_seek(rwctx_t *ctx, int offset, int whence);
u32	rw_tell(rwctx_t *ctx);
int		rw_read(rwctx_t *ctx, void *ptr, int size);
int		rw_write(rwctx_t *ctx, const void *ptr, int size);

/*===================================================================================*/

typedef	struct	__rw_mem
{
	u8 *mem_base;
	int size;
		
	u8 *base;
	u8 *here;
	u8 *stop;
} rw_mem_t;
	    

int	rw_from_mem(rwctx_t *ctx,rw_mem_t *rw_mem);

/*===================================================================================*/

struct	_X_HEAP
{
	u8 *ptr;				/* pointer to the heap: for alignment, ptr is now a pointer instead of an array */
	struct heap_mem *end;	/* the last entry, always unused! */
	struct heap_mem *lfree; /* pointer to the lowest free block */
	
	u32 align_size;
	UINT total_size;  //内存总数量(字节)
	UINT used_cur;    //当前使用量(字节)
	UINT used_max;    //最大使用量(字节)
};

typedef	struct _X_HEAP heap_t;

int		x_heap_init(heap_t *heap,void *base,u32 size,u32 alloc_align);
void*	x_heap_alloc(heap_t *heap,u32 size);
void*	x_heap_realloc(heap_t *heap,void *ptr,u32 size);
void	x_heap_free(heap_t *heap,void *ptr);

/*===================================================================================*/

int	x_get_week_of_day(int year,int month,int day);//0:SUN;1:MON;2:TUE;3:WED;4:THU;5:FRI;6:SAT;
void x_insert_sort_s8(s8 *A, int first, int last);
void x_insert_sort_u8(u8 *A, int first, int last);
void x_insert_sort_s16(s16 *A, int first, int last);
void x_insert_sort_u16(u16 *A, int first, int last);
void x_insert_sort_s32(s32 *A, int first, int last);
void x_insert_sort_u32(u32 *A, int first, int last);

/*===================================================================================*/

#ifdef __cplusplus
}
#endif

#endif
