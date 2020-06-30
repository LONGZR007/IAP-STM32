#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_Mapper.h"
#include "InfoNES_pAPU.h"
#include "K6502.h"
/*
#include "lcd.h"
#include "delay.h"
#include "malloc.h"
 */


//Table for Mirroring
const BYTE PPU_MirrorTable[][ 4 ] =
{
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE1, NAME_TABLE1 },
  { NAME_TABLE0, NAME_TABLE1, NAME_TABLE0, NAME_TABLE1 },
  { NAME_TABLE1, NAME_TABLE1, NAME_TABLE1, NAME_TABLE1 },
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE0, NAME_TABLE0 },
  { NAME_TABLE0, NAME_TABLE1, NAME_TABLE2, NAME_TABLE3 },
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE0, NAME_TABLE1 }
};
/*Mapper Function*/
//Initialize Mapper 
void (*MapperInit)();
//Write to Mapper 
void (*MapperWrite)( WORD wAddr, BYTE byData );
//Write to SRAM 
void (*MapperSram)( WORD wAddr, BYTE byData );
//Write to Apu 
void (*MapperApu)( WORD wAddr, BYTE byData );
//Read from Apu 
BYTE (*MapperReadApu)( WORD wAddr );
//Callback at VSync 
void (*MapperVSync)();
//Callback at HSync 
void (*MapperHSync)();
//Callback at PPU read/write 
void (*MapperPPU)( WORD wAddr );
//Callback at Rendering Screen 1:BG, 0:Sprite
void (*MapperRenderScreen)( BYTE byMode );


/*===================================================================*/
/*                                                                   */
/*                InfoNES_Init() : Initialize InfoNES                */
/*                                                                   */
/*===================================================================*/
void InfoNES_Init()
{
/*
 *  Initialize InfoNES
 *
 *  Remarks
 *    Initialize K6502 and Scanline Table.
 */
  int nIdx;

  // Initialize 6502
  K6502_Init();

  // Initialize Scanline Table
  for ( nIdx = 0; nIdx < 263; ++nIdx )
  {
    if ( nIdx < SCAN_ON_SCREEN_START )
      NES->PPU_ScanTable[ nIdx ] = SCAN_ON_SCREEN;
    else
    if ( nIdx < SCAN_BOTTOM_OFF_SCREEN_START )
      NES->PPU_ScanTable[ nIdx ] = SCAN_ON_SCREEN;
    else
    if ( nIdx < SCAN_UNKNOWN_START )
      NES->PPU_ScanTable[ nIdx ] = SCAN_ON_SCREEN;
    else
    if ( nIdx < SCAN_VBLANK_START )
      NES->PPU_ScanTable[ nIdx ] = SCAN_UNKNOWN;
    else
      NES->PPU_ScanTable[ nIdx ] = SCAN_VBLANK;
  }
}

/*===================================================================*/
/*                                                                   */
/*                InfoNES_Fin() : Completion treatment               */
/*                                                                   */
/*===================================================================*/
void InfoNES_Fin()
{
/*
 *  Completion treatment
 *
 *  Remarks
 *    Release resources
 */
  // Finalize pAPU
  InfoNES_pAPUDone();

  // Release a memory for ROM
  InfoNES_ReleaseRom();
}

