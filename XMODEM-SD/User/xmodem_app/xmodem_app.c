/**
  ******************************************************************************
  * @file    ymodem_app.c
  * @author  long
  * @version V1.0
  * @date    2020-xx-xx
  * @brief   ymodem应用示例
  ******************************************************************************
***/

#include "./xmodem/xmodem.h"
#include "./xmodem_app/xmodem_app.h"
#include "./usart/bsp_debug_usart.h"
#include "./FATFS/ff.h"

/* 全局变量 */
static FIL fnew;												/* 文件对象 */
static FRESULT res_sd;                  /* 文件操作结果 */
static UINT fnum;            					  /* 文件成功读写数量 */
static uint8_t recv_flag = 0;
#define RECV_FILE_NAME     "recv_file.txt"

/**
 * @brief   Ymodem 发送一个字符的接口.
 * @param   ch ：发送的数据
 * @return  返回发送状态
 */
int x_transmit_ch(uint8_t ch)
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
int open_file_receive(void *ptr, char *file_name)
{
  char buff[50];
  
  sprintf(buff, "0:/%s", file_name);
  res_sd = f_open(&fnew, buff, FA_CREATE_ALWAYS | FA_WRITE );
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
  if (recv_flag == 0)
  {
    recv_flag = 1;    // 标记正在接收一个文件
    if(open_file_receive(&fnew, RECV_FILE_NAME) == -1)
    {	
      /* Flash 错误. */
      return -1;
    }
  }
  
  res_sd=f_write(&fnew, (uint8_t *)file_data, (uint32_t)w_size, &fnum);
  if(res_sd != FR_OK)
  {
    /* 不再读写，关闭文件 */
    f_close(&fnew);
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
  f_close(&fnew);
  recv_flag = 0;
  
  return 0;
}

