/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of and a contribution to the lwIP TCP/IP stack.
 *
 * Credits go to Adam Dunkels (and the current maintainers) of this software.
 *
 * Christiaan Simons rewrote this file to get a more stable echo example.
 *
 **/

 /* This file was modified by ST */


#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "tcp_echoserver.h"
#include "netconf.h"
#include <stdlib.h>
#include <string.h>
#include "./LAN8742A/LAN8742A.h"
#include "x_libc.h"
#include "emXGUI.h"
#include "GUI_Network_Dialog.h"

#if LWIP_TCP

static struct tcp_pcb *tcp_echoserver_pcb;

/* ECHO protocol states */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* structure for maintaing connection infos to be passed as argument 
   to LwIP callbacks*/
struct tcp_echoserver_struct
{
  u8_t state;             /* current connection state */
  struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
  struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
  DRV_NETWORK *pnetwork;
};
extern HWND Receive_Handle;
struct tcp_echoserver_struct *tcp_echoserver_es;

static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);


/**
  * @brief  Initializes the tcp echo server
  * @param  None
  * @retval None
  */
uint8_t tcp_echoserver_init(DRV_NETWORK network)
{
  /* create new tcp pcb */
  tcp_echoserver_pcb = tcp_new();

  if (tcp_echoserver_pcb != NULL)
  {
    err_t err;
	
    /* bind echo_pcb to port 7 (ECHO protocol) */
    err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, network.net_local_port);
    
    if (err == ERR_OK)
    {
      /* start tcp listening for echo_pcb */
      tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);
      
      /* initialize LwIP tcp_accept callback function */
      tcp_accept(tcp_echoserver_pcb, tcp_echoserver_accept);
			#ifdef SERIAL_DEBUG
			printf("create tcp server\n");
			#endif
      return 0;
    }
    else 
    {
      /* deallocate the pcb */
      memp_free(MEMP_TCP_PCB, tcp_echoserver_pcb);
      #ifdef SERIAL_DEBUG
      printf("Can not bind pcb\n");
      #endif
      return 1;
    }
  }
  else
  {
    #ifdef SERIAL_DEBUG
    printf("Can not create new pcb\n");
    #endif
    return 1;
  }
}

/**
  * @brief  This functions closes the tcp connection
  * @param  None
  * @retval None
  */
void tcp_echoserver_close(void)
{
	tcp_echoserver_connection_close(tcp_echoserver_pcb,tcp_echoserver_es);
	#ifdef SERIAL_DEBUG
	printf("closes the tcp server\n");
	#endif
}

void network_tcpserver_send(char *str)
{     
  /* allocate pbuf */
  tcp_echoserver_es->p = pbuf_alloc(PBUF_TRANSPORT, strlen(str) , PBUF_POOL);
     
  if (tcp_echoserver_es->p)
  {       
    /* copy data to pbuf */
    pbuf_take(tcp_echoserver_es->p,str, strlen(str));  
    /* send data */
    tcp_echoserver_send(tcp_echoserver_pcb,tcp_echoserver_es);
  }
}

/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used 
  * @retval err_t: error status
  */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;
  WCHAR wbuf[128];

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);
  
  /* 打印连接 IP 和 端口号 */
