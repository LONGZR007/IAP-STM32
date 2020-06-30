/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   用1.5.1版本库建的工程模板
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./clock/RTC/bsp_rtc.h"
#include "./Bsp/usart/bsp_debug_usart.h"


static void Delay(uint16_t time)
{
  uint16_t i;
  while(--time)
  {
    for(i=0;i<2000;i++);
  }
}

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
static uint8_t RTC_Config(uint8_t flag)
{  
  uint32_t count=0;
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

  if(flag==0) /* The RTC Clock may varies due to LSI frequency dispersion. */
  {    
    /* Enable the LSI OSC */ 
    RCC_LSICmd(ENABLE);

    count=0;
    /* Wait till LSI is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
      Delay(1000);
      count++;
      if(count>4000)return 1;
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  }
  else /* LSE used as RTC source clock */
  {
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    count=0;
    /* Wait till LSE is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
      Delay(100);
      count++;
      if(count>4000)return 1;
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);      
  }
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
   
  return 0;
}

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
uint8_t RTC_CheckAndConfig(RTC_TIME *rtc_time,uint8_t flag)
{
  RTC_InitTypeDef RTC_InitStructure;
  
  /* RTC configuration  */
  if(RTC_Config(flag)==1)return 1;
  
  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
  {      
    
    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv  = 0xFF;  /* (32.768kHz / 128) - 1 = 0xFF*/
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   
    /* Check on RTC init */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
      printf("\n\r        /!\\***** RTC Prescaler Config failed ********/!\\ \r\n");
    }

    /* Configure the time register */
    if(RTC_SetTime(RTC_Format_BIN, &rtc_time->RTC_Time) == ERROR)
    {
      printf("\r\n>> !! RTC Set Time failed. !! <<\r\n");
      return 1;
    } 
    else
    {
      printf("\r\n>> !! RTC Set Time success. !! <<\r\n");
      /* Indicator for the RTC configuration */
      RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
    } 
      /* Configure the RTC date register */
    if(RTC_SetDate(RTC_Format_BIN, &rtc_time->RTC_Date) == ERROR)
    {
      printf("\r\n>> !! RTC Set Date failed. !! <<\r\n");
      return 1;
    } 
    else
    {
      printf("\r\n>> !! RTC Set Date success. !! <<\r\n");
      /* Indicator for the RTC configuration */
      RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
    }
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("\r\n Power On Reset occurred....\r\n");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("\r\n External Reset occurred....\r\n");
    }

    printf("\r\n No need to configure RTC....\r\n");
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();


    /* Clear the RTC Alarm Flag */
    RTC_ClearFlag(RTC_FLAG_ALRAF);
  }  
  return 0;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
