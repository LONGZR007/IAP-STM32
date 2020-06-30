/*===================================================================*/
/*                                                                   */
/*                            Mapper 1                               */
/*                                                                   */
/*===================================================================*/


//Mapper1Res MAP1;

// BYTE  Map1_Regs[ 4 ];
// DWORD Map1_Cnt;
// BYTE  Map1_Latch;
// WORD  Map1_Last_Write_Addr;
// Map1_Size_t Map1_Size;
// DWORD Map1_256K_base;
// DWORD Map1_swap;
// // these are the 4 ROM banks currently selected
// DWORD Map1_bank1;
// DWORD Map1_bank2;
// DWORD Map1_bank3;
// DWORD Map1_bank4;

// DWORD Map1_HI1;
// DWORD Map1_HI2;


static void Map1_set_ROM_banks()
{
  ROMBANK0 = ROMPAGE( ( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank1 & ((256/8)-1)) ) % ( Neshd->byRomSize << 1 ) );  
  ROMBANK1 = ROMPAGE( ( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank2 & ((256/8)-1)) ) % ( Neshd->byRomSize << 1 ) );
  ROMBANK2 = ROMPAGE( ( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank3 & ((256/8)-1)) ) % ( Neshd->byRomSize << 1 ) );
  ROMBANK3 = ROMPAGE( ( (MAP1->Map1_256K_base << 5) + (MAP1->Map1_bank4 & ((256/8)-1)) ) % ( Neshd->byRomSize << 1 ) ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 1 Write Function                                          */
/*-------------------------------------------------------------------*/
static void Map1_Write( WORD wAddr, BYTE byData )
{
/*
 * MMC1
 */
  DWORD dwRegNum;

  // if write is to a different reg, reset
  if( ( wAddr & 0x6000 ) != ( MAP1->Map1_Last_Write_Addr & 0x6000 ) )
  {
    MAP1->Map1_Cnt = 0;
    MAP1->Map1_Latch = 0x00;
  }
  MAP1->Map1_Last_Write_Addr = wAddr;

  // if bit 7 set, reset and return
  if ( byData & 0x80 )
  {
    MAP1->Map1_Cnt = 0;
    MAP1->Map1_Latch = 0x00;
    return;
  }

  if ( byData & 0x01 ) MAP1->Map1_Latch |= ( 1 << MAP1->Map1_Cnt );
  MAP1->Map1_Cnt++;
  if( MAP1->Map1_Cnt < 5 ) return;

  dwRegNum = ( wAddr & 0x7FFF ) >> 13;
  MAP1->Map1_Regs[ dwRegNum ] = MAP1->Map1_Latch;

  MAP1->Map1_Cnt = 0;
  MAP1->Map1_Latch = 0x00;

  switch( dwRegNum )
  {
    case 0:
      {
        // set mirroring
        if( MAP1->Map1_Regs[0] & 0x02 )
        {
          if( MAP1->Map1_Regs[0] & 0x01 )
          {
            InfoNES_Mirroring( 0 );
          }
          else
          {
            InfoNES_Mirroring( 1 );
          }
        }
        else
        {
          // one-screen mirroring
          if( MAP1->Map1_Regs[0] & 0x01 )
          {
            InfoNES_Mirroring( 2 );
          }
          else
          {
            InfoNES_Mirroring( 3 );
          }
        }
      }
      break;

    case 1:
      {
        BYTE byBankNum = MAP1->Map1_Regs[1];

        if ( MAP1->Map1_Size == Map1_1024K )
        {
          if ( MAP1->Map1_Regs[0] & 0x10 )
          {
            if ( MAP1->Map1_swap )
            {
              MAP1->Map1_256K_base = (MAP1->Map1_Regs[1] & 0x10) >> 4;
              if(MAP1->Map1_Regs[0] & 0x08)
              {
                MAP1->Map1_256K_base |= ((MAP1->Map1_Regs[2] & 0x10) >> 3);
              }
              Map1_set_ROM_banks();
              MAP1->Map1_swap = 0;
            }
            else
            {
              MAP1->Map1_swap = 1;
            }
          }
          else
          {
            // use 1st or 4th 256K banks
            MAP1->Map1_256K_base = ( MAP1->Map1_Regs[1] & 0x10 ) ? 3 : 0;
            Map1_set_ROM_banks();
          }
        }
        else if((MAP1->Map1_Size == Map1_512K) && (!Neshd->byVRomSize))
        {
          MAP1->Map1_256K_base = (MAP1->Map1_Regs[1] & 0x10) >> 4;
          Map1_set_ROM_banks();
        }
        else if ( Neshd->byVRomSize )
        {
          // set VROM bank at $0000
          if ( MAP1->Map1_Regs[0] & 0x10 )
          {
            // swap 4K
            byBankNum <<= 2;
            PPUBANK[ 0 ] = VROMPAGE( (byBankNum+0) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 1 ] = VROMPAGE( (byBankNum+1) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 2 ] = VROMPAGE( (byBankNum+2) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 3 ] = VROMPAGE( (byBankNum+3) % (Neshd->byVRomSize << 3) );
            InfoNES_SetupChr();
          }
          else
          {
            // swap 8K
            byBankNum <<= 2;
            PPUBANK[ 0 ] = VROMPAGE( (byBankNum+0) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 1 ] = VROMPAGE( (byBankNum+1) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 2 ] = VROMPAGE( (byBankNum+2) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 3 ] = VROMPAGE( (byBankNum+3) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 4 ] = VROMPAGE( (byBankNum+4) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 5 ] = VROMPAGE( (byBankNum+5) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 6 ] = VROMPAGE( (byBankNum+6) % (Neshd->byVRomSize << 3) );
            PPUBANK[ 7 ] = VROMPAGE( (byBankNum+7) % (Neshd->byVRomSize << 3) );
            InfoNES_SetupChr();
          }
        }
      }
      break;

    case 2:
      {
        BYTE byBankNum = MAP1->Map1_Regs[2];

        if((MAP1->Map1_Size == Map1_1024K) && (MAP1->Map1_Regs[0] & 0x08))
        {
          if(MAP1->Map1_swap)
          {
            MAP1->Map1_256K_base =  (MAP1->Map1_Regs[1] & 0x10) >> 4;
            MAP1->Map1_256K_base |= ((MAP1->Map1_Regs[2] & 0x10) >> 3);
            Map1_set_ROM_banks();
            MAP1->Map1_swap = 0;
          }
          else
          {
            MAP1->Map1_swap = 1;
          }
        }

        if(!Neshd->byVRomSize) 
        {
          if ( MAP1->Map1_Regs[ 0 ] & 0x10 )
          {
            byBankNum <<= 2;
#if 0
            PPUBANK[ 4 ] = VRAMPAGE0( byBankNum+0 );
            PPUBANK[ 5 ] = VRAMPAGE0( byBankNum+1 );
            PPUBANK[ 6 ] = VRAMPAGE0( byBankNum+2 );
            PPUBANK[ 7 ] = VRAMPAGE0( byBankNum+3 );
#else
            PPUBANK[ 4 ] = CRAMPAGE( byBankNum+0 );
            PPUBANK[ 5 ] = CRAMPAGE( byBankNum+1 );
            PPUBANK[ 6 ] = CRAMPAGE( byBankNum+2 );
            PPUBANK[ 7 ] = CRAMPAGE( byBankNum+3 );
#endif
            InfoNES_SetupChr();
            break;
          }
        }

        // set 4K VROM bank at $1000
        if(MAP1->Map1_Regs[0] & 0x10)
        {
          // swap 4K
          byBankNum <<= 2;
          PPUBANK[ 4 ] = VROMPAGE( (byBankNum+0) % (Neshd->byVRomSize << 3) );
          PPUBANK[ 5 ] = VROMPAGE( (byBankNum+1) % (Neshd->byVRomSize << 3) );
          PPUBANK[ 6 ] = VROMPAGE( (byBankNum+2) % (Neshd->byVRomSize << 3) );
          PPUBANK[ 7 ] = VROMPAGE( (byBankNum+3) % (Neshd->byVRomSize << 3) );
          InfoNES_SetupChr(); 
        }
      }
      break;

    case 3:
      {
        BYTE byBankNum = MAP1->Map1_Regs[3];

        // set ROM bank
        if ( MAP1->Map1_Regs[0] & 0x08 )
        {
          // 16K of ROM
          byBankNum <<= 1;

          if ( MAP1->Map1_Regs[0] & 0x04 )
          {
            // 16K of ROM at $8000
            MAP1->Map1_bank1 = byBankNum;
            MAP1->Map1_bank2 = byBankNum+1;
            MAP1->Map1_bank3 = MAP1->Map1_HI1;
            MAP1->Map1_bank4 = MAP1->Map1_HI2;
          }
          else
          {
            // 16K of ROM at $C000
            if(MAP1->Map1_Size == Map1_SMALL)
            {
              MAP1->Map1_bank1 = 0;
              MAP1->Map1_bank2 = 1;
              MAP1->Map1_bank3 = byBankNum;
              MAP1->Map1_bank4 = byBankNum+1;
            }
          }
        }
        else
        {
          // 32K of ROM at $8000
          byBankNum <<= 1;

          MAP1->Map1_bank1 = byBankNum;
          MAP1->Map1_bank2 = byBankNum+1;
          if(MAP1->Map1_Size == Map1_SMALL)
          {
            MAP1->Map1_bank3 = byBankNum+2;
            MAP1->Map1_bank4 = byBankNum+3;
          }
        }
        Map1_set_ROM_banks();
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 1                                              */
/*-------------------------------------------------------------------*/
void Map1_Init()
{
  DWORD size_in_K;

  /* Initialize Mapper */
  MapperInit = Map1_Init;

  /* Write to Mapper */
  MapperWrite = Map1_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map0_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  SRAMBANK = SRAM;

  /* Initialize State Registers */
  MAP1->Map1_Cnt = 0;
  MAP1->Map1_Latch = 0x00;

  MAP1->Map1_Regs[ 0 ] = 0x0c;
  MAP1->Map1_Regs[ 1 ] = 0x00;
  MAP1->Map1_Regs[ 2 ] = 0x00;
  MAP1->Map1_Regs[ 3 ] = 0x00;

  size_in_K = ( Neshd->byRomSize << 1 ) * 8;

  if ( size_in_K == 1024 )
  {
    MAP1->Map1_Size = Map1_1024K;
  }
  else if(size_in_K == 512)
  {
    MAP1->Map1_Size = Map1_512K;
  }
  else
  {
    MAP1->Map1_Size = Map1_SMALL;
  }

  MAP1->Map1_256K_base = 0; // use first 256K
  MAP1->Map1_swap = 0;

  if( MAP1->Map1_Size == Map1_SMALL )
  {
    // set two high pages to last two banks
    MAP1->Map1_HI1 = ( Neshd->byRomSize << 1 ) - 2;
    MAP1->Map1_HI2 = ( Neshd->byRomSize << 1 ) - 1;
  }
  else
  {
    // set two high pages to last two banks of current 256K region
    MAP1->Map1_HI1 = ( 256 / 8 ) - 2;
    MAP1->Map1_HI2 = ( 256 / 8 ) - 1;
  }

  // set CPU bank pointers
  MAP1->Map1_bank1 = 0;
  MAP1->Map1_bank2 = 1;
  MAP1->Map1_bank3 = MAP1->Map1_HI1;
  MAP1->Map1_bank4 = MAP1->Map1_HI2;

  /* Set ROM Banks */
  Map1_set_ROM_banks();

  /* Set up wiring of the interrupt pin */
  K6502_Set_Int_Wiring( 1, 1 );
}

