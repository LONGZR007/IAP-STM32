#ifndef XMODEM_H_
#define XMODEM_H_

#include "stm32f4xx.h"
#include <stdio.h>

/* YModem (128 bytes) packet format
 * Byte  0:         Header
 * Byte  1:         Packet number
 * Byte  2:         Packet number complement
 * Bytes 3-132:     Data
 * Bytes 132-133:   CRC
 */

/* YModem (1024 bytes) packet format
 * Byte  0:         Header
 * Byte  1:         Packet number
 * Byte  2:         Packet number complement
 * Bytes 3-1026:    Data
 * Bytes 1027-1028: CRC
 */
 
/* 数据接收缓冲区大小(用户定义,至少要大于 3 + 1024 + 2) */
#define Y_PROT_FRAME_LEN_RECV  1500u

/* 最大允许错误(用户定义). */
#define Y_MAY_ERRORS ((uint8_t)10u)

/* 接收数据超时错误. */
#define Y_RECEIVE_TIMEOUT    0xFFFFFFu

/* 包头接收标志 */
#define Y_IS_PACKET     ((uint8_t)1u)
#define Y_NO_PACKET     ((uint8_t)0u)

/* 数据包大小. */
#define Y_PACKET_128_SIZE   ((uint16_t)128u)
#define Y_PACKET_1024_SIZE  ((uint16_t)1024u)
#define Y_PACKET_CRC_SIZE   ((uint16_t)2u)

/* 包的相对位置(包括报头). */
#define Y_PACKET_NUMBER_INDEX             ((uint16_t)1u)
#define Y_PACKET_NUMBER_COMPLEMENT_INDEX  ((uint16_t)2u)
#define Y_PACKET_DATA_INDEX               ((uint16_t)3u)

/* 协议定义的字节. */
#define Y_SOH ((uint8_t)0x01u)  /**< 包头 (128 bytes). */
#define Y_STX ((uint8_t)0x02u)  /**< 包头 (1024 bytes). */
#define Y_EOT ((uint8_t)0x04u)  /**< 传输结束. */
#define Y_ACK ((uint8_t)0x06u)  /**< 应答. */
#define Y_NAK ((uint8_t)0x15u)  /**< 非应答. */
#define Y_CAN ((uint8_t)0x18u)  /**< 取消. */
#define Y_C   ((uint8_t)0x43u)  /**< ASCII“C”，要通知上位机，我们要用CRC16. */

/* 功能的状态报告. */
typedef enum {
  Y_OK            = 0x00u, /**< 传输成功. */
  Y_ERROR_CRC     = 0x01u, /**< CRC 校验误差. */
  Y_ERROR_NUMBER  = 0x02u, /**< 包数量不匹配错误. */
  Y_ERROR_UART    = 0x04u, /**< 传输错误. */
  Y_ERROR_FLASH   = 0x06u, /**< Flash 错误. */
  Y_EOY           = 0x07u, /**< 文件传输结束 */
  Y_ERROR         = 0xFFu  /**< 其他错误. */
} ymodem_status;

#define Y_UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
  
/***************************** 对外函数 ***************************************/
/* 用户调用 */
void ymodem_receive(void);
void ymodem_data_recv(uint8_t *data, uint16_t data_len);

/* 用户需实现 */
int y_transmit_ch(uint8_t ch);
int receive_nanme_size_callback(void *ptr, char *file_name, uint32_t file_size);
int receive_file_data_callback(void *ptr, char *file_data, uint32_t w_size);
int receive_file_callback(void *ptr);
/***************************** 对外函数 ***************************************/

#endif /* XMODEM_H_ */
