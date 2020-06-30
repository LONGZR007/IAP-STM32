/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"

#ifndef FATFS_FLASH_SPI
	#define FATFS_FLASH_SPI				1
#endif

#ifndef FATFS_USE_SDIO
	#define FATFS_USE_SDIO			1
#endif

/* Set in defines.h file if you want it */
#ifndef TM_FATFS_CUSTOM_FATTIME
	#define TM_FATFS_CUSTOM_FATTIME		0
#endif


/* Include SD card files if is enabled */
#if FATFS_USE_SDIO == 1
	#include "./fatfs/drivers/fatfs_sd_sdio.h"
#endif

#if FATFS_FLASH_SPI == 1
	#include "./fatfs/drivers/fatfs_flash_spi.h"
#endif


/* Definitions of physical drive number for each media */
#define ATA			    0
#define SPI_FLASH		1

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{

	DSTATUS status = STA_NOINIT;
	FLASH_DEBUG_FUNC() ;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = TM_FATFS_SD_SDIO_disk_initialize();	/* SDIO communication */
			#endif
			break;
		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
			status = TM_FATFS_FLASH_SPI_disk_initialize();	/* SDIO communication */
			#endif
			break;

		default:
			status = STA_NOINIT;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{

	DSTATUS status = STA_NOINIT;
	
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = TM_FATFS_SD_SDIO_disk_status();	/* SDIO communication */
			#endif
			break;
		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
			status = TM_FATFS_FLASH_SPI_disk_status();	/* SDIO communication */
			#endif
			break;

		default:
			status = STA_NOINIT;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = TM_FATFS_SD_SDIO_disk_read(buff, sector, count);	/* SDIO communication */
			#endif
			break;
		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
			status = TM_FATFS_FLASH_SPI_disk_read(buff, sector, count);	/* SDIO communication */
			#endif
		break;
		default:
			status = RES_PARERR;
	}
	return status;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DRESULT status = RES_PARERR;
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}
	
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = TM_FATFS_SD_SDIO_disk_write((BYTE *)buff, sector, count);	/* SDIO communication */
			#endif
		break;

		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
			status = TM_FATFS_FLASH_SPI_disk_write((BYTE *)buff, sector, count);	/* SDIO communication */
			#endif
		break;
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			#if FATFS_USE_SDIO == 1
				status = TM_FATFS_SD_SDIO_disk_ioctl(cmd, buff);					/* SDIO communication */
			#endif
			break;
		case SPI_FLASH:
			#if	FATFS_FLASH_SPI ==1
			status = TM_FATFS_FLASH_SPI_disk_ioctl(cmd, buff);	/* SDIO communication */
			#endif
		break;
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif

__weak DWORD get_fattime(void) {
	/* Returns current time packed into a DWORD variable */
	return	  ((DWORD)(2013 - 1980) << 25)	/* Year 2013 */
			| ((DWORD)7 << 21)				/* Month 7 */
			| ((DWORD)28 << 16)				/* Mday 28 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				/* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}


