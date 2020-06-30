#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "bsp_usart_gsm.h"
#include "bsp_sim900a.h"
#include "./usart/bsp_debug_usart.h"
#include "def.h"
#include "ff.h"

static uint8_t MaxMessAdd=50;
uint8_t Sim900aReceiveAcc=0;    // 接收完成

//0表示成功，1表示失败

uint8_t sim900a_cmd(char *cmd, char *reply,uint32_t waittime )
{    
  SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据
  SIM900A_TX(cmd);                    //发送命令
  if(reply == 0)                      //不需要接收数据
  {
    return SIM900A_TRUE;
  }
  SIM900A_DELAY(waittime);                 //延时
  return sim900a_cmd_check(reply);    //对接收数据进行处理
}

uint8_t sim900a_cmd_isr(char *cmd, char *reply,uint32_t waittime )    // 使用中断
{    
  SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据
  SIM900A_TX(cmd);                    //发送命令
  if(reply == 0)                      //不需要接收数据
  {
    return SIM900A_TRUE;
  }
  else
  {
    while(!Sim900aReceiveAcc);
  }
  Sim900aReceiveAcc=0;
//  SIM900A_DELAY(waittime);                 //延时
  return sim900a_cmd_check(reply);    //对接收数据进行处理
}

//0表示成功，1表示失败
uint8_t sim900a_cmd_check(char *reply)
{
  uint16_t len;
  uint16_t n;
  uint16_t off;
  char *redata;    
  redata = SIM900A_RX(len);   //接收数据    
  n = 0;
  off = 0;
  while((n + off)<len)
  {
    if(reply[n] == 0)                 //数据为空或者比较完毕
    {
      return SIM900A_TRUE;
    }        
    if(redata[ n + off]== reply[n])
    {
      n++;                //移动到下一个接收数据
    }
    else
    {
      off++;              //进行下一轮匹配
      n=0;                //重来
    }
  }
  if(reply[n]==0)   //刚好匹配完毕
  {
    return SIM900A_TRUE;
  }    
  return SIM900A_FALSE;       //跳出循环表示比较完毕后都没有相同的数据，因此跳出
}

char * sim900a_waitask(uint8_t waitask_hook(void))      //等待有数据应答
{
  uint16_t len=0;
  char *redata;
  do{
    redata = SIM900A_RX(len);   //接收数据
    if(waitask_hook!=0)
    {
        if(waitask_hook()==SIM900A_TRUE)           //返回 SIM900A_TRUE 表示检测到事件，需要退出
        {
            redata = 0;
            return redata;               
        }
    }
  }while(len==0);                 //接收数据为0时一直等待
  SIM900A_DELAY(20);              //延时，确保能接收到全部数据（115200波特率下，每ms能接收11.52个字节）
  return redata;
}


//本机号码
//0表示成功，1表示失败
uint8_t sim900a_cnum(char *num)
{
  char *redata;
  uint16_t len;
  if(sim900a_cmd("AT+CNUM\r","OK", 100) != SIM900A_TRUE)
  {
    return SIM900A_FALSE;
  }  
  redata = SIM900A_RX(len);   //接收数据  
  if(len == 0)
  {
    return SIM900A_FALSE;
  }  
  //第一个逗号后面的数据为:"本机号码"
  while(*redata != ',')
  {
    len--;
    if(len==0)
    {
        return SIM900A_FALSE;
    }
    redata++;
  }
  redata+=2;  
  while(*redata != '"')
  {
    *num++ = *redata++;
  }
  *num = 0;               //字符串结尾需要清0
  return SIM900A_TRUE;
}


//初始化并检测模块
//0表示成功，1表示失败
uint8_t sim900a_init(void)
{
	char *redata;
	uint16_t len;
	SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据
	sim900a_config();					          //初始化串口  
  SIM900A_DELAY(10);
	if(sim900a_cmd("ATE0\r","OK", 200) != SIM900A_TRUE)
	{
		return SIM900A_FALSE;
	}  
  SIM900A_CLEAN_RX();                                          // 清空了接收缓冲区数据
  
  if(sim900a_cmd("AT+COLP=1\r", "OK", 100) != SIM900A_TRUE)    // 设置被叫号码显示
  {
    return SIM900A_FALSE;
  }
  SIM900A_CLEAN_RX(); 
  
	if(sim900a_cmd("AT+CGMM\r","OK", 100) != SIM900A_TRUE)
	{
		return SIM900A_FALSE;
	}	
	redata = SIM900A_RX(len);   //接收数据	
	if(len == 0)
	{
		return SIM900A_FALSE;
	}
	if (strstr(redata,"SIMCOM_SIM900A") != 0)
	{
		return SIM900A_TRUE;
	}
  if (strstr(redata,"SIMCOM_SIM800A") != 0)
	{
		return SIM900A_TRUE;
	}
	else
		return SIM900A_FALSE;
}

