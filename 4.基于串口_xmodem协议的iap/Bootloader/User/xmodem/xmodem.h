#ifndef _XMODEM_H_
#define _XMODEM_H_

#include <stdio.h>

/* XModem (128 bytes) packet format
 * Byte  0:         Header
 * Byte  1:         Packet number
 * Byte  2:         Packet number complement
 * Bytes 3-132:     Data
 * Bytes 132-133:   CRC
 */

/* XModem (1024 bytes) packet format
 * Byte  0:         Header
 * Byte  1:         Packet number
 * Byte  2:         Packet number complement
 * Bytes 3-1026:    Data
 * Bytes 1027-1028: CRC
 */
 
/* 数据类型定义 */
typedef unsigned          char x_uint8_t;
typedef unsigned short     int x_uint16_t;
typedef unsigned           int x_uint32_t;
 
/* 数据接收缓冲区大小(用户定义,至少要大于 3 + 1024 + 2) */
#define X_PROT_FRAME_LEN_RECV  1500u

/* 最大允许错误(用户定义). */
#define X_MAX_ERRORS ((x_uint8_t)10u)

/* 检测超时错误配置 */
/* 0：使用精确时间进行检测，需实现 x_uint32_t x_get_tick(void) 函数，
      该函数应返回毫秒时间戳，当然，也可以是其他值，不过需要调整 X_RECEIVE_TIMEOUT 的值
      
   1：使用循环减 X_RECEIVE_TIMEOUT 到数值0的方法，不同频率的开发板可能会需要不同的数值*/
#define TIMEOUT_CONFIG      1

/* 接收数据超时错误，推荐配置为 3 秒超时. */
#if TIMEOUT_CONFIG
#define X_RECEIVE_TIMEOUT    0xFFFFFFu
#else
#define X_RECEIVE_TIMEOUT    3000u
#endif

/* 包头接收标志 */
#define X_IS_PACKET     ((x_uint8_t)1u)
#define X_NO_PACKET     ((x_uint8_t)0u)

/* 数据包大小. */
#define X_PACKET_128_SIZE   ((x_uint16_t)128u)
#define X_PACKET_1024_SIZE  ((x_uint16_t)1024u)
#define X_PACKET_CRC_SIZE   ((x_uint16_t)2u)

/* 包的相对位置(包括报头). */
#define X_PACKET_NUMBER_INDEX             ((x_uint16_t)1u)
#define X_PACKET_NUMBER_COMPLEMENT_INDEX  ((x_uint16_t)2u)
#define X_PACKET_DATA_INDEX               ((x_uint16_t)3u)

/* 协议定义的字节. */
#define X_SOH ((x_uint8_t)0x01u)  /**< 包头 (128 bytes). */
#define X_STX ((x_uint8_t)0x02u)  /**< 包头 (1024 bytes). */
#define X_EOT ((x_uint8_t)0x04u)  /**< 传输结束. */
#define X_ACK ((x_uint8_t)0x06u)  /**< 应答. */
#define X_NAK ((x_uint8_t)0x15u)  /**< 非应答. */
#define X_CAN ((x_uint8_t)0x18u)  /**< 取消. */
#define X_C   ((x_uint8_t)0x43u)  /**< ASCII“C”，要通知上位机，我们要用CRC16. */

/* 功能的状态报告. */
typedef enum {
  X_OK            = 0x00u, /**< 传输成功. */
  X_ERROR_CRC     = 0x01u, /**< CRC 校验误差. */
  X_ERROR_NUMBER  = 0x02u, /**< 包数量不匹配错误. */
  X_ERROR_UART    = 0x04u, /**< 传输错误. */
  X_ERROR_FLASH   = 0x06u, /**< Flash 错误. */
  X_ERROR         = 0xFFu  /**< 其他错误. */
} xmodem_status;

#define X_UNUSED(Y) (void)Y      /* To avoid gcc/g++ warnings */
  
/***************************** 对外函数 ***************************************/
/* 用户调用 */
int xmodem_receive(void);
void xmodem_data_recv(x_uint8_t *data, x_uint16_t data_len);

/* 用户需实现 */
x_uint32_t x_get_tick(void);
int x_transmit_ch(x_uint8_t ch);
int receive_file_data_callback(void *ptr, char *file_data, x_uint32_t w_size);
int receive_file_callback(void *ptr);
/***************************** 对外函数 ***************************************/

#endif /* XMODEM_H_ */
