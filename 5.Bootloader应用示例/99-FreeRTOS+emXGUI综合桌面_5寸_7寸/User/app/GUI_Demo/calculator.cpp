
//#include	"Include.h"
#include	<string.h>
#include	<stdlib.h>
#include	<math.h>

#include	"emXGUI.h"
#include	"x_libc.h"


/*============================================================================*/
//
/*============================================================================*/
#define MAXLEN 128
#define MAXIDENT 16

#define	IDC_NULL	0x0000

#define	IDC_0		0x1000
#define	IDC_1		0x1001
#define	IDC_2		0x1002
#define	IDC_3		0x1003
#define	IDC_4		0x1004
#define	IDC_5		0x1005
#define	IDC_6		0x1006
#define	IDC_7		0x1007
#define	IDC_8		0x1008
#define	IDC_9		0x1009
#define	IDC_A		0x100a
#define	IDC_B		0x100b
#define	IDC_C		0x100c
#define	IDC_D		0x100d
#define	IDC_E		0x100e
#define	IDC_F		0x100f

#define	IDC_ADD		0x1100
#define	IDC_SUB		0x1101
#define	IDC_MUL		0x1102
#define	IDC_DIV		0x1103
#define	IDC_DOT		0x1104
#define	IDC_EQU		0x1105
#define	IDC_LBR		0x1106
#define	IDC_BACK	0x1107
#define	IDC_RBR		0x1108
#define	IDC_CLR		0x1109
#define	IDC_XOR		0x110a
#define	IDC_SEM		0x110b
#define	IDC_COM		0x110c


#define	IDC_SIN		0x1200
#define	IDC_ASIN	0x1201
#define	IDC_COS		0x1202
#define	IDC_ACOS	0x1203
#define	IDC_TAN		0x1204
#define	IDC_ATAN	0x1205
#define	IDC_LOG		0x1206
#define	IDC_SQRT	0x1207
#define	IDC_SUM		0x1208
#define	IDC_AVE		0x1209
#define	IDC_SUR		0x120a

#define	IDC_BIN		0x1300
#define	IDC_OCT		0x1301
#define	IDC_DEC		0x1302
#define	IDC_HEX		0x1303

#define	IDC_ANGLE	0x1400
#define	IDC_RADIAN	0x1401

#define	IDC_EIXT	0x1500

#define	IDC_MODE_GROUP		0x2000
#define	IDC_TRIFUN_GROUP	0x2001

/*============================================================================*/

static const WCHAR	*mode_text[4]={

	L"二进制",
	L"八进制",
	L"十进制",
	L"十六进制",
	
};

static const U16	mode_id[4]={

	IDC_BIN,
	IDC_OCT,
	IDC_DEC,
	IDC_HEX,

};

static const WCHAR	*tri_fun_text[2]={
	L"角度",
	L"弧度",
};

static const U16	tri_fun_id[2]={
	IDC_ANGLE,
	IDC_RADIAN,
};

static const	WCHAR *btn_text[8*5]={

	L"0",	L"1",	L"2",	L"3",	L"%",	L"CE",	L"sin",	L"asin",
	L"4",	L"5",	L"6",	L"7",	L"/",	L"<-",	L"cos",	L"acos",
	L"8",	L"9",	L"A",	L"B",	L"*",	L"^",	L"tan",	L"atan",
	L"C",	L"D",	L"E",	L"F",	L"-",	L";",	L"log",	L"sum",
	L".",	L",",	L"(",	L")",	L"+",	L"=",	L"sqrt",L"ave"
	
};

static const U16	btn_id[8*5]={

IDC_0,		IDC_1,		IDC_2,		IDC_3,		IDC_SUR,	IDC_CLR,	IDC_SIN,	IDC_ASIN,
IDC_4,		IDC_5,		IDC_6,		IDC_7,		IDC_DIV,	IDC_BACK,	IDC_COS,	IDC_ACOS,
IDC_8,		IDC_9,		IDC_A,		IDC_B,		IDC_MUL,	IDC_XOR,	IDC_TAN,	IDC_ATAN,
IDC_C,		IDC_D,		IDC_E,		IDC_F,		IDC_SUB,	IDC_SEM,	IDC_LOG,	IDC_SUM,
IDC_DOT,	IDC_COM,	IDC_LBR,	IDC_RBR,	IDC_ADD,	IDC_EQU,	IDC_SQRT,	IDC_AVE,

};

//static const U8  fd[4]={0,1,2,3};
//static const U16 ff=0x3246;


static char output[MAXLEN] __EXRAM;
static char input[MAXLEN] __EXRAM;

 static	int isdigit(char c)
 {
  	if((c>='0') && (c<='9'))	return 1;
	return 0;
 }

/*============================================================================*/

