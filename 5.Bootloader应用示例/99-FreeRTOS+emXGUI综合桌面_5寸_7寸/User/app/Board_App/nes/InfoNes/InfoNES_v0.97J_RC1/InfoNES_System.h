/*===================================================================*/
/*                                                                   */
/*  InfoNES_System.h : The function which depends on a system        */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_SYSTEM_H_INCLUDED
#define InfoNES_SYSTEM_H_INCLUDED

#ifdef	__cplusplus
extern "C"{
#endif

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "InfoNES_Types.h"

extern WORD *WorkFrame;

/* Palette data */
/*
static const WORD NesPalette[ 64 ] =
{	//RGB565
	0x738e,0x20d1,0x0015,0x4013,0x880e,0xa802,0xa000,0x7840,
	0x4140,0x0200,0x0280,0x01c2,0x19cb,0x0000,0x0000,0x0000,
	0xbdd7,0x039d,0x21dd,0x801e,0xb817,0xe00b,0xd940,0xca41,
	0x8b80,0x0480,0x0540,0x0487,0x0411,0x0000,0x0000,0x0000,
	0xffdf,0x3ddf,0x5c9f,0x445f,0xf3df,0xfb96,0xfb8c,0xfcc7,
	0xf5c7,0x8682,0x4ec9,0x5fd3,0x075b,0x0000,0x0000,0x0000,
	0xffdf,0xaf1f,0xc69f,0xd65f,0xfe1f,0xfe1b,0xfdd6,0xfed5,
	0xff14,0xe7d4,0xaf97,0xb7d9,0x9fde,0x0000,0x0000,0x0000,
};*/

static const WORD NesPalette[ 64 ] =
{	//XRGB1555
  0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20,
  0x20a0, 0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000,
  0x5ef7, 0x01dd, 0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521,
  0x45c0, 0x0240, 0x02a0, 0x0247, 0x0211, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x1eff, 0x2e5f, 0x223f, 0x79ff, 0x7dd6, 0x7dcc, 0x7e67,
  0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b, 0x7ef6, 0x7f75,
  0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000, 0x0000
};

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

void NES_Main(void);
/* Menu screen */
int InfoNES_Menu(void);

/* Read ROM image file */
int InfoNES_ReadRom( const char *pszFileName );

/* Release a memory for ROM */
void InfoNES_ReleaseRom(void);

/* Transfer the contents of work frame on the screen */
void InfoNES_LoadFrame(void);

/* Get a joypad state */
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem );

/* memcpy */
void *InfoNES_MemoryCopy( void *dest, const void *src, int count );

/* memset */
void *InfoNES_MemorySet( void *dest, int c, int count );

/* Print debug message */
void InfoNES_DebugPrint( char *pszMsg );

/* Wait */
void InfoNES_Wait(void);

/* Sound Initialize */
void InfoNES_SoundInit( void );

/* Sound Open */
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate );

/* Sound Close */
void InfoNES_SoundClose( void );

void GAME_TX_callback(void);
/* Sound Output 5 Waves - 2 Pulse, 1 Triangle, 1 Noise, 1 DPCM */
void InfoNES_SoundOutput(int samples,WORD* wave);

/* Print system message */
void InfoNES_MessageBox( char *pszMsg, ... );

void My_task(void);

//void	DebugPuts(const char *str);
//void	DebugPrintf(const char *fmt,...);
//#include	"Include.h"

#ifdef	__cplusplus
}
#endif

#endif /* !InfoNES_SYSTEM_H_INCLUDED */
