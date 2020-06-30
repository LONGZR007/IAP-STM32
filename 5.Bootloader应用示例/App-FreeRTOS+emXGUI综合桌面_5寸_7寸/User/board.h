#ifndef __BOARD_H__
#define __BOARD_H__

/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/
/* STM32 固件库头文件 */
#include "stm32f4xx.h"

/* 开发板硬件bsp头文件 */
#include "./led/bsp_led.h" 
#include "./key/bsp_key.h" 
#include "./lcd/bsp_lcd.h"
#include "./sdram/bsp_sdram.h" 
#include "./touch/bsp_i2c_touch.h"
#include "./touch/bsp_touch_gtxx.h"
#include "./usart/bsp_debug_usart.h"
#include "./flash/bsp_spi_flash.h"
#include "./font/fonts.h"
#include "bsp_adc.h"
#include "./Bsp/wm8978/bsp_wm8978.h"  
#include "DHT11/bsp_dht11.h"
#include "./dwt_delay/core_delay.h"  
#include "netconf.h"
#include "./LAN8742A/LAN8742A.h"
#include "bsp_usart_gsm.h"
#include "bsp_sim900a.h"
#include "./beep/bsp_beep.h"   
#include "./clock/RTC/bsp_rtc.h"
#include "./i2c/bsp_i2c_ee.h"
#include "./board_test/bsp_test.h"

/*
*************************************************************************
*                               函数声明
*************************************************************************
*/
	

#endif /* __BOARD_H__ */