static void exit_owner_draw(DRAWITEM_HDR *ds) //绘制一个按钮外观
{
  HDC hdc;
  RECT rc, rc_tmp;
  HWND hwnd;

	hdc = ds->hDC;   
	rc = ds->rc; 
  hwnd = ds->hwnd;

//  SetBrushColor(hdc,MapRGB(hdc,0,100,200));
//	FillRect(hdc, &rc);

  if (ds->State & BST_PUSHED)
	{ //按钮是按下状态
		SetPenColor(hdc, MapRGB(hdc, 250, 250, 250));      //设置画笔色
	}
	else
	{ //按钮是弹起状态

		SetPenColor(hdc, MapRGB(hdc, 1, 191, 255));
	}

  SetPenSize(hdc, 2);

  InflateRect(&rc, 0, -5);
  
  for(int i=0; i<4; i++)
  {
    HLine(hdc, rc.x, rc.y, rc.w);
    rc.y += 9;
  }

}
#if 0
static double __atof(const char* nptr)
{
    double retval = 0.0, dec = 0.0;
    const char* p;
    int shift = 0, sign = 1, exp_sign = 1, exp = 0;

    p = nptr;

    while(*p){
        if(*p == ' ' || *p == '\t')
            continue;
        break;
    }

    /**
     * '+'/'-'判断符号位: 不能两个连续的'+'或'-'否则函数立即返回
     */
    if(*p == '+'){
        sign = 1;
        ++p;
    }else if(*p == '-'){
        sign = -1;
        ++p;
    }else{
        ;
    }
// int_part:
    while(*p){
        if(isdigit(*p)){
            retval = (double)((int)retval * 10 + (*p - '0'));
        }else if(*p == '.'){
            ++p;
                   goto non_int_part;
        }else if(*p == 'e' || *p == 'E'){
            ++p;
            goto exp_part;
        }else{
            return retval;
        }
        ++p;
    }
non_int_part:
    while(*p){
        ++shift;
        if(p - nptr > 100)
            break;

        if(isdigit(*p)){
            int f = shift, f1 = (*p - '0');
            double f2 = (double)f1;
            while(f--){
                f2 /= 10;
            }
            retval += f2;
        }else if(*p == 'e' || *p == 'E'){
            ++p;
            goto exp_part;
        }else{
            return retval;
        }
        ++p;
    }

exp_part:
    /**
     * 判断符号位: 但不允许同时出现两个符号
     * 否则函数立即返回
     */
    if(*p == '-'){
        exp_sign = -1;
        ++p;
    }else if(*p == '+'){
        exp_sign = 1;    
        ++p;
    }else{
        ;
    }

    while(*p){
        if(isdigit(*p)){
            exp = exp*10 + (*p - '0');
        }else{
            break;
        }
        ++p;
    }

final_result:
    if(exp_sign > 0){
        while(exp){
            retval *= 10;
            exp--;
        }
    }else{
        while(exp){
            retval /= 10;
            exp--;
        }
    }

    return retval * sign;
}
#endif

/*============================================================================*/


typedef struct iden
{
	char name[10];
	double m;
}iden;


static double sum(int n,double *m);
static double ave(int n,double *m);
static double variance(int n,double *m);
	
static int identnum;
static iden ident[MAXIDENT] __EXRAM;
static char hisexp[MAXLEN][MAXLEN] __EXRAM;
static BOOL jiao;
static int funnum;
static char hisfun[20] __EXRAM;
static BOOL funflag;
static BOOL opflag;
static int opnum;
static double hisdata;
static char hisop;
static BOOL errorflag;
static char stack[MAXLEN][MAXLEN] __EXRAM;
static int low;
static int now;
static int high;
static int lowexp;
static int nowexp;
static int highexp;
static int errornum;
static int jin;


static char precede(char a, char b);
static BOOL InOP(char ch);
static double Count(char*express, int m);
static double funtion(char *str, char *str1, int n);
static double Change(char *str, int n);
static void ReChange(double m, char *str);
static double operat(double m, double n, char optr);
static void error(int kind,char *str);
static BOOL INoper(char a);


static void error(int kind,char *str)
{
/*
  errornum++;
  if(errornum==1&&!errorflag)
  {
  m_error.Create(IDD_DIALOG1,this);
  }
  else if(errornum==1)
  {
   int j = m_error.m_List.GetCount();
   for (int i=0;i < j;i++)
   m_error.m_List.DeleteString( 0 );
  }
  m_error.addinfo(kind,str);
  errorflag=TRUE;
*/

}

static double variance(int n, double *m)
{
  int i;
  double aveage = ave(n,m);
  double vari=0.0;
  for(i=0;i<n;i++)
	  vari=vari+(m[i]-aveage)*(m[i]-aveage);
  return vari;
}

static double ave(int n, double *m)
{
   return sum(n,m)/n;
}

static double sum(int n, double *m)
{
 int i;
 double s=0.0;
  for(i=0;i<n;i++)
	  s+=m[i];
  return s;
}

static BOOL INoper(char a)
{
  if(a=='+'||a=='-'||a=='*'||a=='/'||a=='%'||a=='^')
	  return TRUE;
  else
	  return FALSE;
}

static double operat(double m, double n, char optr)
{
  int m1,n1;
  if(errornum==0 && opnum==0)
  {
	  hisop = optr;
	  hisdata = n;
	  opflag = TRUE;
  }
  else
  {
	 hisop = '\0';
	 memset(hisfun,'\0',20);
  }
  opnum++;
  if(optr=='+')
   return m+n;
  else if(optr == '-')
   return m-n;
  else if(optr == '*')
   return m*n;
  else if(optr == '/')
  {
	 if(fabs(n)<1e-15)
	 {
		 error(9,"/");
		 return 0.0;
	 }
     return m/n;
  }
  else if(optr == '^')
   return pow(m,n);
  else if(optr == '%')
  {
	m1=(int)(m+1e-14);
	n1=(int)(n+1e-14);
	if(fabs(m-m1)>1e-12||fabs(n-n1)>1e-12)
	{
	  error(5,"%");
	  return 0.0;
	}
	else 
	  return m1%n1;
  }
  else
  {
  error(0,"");
  return 0.0;
  }
}

