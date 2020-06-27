/*===================================================================*/
/*                                                                   */
/*  InfoNES.h : NES Emulator for Win32, Linux(x86), Linux(PS2)       */
/*                                                                   */
/*  2000/05/14  InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_H_INCLUDED
#define InfoNES_H_INCLUDED

#include "InfoNES_Types.h"

/* Initialize Mapper */
extern void (*MapperInit)();
/* Write to Mapper */
extern void (*MapperWrite)( WORD wAddr, BYTE byData );
/* Write to SRAM */
extern void (*MapperSram)( WORD wAddr, BYTE byData );
/* Write to APU */
extern void (*MapperApu)( WORD wAddr, BYTE byData );
/* Read from Apu */
extern BYTE (*MapperReadApu)( WORD wAddr );
/* Callback at VSync */
extern void (*MapperVSync)();
/* Callback at HSync */
extern void (*MapperHSync)();
/* Callback at PPU read/write */
extern void (*MapperPPU)( WORD wAddr );
/* Callback at Rendering Screen 1:BG, 0:Sprite */
extern void (*MapperRenderScreen)( BYTE byMode );

#ifdef	__cplusplus
extern "C"{
#endif

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/



/*-------------------------------------------------------------------*/
/*  NES resources                                                    */
/*-------------------------------------------------------------------*/

#define RAM_SIZE     0x2000
#define SRAM_SIZE    0x2000
#define PPURAM_SIZE  0x4000
#define SPRRAM_SIZE  256


typedef struct
{
    DWORD PAD1_Latch;
    DWORD PAD2_Latch;
    DWORD PAD_System;
    DWORD PAD1_Bit;
    DWORD PAD2_Bit; 
    BYTE *PPU_BG_Base;//BG Base Address
    BYTE *PPU_SP_Base;//Sprite Base Address    
    int  SpriteJustHit;//Sprite #0 Scanline Hit Position
    int  APU_Mute;//APU Mute(0:OFF,1:ON)     
    WORD PPU_Addr;//PPU Address
    WORD PPU_Temp;//PPU Address
    WORD PPU_Increment;//The increase value of the PPU Address
    WORD PPU_Scanline;//Current Scanline 
    WORD PPU_SP_Height;//Sprite Height
    WORD FrameStep;       
    WORD PalTable[32];//Palette Table 
 //   WORD Linebuf[322];//Includding padding bytes
    BYTE PPU_NameTableBank;//Name Table Bank    
    BYTE byVramWriteEnable;//VRAM Write Enable ( 0: Disable, 1: Enable )
    BYTE PPU_Latch_Flag;//PPU Address and Scroll Latch Flag
    BYTE PPU_UpDown_Clip;//Up and Down Clipping Flag (0:non-clip,1:clip)
    BYTE FrameIRQ_Enable;//Frame IRQ (0:Disabled,1:Enabled)  
    BYTE ChrBufUpdate;//Update flag for ChrBuf                                       
    BYTE MapperNo;//Mapper Number
    BYTE ROM_Mirroring;//Mirroring 0:Horizontal 1:Vertical
    BYTE ROM_SRAM;//It has SRAM
    BYTE ROM_Trainer;//It has Trainer
    BYTE ROM_FourScr;//Four screen VRAM 
    BYTE PPU_R0;
    BYTE PPU_R1;
    BYTE PPU_R2;
    BYTE PPU_R3;
    BYTE PPU_R7;
    BYTE PPU_Scr_V;
    BYTE PPU_Scr_V_Next;
    BYTE PPU_Scr_V_Byte;
    BYTE PPU_Scr_V_Byte_Next;
    BYTE PPU_Scr_V_Bit;
    BYTE PPU_Scr_V_Bit_Next; 
    BYTE PPU_Scr_H;
    BYTE PPU_Scr_H_Next;
    BYTE PPU_Scr_H_Byte;
    BYTE PPU_Scr_H_Byte_Next;
    BYTE PPU_Scr_H_Bit;
    BYTE PPU_Scr_H_Bit_Next;
    BYTE FrameCnt;      
    BYTE APU_Reg[ 0x18 ];//APU Register    
    BYTE PPU_ScanTable[ 264 ];//Scanline Table
    BYTE SPRRAM[SPRRAM_SIZE];   
}NesResource;

/* .nes File Header */
#pragma pack(1)
typedef struct 
{
  BYTE byID[ 4 ];
  BYTE byRomSize;
  BYTE byVRomSize;
  BYTE byInfo1;
  BYTE byInfo2;
  BYTE byReserve[ 8 ];
}NesHeader;
#pragma pack()

extern NesResource *NES;
/* RAM */
extern BYTE *RAM;
/* SRAM */
extern BYTE *SRAM;
/* ROM */
extern BYTE *ROM;
/* SRAM BANK ( 8Kb ) */
extern BYTE *SRAMBANK;
/* ROM BANK ( 8Kb * 4 ) */
extern BYTE *ROMBANK0;
extern BYTE *ROMBANK1;
extern BYTE *ROMBANK2;
extern BYTE *ROMBANK3;
/*-------------------------------------------------------------------*/
/*  PPU resources                                                    */
/*-------------------------------------------------------------------*/
/* PPU RAM */
extern BYTE *PPURAM;
/* VROM */
extern BYTE *VROM;
/* PPU BANK ( 1Kb * 16 ) */
extern BYTE *PPUBANK[];
/* Sprite RAM */
extern BYTE SPRRAM[];

#if 0
/* PPU Register */
extern BYTE PPU_R0;
extern BYTE PPU_R1;
extern BYTE PPU_R2;
extern BYTE PPU_R3;
extern BYTE PPU_R7;
extern BYTE PPU_Scr_V;
extern BYTE PPU_Scr_V_Next;
extern BYTE PPU_Scr_V_Byte;
extern BYTE PPU_Scr_V_Byte_Next;
extern BYTE PPU_Scr_V_Bit;
extern BYTE PPU_Scr_V_Bit_Next;
extern BYTE PPU_Scr_H;
extern BYTE PPU_Scr_H_Next;
extern BYTE PPU_Scr_H_Byte;
extern BYTE PPU_Scr_H_Byte_Next;
extern BYTE PPU_Scr_H_Bit;
extern BYTE PPU_Scr_H_Bit_Next;
extern BYTE PPU_Latch_Flag;
extern WORD PPU_Addr;
extern WORD PPU_Temp;
extern WORD PPU_Increment;
extern BYTE PPU_Latch_Flag;
extern BYTE PPU_UpDown_Clip;

/* Current Scanline */
extern WORD PPU_Scanline;
/* Scanline Table */
extern BYTE PPU_ScanTable[];
/* Name Table Bank */
extern BYTE PPU_NameTableBank;
/* BG Base Address */
extern BYTE *PPU_BG_Base;
/* Sprite Base Address */
extern BYTE *PPU_SP_Base;
/* Sprite Height */
extern WORD PPU_SP_Height;
/* VRAM Write Enable ( 0: Disable, 1: Enable ) */
extern BYTE byVramWriteEnable;
/* Frame IRQ ( 0: Disabled, 1: Enabled )*/
extern BYTE FrameIRQ_Enable;
extern WORD FrameStep;
#endif
/*-------------------------------------------------------------------*/
/*  Display and Others resouces                                      */
/*-------------------------------------------------------------------*/
/* Frame Skip */
extern WORD FrameSkip;
extern WORD FrameCnt;
extern WORD FrameWait;
extern BYTE *ChrBuf;
extern BYTE ChrBufUpdate;
extern WORD PalTable[];
/*-------------------------------------------------------------------*/
/*  APU and Pad resources                                            */
/*-------------------------------------------------------------------*/
extern BYTE APU_Reg[];
extern int APU_Mute;
extern DWORD PAD1_Latch;
extern DWORD PAD2_Latch;
extern DWORD PAD_System;
extern DWORD PAD1_Bit;
extern DWORD PAD2_Bit;

/* .nes File Header */
extern NesHeader *Neshd;

#if 0
/* Mapper No. */
extern BYTE MapperNo;
/* Other */
extern BYTE ROM_Mirroring;
extern BYTE ROM_SRAM;
extern BYTE ROM_Trainer;
extern BYTE ROM_FourScr;
#endif

#define NAME_TABLE0  8
#define NAME_TABLE1  9
#define NAME_TABLE2  10
#define NAME_TABLE3  11
#define NAME_TABLE_V_MASK 2
#define NAME_TABLE_H_MASK 1
#define SPR_Y    0
#define SPR_CHR  1
#define SPR_ATTR 2
#define SPR_X    3
#define SPR_ATTR_COLOR  0x3
#define SPR_ATTR_V_FLIP 0x80
#define SPR_ATTR_H_FLIP 0x40
#define SPR_ATTR_PRI    0x20
#define R0_NMI_VB      0x80
#define R0_NMI_SP      0x40
#define R0_SP_SIZE     0x20
#define R0_BG_ADDR     0x10
#define R0_SP_ADDR     0x08
#define R0_INC_ADDR    0x04
#define R0_NAME_ADDR   0x03
#define R1_BACKCOLOR   0xe0
#define R1_SHOW_SP     0x10
#define R1_SHOW_SCR    0x08
#define R1_CLIP_SP     0x04
#define R1_CLIP_BG     0x02
#define R1_MONOCHROME  0x01
#define R2_IN_VBLANK   0x80
#define R2_HIT_SP      0x40
#define R2_MAX_SP      0x20
#define R2_WRITE_FLAG  0x10
#define SCAN_TOP_OFF_SCREEN     0
#define SCAN_ON_SCREEN          1
#define SCAN_BOTTOM_OFF_SCREEN  2
#define SCAN_UNKNOWN            3
#define SCAN_VBLANK             4
#define SCAN_TOP_OFF_SCREEN_START       0 
#define SCAN_ON_SCREEN_START            8
#define SCAN_BOTTOM_OFF_SCREEN_START  232
#define SCAN_UNKNOWN_START            240
#define SCAN_VBLANK_START             243
#define SCAN_VBLANK_END               262

#define STEP_PER_SCANLINE             (113)
#define STEP_PER_FRAME                (29828)

/* NES display size */
#define NES_DISP_WIDTH      256
#define NES_DISP_HEIGHT     240

/* Develop Scroll Registers */
#define InfoNES_SetupScr() \
{ \
  /* V-Scroll Register */ \
  /* PPU_Scr_V_Byte_Next = ( BYTE )( ( PPU_Addr & 0x03e0 ) >> 5 ); */ \
  /* PPU_Scr_V_Bit_Next = ( BYTE )( ( PPU_Addr & 0x7000 ) >> 12 ); */ \
  /* H-Scroll Register */ \
  /* PPU_Scr_H_Byte_Next = ( BYTE )( PPU_Addr & 0x001f ); */ \
  /* NameTableBank */ \
  NES->PPU_NameTableBank = NAME_TABLE0 + ( ( NES->PPU_Addr & 0x0C00 ) >> 10 ); \
}

#define PAD_SYS_QUIT   1
#define PAD_SYS_OK     2
#define PAD_SYS_CANCEL 4
#define PAD_SYS_UP     8
#define PAD_SYS_DOWN   0x10
#define PAD_SYS_LEFT   0x20
#define PAD_SYS_RIGHT  0x40
#define PAD_PUSH(a,b)  ( ( (a) & (b) ) != 0 )

/*-------------------------------------------------------------------*/
/*  Mapper Function                                                  */
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*  ROM information                                                  */
/*-------------------------------------------------------------------*/


/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/
/* Initialize InfoNES */
void InfoNES_Init(void);
/* Completion treatment */
void InfoNES_Fin(void);
/* Load a cassette */
int InfoNES_Load( char *pszFileName );
/* Reset InfoNES */
int InfoNES_Reset(void);
/* Initialize PPU */
void InfoNES_SetupPPU(void);
/* Set up a Mirroring of Name Table */
void InfoNES_Mirroring( int nType );
/* The main loop of InfoNES */ 
void InfoNES_Main(void);
/* The loop of emulation */
void InfoNES_Cycle(void);
/* A function in H-Sync */
int InfoNES_HSync(void);
/* Render a scanline */
void InfoNES_DrawLine(void);
/* Get a position of scanline hits sprite #0 */
void InfoNES_GetSprHitY(void);
/* Develop character data */
void InfoNES_SetupChr(void);


#ifdef	__cplusplus
}
#endif

#endif /* !InfoNES_H_INCLUDED */