/*---------------------------------------------------------------------*/

//发起拨打电话（不管接不接通）
void sim900a_call(char *num)
{
  SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据  
  sim900a_tx_printf("ATD%s;\r",num);
  //拨打后是不返回数据的
  //不管任何应答，开头都是"\r\n",即真正有效的数据是从第3个字节开始
  //对方挂掉电话（没接通），返回：BUSY
  //对方接听了电话：+COLP: "555",129,"",0,"9648674F98DE"   OK
  //对方接听了电话,然后挂掉：NO CARRIER
  //对方超时没接电话：NO ANSWER
}

//来电电话
//0表示成功，1表示失败
uint8_t IsRing(char *num)
{
  char *redata;
  uint16_t len;  
  if(sim900a_cmd_check("RING"))
  {
    return SIM900A_FALSE;
  }  
  redata = SIM900A_RX(len);   //接收数据
  if(len == 0)
  {
    return SIM900A_FALSE;
  }  
  //第一个逗号后面的数据为:”号码“
  while(*redata != ':')
  {
    len--;
    if(len==0)
    {
        return SIM900A_FALSE;
    }
    redata++;
  }
  redata+=3;
  while(*redata != '"')
  {
    *num++ = *redata++;
  }
  *num = 0;               //字符串结尾需要清0
  SIM900A_CLEAN_RX();
  return SIM900A_TRUE;
}

uint8_t IsNOCARRIER(void)
{
  if(sim900a_cmd_check("NO CARRIER"))
  {
    return SIM900A_FALSE;
  }  
  return SIM900A_TRUE;
}
/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/

uint8_t IsASSIC(char * str)
{
  while(*str)
  {
    if(*str>0x7F)
    {
        return SIM900A_FALSE;
    }
    str++;
  }  
  return SIM900A_TRUE;
}



void sim900a_gbk2ucs2(char * ucs2,char * gbk)
{
  WCHAR   tmp;  
  while(*gbk)
  {
    if((*gbk&0xFF) < 0x7F)      //英文
    {        
      *ucs2++ = 0;
      *ucs2++ = *gbk++;  
    }
    else                        //中文
    {
      tmp = SIM900A_SWAP16(*(WCHAR *)gbk);      
      *(WCHAR *)ucs2 = SIM900A_SWAP16(ff_convert(tmp,1));
      gbk+=2;
      ucs2+=2;
    }
  }
}

void sim900a_char2hex(char *hex,char ch)
{
    const char numhex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    *hex++  = numhex[(ch & 0xF0)>>4];
    *hex    = numhex[ ch & 0x0F];
}

void sim900a_gbk2ucs2hex(char * ucs2hex,char * gbk)
{
  WCHAR   tmp;    
  while(*gbk)
  {
    if((*gbk&0xFF) < 0x7F)      //英文
    {        
      *ucs2hex++ = '0';
      *ucs2hex++ = '0';
      sim900a_char2hex(ucs2hex,*gbk);
      ucs2hex+=2;
      gbk++;  
    }
    else                        //中文
    {
      tmp = SIM900A_SWAP16(*(WCHAR *)gbk);
      tmp = ff_convert(tmp,1);
      sim900a_char2hex(ucs2hex,(char)(tmp>>8));
      ucs2hex+=2;
      sim900a_char2hex(ucs2hex,(char)tmp);
      ucs2hex+=2;
      gbk+=2;
    }
  }
  *ucs2hex=0;
}

