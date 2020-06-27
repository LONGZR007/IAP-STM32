/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   使用按键控制彩灯
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./key/bsp_key.h" 
#include "./boot_loader/boot_loader.h" 
#include "./usart/bsp_debug_usart.h"
#include "./tim/bsp_basic_tim.h"
#include "./internalFlash/bsp_internalFlash.h"  
#include "./xmodem/xmodem.h"
#include "./lcd/bsp_lcd.h"
#include "./flash/bsp_spi_flash.h"

sFONT Font16x24 = {
  0,
  16, /* Width */
  24, /* Height */
};

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
	uint8_t update_flag[1];
	
  /*初始化按键*/
  Key_GPIO_Config();
	Debug_USART_Config();
  LED_GPIO_Config();
  
  /*初始化液晶屏*/
  LCD_Init();
  LCD_LayerInit();
  LTDC_Cmd(ENABLE);
	
	/*把背景层刷黑色*/
  LCD_SetLayer(LCD_BACKGROUND_LAYER);  
	LCD_Clear(LCD_COLOR_BLACK);
	
  /*初始化后默认使用前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER); 
	/*默认设置不透明	，该函数参数为不透明度，范围 0-0xff ，0为全透明，0xff为不透明*/
  LCD_SetTransparency(0xFF);
	LCD_Clear(LCD_COLOR_BLACK);
	/*经过LCD_SetLayer(LCD_FOREGROUND_LAYER)函数后，
	以下液晶操作都在前景层刷新，除非重新调用过LCD_SetLayer函数设置背景层*/
  
  /*使用不透明前景层*/
	LCD_SetLayer(LCD_FOREGROUND_LAYER);  
  LCD_SetTransparency(0xff);
	
  LCD_Clear(LCD_COLOR_BLACK);	/* 清屏，显示全黑 */

	/*设置字体颜色及字体的背景颜色(此处的背景不是指LCD的背景层！注意区分)*/
  LCD_SetColors(LCD_COLOR_WHITE,LCD_COLOR_BLACK);
  
  LCD_SetFont(&Font16x24);

	LCD_DisplayStringLine_EN_CH(LINE(1),(uint8_t* )"                 emXGUI 引导装载程序");
  
  /* 按住 KEY1 启动，进入接收应用文件模式，完成后直接跳转到应用程序 */
  if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
  {
    /* 获取 .bin 文件接收数据 */
    LED2_ON;
    if (xmodem_receive() == 0)
    { 
      LED2_OFF;
      iap_jump_app(FLASH_APP_ADDR);
    }
    else
    {
      LED3_ON;
    }
    LED2_OFF;
  }
  
  /* 读更新标志 */
  SPI_FLASH_BufferRead(update_flag, 0, sizeof(update_flag));
  
  if (update_flag[0] == 0)
  {
    /* 有新的 APP 开始更新应用程序 */
    
  }
  
  /* 不更新直接跳转到应用程序 */
  iap_jump_app(FLASH_APP_ADDR);

	while(1);
}



/*********************************************END OF FILE**********************/

