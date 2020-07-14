#ifndef _YMODEM_H_
#define _YMODEM_H_

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
 
/* �������Ͷ��� */
typedef unsigned          char y_uint8_t;
typedef unsigned short     int y_uint16_t;
typedef unsigned           int y_uint32_t;
 
/* ���ݽ��ջ�������С(�û�����,����Ҫ���� 3 + 1024 + 2) */
#define Y_PROT_FRAME_LEN_RECV  1500u

/* ����������(�û�����). */
#define Y_MAY_ERRORS ((y_uint8_t)10u)


/* Э�鴫��״̬����. */
typedef enum {
  Y_EN       = 0x00u, /**< ʹ��. */
  Y_DIS      = 0x01u, /**< ����. */
  Y_RUN_RECV = 0x02u, /**< ����λ��ͨѶ�ɹ���������������. */
  Y_MAX_ERR  = 0x04u, /**< �������ﵽ����������������. */
  Y_DISK_ERR = 0x06u, /**< ����д����ִ������������. */
  Y_END      = 0x07u, /**< �ļ�������������. */
  Y_CANCEL   = 0x07u, /**< ��λ��ȡ���������. */
  Y_ERR      = 0xFFu  /**< ��������. */
} ymodem_status_t;

/* ��ⳬʱ�������� */
/* 0��ʹ�þ�ȷʱ����м�⣬��ʵ�� y_uint32_t y_get_tick(void) ������
      �ú���Ӧ���غ���ʱ�������Ȼ��Ҳ����������ֵ��������Ҫ���� Y_RECEIVE_TIMEOUT ��ֵ
      
   1��ʹ��ѭ���� Y_RECEIVE_TIMEOUT ����ֵ0�ķ�������ͬƵ�ʵĿ�������ܻ���Ҫ��ͬ����ֵ*/
#define TIMEOUT_CONFIG      1

/* �������ݳ�ʱ�����Ƽ�����Ϊ 3 �볬ʱ. */
#if TIMEOUT_CONFIG
#define Y_RECEIVE_TIMEOUT    0xFFFFFFu
#else
#define Y_RECEIVE_TIMEOUT    3000u
#endif

/* ��ͷ���ձ�־ */
#define Y_IS_PACKET     ((y_uint8_t)1u)
#define Y_NO_PACKET     ((y_uint8_t)0u)

/* ���ݰ���С. */
#define Y_PACKET_128_SIZE   ((y_uint16_t)128u)
#define Y_PACKET_1024_SIZE  ((y_uint16_t)1024u)
#define Y_PACKET_CRC_SIZE   ((y_uint16_t)2u)

/* �������λ��(������ͷ). */
#define Y_PACKET_NUMBER_INDEX             ((y_uint16_t)1u)
#define Y_PACKET_NUMBER_COMPLEMENT_INDEX  ((y_uint16_t)2u)
#define Y_PACKET_DATA_INDEX               ((y_uint16_t)3u)

/* Э�鶨����ֽ�. */
#define Y_SOH ((y_uint8_t)0x01u)  /**< ��ͷ (128 bytes). */
#define Y_STX ((y_uint8_t)0x02u)  /**< ��ͷ (1024 bytes). */
#define Y_EOT ((y_uint8_t)0x04u)  /**< �������. */
#define Y_ACK ((y_uint8_t)0x06u)  /**< Ӧ��. */
#define Y_NAK ((y_uint8_t)0x15u)  /**< ��Ӧ��. */
#define Y_CAN ((y_uint8_t)0x18u)  /**< ȡ��. */
#define Y_C   ((y_uint8_t)0x43u)  /**< ASCII��C����Ҫ֪ͨ��λ��������Ҫ��CRC16. */

/* ���ܵ�״̬����. */
typedef enum {
  Y_OK            = 0x00u, /**< ����ɹ�. */
  Y_ERROR_CRC     = 0x01u, /**< CRC У�����. */
  Y_ERROR_NUMBER  = 0x02u, /**< ��������ƥ�����. */
  Y_ERROR_UART    = 0x04u, /**< �������. */
  Y_ERROR_FLASH   = 0x06u, /**< Flash ����. */
  Y_EOY           = 0x07u, /**< �ļ�������� */
  Y_ERROR         = 0xFFu  /**< ��������. */
} ymodem_fun_status_t;

#define Y_UNUSED(Y) (void)Y      /* To avoid gcc/g++ warnings */
  
/***************************** ���⺯�� ***************************************/
/* �û����� */
y_uint16_t ymodem_receive(void);
void ymodem_start_recv(void);
void ymodem_start_recv(void);
ymodem_fun_status_t get_ymodem_recv_status(void);
void ymodem_data_recv(y_uint8_t *data, y_uint16_t data_len);

/* �û���ʵ�� */
y_uint32_t y_get_tick(void);
int y_transmit_ch(y_uint8_t ch);
int receive_nanme_size_callback(void *ptr, char *file_name, y_uint32_t file_size);
int receive_file_data_callback(void *ptr, char *file_data, y_uint32_t w_size);
int receive_file_callback(void *ptr);
/***************************** ���⺯�� ***************************************/

#endif /* XMODEM_H_ */
