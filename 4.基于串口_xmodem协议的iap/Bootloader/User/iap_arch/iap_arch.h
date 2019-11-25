#ifndef __IAP_ARCH_H
#define	__IAP_ARCH_H

#include "stm32f4xx.h"

int x_receive(volatile uint8_t *data, uint32_t len);
int x_transmit_ch(uint8_t ch);

#endif /* __IAP_ARCH_H */

