/**
  ******************************************************************************
  * @file    ymodem_app.c
  * @author  long
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ymodem应用示例
  ******************************************************************************
***/

#include "./ymodem/ymodem.h"
#include "./ymodem_app/ymodem_app.h"
#include "./usart/bsp_debug_usart.h"
#include "./FATFS/ff.h"

/* 全局变量 */
static FIL fnew;												/* 文件对象 */
static FRESULT res_sd;                  /* 文件操作结果 */
static UINT fnum;            					  /* 文件成功读写数量 */

/**
 * @brief   Ymodem 发送一个字符的接口.
 * @param   ch ：发送的数据
 * @return  返回发送状态
 */
int y_transmit_ch(uint8_t ch)
{
	Usart_SendByte(DEBUG_USART, ch);
	
	return 0;
}

/**
 * @brief   文件名和大小接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_nanme_size_callback(void *ptr, char *file_name, uint32_t file_size)
{
  char buff[50];
  FIL **f_ptr = ptr;    /* 文件对象 */
  
  ptr = &fnew;
  
  sprintf(buff, "0:/%s", file_name);
  res_sd = f_open(*f_ptr, buff, FA_OPEN_APPEND | FA_WRITE );
  if ( res_sd != FR_OK )
  {	
    /* Flash 错误. */
    return -1;
  }
  
  return 0;
}

/**
 * @brief   文件数据接收完成回调.
 * @param   *ptr: 控制句柄.
 * @param   *file_name: 文件名字.
 * @param   file_size: 文件大小，若为0xFFFFFFFF，则说明大小无效.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_data_callback(void *ptr, char *file_data, uint32_t w_size)
{
  FIL **f_ptr = ptr;    /* 文件对象 */
  
  res_sd=f_write(*f_ptr, (uint8_t *)file_data, (uint32_t)w_size, &fnum);
  if(res_sd != FR_OK)
  {
    /* 不再读写，关闭文件 */
    f_close(*f_ptr);
    /* Flash 错误. */
    return -1;
  }
  
  return 0;
}

/**
 * @brief   一个文件接收完成回调.
 * @param   *ptr: 控制句柄.
 * @return  返回写入的结果，0：成功，-1：失败.
 */
int receive_file_callback(void *ptr)
{
  FIL **f_ptr = ptr;    /* 文件对象 */
  f_close(*f_ptr);
  
  return 0;
}

