#ifndef __LCD_H
#define	__LCD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup Utilities
  * @{
  */

/** 
  * @brief  LCD液晶参数
  */   
typedef struct
{
  /*根据液晶数据手册的参数配置*/
  uint8_t hbp;  //HSYNC后的无效像素
  uint8_t vbp;  //VSYNC后的无效行数

  uint8_t hsw;  	//HSYNC宽度
  uint8_t vsw;   //VSYNC宽度

  uint8_t hfp;  	//HSYNC前的无效像素
  uint8_t vfp;  	//VSYNC前的无效行数
  
  uint8_t comment_clock_2byte; //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
  uint8_t comment_clock_4byte; //Argb8888等四字节像素时推荐使用的液晶时钟频率

  uint16_t lcd_pixel_width; //液晶分辨率，宽
  uint16_t lcd_pixel_height;//液晶分辨率，高

}LCD_PARAM_TypeDef;


/** 
  * @brief  LCD液晶类型
  */   
typedef enum
{ 
  INCH_5  = 0x00, /* 野火5寸屏 */
  INCH_7,     /* 野火7寸屏 */
  INCH_4_3,  /* 野火4.3寸屏 */
  
  LCD_TYPE_NUM /* LCD类型总数*/
}LCD_TypeDef;

/* 当前使用的LCD，默认为5寸屏 */
extern LCD_TypeDef cur_lcd;
/* 不同液晶屏的参数 */
extern const LCD_PARAM_TypeDef lcd_param[];
/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
/*根据液晶数据手册的参数配置*/
#define HBP  lcd_param[cur_lcd].hbp		//HSYNC后的无效像素
#define VBP  lcd_param[cur_lcd].vbp		//VSYNC后的无效行数

#define HSW  lcd_param[cur_lcd].hsw		//HSYNC宽度
#define VSW  lcd_param[cur_lcd].vsw		//VSYNC宽度

#define HFP  lcd_param[cur_lcd].hfp		//HSYNC前的无效像素
#define VFP  lcd_param[cur_lcd].vfp		//VSYNC前的无效行数


/* LCD Size (Width and Height) */
#define  LCD_PIXEL_WIDTH          lcd_param[cur_lcd].lcd_pixel_width
#define  LCD_PIXEL_HEIGHT         lcd_param[cur_lcd].lcd_pixel_height

/* 使用的各个屏幕中最大的分辨率 */
#define  LCD_MAX_PIXEL_WIDTH    ((uint16_t)800)  
#define  LCD_MAX_PIXEL_HEIGHT   ((uint16_t)480)  

#define  LCD_BUFFER         ((uint32_t)0xD0000000)



/**
  * @}
  */ 
  
  

/*部分液晶信号线的引脚复用编号是AF9*/
#define GPIO_AF_LTDC_AF9          ((uint8_t)0x09)  


/**
  * @}
  */ 
//红色数据线
#define LTDC_R0_GPIO_PORT        	GPIOH
#define LTDC_R0_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_R0_GPIO_PIN         	GPIO_Pin_2
#define LTDC_R0_PINSOURCE        	GPIO_PinSource2
#define LTDC_R0_AF			          GPIO_AF_LTDC

#define LTDC_R1_GPIO_PORT        	GPIOH
#define LTDC_R1_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_R1_GPIO_PIN         	GPIO_Pin_3
#define LTDC_R1_PINSOURCE        	GPIO_PinSource3
#define LTDC_R1_AF			          GPIO_AF_LTDC

#define LTDC_R2_GPIO_PORT        	GPIOH
#define LTDC_R2_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_R2_GPIO_PIN         	GPIO_Pin_8
#define LTDC_R2_PINSOURCE        	GPIO_PinSource8
#define LTDC_R2_AF			          GPIO_AF_LTDC

#define LTDC_R3_GPIO_PORT        	GPIOB
#define LTDC_R3_GPIO_CLK         	RCC_AHB1Periph_GPIOB
#define LTDC_R3_GPIO_PIN         	GPIO_Pin_0
#define LTDC_R3_PINSOURCE        	GPIO_PinSource0
#define LTDC_R3_AF			          GPIO_AF_LTDC_AF9

#define LTDC_R4_GPIO_PORT        	GPIOA
#define LTDC_R4_GPIO_CLK         	RCC_AHB1Periph_GPIOA
#define LTDC_R4_GPIO_PIN         	GPIO_Pin_11
#define LTDC_R4_PINSOURCE        	GPIO_PinSource11
#define LTDC_R4_AF			          GPIO_AF_LTDC

#define LTDC_R5_GPIO_PORT        	GPIOA
#define LTDC_R5_GPIO_CLK         	RCC_AHB1Periph_GPIOA
#define LTDC_R5_GPIO_PIN         	GPIO_Pin_12
#define LTDC_R5_PINSOURCE        	GPIO_PinSource12
#define LTDC_R5_AF			          GPIO_AF_LTDC

#define LTDC_R6_GPIO_PORT        	GPIOB
#define LTDC_R6_GPIO_CLK         	RCC_AHB1Periph_GPIOB
#define LTDC_R6_GPIO_PIN         	GPIO_Pin_1
#define LTDC_R6_PINSOURCE        	GPIO_PinSource1
#define LTDC_R6_AF			          GPIO_AF_LTDC_AF9

#define LTDC_R7_GPIO_PORT        	GPIOG
#define LTDC_R7_GPIO_CLK         	RCC_AHB1Periph_GPIOG
#define LTDC_R7_GPIO_PIN         	GPIO_Pin_6
#define LTDC_R7_PINSOURCE        	GPIO_PinSource6
#define LTDC_R7_AF			          GPIO_AF_LTDC
//绿色数据线
#define LTDC_G0_GPIO_PORT        	GPIOE
#define LTDC_G0_GPIO_CLK         	RCC_AHB1Periph_GPIOE
#define LTDC_G0_GPIO_PIN         	GPIO_Pin_5
#define LTDC_G0_PINSOURCE        	GPIO_PinSource5
#define LTDC_G0_AF			          GPIO_AF_LTDC

#define LTDC_G1_GPIO_PORT        	GPIOE
#define LTDC_G1_GPIO_CLK         	RCC_AHB1Periph_GPIOE
#define LTDC_G1_GPIO_PIN         	GPIO_Pin_6
#define LTDC_G1_PINSOURCE        	GPIO_PinSource6
#define LTDC_G1_AF			          GPIO_AF_LTDC

#define LTDC_G2_GPIO_PORT        	GPIOH
#define LTDC_G2_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_G2_GPIO_PIN         	GPIO_Pin_13
#define LTDC_G2_PINSOURCE        	GPIO_PinSource13
#define LTDC_G2_AF			          GPIO_AF_LTDC

#define LTDC_G3_GPIO_PORT        	GPIOG
#define LTDC_G3_GPIO_CLK         	RCC_AHB1Periph_GPIOG
#define LTDC_G3_GPIO_PIN         	GPIO_Pin_10
#define LTDC_G3_PINSOURCE        	GPIO_PinSource10
#define LTDC_G3_AF			          GPIO_AF_LTDC_AF9

#define LTDC_G4_GPIO_PORT        	GPIOH
#define LTDC_G4_GPIO_CLK         	RCC_AHB1Periph_GPIOH
#define LTDC_G4_GPIO_PIN         	GPIO_Pin_15
#define LTDC_G4_PINSOURCE        	GPIO_PinSource15
#define LTDC_G4_AF			          GPIO_AF_LTDC

#define LTDC_G5_GPIO_PORT        	GPIOI
#define LTDC_G5_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_G5_GPIO_PIN         	GPIO_Pin_0
#define LTDC_G5_PINSOURCE        	GPIO_PinSource0
#define LTDC_G5_AF			          GPIO_AF_LTDC

#define LTDC_G6_GPIO_PORT        	GPIOC
#define LTDC_G6_GPIO_CLK         	RCC_AHB1Periph_GPIOC
#define LTDC_G6_GPIO_PIN         	GPIO_Pin_7
#define LTDC_G6_PINSOURCE        	GPIO_PinSource7
#define LTDC_G6_AF			          GPIO_AF_LTDC

#define LTDC_G7_GPIO_PORT        	GPIOI
#define LTDC_G7_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_G7_GPIO_PIN         	GPIO_Pin_2
#define LTDC_G7_PINSOURCE        	GPIO_PinSource2
#define LTDC_G7_AF			          GPIO_AF_LTDC

//蓝色数据线
#define LTDC_B0_GPIO_PORT        	GPIOE
#define LTDC_B0_GPIO_CLK         	RCC_AHB1Periph_GPIOE
#define LTDC_B0_GPIO_PIN         	GPIO_Pin_4
#define LTDC_B0_PINSOURCE        	GPIO_PinSource4
#define LTDC_B0_AF			          GPIO_AF_LTDC

#define LTDC_B1_GPIO_PORT        	GPIOG
#define LTDC_B1_GPIO_CLK         	RCC_AHB1Periph_GPIOG
#define LTDC_B1_GPIO_PIN         	GPIO_Pin_12
#define LTDC_B1_PINSOURCE        	GPIO_PinSource12
#define LTDC_B1_AF			          GPIO_AF_LTDC

#define LTDC_B2_GPIO_PORT        	GPIOD
#define LTDC_B2_GPIO_CLK         	RCC_AHB1Periph_GPIOD
#define LTDC_B2_GPIO_PIN         	GPIO_Pin_6
#define LTDC_B2_PINSOURCE        	GPIO_PinSource6
#define LTDC_B2_AF			          GPIO_AF_LTDC

#define LTDC_B3_GPIO_PORT        	GPIOG
#define LTDC_B3_GPIO_CLK         	RCC_AHB1Periph_GPIOG
#define LTDC_B3_GPIO_PIN         	GPIO_Pin_11
#define LTDC_B3_PINSOURCE        	GPIO_PinSource11
#define LTDC_B3_AF			          GPIO_AF_LTDC

#define LTDC_B4_GPIO_PORT        	GPIOI
#define LTDC_B4_GPIO_CLK         	RCC_AHB1Periph_GPIOI
#define LTDC_B4_GPIO_PIN         	GPIO_Pin_4
#define LTDC_B4_PINSOURCE        	GPIO_PinSource4
#define LTDC_B4_AF			          GPIO_AF_LTDC

#define LTDC_B5_GPIO_PORT        	GPIOA
#define LTDC_B5_GPIO_CLK         	RCC_AHB1Periph_GPIOA
#define LTDC_B5_GPIO_PIN         	GPIO_Pin_3
#define LTDC_B5_PINSOURCE        	GPIO_PinSource3
#define LTDC_B5_AF			          GPIO_AF_LTDC

#define LTDC_B6_GPIO_PORT        	GPIOB
#define LTDC_B6_GPIO_CLK         	RCC_AHB1Periph_GPIOB
#define LTDC_B6_GPIO_PIN         	GPIO_Pin_8
#define LTDC_B6_PINSOURCE        	GPIO_PinSource8
#define LTDC_B6_AF			          GPIO_AF_LTDC

#define LTDC_B7_GPIO_PORT        	GPIOB
#define LTDC_B7_GPIO_CLK         	RCC_AHB1Periph_GPIOB
#define LTDC_B7_GPIO_PIN         	GPIO_Pin_9
#define LTDC_B7_PINSOURCE        	GPIO_PinSource9
#define LTDC_B7_AF			          GPIO_AF_LTDC

//控制信号线
#define LTDC_CLK_GPIO_PORT        GPIOG
#define LTDC_CLK_GPIO_CLK         RCC_AHB1Periph_GPIOG
#define LTDC_CLK_GPIO_PIN         GPIO_Pin_7
#define LTDC_CLK_PINSOURCE        GPIO_PinSource7
#define LTDC_CLK_AF			          GPIO_AF_LTDC

#define LTDC_HSYNC_GPIO_PORT      GPIOI
#define LTDC_HSYNC_GPIO_CLK       RCC_AHB1Periph_GPIOI
#define LTDC_HSYNC_GPIO_PIN       GPIO_Pin_10
#define LTDC_HSYNC_PINSOURCE      GPIO_PinSource10
#define LTDC_HSYNC_AF			        GPIO_AF_LTDC

#define LTDC_VSYNC_GPIO_PORT      GPIOI
#define LTDC_VSYNC_GPIO_CLK       RCC_AHB1Periph_GPIOI
#define LTDC_VSYNC_GPIO_PIN       GPIO_Pin_9
#define LTDC_VSYNC_PINSOURCE      GPIO_PinSource9
#define LTDC_VSYNC_AF			        GPIO_AF_LTDC


#define LTDC_DE_GPIO_PORT         GPIOF
#define LTDC_DE_GPIO_CLK          RCC_AHB1Periph_GPIOF
#define LTDC_DE_GPIO_PIN          GPIO_Pin_10
#define LTDC_DE_PINSOURCE         GPIO_PinSource10
#define LTDC_DE_AF			          GPIO_AF_LTDC

#define LTDC_DISP_GPIO_PORT        GPIOD
#define LTDC_DISP_GPIO_CLK         RCC_AHB1Periph_GPIOD
#define LTDC_DISP_GPIO_PIN         GPIO_Pin_4

#define LTDC_BL_GPIO_PORT         GPIOD
#define LTDC_BL_GPIO_CLK          RCC_AHB1Periph_GPIOD
#define LTDC_BL_GPIO_PIN          GPIO_Pin_7

void LCD_Init(uint32_t fb_addr, int lcd_clk_mhz, uint32_t pixel_format );
void LCD_BackLed_Control ( int on );
void LCD_LayerCamInit(uint32_t Addr, uint32_t width, uint32_t high);
                      
  /**
  * @}
  */
#endif /* __LCD_H */
