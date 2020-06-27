/************************************************************************/
/*			----- general.h -----				*/
/*									*/
/*	Copyright (C) Egberto Willies, 1988				*/
/*	All rights reserved.						*/
/*									*/
/*Author-								*/
/*	Egberto Willies.						*/
/*									*/
/*Date-									*/
/*	25-Jan-1988							*/
/*									*/
/*Description-								*/
/*	General purpose include file.					*/
/*									*/
/*Notes-								*/
/*									*/
/************************************************************************/
#ifndef	FILE_GENERAL_H
#define	FILE_GENERAL_H

/* Generic definitions */

#define	OFF			0
#define	ON			1

#define	FALSE			0
#define	TRUE			1

#define	BIOS			0
#define	ANSI			1

/* General purpose macros*/

#define	mydebug(str)	printf(str)

/* Structure definition for message by code */

struct code_message
{
int	code;
char	*msg;
};
/************************************************************************/
#endif

