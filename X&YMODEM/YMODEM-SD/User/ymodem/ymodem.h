#ifndef XMODEM_H_
#define XMODEM_H_

#include "stm32f4xx.h"
#include <stdio.h>
//#include "uart.h"
//#include "flash.h"
//#include "stdbool.h"

/* Xmodem (128 字节) 包格式
 * Byte  0:       Header
 * Byte  1:       Packet number
 * Byte  2:       Packet number complement
 * Bytes 3-130:   Data
 * Bytes 131-132: CRC
 */

/* Xmodem (1024 bytes) packet format
 * Byte  0:         Header
 * Byte  1:         Packet number
 * Byte  2:         Packet number complement
 * Bytes 3-1026:    Data
 * Bytes 1027-1028: CRC
 */

/* 最大允许错误(用户定义). */
#define Y_MAY_ERRORS ((uint8_t)3u)

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
  Y_ERROR         = 0xFFu  /**< 其他错误. */
} ymodem_status;

void ymodem_receive(uint16_t rec_num);

#endif /* XMODEM_H_ */