static void ReChange(double m, char *str)
{
	 int i=0,j,n,t;
 char str2[50];
 int flag = FALSE;
 
 if(fabs(m)<1e-14)
  {
  str[0]='0';
  str[1]='\0';
  }
else
{
  if(m<1e-15)
  {
   str[0]='-';
   i++;
   m=-m;
  }
  n=(int)m; //n为整数部分
  m=m-n;   //m为小数部分
  j = 0;
  if(n==0)
  {
  str[i]='0';
  i++;
  }
  while(n!=0)
  {
   if(n%jin >= 10)
    str2[j] = n%jin +'a'-10;
   else
    str2[j] = n%jin+'0';
    n = n /jin;
    j++;
  }
  while(j!=0)
  {
   str[i]=str2[j-1];
   i++;
   j--;
  }
  if(fabs(m)>1e-13)
  {
  str[i]='.';
  flag = TRUE;
  i++;
  t=0;
  }
    while(fabs(m)>1e-6&&t<6)
    {
    n = (int)(m*jin);
    if(n%jin >= 10)
    str[i] = n%jin +'a'-10;
    else
    str[i] = n%jin+'0';
    m = m*jin-n;
    i++;
	t++;
    }
     str[i] = '\0';
   }
if(flag)
{
for(j=i-1;str[j]=='0';j--);
if(str[j]=='.')
 str[j]='\0';
else
 str[j+1]='\0';
}
}

static double Change(char *str, int n)
{
 char *p;
 char *q;
 char str2[2];
 double m = 0.0;
 int er = FALSE;
 int i,k;
 str2[1]='\0';
 if(!str || !str[0])
   {
 //   printf("error!\n");
     return 0.0;
   }
 for(i=0;i<identnum&&strcmp(ident[i].name,str);i++);
 if(i<identnum)
 {
	 return ident[i].m;
 }
 else
 {
 for(p = str; *p && *p != '.';p++,n++);
 k=n;
 for(q = p-1,i=0;q!=str-1;q--,i++,k--)
   {
    if(*q>='0'&&*q<='9'||*q>='a'&&*q<='f'||*q>='A'&&*q<='Z')
    {
     if(jin == 16)
     {
      if(*q>='0'&&*q<='9')
       m = m + (*q - '0')*pow((double)jin,(double)i);
      else if(*q>='a'&&*q<='f')
       m = m + (*q - 'a'+10)*pow((double)jin,(double)i);
      else if(*q>='A'&&*q<='Z')
       m = m + (*q - 'A'+10)*pow((double)jin,(double)i);
     }
     else if(*q-'0'>=jin)
	 {
	  str2[0]=*q;
      error(2,str2);
	 }
     else 
      m = m + (*q - '0')*pow((double)jin,(double)i);
    }
    else
	{
	 str2[0]=*q;
     error(1,str2);
	}
   }//for
 if(*p == '.')
  {
    for(p ++,i = 1;*p&&*p!=' ';p++,i++,n++)
     {
      if(*p>='0'&&*p<='9'||*p>='a'&&*p<='f'||*p>='A'&&*p<='Z')
      {
       if(jin == 16)
       {
      if(*p>='0'&&*p<='9')
       m = m + (*p - '0')*pow((double)jin,(double)(0-i));
      else if(*p>='a'&&*p<='f')
       m = m + (*p - 'a'+10)*pow((double)jin,(double)(0-i));
      else if(*p>='A'&&*p<='Z')
       m = m + (*p - 'A'+10)*pow((double)jin,(double)(0-i));
     }
     else if(*p-'0'>=jin)
	 {
	  str2[0]=*p;
      error(2,str2);
	 }
     else
      m = m + (*p - '0')*pow((double)jin,(double)(0-i));
    }// if(*p>='0'&&*p<='9'||*p>='a'&&*p<='f'||*p>='A'&&*p<='Z')
    else
	{
	 str2[0]=*p;
     error(1,str2);
	 }
     }
  }
 return m;
 }
}

static double funtion(char *str, char *str1, int n)
{

#if 1

  double m;
  int i;
 if(!strcmp("ave",str)||!strcmp("sum",str)||!strcmp("vari",str)||!strcmp("avar",str))
 {//统计函数
	double si[MAXLEN];
	char can[MAXLEN];
	int n=0,j=0;
    for(i=0;str1[i-1]!='\0';i++)
	{
		if(str1[i]==','||str1[i]==' '||str1[i]=='\0')
		{
			can[j]='\0';
            si[n]=Change(can,n+i);
			  j = 0;
				n++;
		}
	   else 
	   {
		 can[j]=str1[i];
		 j++;
	   }
	}
    if(!strcmp("ave",str))  //平均书
		return ave(n,si);
	else if(!strcmp("sum",str)) //和
		return sum(n,si);
	else if(!strcmp("avar",str))  //均方差
		return sqrt(variance(n,si));
	else 
		return variance(n,si);   //方差
 }
 else
 {
  char *str2 = str1;
  /*
  if(jiao==FALSE)
  {
  	str2.Replace("p","3.141592653");
  }*/
  
  for(i=0;i < strlen(str2);i++)
  {
	  str1[i]=str2[i];
  }
  
  str1[i]='\0';
  m = Count(str1,n);
  if(errornum==0 && funnum==0)
  {
   funflag=TRUE;
   strcpy(hisfun,str);
  }
  else
  {
	 memset(hisfun,'\0',20);
	 hisop='\0';
  }
  funnum++;
 if(strcmp("acos",str) == 0)
  return acos(m);
 else if(strcmp("asin",str) == 0)
  return asin(m);
 else if(strcmp("atan",str) == 0)
  return atan(m);
 else if(strcmp("cos",str) == 0)
 {
   if(jiao)
    m = m/57.295827;
  return cos(m);
 }
  else if(strcmp("sin",str) == 0)
  {
  if(jiao)
    m = m/57.295827;
  return sin(m);
  }
 else if(strcmp("tan",str) == 0)
 {
  if(jiao)
    m = m/57.295827;
  return tan(m);
 }
 else if(strcmp("ctag",str) == 0)
  return 1.0/tan(m);
 else if(strcmp("actag",str) == 0)
  return 1.57-atan(m);
  else if(strcmp("sqrt",str) == 0)
  return sqrt(m);
 else if(strcmp("log",str) == 0)
  return log(m);
 else
 {
  error(3,str);
  memset(hisfun,'\0',20);
  opflag=FALSE;
  return 0.0;
 }
 }
 return 0.0;
 
#endif

  return 0.0;
  
}

