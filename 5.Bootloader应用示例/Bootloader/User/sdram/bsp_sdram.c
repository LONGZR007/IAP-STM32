/**
  ******************************************************************************
  * @file    bsp_sdram.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   sdram应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "./sdram/bsp_sdram.h"   

/**
  * @brief  延迟一段时间
  * @param  延迟的时间长度
  * @retval None
  */
static void SDRAM_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
}

/**
  * @brief  初始化控制SDRAM的IO
  * @param  无
  * @retval 无
  */
static void SDRAM_GPIO_Config(void)
{		
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* 使能SDRAM相关的GPIO时钟 */

                         /*地址信号线*/
  RCC_AHB1PeriphClockCmd(FMC_A0_GPIO_CLK | FMC_A1_GPIO_CLK | FMC_A2_GPIO_CLK | 
                         FMC_A3_GPIO_CLK | FMC_A4_GPIO_CLK | FMC_A5_GPIO_CLK |
                         FMC_A6_GPIO_CLK | FMC_A7_GPIO_CLK | FMC_A8_GPIO_CLK |
                         FMC_A9_GPIO_CLK | FMC_A10_GPIO_CLK| FMC_A11_GPIO_CLK|FMC_A12_GPIO_CLK|    
                         /*数据信号线*/
                         FMC_D0_GPIO_CLK | FMC_D1_GPIO_CLK | FMC_D2_GPIO_CLK | 
                         FMC_D3_GPIO_CLK | FMC_D4_GPIO_CLK | FMC_D5_GPIO_CLK |
                         FMC_D6_GPIO_CLK | FMC_D7_GPIO_CLK | FMC_D8_GPIO_CLK |
                         FMC_D9_GPIO_CLK | FMC_D10_GPIO_CLK| FMC_D11_GPIO_CLK|
                         FMC_D12_GPIO_CLK| FMC_D13_GPIO_CLK| FMC_D14_GPIO_CLK|
                         FMC_D15_GPIO_CLK|  
                         /*控制信号线*/
                         FMC_CS_GPIO_CLK | FMC_BA0_GPIO_CLK | FMC_BA1_GPIO_CLK | 
                         FMC_WE_GPIO_CLK | FMC_RAS_GPIO_CLK | FMC_CAS_GPIO_CLK |
                         FMC_CLK_GPIO_CLK | FMC_CKE_GPIO_CLK | FMC_UDQM_GPIO_CLK |
                         FMC_LDQM_GPIO_CLK, ENABLE);


  
  /*-- GPIO 配置 -----------------------------------------------------*/

  /* 通用 GPIO 配置 */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;       //配置为复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;   
  
  
  /*地址信号线 针对引脚配置*/
  GPIO_InitStructure.GPIO_Pin = FMC_A0_GPIO_PIN; 
  GPIO_Init(FMC_A0_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A0_GPIO_PORT, FMC_A0_PINSOURCE , FMC_A0_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A1_GPIO_PIN; 
  GPIO_Init(FMC_A1_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A1_GPIO_PORT, FMC_A1_PINSOURCE , FMC_A1_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A2_GPIO_PIN; 
  GPIO_Init(FMC_A2_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A2_GPIO_PORT, FMC_A2_PINSOURCE , FMC_A2_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A3_GPIO_PIN; 
  GPIO_Init(FMC_A3_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A3_GPIO_PORT, FMC_A3_PINSOURCE , FMC_A3_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A4_GPIO_PIN; 
  GPIO_Init(FMC_A4_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A4_GPIO_PORT, FMC_A4_PINSOURCE , FMC_A4_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A5_GPIO_PIN; 
  GPIO_Init(FMC_A5_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A5_GPIO_PORT, FMC_A5_PINSOURCE , FMC_A5_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A6_GPIO_PIN; 
  GPIO_Init(FMC_A6_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A6_GPIO_PORT, FMC_A6_PINSOURCE , FMC_A6_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A7_GPIO_PIN; 
  GPIO_Init(FMC_A7_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A7_GPIO_PORT, FMC_A7_PINSOURCE , FMC_A7_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A8_GPIO_PIN; 
  GPIO_Init(FMC_A8_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A8_GPIO_PORT, FMC_A8_PINSOURCE , FMC_A8_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A9_GPIO_PIN; 
  GPIO_Init(FMC_A9_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A9_GPIO_PORT, FMC_A9_PINSOURCE , FMC_A9_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A10_GPIO_PIN; 
  GPIO_Init(FMC_A10_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A10_GPIO_PORT, FMC_A10_PINSOURCE , FMC_A10_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A11_GPIO_PIN; 
  GPIO_Init(FMC_A11_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A11_GPIO_PORT, FMC_A11_PINSOURCE , FMC_A11_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_A12_GPIO_PIN; 
  GPIO_Init(FMC_A12_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_A12_GPIO_PORT, FMC_A12_PINSOURCE , FMC_A12_AF);
  
  
  
  /*数据信号线 针对引脚配置*/
  GPIO_InitStructure.GPIO_Pin = FMC_D0_GPIO_PIN; 
  GPIO_Init(FMC_D0_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D0_GPIO_PORT, FMC_D0_PINSOURCE , FMC_D0_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D1_GPIO_PIN; 
  GPIO_Init(FMC_D1_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D1_GPIO_PORT, FMC_D1_PINSOURCE , FMC_D1_AF);
    
  GPIO_InitStructure.GPIO_Pin = FMC_D2_GPIO_PIN; 
  GPIO_Init(FMC_D2_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D2_GPIO_PORT, FMC_D2_PINSOURCE , FMC_D2_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D3_GPIO_PIN; 
  GPIO_Init(FMC_D3_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D3_GPIO_PORT, FMC_D3_PINSOURCE , FMC_D3_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D4_GPIO_PIN; 
  GPIO_Init(FMC_D4_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D4_GPIO_PORT, FMC_D4_PINSOURCE , FMC_D4_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D5_GPIO_PIN; 
  GPIO_Init(FMC_D5_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D5_GPIO_PORT, FMC_D5_PINSOURCE , FMC_D5_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D6_GPIO_PIN; 
  GPIO_Init(FMC_D6_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D6_GPIO_PORT, FMC_D6_PINSOURCE , FMC_D6_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D7_GPIO_PIN; 
  GPIO_Init(FMC_D7_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D7_GPIO_PORT, FMC_D7_PINSOURCE , FMC_D7_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D8_GPIO_PIN; 
  GPIO_Init(FMC_D8_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D8_GPIO_PORT, FMC_D8_PINSOURCE , FMC_D8_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D9_GPIO_PIN; 
  GPIO_Init(FMC_D9_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D9_GPIO_PORT, FMC_D9_PINSOURCE , FMC_D9_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D10_GPIO_PIN; 
  GPIO_Init(FMC_D10_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D10_GPIO_PORT, FMC_D10_PINSOURCE , FMC_D10_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D11_GPIO_PIN; 
  GPIO_Init(FMC_D11_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D11_GPIO_PORT, FMC_D11_PINSOURCE , FMC_D11_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D12_GPIO_PIN; 
  GPIO_Init(FMC_D12_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D12_GPIO_PORT, FMC_D12_PINSOURCE , FMC_D12_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D13_GPIO_PIN; 
  GPIO_Init(FMC_D13_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D13_GPIO_PORT, FMC_D13_PINSOURCE , FMC_D13_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D14_GPIO_PIN; 
  GPIO_Init(FMC_D14_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D14_GPIO_PORT, FMC_D14_PINSOURCE , FMC_D14_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_D15_GPIO_PIN; 
  GPIO_Init(FMC_D15_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_D15_GPIO_PORT, FMC_D15_PINSOURCE , FMC_D15_AF);
  
  /*控制信号线*/
  GPIO_InitStructure.GPIO_Pin = FMC_CS_GPIO_PIN; 
  GPIO_Init(FMC_CS_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_CS_GPIO_PORT, FMC_CS_PINSOURCE , FMC_CS_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_BA0_GPIO_PIN; 
  GPIO_Init(FMC_BA0_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_BA0_GPIO_PORT, FMC_BA0_PINSOURCE , FMC_BA0_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_BA1_GPIO_PIN;
  GPIO_Init(FMC_BA1_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_BA1_GPIO_PORT, FMC_BA1_PINSOURCE , FMC_BA1_AF);
    
  GPIO_InitStructure.GPIO_Pin = FMC_WE_GPIO_PIN; 
  GPIO_Init(FMC_WE_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_WE_GPIO_PORT, FMC_WE_PINSOURCE , FMC_WE_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_RAS_GPIO_PIN; 
  GPIO_Init(FMC_RAS_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_RAS_GPIO_PORT, FMC_RAS_PINSOURCE , FMC_RAS_AF);
    
  GPIO_InitStructure.GPIO_Pin = FMC_CAS_GPIO_PIN; 
  GPIO_Init(FMC_CAS_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_CAS_GPIO_PORT, FMC_CAS_PINSOURCE , FMC_CAS_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_CLK_GPIO_PIN; 
  GPIO_Init(FMC_CLK_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_CLK_GPIO_PORT, FMC_CLK_PINSOURCE , FMC_CLK_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_CKE_GPIO_PIN; 
  GPIO_Init(FMC_CKE_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_CKE_GPIO_PORT, FMC_CKE_PINSOURCE , FMC_CKE_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_UDQM_GPIO_PIN; 
  GPIO_Init(FMC_UDQM_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_UDQM_GPIO_PORT, FMC_UDQM_PINSOURCE , FMC_UDQM_AF);
  
  GPIO_InitStructure.GPIO_Pin = FMC_LDQM_GPIO_PIN; 
  GPIO_Init(FMC_LDQM_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(FMC_LDQM_GPIO_PORT, FMC_LDQM_PINSOURCE , FMC_LDQM_AF);

		
}

/**
  * @brief  对SDRAM芯片进行初始化配置
  * @param  None. 
  * @retval None.
  */
static void SDRAM_InitSequence(void)
{
  FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;
  uint32_t tmpr = 0;
  
/* Step 3 --------------------------------------------------------------------*/
  /* 配置命令：开启提供给SDRAM的时钟 */
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_COMMAND_TARGET_BANK;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* 检查SDRAM标志，等待至SDRAM空闲 */ 
  while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  /* 发送上述命令*/
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);  
  
/* Step 4 --------------------------------------------------------------------*/
  /* 约100ms的延时 */
  SDRAM_delay(10);
    
/* Step 5 --------------------------------------------------------------------*/
  /* 配置命令：对所有的bank预充电 */ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_COMMAND_TARGET_BANK;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* 检查SDRAM标志，等待至SDRAM空闲 */ 
  while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
   /* 发送上述命令*/
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 6 --------------------------------------------------------------------*/
  /* 配置命令：自动刷新 */   
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_COMMAND_TARGET_BANK;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 4;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  /* 检查SDRAM标志，等待至SDRAM空闲 */ 
  while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
   /* 发送第1次自动刷新命令*/
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
  /* 检查SDRAM标志，等待至SDRAM空闲 */ 
  while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
   /* 发送第2次自动刷新命令*/
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 7 --------------------------------------------------------------------*/
  /* 设置sdram寄存器配置 */
  tmpr = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                   SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                   SDRAM_MODEREG_CAS_LATENCY_3           |
                   SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                   SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  /* 配置命令：设置SDRAM寄存器 */
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_LoadMode;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_COMMAND_TARGET_BANK;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = tmpr;
  /* 检查SDRAM标志，等待至SDRAM空闲 */ 
  while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
   /* 发送上述命令*/
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
/* Step 8 --------------------------------------------------------------------*/

  /* 设置刷新计数器 */
  /* (7.81 us x Freq) - 20 */
  FMC_SetRefreshCount(683);
  /* 发送上述命令*/
  while(FMC_GetFlagStatus(FMC_BANK_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
}


/**
  * @brief  初始化配置使用SDRAM的FMC及GPIO接口，
  *         本函数在SDRAM读写操作前需要被调用
  * @param  None
  * @retval None
  */
void SDRAM_Init(void)
{
  FMC_SDRAMInitTypeDef  FMC_SDRAMInitStructure;
  FMC_SDRAMTimingInitTypeDef  FMC_SDRAMTimingInitStructure; 
  
  /* 配置FMC接口相关的 GPIO*/
  SDRAM_GPIO_Config();
  
  /* 使能 FMC 时钟 */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
 
  /* 配置 FMC 相关参数 ---------------------------------------------------------*/
  /* SDCLK： 90 Mhz (HCLK/2 :180Mhz/2) */
  /* TMRD: 2 Clock cycles */
  FMC_SDRAMTimingInitStructure.FMC_LoadToActiveDelay    = 2;      
  /* TXSR: min=70ns (7x11.11ns) */
  FMC_SDRAMTimingInitStructure.FMC_ExitSelfRefreshDelay = 7;
  /* TRAS: min=42ns (4x11.11ns) max=120k (ns) */
  FMC_SDRAMTimingInitStructure.FMC_SelfRefreshTime      = 4;
  /* TRC:  min=70 (7x11.11ns) */        
  FMC_SDRAMTimingInitStructure.FMC_RowCycleDelay        = 7;         
  /* TWR:  min=1+ 7ns (1+1x11.11ns) */
  FMC_SDRAMTimingInitStructure.FMC_WriteRecoveryTime    = 2;      
  /* TRP:  20ns => 2x11.11ns */
  FMC_SDRAMTimingInitStructure.FMC_RPDelay              = 2;                
  /* TRCD: 20ns => 2x11.11ns */
  FMC_SDRAMTimingInitStructure.FMC_RCDDelay             = 2;

/* FMC SDRAM 控制配置 */
  FMC_SDRAMInitStructure.FMC_Bank = FMC_BANK_SDRAM;
  /* 行地址线宽度: [7:0] */
  FMC_SDRAMInitStructure.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_9b;
  /* 列地址线宽度: [11:0] */
  FMC_SDRAMInitStructure.FMC_RowBitsNumber = FMC_RowBits_Number_13b;
  /* 数据线宽度 */
  FMC_SDRAMInitStructure.FMC_SDMemoryDataWidth = SDRAM_MEMORY_WIDTH;
  /* SDRAM内部bank数量*/
  FMC_SDRAMInitStructure.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
  /* CAS潜伏期 */
  FMC_SDRAMInitStructure.FMC_CASLatency = SDRAM_CAS_LATENCY; 
  /* 禁止写保护*/
  FMC_SDRAMInitStructure.FMC_WriteProtection = FMC_Write_Protection_Disable;
  /* SDCLK时钟分频因子，SDCLK = HCLK/SDCLOCK_PERIOD*/
  FMC_SDRAMInitStructure.FMC_SDClockPeriod = SDCLOCK_PERIOD; 
  /* 突发读模式设置*/  
  FMC_SDRAMInitStructure.FMC_ReadBurst = SDRAM_READBURST;
  /* 读延迟配置 */
  FMC_SDRAMInitStructure.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
  /* SDRAM时序参数 */
  FMC_SDRAMInitStructure.FMC_SDRAMTimingStruct = &FMC_SDRAMTimingInitStructure;
  
  /* FMC SDRAM bank initialization */
  FMC_SDRAMInit(&FMC_SDRAMInitStructure); 
  
  /* FMC SDRAM device initialization sequence */
  SDRAM_InitSequence(); 
  
}



/**
  * @brief  以“字”为单位向sdram写入数据 
  * @param  pBuffer: 指向数据的指针 
  * @param  uwWriteAddress: 要写入的SDRAM内部地址
  * @param  uwBufferSize: 要写入数据大小
  * @retval None.
  */
void SDRAM_WriteBuffer(uint32_t* pBuffer, uint32_t uwWriteAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwWriteAddress;

  /* 禁止写保护 */
  FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM, DISABLE);
  
  /* 检查SDRAM标志，等待至SDRAM空闲 */ 
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }

  /* 循环写入数据 */
  for (; uwBufferSize != 0; uwBufferSize--) 
  {
    /* 发送数据到SDRAM */
    *(uint32_t *) (SDRAM_BANK_ADDR + write_pointer) = *pBuffer++;

    /* 地址自增*/
    write_pointer += 4;
  }
    
}

/**
  * @brief  从SDRAM中读取数据 
  * @param  pBuffer: 指向存储数据的buffer
  * @param  ReadAddress: 要读取数据的地十
  * @param  uwBufferSize: 要读取的数据大小
  * @retval None.
  */
void SDRAM_ReadBuffer(uint32_t* pBuffer, uint32_t uwReadAddress, uint32_t uwBufferSize)
{
  __IO uint32_t write_pointer = (uint32_t)uwReadAddress;
  
   
  /* 检查SDRAM标志，等待至SDRAM空闲 */  
  while(FMC_GetFlagStatus(FMC_Bank2_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  
  /*读取数据 */
  for(; uwBufferSize != 0x00; uwBufferSize--)
  {
   *pBuffer++ = *(__IO uint32_t *)(SDRAM_BANK_ADDR + write_pointer );
    
   /* 地址自增*/
    write_pointer += 4;
  } 
}

/**
  * @brief  测试SDRAM是否正常 
  * @param  None
  * @retval 正常返回1，异常返回0
  */
uint8_t SDRAM_Test(void)
{
  /*写入数据计数器*/
  uint32_t counter=0;
  
  /* 8位的数据 */
  uint8_t ubWritedata_8b = 0, ubReaddata_8b = 0;  
  
  /* 16位的数据 */
  uint16_t uhWritedata_16b = 0, uhReaddata_16b = 0; 
  
//  SDRAM_INFO("正在检测SDRAM，以8位、16位的方式读写sdram...");


  /*按8位格式读写数据，并校验*/
  
  /* 把SDRAM数据全部重置为0 ，IS42S16400J_SIZE是以16位为单位的 */
  for (counter = 0x00; counter < IS42S16400J_SIZE; counter++)
  {
    *(__IO uint8_t*) (SDRAM_BANK_ADDR + counter) = (uint8_t)0x0;
  }
  
  /* 向整个SDRAM写入数据  8位 */
  for (counter = 0; counter < IS42S16400J_SIZE; counter++)
  {
    *(__IO uint8_t*) (SDRAM_BANK_ADDR + counter) = (uint8_t)(ubWritedata_8b + counter);
  }
  
  /* 读取 SDRAM 数据并检测*/
  for(counter = 0; counter<IS42S16400J_SIZE;counter++ )
  {
    ubReaddata_8b = *(__IO uint8_t*)(SDRAM_BANK_ADDR + counter);  //从该地址读出数据
    
    if(ubReaddata_8b != (uint8_t)(ubWritedata_8b + counter))      //检测数据，若不相等，跳出函数,返回检测失败结果。
    {
//      SDRAM_ERROR("8位数据读写错误！");
      return 0;
    }
  }
	
  
  /*按16位格式读写数据，并检测*/
  
  /* 把SDRAM数据全部重置为0 */
  for (counter = 0x00; counter < IS42S16400J_SIZE/2; counter++)
  {
    *(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*counter) = (uint16_t)0x00;
  }
  
  /* 向整个SDRAM写入数据  16位 */
  for (counter = 0; counter < IS42S16400J_SIZE/2; counter++)
  {
    *(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*counter) = (uint16_t)(uhWritedata_16b + counter);
  }
  
    /* 读取 SDRAM 数据并检测*/
  for(counter = 0; counter<IS42S16400J_SIZE/2;counter++ )
  {
    uhReaddata_16b = *(__IO uint16_t*)(SDRAM_BANK_ADDR + 2*counter);  //从该地址读出数据
    
    if(uhReaddata_16b != (uint16_t)(uhWritedata_16b + counter))      //检测数据，若不相等，跳出函数,返回检测失败结果。
    {
//      SDRAM_ERROR("16位数据读写错误！");

      return 0;
    }
  }

  
//  SDRAM_INFO("SDRAM读写测试正常！");
  /*检测正常，return 1 */
  return 1;
  

}


/*********************************************END OF FILE**********************/