void sim900a_utf82ucs2hex(char * ucs2hex,char * utf8,uint16_t utf8len)
{
    char *ucs,*tmp;//ucs用于存储unicode变量，tmp用作unicode变量的指针运算，如果运算后free tmp会出错
		
		ucs	= (char *)malloc(sizeof(char)*utf8len*2);		//UC编码全为2字节一个			
		
		if(ucs == NULL)
		{
				return;
		}
	
		tmp = ucs;	
	
		/* 转换成UCS编码 */
//		GUI_UC_ConvertUTF82UC((const char GUI_UNI_PTR*)utf8,utf8len,(unsigned short *)tmp,utf8len*2);
		
		while(!(*tmp == 0 && *(tmp+1) == 0))//非结束符
		{
			*(WCHAR *)tmp = SIM900A_SWAP16(*(WCHAR *)tmp);
			
			if(((*tmp)&0xFF) < 0x7F)      //英文
        {	
				
					sim900a_char2hex(ucs2hex,*tmp);
					ucs2hex+=2;
					tmp++;
					
					sim900a_char2hex(ucs2hex,*tmp);
					ucs2hex+=2;
					tmp++;
					
        }
			else
			{			
				sim900a_char2hex(ucs2hex,(char)(*tmp));
				ucs2hex+=2;
				tmp++;
				sim900a_char2hex(ucs2hex,(char)(*tmp));
				ucs2hex+=2;
				tmp++;
			}
		}
		
    *ucs2hex=0;
		
		free(ucs);
		ucs = NULL;
}


//发送短信（支持中英文,中文为GBK码）
void sim900a_sms(char *num,char *smstext)
{
  char ucsbuff[160];
  SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据
  if(IsASSIC(smstext)==SIM900A_TRUE)
  {
    //英文
    sim900a_tx_printf("AT+CSCS=\"GSM\"\r");     //"GSM"字符集
    SIM900A_DELAY(100);
    
    sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
    SIM900A_DELAY(100);
    
    sim900a_tx_printf("AT+CMGS=\"%s\"\r",num);  //电话号码
    SIM900A_DELAY(100);

    sim900a_tx_printf("%s",smstext);            //短信内容
    //SIM900A_DELAY(100);          
  }
  else
  {
    //中文
    sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");    //"UCS2"字符集
    SIM900A_DELAY(100);
    
    sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
    SIM900A_DELAY(100);
    
    sim900a_tx_printf("AT+CSMP=17,167,0,8\r");  //
    SIM900A_DELAY(100);
    
    sim900a_gbk2ucs2hex(ucsbuff,num);
    sim900a_tx_printf("AT+CMGS=\"%s\"\r",ucsbuff);  //UCS2的电话号码(需要转成 ucs2码)
    SIM900A_DELAY(100);
    
    sim900a_gbk2ucs2hex(ucsbuff,smstext);
    sim900a_tx_printf("%s",ucsbuff);          //UCS2的文本内容(需要转成 ucs2码)     
  }
  SIM900A_DELAY(1); 
	GSM_USART->DR=(u32)0x1A;		//发送十六进制数：0X1A,信息结束符号
}

//发送短信（支持中英文,输入时使用UTF8编码）
void sim900a_sms_utf8(char *num,char *smstext,uint16_t numlen,uint16_t textlen)
{
    char ucsbuff[160];
	  char end[2] = {0x1A,0x00};

    SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据
	
    if(IsASSIC(smstext)==SIM900A_TRUE)
    {
        //英文
        sim900a_tx_printf("AT+CSCS=\"GSM\"\r");     //"GSM"字符集
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",num);  //电话号码
        SIM900A_DELAY(100);

        sim900a_tx_printf("%s",smstext);            //短信内容
        //SIM900A_DELAY(100);          
    }
    else
    {
        //中文
        sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");    //"UCS2"字符集
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
        SIM900A_DELAY(100);
        
        sim900a_tx_printf("AT+CSMP=17,167,0,8\r");  //
        SIM900A_DELAY(100);
        
        sim900a_utf82ucs2hex(ucsbuff,num,numlen);
        sim900a_tx_printf("AT+CMGS=\"%s\"\r",ucsbuff);  //UCS2的电话号码(需要转成 ucs2码)
        SIM900A_DELAY(100);

        sim900a_utf82ucs2hex(ucsbuff,smstext,textlen);
        sim900a_tx_printf("%s\r",ucsbuff);          //UCS2的文本内容(需要转成 ucs2码)
        //SIM900A_DELAY(100);			
    }
    sim900a_tx_printf("%s",end);
}

uint16_t sim900a_ReadSaveAdd(void)
{
  char *redata,temp[20]={0};
  uint16_t len;
	char result=0;

	redata = SIM900A_RX(len);   //接收数据
  
  if(strstr(redata,"ERROR") != NULL)
  {
    return 0;
  }
  
  return atoi(redata+9);
}

