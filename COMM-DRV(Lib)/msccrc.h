/****************************************************************/
/*			----- msccrc.h -----			*/
/*								*/
/*	Willies' Computer Software Company			*/
/*	Copyright (C) Egberto Willies, 1991			*/
/*	All rights reserved.					*/
/*								*/
/*Author							*/
/*	Egberto Willies 05-31-1991				*/
/*								*/
/*Configuration-						*/
/*								*/
/*Description-							*/
/*								*/
/****************************************************************/
#ifndef	MSCCRC_H
#define	MSCCRC_H

#define CRC16(cp,crc) (crc16tab[(((unsigned short)crc>>8)&255)]^((unsigned short)crc<<8)^(unsigned char)cp)
#define CRC32(cp,crc) (crc32tab[((unsigned long)crc^(unsigned char)cp)&0xff]^(((unsigned long)crc>>8)&0x00FFFFFF))

extern unsigned short	crc16tab[];
extern unsigned long	crc32tab[];
#endif
