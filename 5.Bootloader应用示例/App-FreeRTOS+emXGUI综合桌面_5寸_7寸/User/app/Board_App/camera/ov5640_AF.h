#ifndef __OV5640_AF_H
#define	__OV5640_AF_H

/**
  ******************************************************************************
  * @file    DCMI/Camera/dcmi_ov5640.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    13-April-2012
  * @brief   Header for dcmi_ov5640.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include "stm32f4xx.h"
#include "./camera/bsp_ov5640.h"

//#define Delay(ms)  Delay_ms(ms)

void OV5640_AUTO_FOCUS(void);
void OV5640_FOCUS_AD5820_Init(void);
void OV5640_FOCUS_AD5820_Constant_Focus(void);
uint8_t OV5640_FOCUS_AD5820_Pause_Focus(void);
#endif /* __OV5640_AF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

