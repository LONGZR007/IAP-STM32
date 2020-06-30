/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   用于从SD卡拷贝数据文件到FLASH的工程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
/**
  ******************************************************************************
  *                              头文件
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "./Bsp/led/bsp_led.h" 
#include "./Bsp/usart/bsp_debug_usart.h"
#include "ff.h"
#include "./fatfs/drivers/fatfs_flash_spi.h"
#include "./res_mgr/res_mgr.h"



/**
  ******************************************************************************
  *                              定义变量
  ******************************************************************************
  */
extern FATFS sd_fs;													/* Work area (file system object) for logical drives */
extern FATFS flash_fs;

//要复制的文件路径，到aux_data.c修改
extern char src_dir[];

/*
*********************************************************************************************************
* Description : 初始化BL8782 wifi模块使能引脚，并关闭使能
* Argument(s) : none.
* Return(s)   : none.
*********************************************************************************************************
*/
static void BL8782_PDN_INIT(void)
{
  /*定义一个GPIO_InitTypeDef类型的结构体*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOB, ENABLE); 							   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
  
  GPIO_ResetBits(GPIOB,GPIO_Pin_13);  //禁用WiFi模块
}
/*
*********************************************************************************************************
* Description : 初始化KEY1
* Argument(s) : none.
* Return(s)   : none.
*********************************************************************************************************
*/
static void KEY1_INIT(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  /*设置引脚为输入模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  /*设置引脚不上拉也不下拉*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	/* 设置引脚速度 */
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_2MHz;
  /*使用上面的结构体初始化按键*/
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  主函数
  * @param  无
  * @retval 无
	* @note    读取SD卡XBF字库文件(eg.xinsongti25.xbf)内容烧写到SPI Flash无文件系统
	*          扇区中。
  */
int main(void)
{
  FRESULT res = FR_OK;
  
	BL8782_PDN_INIT();
	/* 初始化LED */
  LED_GPIO_Config();
  KEY1_INIT();

  /* 初始化调试串口，一般为串口1 */
  Debug_USART_Config();
  TM_FATFS_FLASH_SPI_disk_initialize();
  
  res = FR_DISK_ERR;
       
  //挂载SD卡
  res = f_mount(&sd_fs,SD_ROOT,1);

  //如果文件系统挂载失败就退出
  if(res != FR_OK)
  {
    BURN_ERROR("f_mount ERROR!请给开发板插入SD卡然后重新复位开发板!");
    LED_RED;
    while(1);
  }    
    
  printf("\r\n 按一次KEY1开始烧写字库并复制文件到FLASH。 \r\n"); 
  printf("\r\n 注意该操作会把FLASH的原内容会被删除！！ \r\n"); 
  
  while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0);
  while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==1); 
 
  printf("\r\n 正在进行整片擦除，时间很长，请耐心等候...\r\n");
  SPI_FLASH_BulkErase();    
  
  /* 生成烧录目录信息文件 */
  Make_Catalog(src_dir,0);
  
  /* 烧录 目录信息至FLASH*/
  Burn_Catalog();  
  /* 根据 目录 烧录内容至FLASH*/
  Burn_Content();
  /* 校验烧录的内容 */
  Check_Resource();
  
//  printf("读取资源位置测试\r\n");
//  printf("GB2312_H2424.FON offset = %d\r\n", 
//                  GetResOffset("GB2312_H2424.FON"));
  
#if 0
  printf("\r\n 正在进行整片擦除，时间很长，请耐心等候...\r\n");
  SPI_FLASH_BulkErase();   
  //烧录数据到flash的非文件系统区域    
  res = burn_file_sd2flash(burn_data,AUX_MAX_NUM); 
  
  if(res == FR_OK)
  {
    printf("\r\n\r\n\r\n"); 

    //复制文件到FLASH的文件系统区域
    copy_file_sd2flash(src_dir,dst_dir);
    if(res == FR_OK)
    {
      printf("\r\n 所有数据已成功复制到FLASH！！！ \r\n");  
      LED_GREEN;
    }
    else
    {
      printf("\r\n 复制文件到FLASH失败(文件系统部分)，请复位重试！！ \r\n"); 
    }
    
  }
  else
  {
    printf("\r\n 拷贝数据到FLASH失败(非文件系统部分)，请复位重试！！ \r\n"); 
  }
#endif 
  
  while(1);


}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