//保存短信并发送（支持中英文,中文为GBK码）
uint16_t sim900a_save_sms(char *num,char *smstext)
{
  char ucsbuff[160];
  uint16_t Add;
  SIM900A_CLEAN_RX();                 //清空了接收缓冲区数据
  if(IsASSIC(smstext)==SIM900A_TRUE)
  {
    //英文
    sim900a_tx_printf("AT+CSCS=\"GSM\"\r");     //"GSM"字符集
    SIM900A_DELAY(100);
    
    sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
    SIM900A_DELAY(100);
    
    sim900a_tx_printf("AT+CMGW=\"%s\"\r",num);  //电话号码
    SIM900A_DELAY(100);

    sim900a_tx_printf("%s",smstext);            //短信内容
    SIM900A_DELAY(1); 
  }
  else
  {
    //中文
    sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");    //"UCS2"字符集
    SIM900A_DELAY(100);
    
    sim900a_tx_printf("AT+CMGF=1\r");           //文本模式
    SIM900A_DELAY(100);
    
    sim900a_tx_printf("AT+CSMP=17,167,0,8\r");  //
    SIM900A_DELAY(100);
    
    sim900a_gbk2ucs2hex(ucsbuff,num);
    sim900a_tx_printf("AT+CMGW=\"%s\"\r",ucsbuff);  //UCS2的电话号码(需要转成 ucs2码)
    SIM900A_DELAY(100);
    
    sim900a_gbk2ucs2hex(ucsbuff,smstext);
    sim900a_tx_printf("%s",ucsbuff);          //UCS2的文本内容(需要转成 ucs2码)  
  }
  
  SIM900A_CLEAN_RX();         // 清空了接收缓冲区数据
  GSM_USART->DR=(u32)0x1A;		// 发送十六进制数：0X1A,信息结束符号
  SIM900A_DELAY(200);
  
  Add = sim900a_ReadSaveAdd();
  
  sim900a_tx_printf("AT+CSCS=\"UCS2\"\r");    //"UCS2"字符集
  SIM900A_DELAY(100);
  
  if (Add == 0) 
    return 0;
  
  sim900a_tx_printf("AT+CMSS=%d\r", Add);  //发送短信

  return Add;
}

//查询是否接收到新短信
//0:无新短信，非0：新短信地址
uint8_t IsReceiveMS(void)
{
	char address[3]={0};
	uint8_t addressnum=0;
	char *redata;
  uint16_t len;
/*------------- 查询是否有新短信并提取存储位置 ----------------------------*/
  if(sim900a_cmd_check("+CMTI:"))
  {
    return 0;
  } 
  redata = SIM900A_RX(len);   //接收数据
printf("CMTI:redata:%s,len=%d\n",redata,len);
  if(len == 0)
  {
     return 0;
  }  
  //第一个逗号后面的数据为:”短信存储地址“
  while(*redata != ',')
  {
    len--;
    if(len==0)
    {
        return 0;
    }
    redata++;
  }
	redata+=1;//去掉；','
	address[0]= *redata++;
	address[1]= *redata++;
	address[2]= *redata++;
	if((address[2]>='0')&&(address[2]<='9'))
		addressnum=(address[0]-'0')*100+(address[1]-'0')*10+(address[2]-'0');
	else if((address[1]>='0')&&(address[1]<='9'))
		addressnum=(address[0]-'0')*10+(address[1]-'0');
  else
    addressnum=(address[0]-'0');
	
	printf("address:%c%c%c->%d\n",address[0],address[1],address[2],addressnum);	
  return addressnum;
}	