static double Count(char* express, int m)
{
   char *optr;   //operating stack
   double *opnd;  //data stack
   
   char str[MAXLEN],str1[MAXLEN],err[MAXLEN];
   char op_stk[MAXLEN];
   double dat_stk[MAXLEN];
   int i,j,k,n,len;
   
   optr = (char *)op_stk;
   *optr = '\0';
   optr ++;
   opnd = (double *)dat_stk;
   n = 0;
   
   len=strlen(express);
   while(n <= len)
    {
      if(InOP(express[n])) //operating signs
      {
      if(express[n-1]==')'&&express[n]=='(') //省略*
	  {
		*optr='*';
		optr++;
	  }
	 if(express[n]=='-' && (InOP(express[n-1])&&express[n-1]!=')'||n==0))
         {
            *optr = '~';
	    optr ++;
            n++;
            m++;
         }
	else if((express[n-1]=='('&&express[n]!='-'||INoper(express[n-1]))&&INoper(express[n]))
	{
        err[0]=express[n-1];
		err[1]=express[n];
		err[2]='\0';
		error(6,err);
		n++;
	}
        else
        {
	switch(precede(*(optr-1),express[n]))
         {
          case '<':  //push
	   *optr = express[n];
	   n++;
	   optr++;
	   break;
          case '=': //pop
	   optr --;
	   n++;
	   break;
          case '>':
          if(*(optr-1)=='~')  //only one operater data
           {
            *(opnd-1)=-*(opnd-1);
            optr--;
           }
          else
           {
           *(opnd-2) = operat(*(opnd-2),*(opnd-1),*(optr-1));  //operating
           opnd--;
           optr--;
           }
           break;
		  default:
			  n++;
			  break;
         }
         }
      }//if(InOP(express[n]))
     else 
      {
       i = 0;
       while(!InOP(express[n])) //copy the data to the str
        {
	    str[i] = express[n];
	    i++;
	    n++;
        }

	m+=i;
	str[i] = '\0';
       if(express[n]=='(')  //str is the funtion name
        { 
	 k = 1;
	 j = 0;
	 n++;
	 while(k!=0&&express[n]!='\0')   //copy the funtion parameter
       {
	   str1[j] = express[n];
	   j++;
	   n++;
	   if(express[n]=='(')
	    k++;
	   else if(express[n]==')')
	    k--;
	  }
	 if(express[n]==')')
	 {
	  n++;
	  str1[j] = '\0';
	 *opnd = funtion(str,str1,m);
	 opnd++;
	 m += j;
	 }
	else
	 error(3,str1);
	}//if(express[n]=='(')
       else
        {
	  *opnd = Change(str,m);
	  opnd++;
        }
       }
    }
   if(*optr=='\0')
    return *(opnd-1);
   else 
   {
	if(errornum==0)
    error(0,express);
    return 0.0;
   }
}

static BOOL InOP(char ch)
{
     if(ch=='('||ch==')'||ch=='+'||ch=='-'||ch=='*'||ch=='/'||ch=='%'||ch=='\0'||ch=='^'||ch=='~')//~表示取反 ^表示乘方
  return TRUE;
 else
  return FALSE;
}

char precede(char a, char b)
{
  	 if(a == '+' || a == '-')
  {
   if(b == '*'||b == '/'||b == '('||b=='%'||b=='^'||b=='~')
    return '<';
   else return '>';
  }//if(a == '+' || a == '-')  
 else if(a == '*' || a == '/'||a=='%')
   {
     if(b == '('||b=='^'||b=='~')
     return '<';
     else 
      return '>';
   }//else if(a == '*' || a == '/'||a=='%')
 else if(a == '(')
   {
     if(b == ')')
      return '=';
     else if(b == '\0')
	 {
	  error(4,"");
      return '\0';
	 }
     else 
      return '<';
   }//else if(a == '(')
 else if(a == ')')
 {
  error(4,"");
  return '\0';
 }
  
 else if(a == '\0')
 {
  if(b == '\0')
   return '=';
  else if(b == ')')
  {
   error(4,"");
   return '\0';
  }
  else return '<';
 }
 else if(a == '^')
  {
   if(b == '(')
    return '<';
   else return '>';
  } 
 else if(a == '~')
  {
   if(b == '(' ||b=='^')
    return '<';
   else
    return '>';
  }
 error(0,"");
 return '\0';
}

