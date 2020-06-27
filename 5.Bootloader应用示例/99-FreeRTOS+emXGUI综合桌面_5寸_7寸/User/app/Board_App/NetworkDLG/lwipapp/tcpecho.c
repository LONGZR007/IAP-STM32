#include "tcpecho.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include <stdio.h>	
#include <string.h>


static err_t tcpecho_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  if (p != NULL) 
  {        
	/* 更新窗口*/
	tcp_recved(tpcb, p->tot_len);
    
    /* 返回接收到的数据*/  
  tcp_write(tpcb, p->payload, p->tot_len, 1);
    
  memset(p->payload, 0 , p->tot_len);
  pbuf_free(p);
    
  } 
  else if (err == ERR_OK) 
  {
    return tcp_close(tpcb);
  }
  return ERR_OK;
}

static err_t tcpecho_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{     

  tcp_recv(newpcb, tcpecho_recv);
  return ERR_OK;
}

void TCP_Echo_Init(void)
{
  struct tcp_pcb *pcb = NULL;	            		
  
  /* 创建一个TCP控制块  */
  pcb = tcp_new();	  
  
  /* 绑定TCP控制块 */
  tcp_bind(pcb, IP_ADDR_ANY, TCP_ECHO_PORT);       


  /* 进入监听状态 */
  pcb = tcp_listen(pcb);				

  /* 处理连接 */	
  tcp_accept(pcb, tcpecho_accept);   
}









