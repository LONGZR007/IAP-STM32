/**
  ******************************************************************************
  * @file    bsp_lcd.c
  * @author  fire
  * @version V1.1
  * @date    2015-xx-xx
  * @brief   LCD驱动函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  

/* Includes ------------------------------------------------------------------*/
#include "bsp_lcd.h"

/* 不同液晶屏的参数 */
const LCD_PARAM_TypeDef lcd_param[LCD_TYPE_NUM]={

  /* 5寸屏参数 */
  {
    /*根据液晶数据手册的参数配置*/
    .hbp = 46,  //HSYNC后的无效像素
    .vbp = 23,  //VSYNC后的无效行数

    .hsw = 1,  	//HSYNC宽度
    .vsw = 1,   //VSYNC宽度

    .hfp = 22,  	//HSYNC前的无效像素
    .vfp = 22,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 33, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 21, //Argb8888等四字节像素时推荐使用的液晶时钟频率

    
    .lcd_pixel_width = LCD_MAX_PIXEL_WIDTH,//液晶分辨率，宽
    .lcd_pixel_height = LCD_MAX_PIXEL_HEIGHT,//液晶分辨率，高
  },
  
   /* 7寸屏参数（与5寸一样） */
  {
    /*根据液晶数据手册的参数配置*/
    .hbp = 46,  //HSYNC后的无效像素
    .vbp = 23,  //VSYNC后的无效行数

    .hsw = 1,  	//HSYNC宽度
    .vsw = 1,   //VSYNC宽度

    .hfp = 22,  	//HSYNC前的无效像素
    .vfp = 22,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 33, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 21, //Argb8888等四字节像素时推荐使用的液晶时钟频率

    
    .lcd_pixel_width = LCD_MAX_PIXEL_WIDTH,//液晶分辨率，宽
    .lcd_pixel_height = LCD_MAX_PIXEL_HEIGHT,//液晶分辨率，高
  },

  /* 4.3寸屏参数 */
  {
      /*根据液晶数据手册的参数配置*/
    .hbp = 8,  //HSYNC后的无效像素
    .vbp = 2,  //VSYNC后的无效行数

    .hsw = 41,  	//HSYNC宽度
    .vsw = 10,   //VSYNC宽度

    .hfp = 4,  	//HSYNC前的无效像素
    .vfp = 4,  	//VSYNC前的无效行数
    
    .comment_clock_2byte = 15, //rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
    .comment_clock_4byte = 15, //Argb8888等四字节像素时推荐使用的液晶时钟频率
    
    .lcd_pixel_width = 480,//液晶分辨率，宽
    .lcd_pixel_height = 272,//液晶分辨率，高
  }
};

/* 当前使用的LCD，默认为5/7寸屏
  * 在触摸驱动初始化时可根据触摸芯片的型号驱分不同的LCD
*/
LCD_TypeDef cur_lcd = INCH_5;

/* 每个像素点占多少个字节
ARGB8888/RGB888/RGB565/ARGB1555/ARGB4444/L8/AL44/AL88 
使用宏LTDC_Pixelformat_ARGB8888作为索引即可
*/
const uint8_t PIXEL_BPP[]={4,3,2,2,2,1,1,2};  

/**
  * @brief  初始化LCD的GPIO，RGB888及背光、DISP
  * @param  无
  * @retval 无
  */