static void str_back(char *dst)
{
	if(dst==0)		return;
	if(dst[0]==0)	return;

	while(1)
	{
		if(*(dst+1)==0)
		{
			*dst=0;
			return;
		}
		dst++;
	}

}

static void str_add(char *dst,char *src)
{
	while(1)
	{
		if(*dst==0)	break;
		dst++;
	}
	
	while(*src)
	{
		*dst++=*src++;
	}
	*dst='\0';
}

static void OnEql(char *result,char *input)
{
	int hise=TRUE;
	double m;
	char reid[3][10];
	int len,erro,i,j,k,n;
	char express[MAXLEN],line[MAXLEN],str1[MAXLEN],resul[MAXLEN];

	////
	
	//CString result;
	//m_EditBox.GetLine(1,Line);
	
	result[0]='\0';
	
	for(i=0;i<MAXLEN;i++)
	{
		line[i]=input[i];
		if(input[i]==0)	break;
	}
	
	
	errornum=erro=0;
	opnum  =0;
	funnum =0;
	opflag =FALSE;
	funflag=FALSE;
	
	
	if(strlen(line)!=0)
	{
	for(i=0;i<2;i++)
		memset(reid[i],'\0',10);
    k = 0;
	len=strlen(line);
	
    for(j=0;j<=len;j++)
	{
	  if(line[j]==';'||!line[j])
	  {
		 erro=errornum;
		 str1[k]='\0';
		 k=0;
		 m = Count(str1,j);
		 if(erro==errornum)
		 {
			ReChange(m,resul);
		////?	result+=resul;
			str_add(result,resul);
		 }
       else
	   {
		////?   result+=str1;
		str_add(result,str1);
	   }
	   if(line[j])
	   {
		 ////?result+=';';
		 str_add(result,";");
		 
	   }
	  }//if(Line[j]==';')
	  else if(line[j]=='=')
	  {
		  if(k==0)
		  {
			  error(7,str1);
		  }
		  else
		  {
			  erro=errornum;
			  str1[k]='\0';
			  k=0;
		  for( i=0;i<identnum&&strcmp(ident[i].name,str1);i++);
		    if(i==identnum)
			{
		     strcpy(ident[identnum].name,str1);
		     identnum++;
			}
		  }
		  j++;
		  for(n=0;line[j]!=';'&&line[j];n++,j++)
			  express[n]=line[j];
		  express[n]='\0';
          m = Count(express,j);
		  if(erro==errornum)
		  {
			  ident[i].m=m;
		////?	  result+=ident[i].name;
		////?	  result+='=';
			str_add(result,ident[i].name);
			str_add(result,"=");
			
             ReChange(m,resul);
		////?	 result+=resul;
			str_add(result,resul);
		  }
		 else 
		 {
         ////? 	  result+=ident[i].name;
		////?	  result+='=';
         ////?     result+=express;
         
         	str_add(result,ident[i].name);
         	str_add(result,"=");
         	str_add(result,express);
		 }
		 if(line[j])
		 {
			////? result+=';';
			str_add(result,";");
			
		 }
	  }//else if(Line[j]=='=')
	  else 
	  {
		str1[k++]=line[j];
	  }
	}//for(j=0;j<strlen(Line);j++)

   if(hisop!='\0'&& !opflag&&!funflag&&hisfun[0]=='\0'&&opnum==0&&funnum==0)
   {
	 m = operat(m,hisdata,hisop);
	 ReChange(m,resul);
	 result=resul;
	 hise=FALSE;
   }
    else if(hisfun[0]!='\0'&&!opflag&&!funflag&&hisop=='\0'&&opnum==0&&funnum==0)
	{
		m=funtion(hisfun, line,strlen(line));
		ReChange(m,resul);
		result=resul;
		hise=FALSE;
	}
   if(errornum==0)
   {
	if(strcmp(stack[now],result))    //只有数据不相同是才入历史栈
	{
	now = (now+1)%MAXLEN;
	if(now==low)
		low = (low+1)%MAXLEN;
	strcpy(stack[now],result);
	if(now<low||now-low>1)
	 //?? m_back.EnableWindow(TRUE);
	high=now;
	}
   }
   if(hise)
	{
	//??m_backexp.EnableWindow(TRUE);
	if(strcmp(hisexp[nowexp],line))    //只有表达式不相同是才入历史栈
	{
	nowexp = (nowexp+1)%MAXLEN;
	if(nowexp==lowexp)
		lowexp = (lowexp+1)%MAXLEN;
	strcpy(hisexp[nowexp],line);
	 //??m_backexp.EnableWindow(TRUE);
	highexp=nowexp;
	}
	}
	//??m_EditBox.SetSel(0, -1);
   //?? m_EditBox.Clear();
	//??m_EditBox.ReplaceSel(result);
  }
  else
  {
	  error(8,"");
  }
}

/*============================================================================*/

static	WCHAR wstr_buf[1024] __EXRAM;
static	char	str_buf[1024] __EXRAM;

//BS_FLAT

