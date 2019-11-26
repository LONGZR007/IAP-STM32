#ifndef __IAP_ARCH_H
#define	__IAP_ARCH_H

#include "stm32f4xx.h"

#define     RECEIVE_TIMEOUT    0xFFFFFFFF

int x_receive(volatile uint8_t *data, uint32_t len);
int x_transmit_ch(uint8_t ch);
uint32_t x_receive_flash_erasure(uint32_t address);
int x_receive_flash_writea(uint32_t start_address, const void *data, uint32_t len);

#endif /* __IAP_ARCH_H */