//读取短信内容
//形参：	messadd：	短信地址
//			num：		保存发件人号码(unicode编码格式的字符串)
//			str：		保存短信内容(unicode编码格式的字符串)
//返回值：	0表示失败
//			1表示成功读取到短信，该短信未读（此处是第一次读，读完后会自动被标准为已读）
//			2表示成功读取到短信，该短信已读
//			3表示成功读取到短信，该短信是已发短信
uint8_t readmessage(uint8_t messadd,char *num,char *str)
{
	char *redata,cmd[20]={0};
  uint16_t len;
	char result=0;
  
	if(messadd>MaxMessAdd)return 0;
	
/*------------- 读取短信内容 ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);	
	if(sim900a_cmd(cmd,"+CMGR:",100) != SIM900A_TRUE)
	{
		return 0;
	}
	redata = SIM900A_RX(len);   //接收数据
	if(len == 0)
	{
		return 0;
	}
	//printf("CMGR:redata:%s\nlen=%d\n",redata,len);

	if(strstr(redata,"UNREAD") != NULL)
  {
    result=1;
  }
	else if(strstr(redata,"READ") != NULL)
  {
    result=2;
  }
  else if(strstr(redata,"SENT") != NULL)
  {
    result=3;
  }
  else if(strstr(redata,"UNSENT") != NULL)
  {
    result=4;
  }
	//第一个逗号后面的数据为:”发件人号码“
	while(*redata != ',')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//去掉',"'
	while(*redata != '"')
	{
		*num++ = *redata++;
		len--;
	}
	*num = 0;               //字符串结尾需要清0
	if (result != 3)
  {
    while(*redata != '+')
    {
      len--;
      if(len==0)
      {
        return 0;
      }
      redata++;
    }
    redata+=6;//去掉'+32"\r'
  }
  else
  {
    redata+=6;//去掉'",""\r'
  }
  
	while(*redata != '\r')
	{
		*str++ = *redata++;
	}
	*str = 0;               //字符串结尾需要清0
	
	//printf("CMGR:result:%d\n",result);
	return result;
}

//读取短信号码和时间
//形参：messadd：	短信地址
//			NumAndTime:		保存发件人号码和时间(unicode编码格式的字符串)
//返回值：	0表示失败
//			1表示成功读取到短信，该短信未读（此处是第一次读，读完后会自动被标准为已读）
//			2表示成功读取到短信，该短信已读
//			3表示成功读取到短信，该短信是已发短信
uint8_t ReadMessageInfo(uint8_t messadd, char *Num, char *Time)
{
	char *redata,cmd[20]={0};
  uint16_t len;
	char result=0;
  
	if(messadd>MaxMessAdd)return 0;
	
/*------------- 读取短信内容 ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);	
	if(sim900a_cmd(cmd,"+CMGR:",100) != SIM900A_TRUE)
	{
		return 0;
	}
	redata = SIM900A_RX(len);   //接收数据
	if(len == 0)
	{
		return 0;
	}
	//printf("CMGR:redata:%s\nlen=%d\n",redata,len);

	if(strstr(redata,"UNREAD") != NULL)
  {
    result=1;
  }
	else if(strstr(redata,"READ") != NULL)
  {
    result=2;
  }
  else if(strstr(redata,"SENT") != NULL)
  {
    result=3;
  }
  else if(strstr(redata,"UNSENT") != NULL)
  {
    result=4;
  }
  
	//第一个逗号后面的数据为:”发件人号码“
	while(*redata != ',')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//去掉',"'
	while(*redata != '"')
	{
		*Num = *redata;
    Num++;
    redata++;
		len--;
	}
	*Num = 0;               //字符串结尾需要清0
  
  if(result != 3)
	{
    //第三个逗号后面的数据为:时间->"19/09/03,16:07:18+32"
    while(*redata != '/')
    {
      len--;
      if(len==0)
      {
        return 0;
      }
      redata++;
    }
    
    for (uint8_t i=0; i<12; i++)    // 记录时间
    {
      len--;
      if(len==0)
      {
        return 0;
      }
      *Time++ = *redata++;
    }
  }
  *Time = '\0';

	return result;
}

#include "ff.h"//firecc936.c文件中：ff_convert()函数实现unicode与gbk互转功能

uint8_t hexuni2gbk(char *hexuni,char *chgbk)
{
	uint16_t len=0,i=0;
	WCHAR wgbk=0;
	WCHAR tmp=0;
	uint8_t unitmp[4]={0};
	SIM900A_CLEAN_RX();
	len=strlen(hexuni);
	if(!len)return SIM900A_FALSE;
//	printf("hexuni:%s::len:%d\n",hexuni,len);
	for(i=0;i<len/4;++i)
	{
		if(hexuni[4*i]>=0x41)	unitmp[0]=hexuni[4*i]-0x41+10;
		else	unitmp[0]=hexuni[4*i]-0x30;
		if(hexuni[4*i+1]>=0x41)unitmp[1]=hexuni[4*i+1]-0x41+10;
		else	unitmp[1]=hexuni[4*i+1]-0x30;
		if(hexuni[4*i+2]>=0x41)unitmp[2]=hexuni[4*i+2]-0x41+10;
		else	unitmp[2]=hexuni[4*i+2]-0x30;
		if(hexuni[4*i+3]>=0x41)unitmp[3]=hexuni[4*i+3]-0x41+10;
		else	unitmp[3]=hexuni[4*i+3]-0x30;
		
		tmp=unitmp[0]*0x1000+unitmp[1]*0x100+unitmp[2]*16+unitmp[3];
		wgbk=ff_convert(tmp,0);
//		printf("tmp:%X->wgbk:%X\n",tmp,wgbk);
		
		if(wgbk<0x80)
		{
			*chgbk=(char)wgbk;
			chgbk++;
		}
		else
		{
			*chgbk=(char)(wgbk>>8);
			chgbk++;
			*chgbk=(char)wgbk;
			chgbk++;
		}		
	}	
	*chgbk=0;
	return SIM900A_TRUE;		
}




/*---------------------------------------------------------------------*/

void sim900a_gprs_init(void)
{
  sim900a_tx_printf("AT+CGCLASS=\"B\"\r");                       //设置移动台类别为"B"
  SIM900A_DELAY(100);
  sim900a_tx_printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r");          //PDP上下文标识1，互联网协议，接入点名称：CMNET
  SIM900A_DELAY(100);
  sim900a_tx_printf("AT+CGATT=1\r");                             //附着 GPRS 网络
  SIM900A_DELAY(100);
  sim900a_tx_printf("AT+CIPCSGP=1,\"CMNET\"\r");                 //设置为 GPRS 连接模式 ，接入点为CMNET
	SIM900A_DELAY(100);
}

uint8_t sim900a_gprs_tcp_link(char *localport,char * serverip,char * serverport)
{
  char *redata;
	sim900a_tx_printf("AT+CLPORT=\"TCP\",\"%s\"\r",localport);         //获取本地端口
  SIM900A_DELAY(100);

	SIM900A_CLEAN_RX();
  //设置服务器IP和端口
  sim900a_tx_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
	
	redata=sim900a_waitask(0);
	SIM900A_CLEAN_RX();
	redata=sim900a_waitask(0);
	if((strstr(redata,"OK") == NULL)&&(strstr(redata,"ALREADY")==NULL))
  {
    return SIM900A_FALSE;
  }
	return SIM900A_TRUE;
}

uint8_t sim900a_gprs_udp_link(char *localport,char * serverip,char * serverport)
{
	char *redata;
  sim900a_tx_printf("AT+CLPORT=\"UDP\",\"%s\"\r",localport);              //获取本地端口
  SIM900A_DELAY(100);
	
	SIM900A_CLEAN_RX();
  sim900a_tx_printf("AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r",serverip,serverport);   //设置服务器IP和端口
	
	redata=sim900a_waitask(0);
	SIM900A_CLEAN_RX();
	redata=sim900a_waitask(0);
	
	if(strstr(redata,"OK") == NULL)
  {
    return SIM900A_FALSE;
  }
	return SIM900A_TRUE;
}


uint8_t sim900a_gprs_send(const char * str)
{
  char *redata;
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+CIPSEND\r");
  //SIM900A_DELAY(100);	
  redata=sim900a_waitask(0);
	
	SIM900A_CLEAN_RX();
  sim900a_tx_printf("%s",str);
	while((GSM_USART->SR&0X40)==0);//等待上一次数据发送完成  
	GSM_USART->DR=(u32)0x1A;		//发送十六进制数：0X1A,信息结束符号
	
	redata=sim900a_waitask(0);
	if(strstr(redata,"OK")==NULL)
  {
    return SIM900A_FALSE;
  }
	return SIM900A_TRUE;
}

uint8_t sim900a_gprs_link_close(void)              //IP链接断开
{
	SIM900A_CLEAN_RX();
	if(sim900a_cmd("AT+CIPCLOSE=1\r","OK",200) != SIM900A_TRUE)
  {
    return SIM900A_FALSE;
  }
	return SIM900A_TRUE;
}

uint8_t sim900a_gprs_shut_close(void)               //关闭场景
{
	SIM900A_CLEAN_RX();
	if(sim900a_cmd("AT+CIPSHUT\r","OK",200) != SIM900A_TRUE)
  {
    return SIM900A_FALSE;
  }
	return SIM900A_TRUE;
}

uint8_t PostGPRS(void)
{
	char *redata;
  uint16_t len; 
  redata = SIM900A_RX(len);   //接收数据 
  if(len == 0)
  {
    return SIM900A_FALSE;
  }
	printf("PostUDP:%s\n",redata);
	SIM900A_CLEAN_RX();
  return SIM900A_TRUE;

}
/*---------------------------------------------------------------------*/
