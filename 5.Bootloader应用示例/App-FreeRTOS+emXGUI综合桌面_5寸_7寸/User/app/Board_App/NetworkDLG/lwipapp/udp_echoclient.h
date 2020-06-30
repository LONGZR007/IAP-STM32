/**
  ******************************************************************************
  * @file    udp_echoclient.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   Header file for udp_echoclient.c
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

/* Define to prevent recursive inclusion -------------------------------------*/  
#ifndef __UDP_ECHOCLIENT_H__
#define __UDP_ECHOCLIENT_H__

/* Includes ------------------------------------------------------------------*/
#include "netconf.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t udp_echoclient_connect(DRV_NETWORK network);
void udp_echoclient_disconnect(void);
uint8_t udp_echoclient_send(char *data);
#endif /* __UDP_ECHOCLIENT_H__ */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
