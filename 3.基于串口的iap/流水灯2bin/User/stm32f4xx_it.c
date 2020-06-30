/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
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
#include "stm32f4xx_it.h"

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/
void TIM1_BRK_TIM9_IRQHandler(void){}                    
void WWDG_IRQHandler     (void){}                                                   
void PVD_IRQHandler    (void){}                                   
void TAMP_STAMP_IRQHandler  (void){}                 
void RTC_WKUP_IRQHandler (void){}                                
void FLASH_IRQHandler   (void){}                                                     
void RCC_IRQHandler   (void){}                                                          
void EXTI0_IRQHandler  (void){}                                                         
void EXTI1_IRQHandler  (void){}                                                          
void EXTI2_IRQHandler  (void){}                                                         
void EXTI3_IRQHandler  (void){}                                                        
void EXTI4_IRQHandler  (void){}                                                         
void DMA1_Stream0_IRQHandler(void){}                                        
void DMA1_Stream1_IRQHandler(void){}                                           
void DMA1_Stream2_IRQHandler (void){}                                          
void DMA1_Stream3_IRQHandler (void){}                                          
void DMA1_Stream4_IRQHandler  (void){}                                         
void DMA1_Stream5_IRQHandler  (void){}                                         
void DMA1_Stream6_IRQHandler  (void){}                                         
void ADC_IRQHandler    (void){}                                      
void CAN1_TX_IRQHandler (void){}                                                            
void CAN1_RX0_IRQHandler(void){}                                                           
void CAN1_RX1_IRQHandler(void){}                                                            
void CAN1_SCE_IRQHandler (void){}                                                           
void EXTI9_5_IRQHandler  (void){}                                               
//void TIM1_BRK_TIM9_IRQHandler (void){}                        
void TIM1_UP_TIM10_IRQHandler  (void){}                     
void TIM1_TRG_COM_TIM11_IRQHandler  (void){} 
void TIM1_CC_IRQHandler   (void){}                                             
void TIM2_IRQHandler   (void){}                                                         
void TIM3_IRQHandler  (void){}                                                          
void TIM4_IRQHandler    (void){}                                                        
void I2C1_EV_IRQHandler  (void){}                                                        
void I2C1_ER_IRQHandler  (void){}                                                        
void I2C2_EV_IRQHandler  (void){}                                                       
void I2C2_ER_IRQHandler  (void){}                                                          
void SPI1_IRQHandler    (void){}                                                       
void SPI2_IRQHandler    (void){}                                                        
void USART1_IRQHandler  (void){}                                                      
void USART2_IRQHandler   (void){}                                                     
void USART3_IRQHandler   (void){}                                                    
void EXTI15_10_IRQHandler (void){}                                            
void RTC_Alarm_IRQHandler (void){}                            
void OTG_FS_WKUP_IRQHandler  (void){}                               
void TIM8_BRK_TIM12_IRQHandler (void){}                      
void TIM8_UP_TIM13_IRQHandler   (void){}                     
void TIM8_TRG_COM_TIM14_IRQHandler (void){}  
void TIM8_CC_IRQHandler   (void){}                                             
void DMA1_Stream7_IRQHandler (void){}                                                 
void FMC_IRQHandler    (void){}                                                         
void SDIO_IRQHandler   (void){}                                                          
//void TIM5_IRQHandler    (void){}                                                         
void SPI3_IRQHandler   (void){}                                                          
void UART4_IRQHandler  (void){}                                                         
void UART5_IRQHandler  (void){}                                                         
void TIM6_DAC_IRQHandler (void){}                            
//void TIM7_IRQHandler       (void){}                        
void DMA2_Stream0_IRQHandler  (void){}                                        
void DMA2_Stream1_IRQHandler  (void){}                                         
void DMA2_Stream2_IRQHandler  (void){}                                          
void DMA2_Stream3_IRQHandler  (void){}                                          
void DMA2_Stream4_IRQHandler  (void){}                                       
void ETH_IRQHandler      (void){}                                                    
void ETH_WKUP_IRQHandler    (void){}                             
void CAN2_TX_IRQHandler    (void){}                                                        
void CAN2_RX0_IRQHandler   (void){}                                                        
void CAN2_RX1_IRQHandler   (void){}                                                        
void CAN2_SCE_IRQHandler   (void){}                                                        
void OTG_FS_IRQHandler     (void){}                                                
void DMA2_Stream5_IRQHandler (void){}                                          
void DMA2_Stream6_IRQHandler (void){}                                          
void DMA2_Stream7_IRQHandler (void){}                                          
void USART6_IRQHandler    (void){}                                                     
void I2C3_EV_IRQHandler   (void){}                                                        
void I2C3_ER_IRQHandler   (void){}                                                        
void OTG_HS_EP1_OUT_IRQHandler  (void){}                          
void OTG_HS_EP1_IN_IRQHandler(void){}                             
void OTG_HS_WKUP_IRQHandler(void){}                                 
void OTG_HS_IRQHandler(void){}                                                    
void DCMI_IRQHandler (void){}                                                                                                             
void HASH_RNG_IRQHandler(void){} 
void FPU_IRQHandler (void){} 
/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
