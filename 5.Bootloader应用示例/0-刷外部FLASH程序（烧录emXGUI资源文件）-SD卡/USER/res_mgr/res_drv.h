#ifndef __RES_DRV_H
#define	__RES_DRV_H

#include "stm32f4xx.h"


int RES_GetOffset(const char *res_name);
void RES_DevTest(void);
void RES_DevInit(void);
uint32_t RES_DevGetID(void);
int8_t RES_DevWrite(u8 *buf,u32 addr,u32 size);
int8_t RES_DevRead(u8 *buf,u32 addr,u32 size);
int RES_DevEraseSector(u32 addr);

#endif /* __RES_DRV_H */
