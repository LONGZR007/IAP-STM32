/**
  ******************************************************************************
  * @file    udp_echoserver.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   UDP echo server
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "netconf.h"
#include "LAN8742A.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>
#include "udp_echoserver.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#undef  UDP_SERVER_PORT
#define UDP_SERVER_PORT 8080

#undef  UDP_CLIENT_PORT
#define UDP_CLIENT_PORT 8088
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct udp_pcb *upcb;

/* Private function prototypes -----------------------------------------------*/
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initialize the server application.
  * @param  None
  * @retval None
  */
void udp_echoserver_init(void)
{
   err_t err;
   
   /* Create a new UDP control block  */
   upcb = udp_new();
   
   if (upcb)
   {
     /* Bind the upcb to the UDP_PORT port */
     /* Using IP_ADDR_ANY allow the upcb to be used by any local interface */
      err = udp_bind(upcb, IP_ADDR_ANY, UDP_SERVER_PORT);
      
      if(err == ERR_OK)
      {
        /* Set a receive callback for the upcb */
        udp_recv(upcb, udp_echoserver_receive_callback, NULL);
				#ifdef SERIAL_DEBUG
				printf("Start the server application\n");
				#endif
      }
      else
      {
        udp_remove(upcb);
        printf("can not bind pcb");
      }
   }
   else
   {
     printf("can not create pcb");
   } 
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_echoserver_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
  char recdata[100]={0};
  /* Connect to the remote client */
  udp_connect(upcb, addr, UDP_CLIENT_PORT);
   
	#ifdef SERIAL_DEBUG
	memcpy((char *)recdata,p->payload,p->len);
	printf("upd_rec:%s",recdata);
	#endif	
  /* Tell the client that we have accepted it */
  udp_send(upcb, p);
	
  /* free the UDP connection, so we can accept new clients */
  udp_disconnect(upcb);
	
  /* Free the p buffer */
  pbuf_free(p);
   
}

/**
  * @brief  Stop the server application.
  * @param  None
  * @retval None
  */
void udp_echoserver_Stop(void)
{
	#ifdef SERIAL_DEBUG
	printf("Stop the server application\n");
	#endif	
	udp_remove(upcb);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
