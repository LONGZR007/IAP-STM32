#ifndef _BSP_SIM900A_H_
#define _BSP_SIM900A_H_

#include "stm32f4xx.h"
#include "bsp_usart_gsm.h"

typedef enum{
  SIM900A_TRUE, 
  SIM900A_FALSE,         
}sim900a_res_e;


//                  指令             正常返回
//本机号码          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //很多SIM卡默认都是没设置本机号码的，解决方法如下 http://www.multisilicon.com/blog/a21234642.html
//SIM营运商         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM卡状态         AT+CPIN?\r        +CPIN: READY   OK
//SIM卡信号强度     AT+CSQ\r          +CSQ: 8,0   OK

uint8_t   sim900a_cmd(char *cmd, char *reply,uint32_t waittime );
uint8_t   sim900a_cmd_check(char *reply);

#define 	sim900a_config() 				        GSM_USART_Config()						
#define   sim900a_tx_printf(fmt, ...)     GSM_USART_printf(GSM_USART,fmt,##__VA_ARGS__)     //printf格式发送命令（纯发送，不进行任何接收数据处理）

#define   SIM900A_CLEAN_RX()              gsm_clean_rebuff()
#define   sim900a_ate0()                  sim900a_cmd("ATE0\r","OK",100)              //关闭回显
#define   SIM900A_TX(cmd)                	sim900a_tx_printf("%s",cmd)
#define   SIM900A_IS_RX()                 (USART_GetFlagStatus(GSM_USART, USART_FLAG_RXNE) != RESET)
#define   SIM900A_RX(len)                 ((char *)gsm_get_rebuff(&(len)))
#define   SIM900A_DELAY(time)             CPU_TS_Tmr_Delay_MS(time)                 //延时
#define   SIM900A_SWAP16(data)    		    __REVSH(data)

/*************************** 电话 功能 ***************************/
#define  	SIM900A_HANGON()				        SIM900A_TX("ATA\r");								
#define  	SIM900A_HANGOFF()				        SIM900A_TX("ATH\r");	          //挂断电话	

uint8_t 	sim900a_init(void);															                //初始化并检测模块
uint8_t   sim900a_cnum(char *num);															          //获取本机号码
void      sim900a_call(char *num);															          //发起拨打电话（不管接不接通）
uint8_t 	IsRing(char *num);															                //查询是否来电，并保存来电号码
uint8_t   IsNOCARRIER(void);

/***************************  短信功能  ****************************/
void      sim900a_sms(char *num,char *smstext);                           //发送短信（支持中英文,中文为GBK码）
char *    sim900a_waitask(uint8_t waitask_hook(void));                    //等待有数据应答，返回接收缓冲区地址
void      sim900a_gbk2ucs2(char * ucs2,char * gbk);           
void 			sim900a_sms_utf8(char *num,char *smstext,uint16_t numlen,uint16_t textlen);//发送短信（支持中英文,输入时使用UTF8编码）
uint16_t sim900a_save_sms(char *num,char *smstext);
uint8_t 	IsReceiveMS(void);
uint8_t 	readmessage(uint8_t messadd,char *num,char *str);
uint8_t   ReadMessageInfo(uint8_t messadd, char *Num, char *Time);
uint8_t 	hexuni2gbk(char *hexuni,char *chgbk);

/*************************** GPRS 功能 ***************************/
void 	    sim900a_gprs_init		(void);																       //GPRS初始化环境
uint8_t   sim900a_gprs_tcp_link	(char *localport,char * serverip,char * serverport);				//TCP连接
uint8_t   sim900a_gprs_udp_link	(char *localport,char * serverip,char * serverport);				//UDP连接
uint8_t   sim900a_gprs_send		(const char * str);													 //发送数据
uint8_t   sim900a_gprs_link_close	(void);              										 //IP链接断开
uint8_t   sim900a_gprs_shut_close	(void);               									 //关闭场景
uint8_t   PostGPRS(void);                                                  //IP链接断开
uint8_t sim900a_cmd_isr(char *cmd, char *reply,uint32_t waittime );
#endif  //_BSP_SIM900A_H_

