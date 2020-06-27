/*===================================================================*/
/*                                                                   */
/*                       Mapper 118 (Others)                         */
/*                                                                   */
/*===================================================================*/

// BYTE  Map118_Regs[ 8 ];
// DWORD Map118_Prg0, Map118_Prg1;
// DWORD Map118_Chr0, Map118_Chr1, Map118_Chr2, Map118_Chr3;
// DWORD Map118_Chr4, Map118_Chr5, Map118_Chr6, Map118_Chr7;
// BYTE Map118_IRQ_Enable;
// BYTE Map118_IRQ_Cnt;
// BYTE Map118_IRQ_Latch;

#define Map118_Chr_Swap()    ( MAP118->Map118_Regs[ 0 ] & 0x80 )
#define Map118_Prg_Swap()    ( MAP118->Map118_Regs[ 0 ] & 0x40 )

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 118                                            */
/*-------------------------------------------------------------------*/
void Map118_Init()
{
  int nPage;

  /* Initialize Mapper */
  MapperInit = Map118_Init;

  /* Write to Mapper */
  MapperWrite = Map118_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map118_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  SRAMBANK = SRAM;

  /* Initialize State Registers */
  for ( nPage = 0; nPage < 8; nPage++)
  {
    MAP118->Map118_Regs[ nPage ] = 0x00;
  }

  /* Set ROM Banks */
  MAP118->Map118_Prg0 = 0;
  MAP118->Map118_Prg1 = 1;
  Map118_Set_CPU_Banks();

  /* Set PPU Banks */
  if ( Neshd->byVRomSize > 0 )
  {
    MAP118->Map118_Chr0 = 0;
    MAP118->Map118_Chr1 = 1;
    MAP118->Map118_Chr2 = 2;
    MAP118->Map118_Chr3 = 3;
    MAP118->Map118_Chr4 = 4;
    MAP118->Map118_Chr5 = 5;
    MAP118->Map118_Chr6 = 6;
    MAP118->Map118_Chr7 = 7;
    Map118_Set_PPU_Banks();
  } else {
    MAP118->Map118_Chr0 = MAP118->Map118_Chr1 = MAP118->Map118_Chr2 = MAP118->Map118_Chr3 = 0;
    MAP118->Map118_Chr4 = MAP118->Map118_Chr5 = MAP118->Map118_Chr6 = MAP118->Map118_Chr7 = 0;
  }

  /* Initialize IRQ Registers */
  MAP118->Map118_IRQ_Enable = 0;
  MAP118->Map118_IRQ_Cnt = 0;
  MAP118->Map118_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 118 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map118_Write( WORD wAddr, BYTE byData )
{
  DWORD dwBankNum;

  switch ( wAddr & 0xe001 )
  {
    case 0x8000:
      MAP118->Map118_Regs[ 0 ] = byData;
      Map118_Set_PPU_Banks();
      Map118_Set_CPU_Banks();
      break;

    case 0x8001:
      MAP118->Map118_Regs[ 1 ] = byData;
      dwBankNum = MAP118->Map118_Regs[ 1 ];

      /* Name Table Mirroring */
      if ( ( MAP118->Map118_Regs[ 0 ] & 0x07 ) < 6 )
      {
        if ( byData & 0x80 )
        {
          InfoNES_Mirroring( 3 );
        } else {
          InfoNES_Mirroring( 2 );
        }
      }

      switch ( MAP118->Map118_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks */
        case 0x00:
          if ( Neshd->byVRomSize > 0 )
          {
            dwBankNum &= 0xfe;
            MAP118->Map118_Chr0 = dwBankNum;
            MAP118->Map118_Chr1 = dwBankNum + 1;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x01:
          if ( Neshd->byVRomSize > 0 )
          {
            dwBankNum &= 0xfe;
            MAP118->Map118_Chr2 = dwBankNum;
            MAP118->Map118_Chr3 = dwBankNum + 1;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x02:
          if ( Neshd->byVRomSize > 0 )
          {
            MAP118->Map118_Chr4 = dwBankNum;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x03:
          if ( Neshd->byVRomSize > 0 )
          {
            MAP118->Map118_Chr5 = dwBankNum;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x04:
          if ( Neshd->byVRomSize > 0 )
          {
            MAP118->Map118_Chr6 = dwBankNum;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x05:
          if ( Neshd->byVRomSize > 0 )
          {
            MAP118->Map118_Chr7 = dwBankNum;
            Map118_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks */
        case 0x06:
          MAP118->Map118_Prg0 = dwBankNum;
          Map118_Set_CPU_Banks();
          break;

        case 0x07:
          MAP118->Map118_Prg1 = dwBankNum;
          Map118_Set_CPU_Banks();
          break;
      }
      break;

    case 0xc000:
      MAP118->Map118_Regs[ 4 ] = byData;
      MAP118->Map118_IRQ_Cnt = MAP118->Map118_Regs[ 4 ];
      break;

    case 0xc001:
      MAP118->Map118_Regs[ 5 ] = byData;
      MAP118->Map118_IRQ_Latch = MAP118->Map118_Regs[ 5 ];
      break;

    case 0xe000:
      MAP118->Map118_Regs[ 6 ] = byData;
      MAP118->Map118_IRQ_Enable = 0;
      break;

    case 0xe001:
      MAP118->Map118_Regs[ 7 ] = byData;
      MAP118->Map118_IRQ_Enable = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 118 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map118_HSync()
{
/*
 *  Callback at HSync
 *
 */
  if ( MAP118->Map118_IRQ_Enable )
  {
    if ( /* 0 <= PPU_Scanline && */ NES->PPU_Scanline <= 239 )
    {
      if ( NES->PPU_R1 & R1_SHOW_SCR || NES->PPU_R1 & R1_SHOW_SP )
      {
        if ( !( MAP118->Map118_IRQ_Cnt-- ) )
        {
          MAP118->Map118_IRQ_Cnt = MAP118->Map118_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 118 Set CPU Banks Function                                */
/*-------------------------------------------------------------------*/
void Map118_Set_CPU_Banks()
{
  if ( Map118_Prg_Swap() )
  {
    ROMBANK0 = ROMLASTPAGE( 1 );
    ROMBANK1 = ROMPAGE( MAP118->Map118_Prg1 % ( Neshd->byRomSize << 1 ) );
    ROMBANK2 = ROMPAGE( MAP118->Map118_Prg0 % ( Neshd->byRomSize << 1 ) );
    ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    ROMBANK0 = ROMPAGE( MAP118->Map118_Prg0 % ( Neshd->byRomSize << 1 ) );
    ROMBANK1 = ROMPAGE( MAP118->Map118_Prg1 % ( Neshd->byRomSize << 1 ) );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 118 Set PPU Banks Function                                */
/*-------------------------------------------------------------------*/
void Map118_Set_PPU_Banks()
{
  if ( Neshd->byVRomSize > 0 )
  {
    if ( Map118_Chr_Swap() )
    { 
      PPUBANK[ 0 ] = VROMPAGE( MAP118->Map118_Chr4 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( MAP118->Map118_Chr5 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( MAP118->Map118_Chr6 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( MAP118->Map118_Chr7 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( MAP118->Map118_Chr0 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( MAP118->Map118_Chr1 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( MAP118->Map118_Chr2 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( MAP118->Map118_Chr3 % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( MAP118->Map118_Chr0 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( MAP118->Map118_Chr1 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( MAP118->Map118_Chr2 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( MAP118->Map118_Chr3 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( MAP118->Map118_Chr4 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( MAP118->Map118_Chr5 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( MAP118->Map118_Chr6 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( MAP118->Map118_Chr7 % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
    }
  }
}