//  printf("IP 是：%x\n", newpcb->remote_ip.addr);
//  printf("端口号是：%d\n", newpcb->remote_port);

  /* 设置当前连接的 IP */
  x_wsprintf(wbuf, L"%d", newpcb->remote_ip.addr & 0xFF);//ip4_addr1(newpcb->remote_ip.addr)
  SetWindowText(GetDlgItem(Network_Main_Handle, ID_TEXTBOX_RemoteIP1), wbuf);
  
  x_wsprintf(wbuf, L"%d", (newpcb->remote_ip.addr >> 8) & 0xFF);//ip4_addr2(newpcb->remote_ip.addr)
  SetWindowText(GetDlgItem(Network_Main_Handle, ID_TEXTBOX_RemoteIP2), wbuf);
  
  x_wsprintf(wbuf, L"%d", (newpcb->remote_ip.addr >> 16) & 0xFF);//ip4_addr3(newpcb->remote_ip.addr)
  SetWindowText(GetDlgItem(Network_Main_Handle, ID_TEXTBOX_RemoteIP3), wbuf);
  
  x_wsprintf(wbuf, L"%d", (newpcb->remote_ip.addr >> 24) & 0xFF);//ip4_addr4(newpcb->remote_ip.addr)
  SetWindowText(GetDlgItem(Network_Main_Handle, ID_TEXTBOX_RemoteIP4), wbuf);
  
  x_wsprintf(wbuf, L"%d", newpcb->remote_port);
  SetWindowText(GetDlgItem(Network_Main_Handle, ID_TEXTBOX_RemotePort), wbuf);

  /* set priority for the newly accepted tcp connection newpcb */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* allocate structure es to maintain tcp connection informations */
  es = (struct tcp_echoserver_struct *)mem_malloc(sizeof(struct tcp_echoserver_struct));
	
	tcp_echoserver_es=es;
	
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;
    es->pcb = newpcb;
    es->p = NULL;
    es->pnetwork=&drv_network;
    /* pass newly allocated es structure as argument to newpcb */
    tcp_arg(newpcb, es);
    
    /* initialize lwip tcp_recv callback function for newpcb  */ 
    tcp_recv(newpcb, tcp_echoserver_recv);
    
    /* initialize lwip tcp_err callback function for newpcb  */
    tcp_err(newpcb, tcp_echoserver_error);
    
    /* initialize lwip tcp_poll callback function for newpcb */
    tcp_poll(newpcb, tcp_echoserver_poll, 1);
    
    ret_err = ERR_OK;
  }
  else
  {
    /*  close tcp connection */
    tcp_echoserver_connection_close(newpcb, es);
    /* return memory error */
    ret_err = ERR_MEM;
  }
  return ret_err;  
}