static void LCD_GPIO_Config(void)
{
 GPIO_InitTypeDef GPIO_InitStruct;

 /* 使能LCD使用到的引脚时钟 */
                         //红色数据线
 RCC_AHB1PeriphClockCmd(LTDC_R0_GPIO_CLK | LTDC_R1_GPIO_CLK | LTDC_R2_GPIO_CLK|
                        LTDC_R3_GPIO_CLK | LTDC_R4_GPIO_CLK | LTDC_R5_GPIO_CLK|
                        LTDC_R6_GPIO_CLK | LTDC_R7_GPIO_CLK |
                         //绿色数据线
                         LTDC_G0_GPIO_CLK|LTDC_G1_GPIO_CLK|LTDC_G2_GPIO_CLK|
                         LTDC_G3_GPIO_CLK|LTDC_G4_GPIO_CLK|LTDC_G5_GPIO_CLK|
                         LTDC_G6_GPIO_CLK|LTDC_G7_GPIO_CLK|
                         //蓝色数据线
                         LTDC_B0_GPIO_CLK|LTDC_B1_GPIO_CLK|LTDC_B2_GPIO_CLK|
                         LTDC_B3_GPIO_CLK|LTDC_B4_GPIO_CLK|LTDC_B5_GPIO_CLK|
                         LTDC_B6_GPIO_CLK|LTDC_B7_GPIO_CLK|
                         //控制信号线
                         LTDC_CLK_GPIO_CLK | LTDC_HSYNC_GPIO_CLK |LTDC_VSYNC_GPIO_CLK|
                         LTDC_DE_GPIO_CLK  | LTDC_BL_GPIO_CLK    |LTDC_DISP_GPIO_CLK ,ENABLE);


/* GPIO配置 */

/* 红色数据线 */
 GPIO_InitStruct.GPIO_Pin = LTDC_R0_GPIO_PIN;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

 GPIO_Init(LTDC_R0_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R0_GPIO_PORT, LTDC_R0_PINSOURCE, LTDC_R0_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R1_GPIO_PIN;
 GPIO_Init(LTDC_R1_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R1_GPIO_PORT, LTDC_R1_PINSOURCE, LTDC_R1_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R2_GPIO_PIN;
 GPIO_Init(LTDC_R2_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R2_GPIO_PORT, LTDC_R2_PINSOURCE, LTDC_R2_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R3_GPIO_PIN;
 GPIO_Init(LTDC_R3_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R3_GPIO_PORT, LTDC_R3_PINSOURCE, LTDC_R3_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R4_GPIO_PIN;
 GPIO_Init(LTDC_R4_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R4_GPIO_PORT, LTDC_R4_PINSOURCE, LTDC_R4_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R5_GPIO_PIN;
 GPIO_Init(LTDC_R5_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R5_GPIO_PORT, LTDC_R5_PINSOURCE, LTDC_R5_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R6_GPIO_PIN;
 GPIO_Init(LTDC_R6_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R6_GPIO_PORT, LTDC_R6_PINSOURCE, LTDC_R6_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_R7_GPIO_PIN;
 GPIO_Init(LTDC_R7_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_R7_GPIO_PORT, LTDC_R7_PINSOURCE, LTDC_R7_AF);

 //绿色数据线
 GPIO_InitStruct.GPIO_Pin = LTDC_G0_GPIO_PIN;
 GPIO_Init(LTDC_G0_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G0_GPIO_PORT, LTDC_G0_PINSOURCE, LTDC_G0_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G1_GPIO_PIN;
 GPIO_Init(LTDC_G1_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G1_GPIO_PORT, LTDC_G1_PINSOURCE, LTDC_G1_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G2_GPIO_PIN;
 GPIO_Init(LTDC_G2_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G2_GPIO_PORT, LTDC_G2_PINSOURCE, LTDC_G2_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G3_GPIO_PIN;
 GPIO_Init(LTDC_G3_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G3_GPIO_PORT, LTDC_G3_PINSOURCE, LTDC_G3_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G4_GPIO_PIN;
 GPIO_Init(LTDC_G4_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G4_GPIO_PORT, LTDC_G4_PINSOURCE, LTDC_G4_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G5_GPIO_PIN;
 GPIO_Init(LTDC_G5_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G5_GPIO_PORT, LTDC_G5_PINSOURCE, LTDC_G5_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G6_GPIO_PIN;
 GPIO_Init(LTDC_G6_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G6_GPIO_PORT, LTDC_G6_PINSOURCE, LTDC_G6_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_G7_GPIO_PIN;
 GPIO_Init(LTDC_G7_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_G7_GPIO_PORT, LTDC_G7_PINSOURCE, LTDC_G7_AF);

 //蓝色数据线
 GPIO_InitStruct.GPIO_Pin = LTDC_B0_GPIO_PIN;
 GPIO_Init(LTDC_B0_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B0_GPIO_PORT, LTDC_B0_PINSOURCE, LTDC_B0_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B1_GPIO_PIN;
 GPIO_Init(LTDC_B1_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B1_GPIO_PORT, LTDC_B1_PINSOURCE, LTDC_B1_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B2_GPIO_PIN;
 GPIO_Init(LTDC_B2_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B2_GPIO_PORT, LTDC_B2_PINSOURCE, LTDC_B2_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B3_GPIO_PIN;
 GPIO_Init(LTDC_B3_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B3_GPIO_PORT, LTDC_B3_PINSOURCE, LTDC_B3_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B4_GPIO_PIN;
 GPIO_Init(LTDC_B4_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B4_GPIO_PORT, LTDC_B4_PINSOURCE, LTDC_B4_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B5_GPIO_PIN;
 GPIO_Init(LTDC_B5_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B5_GPIO_PORT, LTDC_B5_PINSOURCE, LTDC_B5_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B6_GPIO_PIN;
 GPIO_Init(LTDC_B6_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B6_GPIO_PORT, LTDC_B6_PINSOURCE, LTDC_B6_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_B7_GPIO_PIN;
 GPIO_Init(LTDC_B7_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_B7_GPIO_PORT, LTDC_B7_PINSOURCE, LTDC_B7_AF);

 //控制信号线
 GPIO_InitStruct.GPIO_Pin = LTDC_CLK_GPIO_PIN;
 GPIO_Init(LTDC_CLK_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_CLK_GPIO_PORT, LTDC_CLK_PINSOURCE, LTDC_CLK_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_HSYNC_GPIO_PIN;
 GPIO_Init(LTDC_HSYNC_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_HSYNC_GPIO_PORT, LTDC_HSYNC_PINSOURCE, LTDC_HSYNC_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_VSYNC_GPIO_PIN;
 GPIO_Init(LTDC_VSYNC_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_VSYNC_GPIO_PORT, LTDC_VSYNC_PINSOURCE, LTDC_VSYNC_AF);

 GPIO_InitStruct.GPIO_Pin = LTDC_DE_GPIO_PIN;
 GPIO_Init(LTDC_DE_GPIO_PORT, &GPIO_InitStruct);
 GPIO_PinAFConfig(LTDC_DE_GPIO_PORT, LTDC_DE_PINSOURCE, LTDC_DE_AF);

 //BL DISP
 GPIO_InitStruct.GPIO_Pin = LTDC_DISP_GPIO_PIN;
 GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

 GPIO_Init(LTDC_DISP_GPIO_PORT, &GPIO_InitStruct);


 GPIO_InitStruct.GPIO_Pin = LTDC_BL_GPIO_PIN;
 GPIO_Init(LTDC_BL_GPIO_PORT, &GPIO_InitStruct);

 //拉高使能lcd
 GPIO_SetBits(LTDC_DISP_GPIO_PORT,LTDC_DISP_GPIO_PIN);
 GPIO_SetBits(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN);
}

/**
 * @brief  背光LED控制
 * @param  on 1为亮，其余值为灭
 * @retval 无
 */
void LCD_BackLed_Control ( int on )
{
	if ( on )
		  GPIO_SetBits(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN);
	else
		  GPIO_ResetBits(LTDC_BL_GPIO_PORT,LTDC_BL_GPIO_PIN);		
}

/**
  * @brief 初始化LTDC的 层 参数
  * @param fb_addr 显存首地址
  * @param pixel_format 像素格式，
           如LTDC_Pixelformat_ARGB8888 、LTDC_Pixelformat_RGB565等
  * @retval None
  */
void LCD_LayerInit(uint32_t fb_addr, uint32_t pixel_format )
{
  LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 
  
  /* 层窗口配置 */
  /* 配置本层的窗口边界，注意这些参数是包含HBP HSW VBP VSW的 */    
	//一行的第一个起始像素，该成员值应用为 (LTDC_InitStruct.LTDC_AccumulatedHBP+1)的值
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = HBP + HSW;
	//一行的最后一个像素，该成员值应用为 (LTDC_InitStruct.LTDC_AccumulatedActiveW)的值
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = HSW+HBP+LCD_PIXEL_WIDTH-1;
	//一列的第一个起始像素，该成员值应用为 (LTDC_InitStruct.LTDC_AccumulatedVBP+1)的值
	LTDC_Layer_InitStruct.LTDC_VerticalStart =  VBP + VSW;
	//一列的最后一个像素，该成员值应用为 (LTDC_InitStruct.LTDC_AccumulatedActiveH)的值
	LTDC_Layer_InitStruct.LTDC_VerticalStop = VSW+VBP+LCD_PIXEL_HEIGHT-1;
	
  /* 像素格式配置*/
  LTDC_Layer_InitStruct.LTDC_PixelFormat = pixel_format;
  /* 恒定Alpha值配置，0-255 */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
  /* 默认背景颜色，该颜色在定义的层窗口外或在层禁止时使用。 */          
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0xFF;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0xFF;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0xFF;         
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0xFF;
  /* 配置混合因子 CA表示使用恒定Alpha值，PAxCA表示使用像素Alpha x 恒定Alpha值 */       
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
  
  /* 该成员应写入(一行像素数据占用的字节数+3)
  Line Lenth = 行有效像素个数 x 每个像素的字节数 + 3 
  行有效像素个数 = LCD_PIXEL_WIDTH 
  每个像素的字节数 = 2（RGB565/RGB1555）/ 3 (RGB888)/ 4（ARGB8888） 
  */ 
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_PIXEL_WIDTH * PIXEL_BPP[pixel_format]) + 3);
  /* 从某行的起始位置到下一行起始位置处的像素增量
  Pitch = 行有效像素个数 x 每个像素的字节数 */ 
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * PIXEL_BPP[pixel_format]);
  
  /* 配置有效的行数 */  
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;
  
  /* 配置本层的显存首地址 */    
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = fb_addr;
  
  /* 以上面的配置初始化第 1 层*/
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

  /* 立即重载配置 */  
  LTDC_ReloadConfig(LTDC_IMReload);

 /*只使用一层 */
  LTDC_LayerCmd(LTDC_Layer1, ENABLE); 
  LTDC_LayerCmd(LTDC_Layer2, DISABLE);
 
  /* 立即重载配置 */  
  LTDC_ReloadConfig(LTDC_IMReload);
}

/**
  * @brief LCD初始化
  * @param fb_addr 显存首地址
  * @param  lcd_clk_mhz 像素时钟频率，为 0 时直接使用推荐时钟频率
            RGB565格式推荐为30~33，
            XRGB8888格式推荐为20~22
            极限范围为15~52，其余值会超出LTDC时钟分频配置范围
  * @param pixel_format 像素格式，如LTDC_Pixelformat_ARGB8888 、LTDC_Pixelformat_RGB565等
  * @retval  None 
  */
void LCD_Init(uint32_t fb_addr, int lcd_clk_mhz, uint32_t pixel_format )
{ 
  uint32_t div;
  LTDC_InitTypeDef       LTDC_InitStruct;
  
  /* lcd_clk_mhz为0时使用推荐时钟频率 */
  if(lcd_clk_mhz == 0)
  {
    if(pixel_format == LTDC_Pixelformat_RGB565||
        pixel_format == LTDC_Pixelformat_ARGB1555||
        pixel_format == LTDC_Pixelformat_ARGB4444||
        pixel_format == LTDC_Pixelformat_L8||
        pixel_format == LTDC_Pixelformat_AL88)
    {
      lcd_clk_mhz = lcd_param[cur_lcd].comment_clock_2byte;
    }
    else if(pixel_format == LTDC_Pixelformat_ARGB8888||
              pixel_format == LTDC_Pixelformat_RGB888)
    {
      lcd_clk_mhz = lcd_param[cur_lcd].comment_clock_4byte;
    }
  }
  
  /* 使能LTDC外设时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);
  
  /* 使能DMA2D时钟 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
	
	/* 初始化LCD的控制引脚 */
  LCD_GPIO_Config();   
	
	/* 配置 PLLSAI 分频器，它的输出作为像素同步时钟CLK*/
  /* PLLSAI_VCO 输入时钟 = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO 输出时钟 = PLLSAI_VCO输入 * PLLSAI_N = 420 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO 输出/PLLSAI_R = 420/div  Mhz */
  /* LTDC 时钟频率 = PLLLCDCLK / DIV = 420/div/4 = PIXEL_CLK_MHZ Mhz */
	/* LTDC时钟太高会导花屏，若对刷屏速度要求不高，降低时钟频率可减少花屏现象*/
	/* 以下函数三个参数分别为：PLLSAIN,PLLSAIQ,PLLSAIR，其中PLLSAIQ与LTDC无关*/
  div = 420/4/lcd_clk_mhz;
  
  RCC_PLLSAIConfig(420,7,div);
	/*以下函数的参数为DIV值*/
  RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div4);
  
  /* 使能 PLLSAI 时钟 */
  RCC_PLLSAICmd(ENABLE);
  /* 等待 PLLSAI 初始化完成 */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
  {
  }
  
  /* LTDC配置*********************************************************/  
  /*信号极性配置*/
  /* 行同步信号极性 */
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
  /* 垂直同步信号极性 */  
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
  /* 数据使能信号极性 */
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;     
  /* 像素同步时钟极性 */ 
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;
  
  /* 配置LCD背景颜色 */                   
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;    
 
  /* 时间参数配置 */  
 /* 配置行同步信号宽度(HSW-1) */
 LTDC_InitStruct.LTDC_HorizontalSync =HSW-1;
 /* 配置垂直同步信号宽度(VSW-1) */
 LTDC_InitStruct.LTDC_VerticalSync = VSW-1;
 /* 配置(HSW+HBP-1) */
 LTDC_InitStruct.LTDC_AccumulatedHBP =HSW+HBP-1;
 /* 配置(VSW+VBP-1) */
 LTDC_InitStruct.LTDC_AccumulatedVBP = VSW+VBP-1;
 /* 配置(HSW+HBP+有效像素宽度-1) */
 LTDC_InitStruct.LTDC_AccumulatedActiveW = HSW+HBP+LCD_PIXEL_WIDTH-1;
 /* 配置(VSW+VBP+有效像素高度-1) */
 LTDC_InitStruct.LTDC_AccumulatedActiveH = VSW+VBP+LCD_PIXEL_HEIGHT-1;
 /* 配置总宽度(HSW+HBP+有效像素宽度+HFP-1) */
 LTDC_InitStruct.LTDC_TotalWidth =HSW+ HBP+LCD_PIXEL_WIDTH  + HFP-1; 
 /* 配置总高度(VSW+VBP+有效像素高度+VFP-1) */
 LTDC_InitStruct.LTDC_TotalHeigh =VSW+ VBP+LCD_PIXEL_HEIGHT  + VFP-1;

  LTDC_Init(&LTDC_InitStruct);
  
  LTDC_Cmd(ENABLE);  

  LCD_LayerInit(fb_addr, pixel_format);
}  

void LCD_LayerCamInit(uint32_t Addr, uint32_t width, uint32_t high)
{
 LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

 /* Windowing configuration */
 /* In this case all the active display area is used to display a picture then :
 Horizontal start = horizontal synchronization + Horizontal back porch = 30
 Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
 Vertical start   = vertical synchronization + vertical back porch     = 4
 Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */
 LTDC_Layer_InitStruct.LTDC_HorizontalStart = HBP + 1;
 LTDC_Layer_InitStruct.LTDC_HorizontalStop = (width + HBP);
 LTDC_Layer_InitStruct.LTDC_VerticalStart =  VBP + 1;
 LTDC_Layer_InitStruct.LTDC_VerticalStop = (high + VBP);

 /* Pixel Format configuration*/
 LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
 /* Alpha constant (255 totally opaque) */
 LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
 /* Default Color configuration (configure A,R,G,B component values) */
 LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;
 LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;
 LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;
 LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
 /* Configure blending factors */
 LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
 LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

 /* the length of one line of pixels in bytes + 3 then :
 Line Lenth = Active high width x number of bytes per pixel + 3
 Active high width         = LCD_PIXEL_WIDTH
 number of bytes per pixel = 2    (pixel_format : RGB565)
 */
 LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((800 * 2) + 3);
 /* the pitch is the increment from the start of one line of pixels to the
 start of the next line in bytes, then :
 Pitch = Active high width x number of bytes per pixel */
 LTDC_Layer_InitStruct.LTDC_CFBPitch = (800 * 2);

 /* Configure the number of lines */
 LTDC_Layer_InitStruct.LTDC_CFBLineNumber = high;

 /* Start Address configuration : the LCD Frame buffer is defined on SDRAM */
 LTDC_Layer_InitStruct.LTDC_CFBStartAdress = Addr;

 /* Initialize LTDC layer 1 */
 LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

  /* Configure Layer2 */
 /* Pixel Format configuration*/
 LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_ARGB1555;
 
  /* Start Address configuration : the LCD Frame buffer is defined on SDRAM w/ Offset */
 LTDC_Layer_InitStruct.LTDC_CFBStartAdress = Addr + 800*480*2;

  /* Configure blending factors */
 LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
 LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;

  /* Initialize LTDC layer 2 */
 LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);

  /* LTDC configuration reload */
 LTDC_ReloadConfig(LTDC_IMReload);

  /* Enable foreground & background Layers */
 LTDC_LayerCmd(LTDC_Layer1, ENABLE);
 //LTDC_LayerCmd(LTDC_Layer2, ENABLE);

  /* LTDC configuration reload */
 LTDC_ReloadConfig(LTDC_IMReload);



  /* dithering activation */
 LTDC_DitherCmd(ENABLE);

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