static	LRESULT	WinProc(HWND hwnd,U32 msg,WPARAM wParam,LPARAM lParam)
{
	int i,x,y;
	RECT rc,rc0,*m_rc;
	WCHAR buf[128];
	////
	
	switch(msg)
	{
	
		case	WM_CREATE:
		
				input[0]	=0;
				output[0]	=0;
				jiao 		=TRUE;
				jin  		=10;
				
				m_rc =new RECT[40];
				
				GetClientRect(hwnd,&rc);
				
				x=4;
				y=64;
				
				CreateWindow(GROUPBOX,L"",WS_VISIBLE,x,y,rc.w>>1,60,hwnd,IDC_MODE_GROUP,NULL,NULL);
				//SetWindowColor(GetDlgItem(hwnd,IDC_MODE_GROUP),RGB(0,0,0),RGB_TRANS,GetWindowBkColor(hwnd));
				
				GetClientRect(GetDlgItem(hwnd,IDC_MODE_GROUP),&rc);
			
				//GetMatrixRectangle(m_rc,2,16,rc.w,rc.h-16,3,3,2,2);
				
				rc0.x =2;
				rc0.y =16;
				rc0.w =rc.w;
				rc0.h =rc.h-16;
				MakeMatrixRect(m_rc,&rc0,2,2,2,2);		
				for(i=0;i<4;i++)
				{					
					CreateWindow(BUTTON,mode_text[i],WS_VISIBLE|BS_RADIOBOX,m_rc[i].x,m_rc[i].y,m_rc[i].w,m_rc[i].h,GetDlgItem(hwnd,IDC_MODE_GROUP),mode_id[i],NULL,NULL);
	
					if(mode_id[i]==IDC_DEC)
					{
						HWND wnd;
						
						wnd	=GetDlgItem(hwnd,IDC_MODE_GROUP);
						wnd	=GetDlgItem(wnd,IDC_DEC);
						SendMessage(wnd,BM_SETSTATE,BST_CHECKED,0);
					
					}
				}
				////

				CreateWindow(BUTTON,L"-",WS_VISIBLE|WS_OWNERDRAW|WS_TRANSPARENT,747,15,36,36,hwnd,IDC_EIXT,NULL,NULL);
				
				GetClientRect(hwnd,&rc);
				x=(rc.w>>1)+8;
				
				CreateWindow(GROUPBOX,L"",WS_VISIBLE,x,y,(rc.w>>1)-8*2,60,hwnd,IDC_TRIFUN_GROUP,NULL,NULL);
								
				//SetWindowColor(GetDlgItem(hwnd,IDC_TRIFUN_GROUP),RGB(0,0,0),RGB_TRANS,GetWindowBkColor(hwnd));
				
				GetClientRect(GetDlgItem(hwnd,IDC_TRIFUN_GROUP),&rc);
				//GetMatrixRectangle(m_rc,2,16,rc.w,rc.h-16,3,3,2,1);
				rc0.x =2;
				rc0.y =16;
				rc0.w =rc.w;
				rc0.h =rc.h-16;
				MakeMatrixRect(m_rc,&rc0,3,3,2,1);		
				for(i=0;i<2;i++)
				{
					CreateWindow(	BUTTON,
									tri_fun_text[i],
									WS_VISIBLE|BS_RADIOBOX,
									m_rc[i].x,m_rc[i].y,m_rc[i].w,m_rc[i].h,
									GetDlgItem(hwnd,IDC_TRIFUN_GROUP),
									tri_fun_id[i],NULL,NULL);
									
					if(tri_fun_id[i]==IDC_ANGLE)
					{
						HWND wnd;
						
						wnd	=GetDlgItem(hwnd,IDC_TRIFUN_GROUP);
						wnd	=GetDlgItem(wnd,IDC_ANGLE);
						SendMessage(wnd,BM_SETSTATE,BST_CHECKED,0);
					
					}

				}
					
				////
					/*
				for(i=0;i<40;i++)
				{
				  btn_id[i]=i;
				}
				  */

				x=0;
				y+=64;
				GetClientRect(hwnd,&rc);
				//GetMatrixRectangle(m_rc,x,y,rc.w,rc.h-y,4,4,8,5);
				rc0.x =x;
				rc0.y =y;
				rc0.w =rc.w;
				rc0.h =rc.h-y;
				MakeMatrixRect(m_rc,&rc0,4,4,8,5);		
				
	
			//	DebugPutHex(0,16,(U8*)&fd);
			//	DebugPutHex(0,16,(U8*)&ff);

				for(i=0;i<40;i++)
				{
					CreateWindow(BUTTON,btn_text[i],WS_OWNERDRAW|WS_VISIBLE|WS_BORDER,m_rc[i].x,m_rc[i].y,m_rc[i].w,m_rc[i].h,hwnd,btn_id[i],NULL,NULL);
					//DebugPrintf("# - '%s' - %04XH\r\n",btn_text[i],btn_id[i]);
				#if 0
					switch(btn_id[i])
					{
						case	IDC_0:
						case	IDC_1:
						case	IDC_2:
						case	IDC_3:
						case	IDC_4:
						case	IDC_5:
						case	IDC_6:
						case	IDC_7:
						case	IDC_8:
						case	IDC_9:
						case	IDC_A:
						case	IDC_B:
						case	IDC_C:
						case	IDC_D:
						case	IDC_E:
						case	IDC_F:
								SetWindowLong(GetDlgItem(hwnd,btn_id[i]),GWL_BKCOLOR,RGB(20,128,200));
								break;
								////
								
						case	IDC_SIN:
						case	IDC_ASIN:
						case	IDC_COS:
						case	IDC_ACOS:
						case	IDC_TAN:
						case	IDC_ATAN:
						case	IDC_LOG:
						case	IDC_SUM:
						case	IDC_SQRT:
						case	IDC_AVE:
								SetWindowLong(GetDlgItem(hwnd,btn_id[i]),GWL_BKCOLOR,RGB(240,128,20));
								break;
								////
								
						case	IDC_SUR:
						case	IDC_DIV:
						case	IDC_MUL:
						case	IDC_SUB:
						case	IDC_ADD:
						case	IDC_XOR:
						case	IDC_SEM:
						case	IDC_DOT:
						case	IDC_COM:
						case	IDC_LBR:
						case	IDC_RBR:
								SetWindowLong(GetDlgItem(hwnd,btn_id[i]),GWL_BKCOLOR,RGB(200,20,128));
								break;
								////
						
						case	IDC_EQU:
								SetWindowLong(GetDlgItem(hwnd,btn_id[i]),GWL_BKCOLOR,RGB(20,200,20));
								break;
								////
								
						case	IDC_CLR:
								SetWindowLong(GetDlgItem(hwnd,btn_id[i]),GWL_BKCOLOR,RGB(200,20,20));
								break;
								////
									
						case	IDC_BACK:
								SetWindowLong(GetDlgItem(hwnd,btn_id[i]),GWL_BKCOLOR,RGB(200,200,20));
								break;
								////
									
				
					}
				#endif
					
					UpdateWindow(GetDlgItem(hwnd,btn_id[i]));
					
				}
				
				delete m_rc;

				break;
				////////
				
		case	WM_NOTIFY:
				{
					U16	id,code;
					HWND wnd;
					
					code	=HIWORD(wParam);
					id		=LOWORD(wParam);
							
				//	if(code==BN_PUSHED)
					{
						
						switch(id)
						{
							case	IDC_EIXT:
									 PostCloseMessage(hwnd);    // 发送关闭窗口的消息
									break;

							case	IDC_BIN:
									jin	=2;
									output[0]	=0;
									break;
									////
							case	IDC_OCT:
									jin	=8;
									output[0]	=0;
									break;
									////
							case	IDC_DEC:
									jin	=10;
									output[0]	=0;							
									break;
									////
							case	IDC_HEX:
									jin	=16;
									output[0]	=0;
									break;
									////
									
							case	IDC_ANGLE:
									jiao =TRUE;
									output[0]	=0;
									break;
									////
									
							case	IDC_RADIAN:
									jiao =FALSE;
									output[0]	=0;
									break;
									////
							
							case	IDC_CLR:
									input[0]	=0;
									output[0]	=0;
									break;
									////
									
							case	IDC_BACK:
									output[0]	=0;
									str_back(input);
									//StrPrintf(output,"%s",input);
									break;
									////
									
							case	IDC_EQU:
									
									OnEql(output,input);
									break;
									////
						
							default:
							
									if(jin==2)
									{
										switch(id)
										{
											case	IDC_2:
											case	IDC_3:
											case	IDC_4:
											case	IDC_5:
											case	IDC_6:
											case	IDC_7:
											case	IDC_8:
											case	IDC_9:
											case	IDC_A:
											case	IDC_B:
											case	IDC_C:
											case	IDC_D:
											case	IDC_E:
											case	IDC_F:
													
													break;
													////
											default:
													wnd	=GetDlgItem(hwnd,id);
													GetWindowText(wnd,buf,128);
													x_wcstombs_cp936(str_buf,buf,1024);
													str_add(input,str_buf);
													output[0]	=0;
													break;
													/////

											
										}
									}
									
									if(jin==8)
									{
										switch(id)
										{
											case	IDC_8:
											case	IDC_9:
											case	IDC_A:
											case	IDC_B:
											case	IDC_C:
											case	IDC_D:
											case	IDC_E:
											case	IDC_F:
													
													break;
													////
											default:
													wnd	=GetDlgItem(hwnd,id);
													GetWindowText(wnd,buf,128);
													x_wcstombs_cp936(str_buf,buf,1024);
													str_add(input,str_buf);
													output[0]	=0;
													break;
													/////

											
										}
									}
									
									if(jin==10)
									{
										switch(id)
										{
											case	IDC_A:
											case	IDC_B:
											case	IDC_C:
											case	IDC_D:
											case	IDC_E:
											case	IDC_F:
													
													break;
													////
											default:
													wnd	=GetDlgItem(hwnd,id);
													GetWindowText(wnd,buf,128);
													x_wcstombs_cp936(str_buf,buf,1024);
													str_add(input,str_buf);
													output[0]	=0;
													break;
													/////

											
										}
									}
									
									if(jin==16)
									{
										wnd	=GetDlgItem(hwnd,id);
										GetWindowText(wnd,buf,128);
										x_wcstombs_cp936(str_buf,buf,1024);
										str_add(input,str_buf);
										output[0]	=0;
										break;
										/////
										
									}
									
									
						}
					
						
						InvalidateRect(hwnd,0,0);
					}
				}
		
				break;
				/////////
#if 1
		case	WM_DRAWITEM:
				{	
					DRAWITEM_HDR *item;
					int x,y;
					HDC hdc;
					//HBRUSH hbr;
					
					item=(DRAWITEM_HDR*)lParam;
					hdc =item->hDC;
				
					switch(item->ID)
					{
						case  IDC_EIXT:
						{
							exit_owner_draw(item);
							return TRUE;
						}
					
						case	IDC_0:
						case	IDC_1:
						case	IDC_2:
						case	IDC_3:
						case	IDC_4:
						case	IDC_5:
						case	IDC_6:
						case	IDC_7:
						case	IDC_8:
						case	IDC_9:
						case	IDC_A:
						case	IDC_B:
						case	IDC_C:
						case	IDC_D:
						case	IDC_E:
						case	IDC_F:
						case	IDC_SUR:
						case	IDC_CLR:
						case	IDC_SIN:
						case	IDC_ASIN:
						case	IDC_DIV:
						case	IDC_BACK:
						case	IDC_COS:
						case	IDC_ACOS:
						case	IDC_MUL:
						case	IDC_XOR:
						case	IDC_TAN:
						case	IDC_ATAN:
						case	IDC_SUB:
						case	IDC_SEM:
						case	IDC_LOG:
						case	IDC_SUM:
						case	IDC_DOT:
						case	IDC_COM:
						case	IDC_LBR:
						case	IDC_RBR:
						case	IDC_ADD:
						case	IDC_EQU:
						case	IDC_SQRT:
						case	IDC_AVE:

						GetWindowText(item->hwnd,buf,128);
						rc =item->rc;
						if(SendMessage(item->hwnd,BM_GETSTATE,0,0)&BST_PUSHED)
						{

							SetPenColor(hdc,MapRGB(hdc,10,170,170));
							SetBrushColor(hdc,MapRGB(hdc,0,100,200));

							DrawRect(hdc,&rc);

							InflateRect(&rc,-1,-1);
							FillRect(hdc,&rc);

							OffsetRect(&rc,2,2);
							SetTextColor(hdc,MapRGB(hdc,50,250,100));
							DrawText(item->hDC,buf,-1,&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
							
						}
						else
						{
							GradientFillRect(hdc,&rc,MapRGB(hdc,200,200,200),MapRGB(hdc,0,30,30),TRUE);
							
							SetTextColor(hdc,MapRGB(hdc,250,250,250));
							SetPenColor(hdc,MapRGB(hdc,10,70,70));
							DrawText(item->hDC,buf,-1,&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_BORDER);

						}
						return TRUE;
						///////
						
						default:
								return DefWindowProc(hwnd,msg,wParam,lParam);
					}
				}
				break;
				////////
#endif

		case	WM_PAINT:
				{
					HDC hdc;
					RECT rc0;
					HBRUSH hpen;
					PAINTSTRUCT ps;
					
					
					hdc=BeginPaint(hwnd,&ps);
					if(hdc)
					{
						GetClientRect(hwnd,&rc);
						rc.x	=6;
						rc.y	=10;
						rc.w	-=6*2+66;
						rc.h	=48;
						
						SetBrushColor(hdc, MapRGB(hdc, 250, 250, 250));
						EnableAntiAlias(hdc, TRUE);                             // 使能抗锯齿
						FillRoundRect(hdc,&rc,10);
						EnableAntiAlias(hdc, FALSE);                            // 禁用抗锯齿
						
						//??hpen	=CreatePen(PS_SOLID,1,RGB(0,0,0));
						//??FrameRect(hdc,&rc,hpen);
						//??DeleteObject(hpen);
						
						////
						
						//SetBkMode(hdc,TRANSPARENT);
						
						if(input[0])
						{
							//??SetFontSize(hdc,16,0);
							SetTextColor(hdc,MapRGB(hdc,0,0,0));
							rc0.x	=10;
							rc0.y	=10;
							rc0.w	=rc.w-4*2;
							rc0.h	=24;
							x_mbstowcs_cp936(wstr_buf,input,1024);
							DrawText(hdc,wstr_buf,-1,&rc0,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
						}
						
						if(output[0])
						{
							//??SetFontSize(hdc,24,0);
							SetTextColor(hdc,MapRGB(hdc,200,0,0));
							rc0.x	=10;
							rc0.y	=4+20+10;
							rc0.w	=rc.w-4*2;
							rc0.h	=24;
							x_mbstowcs_cp936(wstr_buf,output,1024);
							DrawText(hdc,wstr_buf,-1,&rc0,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
						}
						////
						
						//??SetFontSize(hdc,12,0);
						SetTextColor(hdc,MapRGB(hdc,200,0,0));
//						switch(jin)
//						{
//							case	2:
//									
//									TextOut(hdc,8,4+48-16,L"BIN",-1);
//									break;
//									////
//						
//							case	8:
//									TextOut(hdc,8,4+48-16,L"OCT",-1);
//									break;
//									////
//						
//							case	10:
//									TextOut(hdc,8,4+48-16,L"DEC",-1);
//									break;
//									////
//						
//							case	16:
//									TextOut(hdc,8,4+48-16,L"HEX",-1);
//									break;
//									////
//						
//						}
						////
					}
					
					EndPaint(hwnd,&ps);
				}
				break;
				////
				
		default:
		return	DefWindowProc(hwnd,msg,wParam,lParam);
	}

	return WM_NULL;
}

/*============================================================================*/
/*============================================================================*/

extern "C" int	Calculator_WinMain(void)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS wcex;
	////
	

	wcex.Tag 	= WNDCLASS_TAG;

	wcex.Style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WinProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;


	hwnd	=CreateWindowEx(WS_EX_FRAMEBUFFER,&wcex,L" ",WS_VISIBLE|WS_CLIPCHILDREN,
							0,0,GUI_XSIZE,GUI_YSIZE,
							NULL,0,NULL,NULL);
								
	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);
	while(GetMessage(&msg,hwnd))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}	
	
	return	1;
	
}