/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  WCHAR *recdata = NULL;
  WCHAR *wbuf;
  int WinTexeLen = 0;
	
  struct tcp_echoserver_struct *es;
  err_t ret_err;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct tcp_echoserver_struct *)arg;
  
  /* if we receive an empty tcp frame from client => close connection */
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p == NULL)
    {
       /* we're done sending, close connection */
       tcp_echoserver_connection_close(tpcb, es);
    }
    else
    {
      /* we're not done yet */
      /* acknowledge received packet */
      tcp_sent(tpcb, tcp_echoserver_sent);
      
      /* send remaining data*/
      tcp_echoserver_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from client but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    es->p = NULL;
    pbuf_free(p);
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED)
  {
    /* first data chunk in p->payload */
    es->state = ES_RECEIVED;
    
    /* store reference to incoming pbuf (chain) */
    es->p = p;
    
    /* initialize LwIP tcp_sent callback function */
    tcp_sent(tpcb, tcp_echoserver_sent);
    
		/* 进入临界段，临界段可以嵌套 */
    taskENTER_CRITICAL();
    
		recdata=(WCHAR *)malloc(p->len*sizeof(char)*2);
		if(recdata!=NULL)
		{
#ifdef SERIAL_DEBUG      
			printf("tcpserver_rec:%s",(char *)p->payload);
#endif
      x_mbstowcs_cp936(recdata, p->payload, p->len*2);
      WinTexeLen = GetWindowTextLength(Receive_Handle);                       // 获取文本长度
      wbuf = (WCHAR *)GUI_VMEM_Alloc(WinTexeLen*2 + p->len*sizeof(WCHAR));    // 申请文本长度 + 新消息长度的内存
      GetWindowText(Receive_Handle, wbuf, WinTexeLen+1);                      // 得到原文本
      x_wstrcat(wbuf, recdata);                                               // 追加新文本
      SetWindowText(Receive_Handle, wbuf);                                    // 设置接收窗口的文本
      GUI_VMEM_Free(wbuf);                                                    // 释放申请的内存
		}
		free(recdata);  
    
		taskEXIT_CRITICAL();
    
    /* send back the received data (echo) */
    tcp_echoserver_send(tpcb, es);
    
    ret_err = ERR_OK;
  }
  else if (es->state == ES_RECEIVED)
  {
    /* more data received from client and previous data has been already sent*/
    if(es->p == NULL)
    {
      es->p = p;
			
      /* 进入临界段，临界段可以嵌套 */
      taskENTER_CRITICAL();
      
      recdata=(WCHAR *)malloc(p->len*sizeof(char)*2);
      if(recdata!=NULL)
      {
        #ifdef SERIAL_DEBUG      
          printf("tcpserver_rec:%s",(char *)p->payload);
        #endif
        x_mbstowcs_cp936(recdata, p->payload, p->len*2);
        WinTexeLen = GetWindowTextLength(Receive_Handle);                       // 获取文本长度
        wbuf = (WCHAR *)GUI_VMEM_Alloc(WinTexeLen*2 + p->len*sizeof(WCHAR));    // 申请文本长度 + 新消息长度的内存
        GetWindowText(Receive_Handle, wbuf, WinTexeLen+1);                      // 得到原文本
        x_wstrcat(wbuf, recdata);                                               // 追加新文本
        SetWindowText(Receive_Handle, wbuf);                                    // 设置接收窗口的文本
        GUI_VMEM_Free(wbuf);                                                    // 释放申请的内存      
      }
      free(recdata);

      taskEXIT_CRITICAL();
		
      /* send back received data */
      tcp_echoserver_send(tpcb, es);
    }
    else
    {
      struct pbuf *ptr;

      /* chain pbufs to the end of what we recv'ed previously  */
      ptr = es->p;
      pbuf_chain(ptr,p);
    }
    ret_err = ERR_OK;
  }
  
  /* data received when connection already closed */
  else
  {
    /* Acknowledge data reception */
    tcp_recved(tpcb, p->tot_len);
    
    /* free pbuf and do nothing */
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs. 
  * @param  arg: pointer on argument parameter 
  * @param  err: not used
  * @retval None
  */
static void tcp_echoserver_error(void *arg, err_t err)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    /*  free es structure */
    mem_free(es);
  }
}

/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    if (es->p != NULL)
    {
      /* there is a remaining pbuf (chain) , try to send data */
      tcp_echoserver_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /*  close tcp connection */
        tcp_echoserver_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data) 
  * @param  None
  * @retval None
  */
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcp_echoserver_struct *)arg;
  
  if(es->p != NULL)
  {
    /* still got pbufs to send */
    tcp_echoserver_send(tpcb, es);
  }
  else
  {
    /* if no more data to send and client closed connection*/
    if(es->state == ES_CLOSING)
      tcp_echoserver_connection_close(tpcb, es);
  }
  return ERR_OK;
}


/**
  * @brief  This function is used to send data for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (es->p != NULL) && 
         (es->p->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);
    
    if (wr_err == ERR_OK)
    {
      u16_t plen;

      plen = ptr->len;
     
      /* continue with next pbuf in chain (if any) */
      es->p = ptr->next;
      
      if(es->p != NULL)
      {
        /* increment reference count for es->p */
        pbuf_ref(es->p);
      }
      
      /* free pbuf: will free pbufs up to es->p (because es->p has a reference count > 0) */
      pbuf_free(ptr);

      /* Update tcp window size to be advertized : should be called when received
      data (with the amount plen) has been processed by the application layer */
      tcp_recved(tpcb, plen);
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later / harder, defer to poll */
     es->p = ptr;
   }
   else
   {
     /* other problem ?? */
   }
  }
}

/**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  es: pointer on echo_state structure
  * @retval None
  */
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  
  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* delete es structure */
  if (es != NULL)
  {
    mem_free(es);
  }  
  
  /* close tcp connection */
  tcp_close(tpcb);
}

#endif /* LWIP_TCP */