/*===================================================================*/
/*                                                                   */
/*                  InfoNES_Load() : Load a cassette                 */
/*                                                                   */
/*===================================================================*/
int InfoNES_Load( char *pszFileName )
{
/*
 *  Load a cassette
 *
 *  Parameters
 *    const char *pszFileName            (Read)
 *      File name of ROM image
 *
 *  Return values
 *     0 : It was finished normally.
 *    -1 : An error occurred.
 *
 *  Remarks
 *    Read a ROM image in the memory. 
 *    Reset InfoNES.
 */

  // Release a memory for ROM
  InfoNES_ReleaseRom();

  // Read a ROM image in the memory
  if ( InfoNES_ReadRom( pszFileName ) < 0 )
  {
    return -1;
  }

  // Reset InfoNES
  if ( InfoNES_Reset() < 0 )
  {
    return -1;
  }

  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*                 InfoNES_Reset() : Reset InfoNES                   */
/*                                                                   */
/*===================================================================*/

int InfoNES_Reset()
{
/*
 *  Reset InfoNES
 *
 *  Return values
 *     0 : Normally
 *    -1 : Non support mapper
 *
 *  Remarks
 *    Initialize Resources, PPU and Mapper.
 *    Reset CPU.
 */
  int nIdx;

  // Get information on the ROM
  NES->ROM_Mirroring = Neshd->byInfo1 & 1;
  NES->ROM_SRAM = Neshd->byInfo1 & 2;
  NES->ROM_Trainer = Neshd->byInfo1 & 4;
  NES->ROM_FourScr = Neshd->byInfo1 & 8;

  /*-------------------------------------------------------------------*/
  /*  Initialize resources                                             */
  /*-------------------------------------------------------------------*/

  // Clear RAM
  InfoNES_MemorySet( RAM, 0, RAM_SIZE );

  // Reset frame skip and frame count
  //zcl
  //NES->FrameSkip = 2;
  NES->FrameCnt = 0;

  // Reset update flag of ChrBuf
  NES->ChrBufUpdate = 0xff;

  // Reset palette table
  InfoNES_MemorySet( NES->PalTable, 0, 32 );

  // Reset APU register
  InfoNES_MemorySet( NES->APU_Reg, 0, 0x18 );

  // Reset joypad
  NES->PAD1_Latch = NES->PAD2_Latch = NES->PAD_System = 0;
  NES->PAD1_Bit = NES->PAD2_Bit = 0;

  /*-------------------------------------------------------------------*/
  /*  Initialize PPU                                                   */
  /*-------------------------------------------------------------------*/

  InfoNES_SetupPPU();

  /*-------------------------------------------------------------------*/
  /*  Initialize pAPU                                                  */
  /*-------------------------------------------------------------------*/

  InfoNES_pAPUInit();

  /*-------------------------------------------------------------------*/
  /*  Initialize Mapper                                                */
  /*-------------------------------------------------------------------*/
  // Get Mapper Table Index
  for ( nIdx = 0; MapTab[ nIdx ].nMapperNo != -1; ++nIdx )
  {
    if ( MapTab[ nIdx ].nMapperNo == NES->MapperNo )
      break;
  }

  if ( MapTab[ nIdx ].nMapperNo == -1 )
  {
    // Non support mapper
    InfoNES_MessageBox( "Mapper is unsupported.", NES->MapperNo );
    return -1;
  }


  // Set up a mapper initialization function
  MapTab[nIdx].pMapperInit();

  /*-------------------------------------------------------------------*/
  /*  Reset CPU                                                        */
  /*-------------------------------------------------------------------*/

  K6502_Reset();

  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*                InfoNES_SetupPPU() : Initialize PPU                */
/*                                                                   */
/*===================================================================*/
void InfoNES_SetupPPU()
{
/*
 *  Initialize PPU
 *
 */
  int nPage;

  // Clear PPU and Sprite Memory
  InfoNES_MemorySet( PPURAM, 0, PPURAM_SIZE );
  InfoNES_MemorySet( NES->SPRRAM, 0, SPRRAM_SIZE );

  // Reset PPU Register
  NES->PPU_R0 = NES->PPU_R1 = NES->PPU_R2 = NES->PPU_R3 = NES->PPU_R7 = 0;

  // Reset latch flag
  NES->PPU_Latch_Flag = 0;

  // Reset up and down clipping flag
  NES->PPU_UpDown_Clip = 0;

  NES->FrameStep = 0;
  NES->FrameIRQ_Enable = 0;

  // Reset Scroll values
  NES->PPU_Scr_V = NES->PPU_Scr_V_Next = NES->PPU_Scr_V_Byte = NES->PPU_Scr_V_Byte_Next = NES->PPU_Scr_V_Bit = NES->PPU_Scr_V_Bit_Next = 0;
  NES->PPU_Scr_H = NES->PPU_Scr_H_Next = NES->PPU_Scr_H_Byte = NES->PPU_Scr_H_Byte_Next = NES->PPU_Scr_H_Bit = NES->PPU_Scr_H_Bit_Next = 0;

  // Reset PPU address
  NES->PPU_Addr = 0;
  NES->PPU_Temp = 0;

  // Reset scanline
  NES->PPU_Scanline = 0;

  // Reset hit position of sprite #0 
  NES->SpriteJustHit = 0;

  // Reset information on PPU_R0
  NES->PPU_Increment = 1;
  NES->PPU_NameTableBank = NAME_TABLE0;
  NES->PPU_BG_Base = ChrBuf;
  NES->PPU_SP_Base = ChrBuf + 256 * 64;
  NES->PPU_SP_Height = 8;

  // Reset PPU banks
  for ( nPage = 0; nPage < 16; ++nPage )
    PPUBANK[ nPage ] = &PPURAM[ nPage * 0x400 ];

  /* Mirroring of Name Table */
  InfoNES_Mirroring( NES->ROM_Mirroring );

  /* Reset VRAM Write Enable */
  NES->byVramWriteEnable = ( Neshd->byVRomSize == 0 ) ? 1 : 0;
}

/*===================================================================*/
/*                                                                   */
/*       InfoNES_Mirroring() : Set up a Mirroring of Name Table      */
/*                                                                   */
/*===================================================================*/
void InfoNES_Mirroring( int nType )
{
/*
 *  Set up a Mirroring of Name Table
 *
 *  Parameters
 *    int nType          (Read)
 *      Mirroring Type
 *        0 : Horizontal
 *        1 : Vertical
 *        2 : One Screen 0x2400
 *        3 : One Screen 0x2000
 *        4 : Four Screen
 *        5 : Special for Mapper #233
 */

  PPUBANK[ NAME_TABLE0 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 0 ] * 0x400 ];
  PPUBANK[ NAME_TABLE1 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 1 ] * 0x400 ];
  PPUBANK[ NAME_TABLE2 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 2 ] * 0x400 ];
  PPUBANK[ NAME_TABLE3 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 3 ] * 0x400 ];
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_Main() : The main loop of InfoNES            */
/*                                                                   */
/*===================================================================*/
void InfoNES_Main()
{
/*
 *  The main loop of InfoNES
 *
 */

  // Initialize InfoNES
  InfoNES_Init();

  // Main loop
  while ( 1 )
  {
    /*-------------------------------------------------------------------*/
    /*  To the menu screen                                               */
    /*-------------------------------------------------------------------*/
    if ( InfoNES_Menu() == -1 )
      break;  // Quit
    
    /*-------------------------------------------------------------------*/
    /*  Start a NES emulation                                            */
    /*-------------------------------------------------------------------*/
    InfoNES_Cycle();
  }

  // Completion treatment
  InfoNES_Fin();
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_Cycle() : The loop of emulation              */
/*                                                                   */
/*===================================================================*/
void InfoNES_Cycle()
{
/*
 *  The loop of emulation
 *
 */

#if 0
  // Set the PPU adress to the buffered value
  if ( ( NES->PPU_R1 & R1_SHOW_SP ) || ( NES->PPU_R1 & R1_SHOW_SCR ) )
		NES->PPU_Addr = NES->PPU_Temp;
#endif

  // Emulation loop
  for (;;)
  {    
    int nStep;

    // Set a flag if a scanning line is a hit in the sprite #0
    if ( NES->SpriteJustHit == NES->PPU_Scanline &&
      NES->PPU_ScanTable[ NES->PPU_Scanline ] == SCAN_ON_SCREEN )
    {
      // # of Steps to execute before sprite #0 hit
      nStep = NES->SPRRAM[ SPR_X ] * STEP_PER_SCANLINE / NES_DISP_WIDTH;

       // Execute instructions
      K6502_Step( nStep );

      // Set a sprite hit flag
      if ( ( NES->PPU_R1 & R1_SHOW_SP ) && ( NES->PPU_R1 & R1_SHOW_SCR ) )
      {
        NES->PPU_R2 |= R2_HIT_SP;
      }

      // NMI is required if there is necessity
      if ( ( NES->PPU_R0 & R0_NMI_SP ) && ( NES->PPU_R1 & R1_SHOW_SP ) )
      {
        NMI_REQ;
      }

      // Execute instructions
      K6502_Step( STEP_PER_SCANLINE - nStep );
    }
    else
    {
      // Execute instructions
      K6502_Step( STEP_PER_SCANLINE );
     }

    // Frame IRQ in H-Sync
    NES->FrameStep += STEP_PER_SCANLINE;
    if ( NES->FrameStep > STEP_PER_FRAME && NES->FrameIRQ_Enable )
    {
      NES->FrameStep %= STEP_PER_FRAME;
      IRQ_REQ;
      NES->APU_Reg[ /*0x4015*/0x15 ] |= 0x40;
    }
     // A mapper function in H-Sync
    MapperHSync();
    
    // A function in H-Sync
    if ( InfoNES_HSync() == -1 ) 	return;  // To the menu screen

    // HSYNC Wait
    InfoNES_Wait();
  }
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_HSync() : A function in H-Sync               */
/*                                                                   */
/*===================================================================*/

int InfoNES_HSync()
{
/*
 *  A function in H-Sync
 *
 *  Return values
 *    0 : Normally
 *   -1 : Exit an emulation
 */
//	Sleep(10);
  /*-------------------------------------------------------------------*/
  /*  Render a scanline                                                */
  /*-------------------------------------------------------------------*/
  if ( NES->FrameCnt == 0 &&
       NES->PPU_ScanTable[ NES->PPU_Scanline ] == SCAN_ON_SCREEN )
  {

    InfoNES_DrawLine();

  }

  /*-------------------------------------------------------------------*/
  /*  Set new scroll values                                            */
  /*-------------------------------------------------------------------*/
  NES->PPU_Scr_V      = NES->PPU_Scr_V_Next;
  NES->PPU_Scr_V_Byte = NES->PPU_Scr_V_Byte_Next;
  NES->PPU_Scr_V_Bit  = NES->PPU_Scr_V_Bit_Next;

  NES->PPU_Scr_H      = NES->PPU_Scr_H_Next;
  NES->PPU_Scr_H_Byte = NES->PPU_Scr_H_Byte_Next;
  NES->PPU_Scr_H_Bit  = NES->PPU_Scr_H_Bit_Next;

  /*-------------------------------------------------------------------*/
  /*  Next Scanline                                                    */
  /*-------------------------------------------------------------------*/
  NES->PPU_Scanline = ( NES->PPU_Scanline == SCAN_VBLANK_END ) ? 0 : NES->PPU_Scanline + 1;


  /*-------------------------------------------------------------------*/
  /*  Operation in the specific scanning line                          */
  /*-------------------------------------------------------------------*/
  switch ( NES->PPU_Scanline )
  {
    case SCAN_TOP_OFF_SCREEN:

      // Reset a PPU status
      NES->PPU_R2 = 0;

      // Set up a character data
      if ( Neshd->byVRomSize == 0 && NES->FrameCnt == 0 )
      {
        InfoNES_SetupChr();
      }

      // Get position of sprite #0
      InfoNES_GetSprHitY();
      break;

    case SCAN_UNKNOWN_START:

       if ( NES->FrameCnt == 0 )
       {
    	   InfoNES_LoadFrame();
       }
      break;

    case SCAN_VBLANK_START:


     if(NES->FrameCnt > 1) 	//FrameSkip(Ìø1Ö¡)
     {
    	 NES->FrameCnt =0;
     }
     else
     {
    	 NES->FrameCnt ++;
     }

      // Set a V-Blank flag
      NES->PPU_R2 = R2_IN_VBLANK;
      // Reset latch flag
      NES->PPU_Latch_Flag = 0;

      // pAPU Sound function in V-Sync
      if ( !NES->APU_Mute )
      {
    	  InfoNES_pAPUVsync();
      }

      // A mapper function in V-Sync
      MapperVSync();


      // Get the condition of the joypad
      InfoNES_PadState( &NES->PAD1_Latch, &NES->PAD2_Latch, &NES->PAD_System );

      // NMI on V-Blank
      if ( NES->PPU_R0 & R0_NMI_VB )
      {
        NMI_REQ;
      }

      // Exit an emulation if a QUIT button is pushed
      if ( PAD_PUSH( NES->PAD_System, PAD_SYS_QUIT ) )
      {
        return -1;  // Exit an emulation
      }

      break;

    default:

    	break;
  }

  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_DrawLine() : Render a scanline               */
/*                                                                   */
/*===================================================================*/
void InfoNES_DrawLine()
{
/*
 *  Render a scanline
 *
 */
  int i;
  int nX;
  int nY;
  int nY4;
  int nYBit;
  WORD *pPalTbl;
  BYTE *pAttrBase;
  WORD *pPoint;
  int nNameTable;
  BYTE *pbyNameTable;
  BYTE *pbyChrData;
  BYTE *pSPRRAM;
  int nAttr;
  int nSprCnt;
  int nIdx;
  int nSprData;
  BYTE bySprCol;
  BYTE pSprBuf[ NES_DISP_WIDTH + 8 ];

  /*-------------------------------------------------------------------*/
  /*  Render Background                                                */
  /*-------------------------------------------------------------------*/

  /* MMC5 VROM switch */
  MapperRenderScreen( 1 );

  // Pointer to the render position
  //pPoint = NES->Linebuf;
  pPoint = &WorkFrame[ NES->PPU_Scanline * NES_DISP_WIDTH ];

  // Clear a scanline if screen is off
  if ( !( NES->PPU_R1 & R1_SHOW_SCR ) )
  {
    InfoNES_MemorySet( pPoint, 0, NES_DISP_WIDTH << 1 );
  }
  else
  {
    nNameTable = NES->PPU_NameTableBank;

    nY = NES->PPU_Scr_V_Byte + ( NES->PPU_Scanline >> 3 );

    nYBit = NES->PPU_Scr_V_Bit + ( NES->PPU_Scanline & 7 );

    if ( nYBit > 7 )
    {
      ++nY;
      nYBit &= 7;
    }
    nYBit <<= 3;

    if ( nY > 29 )
    {
      // Next NameTable (An up-down direction)
      nNameTable ^= NAME_TABLE_V_MASK;
      nY -= 30;
    }

    nX = NES->PPU_Scr_H_Byte;

    nY4 = ( ( nY & 2 ) << 1 );

    /*-------------------------------------------------------------------*/
    /*  Rendering of the block of the left end                           */
    /*-------------------------------------------------------------------*/

    pbyNameTable = PPUBANK[ nNameTable ] + nY * 32 + nX;
    pbyChrData = NES->PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
    pAttrBase = PPUBANK[ nNameTable ] + 0x3c0 + ( nY / 4 ) * 8;
    pPalTbl =  &NES->PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

    for ( nIdx = NES->PPU_Scr_H_Bit; nIdx < 8; ++nIdx )
    {
      *( pPoint++ ) = pPalTbl[ pbyChrData[ nIdx ] ];
    }

    // Callback at PPU read/write
    MapperPPU( PATTBL( pbyChrData ) );

    ++nX;
    ++pbyNameTable;

    /*-------------------------------------------------------------------*/
    /*  Rendering of the left table                                      */
    /*-------------------------------------------------------------------*/

    for ( ; nX < 32; ++nX )
    {
      pbyChrData = NES->PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
      pPalTbl = &NES->PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

      pPoint[ 0 ] = pPalTbl[ pbyChrData[ 0 ] ]; 
      pPoint[ 1 ] = pPalTbl[ pbyChrData[ 1 ] ];
      pPoint[ 2 ] = pPalTbl[ pbyChrData[ 2 ] ];
      pPoint[ 3 ] = pPalTbl[ pbyChrData[ 3 ] ];
      pPoint[ 4 ] = pPalTbl[ pbyChrData[ 4 ] ];
      pPoint[ 5 ] = pPalTbl[ pbyChrData[ 5 ] ];
      pPoint[ 6 ] = pPalTbl[ pbyChrData[ 6 ] ];
      pPoint[ 7 ] = pPalTbl[ pbyChrData[ 7 ] ];
      pPoint += 8;

      // Callback at PPU read/write
      MapperPPU( PATTBL( pbyChrData ) );

      ++pbyNameTable;
    }

    // Holizontal Mirror
    nNameTable ^= NAME_TABLE_H_MASK;

    pbyNameTable = PPUBANK[ nNameTable ] + nY * 32;
    pAttrBase = PPUBANK[ nNameTable ] + 0x3c0 + ( nY / 4 ) * 8;

    /*-------------------------------------------------------------------*/
    /*  Rendering of the right table                                     */
    /*-------------------------------------------------------------------*/

    for ( nX = 0; nX < NES->PPU_Scr_H_Byte; ++nX )
    {
      pbyChrData = NES->PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
      pPalTbl = &NES->PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

      pPoint[ 0 ] = pPalTbl[ pbyChrData[ 0 ] ]; 
      pPoint[ 1 ] = pPalTbl[ pbyChrData[ 1 ] ];
      pPoint[ 2 ] = pPalTbl[ pbyChrData[ 2 ] ];
      pPoint[ 3 ] = pPalTbl[ pbyChrData[ 3 ] ];
      pPoint[ 4 ] = pPalTbl[ pbyChrData[ 4 ] ];
      pPoint[ 5 ] = pPalTbl[ pbyChrData[ 5 ] ];
      pPoint[ 6 ] = pPalTbl[ pbyChrData[ 6 ] ];
      pPoint[ 7 ] = pPalTbl[ pbyChrData[ 7 ] ];
      pPoint += 8;

      // Callback at PPU read/write
      MapperPPU( PATTBL( pbyChrData ) );

      ++pbyNameTable;
    }

    /*-------------------------------------------------------------------*/
    /*  Rendering of the block of the right end                          */
    /*-------------------------------------------------------------------*/

    pbyChrData = NES->PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
    pPalTbl = &NES->PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];
    for ( nIdx = 0; nIdx < NES->PPU_Scr_H_Bit; ++nIdx )
    {
      pPoint[ nIdx ] = pPalTbl[ pbyChrData[ nIdx ] ];
    }

    // Callback at PPU read/write
    MapperPPU( PATTBL( pbyChrData ) );

    /*-------------------------------------------------------------------*/
    /*  Backgroud Clipping                                               */
    /*-------------------------------------------------------------------*/
    if ( !( NES->PPU_R1 & R1_CLIP_BG ) )
    {
      WORD *pPointTop;
      //pPointTop = NES->Linebuf;//&WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
      pPointTop = &WorkFrame[ NES->PPU_Scanline * NES_DISP_WIDTH ];
      InfoNES_MemorySet( pPointTop, 0, 8 << 1 );
    }

    /*-------------------------------------------------------------------*/
    /*  Clear a scanline if up and down clipping flag is set             */
    /*-------------------------------------------------------------------*/
    if ( NES->PPU_UpDown_Clip && 
       ( SCAN_ON_SCREEN_START > NES->PPU_Scanline || NES->PPU_Scanline > SCAN_BOTTOM_OFF_SCREEN_START ) )
    {
      WORD *pPointTop;
      //pPointTop = NES->Linebuf;//&WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
      pPointTop = &WorkFrame[ NES->PPU_Scanline * NES_DISP_WIDTH ];
      InfoNES_MemorySet( pPointTop, 0, NES_DISP_WIDTH << 1 );
    }  
  }

  /*-------------------------------------------------------------------*/
  /*  Render a sprite                                                  */
  /*-------------------------------------------------------------------*/

  /* MMC5 VROM switch */
  MapperRenderScreen( 0 );

  if ( NES->PPU_R1 & R1_SHOW_SP )
  {
    // Reset Scanline Sprite Count
    NES->PPU_R2 &= ~R2_MAX_SP;

    // Reset sprite buffer
    InfoNES_MemorySet( pSprBuf, 0, sizeof(pSprBuf) );

    // Render a sprite to the sprite buffer
    nSprCnt = 0;
    for ( pSPRRAM = NES->SPRRAM + ( 63 << 2 ); pSPRRAM >= NES->SPRRAM; pSPRRAM -= 4 )
    {
      nY = pSPRRAM[ SPR_Y ] + 1;
      if ( nY > NES->PPU_Scanline || nY + NES->PPU_SP_Height <= NES->PPU_Scanline )
        continue;  // Next sprite

     /*-------------------------------------------------------------------*/
     /*  A sprite in scanning line                                        */
     /*-------------------------------------------------------------------*/

      // Holizontal Sprite Count +1
      ++nSprCnt;
      
      nAttr = pSPRRAM[ SPR_ATTR ];
      nYBit = NES->PPU_Scanline - nY;
      nYBit = ( nAttr & SPR_ATTR_V_FLIP ) ? ( NES->PPU_SP_Height - nYBit - 1 ) << 3 : nYBit << 3;

      if ( NES->PPU_R0 & R0_SP_SIZE )
      {
        // Sprite size 8x16
        if ( pSPRRAM[ SPR_CHR ] & 1 )
        {
          pbyChrData = ChrBuf + 256 * 64 + ( ( pSPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit;
        }
        else
        {
          pbyChrData = ChrBuf + ( ( pSPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit;
        }
      }
      else
      {
        // Sprite size 8x8
        pbyChrData = NES->PPU_SP_Base + ( pSPRRAM[ SPR_CHR ] << 6 ) + nYBit;
      }

      nAttr ^= SPR_ATTR_PRI;
      bySprCol = ( nAttr & ( SPR_ATTR_COLOR | SPR_ATTR_PRI ) ) << 2;
      nX = pSPRRAM[ SPR_X ];

      if ( nAttr & SPR_ATTR_H_FLIP )
      {
        // Horizontal flip
        if ( pbyChrData[ 7 ] )
          pSprBuf[ nX ]     = bySprCol | pbyChrData[ 7 ];
        if ( pbyChrData[ 6 ] )
          pSprBuf[ nX + 1 ] = bySprCol | pbyChrData[ 6 ];
        if ( pbyChrData[ 5 ] )
          pSprBuf[ nX + 2 ] = bySprCol | pbyChrData[ 5 ];
        if ( pbyChrData[ 4 ] )
          pSprBuf[ nX + 3 ] = bySprCol | pbyChrData[ 4 ];
        if ( pbyChrData[ 3 ] )
          pSprBuf[ nX + 4 ] = bySprCol | pbyChrData[ 3 ];
        if ( pbyChrData[ 2 ] )
          pSprBuf[ nX + 5 ] = bySprCol | pbyChrData[ 2 ];
        if ( pbyChrData[ 1 ] )
          pSprBuf[ nX + 6 ] = bySprCol | pbyChrData[ 1 ];
        if ( pbyChrData[ 0 ] )
          pSprBuf[ nX + 7 ] = bySprCol | pbyChrData[ 0 ];
      }
      else
      {
        // Non flip
        if ( pbyChrData[ 0 ] )
          pSprBuf[ nX ]     = bySprCol | pbyChrData[ 0 ];
        if ( pbyChrData[ 1 ] )
          pSprBuf[ nX + 1 ] = bySprCol | pbyChrData[ 1 ];
        if ( pbyChrData[ 2 ] )
          pSprBuf[ nX + 2 ] = bySprCol | pbyChrData[ 2 ];
        if ( pbyChrData[ 3 ] )
          pSprBuf[ nX + 3 ] = bySprCol | pbyChrData[ 3 ];
        if ( pbyChrData[ 4 ] )
          pSprBuf[ nX + 4 ] = bySprCol | pbyChrData[ 4 ];
        if ( pbyChrData[ 5 ] )
          pSprBuf[ nX + 5 ] = bySprCol | pbyChrData[ 5 ];
        if ( pbyChrData[ 6 ] )
          pSprBuf[ nX + 6 ] = bySprCol | pbyChrData[ 6 ];
        if ( pbyChrData[ 7 ] )
          pSprBuf[ nX + 7 ] = bySprCol | pbyChrData[ 7 ];
      }
    }

    // Rendering sprite
    pPoint -= ( NES_DISP_WIDTH - NES->PPU_Scr_H_Bit );
    for ( nX = 0; nX < NES_DISP_WIDTH; ++nX )
    {
      nSprData = pSprBuf[ nX ];
      if ( nSprData )
      if ( (nSprData & 0x80) || (pPoint[ nX ] & 0x8000 ) )
      {
        pPoint[ nX ] = NES->PalTable[ ( nSprData & 0xf ) + 0x10 ];
      }
    }

    /*-------------------------------------------------------------------*/
    /*  Sprite Clipping                                                  */
    /*-------------------------------------------------------------------*/
    if ( !( NES->PPU_R1 & R1_CLIP_SP ) )
    {
      WORD *pPointTop;
      //pPointTop = NES->Linebuf;//&WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
      pPointTop = &WorkFrame[ NES->PPU_Scanline * NES_DISP_WIDTH ];
      InfoNES_MemorySet( pPointTop, 0, 8 << 1 );
    }

    if ( nSprCnt >= 8 )
    {
      NES->PPU_R2 |= R2_MAX_SP;  // Set a flag of maximum sprites on scanline
    }
  }


  /*
  for(i=0;i<160;i++)
  {
	  LCD_WR_DATA(NES->Linebuf[(i<<8)/160]); todo: lcd draw line
  }
  */


}

/*===================================================================*/
/*                                                                   */
/* InfoNES_GetSprHitY() : Get a position of scanline hits sprite #0  */
/*                                                                   */
/*===================================================================*/
void InfoNES_GetSprHitY()
{
/*
 * Get a position of scanline hits sprite #0
 *
 */

  int nYBit;
  DWORD *pdwChrData;
  int nOff;

  if ( NES->SPRRAM[ SPR_ATTR ] & SPR_ATTR_V_FLIP )
  {
    // Vertical flip
    nYBit = ( NES->PPU_SP_Height - 1 ) << 3;
    nOff = -2;
  }
  else
  {
    // Non flip
    nYBit = 0;
    nOff = 2;
  }

  if ( NES->PPU_R0 & R0_SP_SIZE )
  {
    // Sprite size 8x16
    if ( NES->SPRRAM[ SPR_CHR ] & 1 )
    {
      pdwChrData = (DWORD *)( ChrBuf + 256 * 64 + ( ( NES->SPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit );
    }
    else
    {
      pdwChrData = (DWORD * )( ChrBuf + ( ( NES->SPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit );
    } 
  }
  else
  {
    // Sprite size 8x8
    pdwChrData = (DWORD *)( NES->PPU_SP_Base + ( NES->SPRRAM[ SPR_CHR ] << 6 ) + nYBit );
  }

  if ( ( NES->SPRRAM[ SPR_Y ] + 1 <= SCAN_UNKNOWN_START ) && ( NES->SPRRAM[SPR_Y] > 0 ) )
	{
		int nLine;
		for ( nLine = 0; nLine < NES->PPU_SP_Height; nLine++ )
		{
			if ( pdwChrData[ 0 ] | pdwChrData[ 1 ] )
			{
        // Scanline hits sprite #0
				NES->SpriteJustHit = NES->SPRRAM[SPR_Y] + 1 + nLine;
				nLine = SCAN_VBLANK_END;
			}
			pdwChrData += nOff;
		}
  } else {
    // Scanline didn't hit sprite #0
		NES->SpriteJustHit = SCAN_UNKNOWN_START + 1;
  }
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_SetupChr() : Develop character data            */
/*                                                                   */
/*===================================================================*/
void InfoNES_SetupChr()
{
/*
 *  Develop character data
 *
 */

  BYTE *pbyBGData;
  BYTE byData1;
  BYTE byData2;
  int nIdx;
  int nY;
  int nOff;
  static BYTE *pbyPrevBank[ 8 ];
  int nBank;

  for ( nBank = 0; nBank < 8; ++nBank )
  {
    if ( pbyPrevBank[ nBank ] == PPUBANK[ nBank ] && !( ( NES->ChrBufUpdate >> nBank ) & 1 ) )
      continue;  // Next bank

    /*-------------------------------------------------------------------*/
    /*  An address is different from the last time                       */
    /*    or                                                             */
    /*  An update flag is being set                                      */
    /*-------------------------------------------------------------------*/

    for ( nIdx = 0; nIdx < 64; ++nIdx )
    {
      nOff = ( nBank << 12 ) + ( nIdx << 6 );

      for ( nY = 0; nY < 8; ++nY )
      {
        pbyBGData = PPUBANK[ nBank ] + ( nIdx << 4 ) + nY;

        byData1 = ( ( pbyBGData[ 0 ] >> 1 ) & 0x55 ) | ( pbyBGData[ 8 ] & 0xAA );
        byData2 = ( pbyBGData[ 0 ] & 0x55 ) | ( ( pbyBGData[ 8 ] << 1 ) & 0xAA );

        ChrBuf[ nOff ]     = ( byData1 >> 6 ) & 3;
        ChrBuf[ nOff + 1 ] = ( byData2 >> 6 ) & 3;
        ChrBuf[ nOff + 2 ] = ( byData1 >> 4 ) & 3;
        ChrBuf[ nOff + 3 ] = ( byData2 >> 4 ) & 3;
        ChrBuf[ nOff + 4 ] = ( byData1 >> 2 ) & 3;
        ChrBuf[ nOff + 5 ] = ( byData2 >> 2 ) & 3;
        ChrBuf[ nOff + 6 ] = byData1 & 3;
        ChrBuf[ nOff + 7 ] = byData2 & 3;

        nOff += 8;
      }
    }
    // Keep this address
    pbyPrevBank[ nBank ] = PPUBANK[ nBank ];
  }

  // Reset update flag
  NES->ChrBufUpdate = 0;
}
