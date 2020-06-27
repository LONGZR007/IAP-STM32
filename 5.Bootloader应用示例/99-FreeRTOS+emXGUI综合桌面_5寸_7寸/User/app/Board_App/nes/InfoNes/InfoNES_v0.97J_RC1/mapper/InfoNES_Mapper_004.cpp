/*===================================================================*/
/*                                                                   */
/*                       Mapper 4 (MMC3)                             */
/*                                                                   */
/*===================================================================*/
#define Map4_Chr_Swap()    ( MAP4->Map4_Regs[ 0 ] & 0x80 )
#define Map4_Prg_Swap()    ( MAP4->Map4_Regs[ 0 ] & 0x40 )



//Mapper4Res MAP4;

// BYTE  Map4_Regs[ 8 ];
// DWORD Map4_Rom_Bank;
// DWORD Map4_Prg0, Map4_Prg1;
// DWORD Map4_Chr01, Map4_Chr23;
// DWORD Map4_Chr4, Map4_Chr5, Map4_Chr6, Map4_Chr7;
// BYTE Map4_IRQ_Enable;
// BYTE Map4_IRQ_Cnt;
// BYTE Map4_IRQ_Latch;
// BYTE Map4_IRQ_Request;
// BYTE Map4_IRQ_Present;
// BYTE Map4_IRQ_Present_Vbl;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 4                                              */
/*-------------------------------------------------------------------*/
void Map4_Init()
{
  int nPage;

  /* Initialize Mapper */
  MapperInit = Map4_Init;

  /* Write to Mapper */
  MapperWrite = Map4_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map0_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map4_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  SRAMBANK = SRAM;

  /* Initialize State Registers */
  for ( nPage = 0; nPage < 8; nPage++ )
  {
    MAP4->Map4_Regs[ nPage ] = 0x00;
  }

  /* Set ROM Banks */
  MAP4->Map4_Prg0 = 0;
  MAP4->Map4_Prg1 = 1;
  Map4_Set_CPU_Banks();

  /* Set PPU Banks */
  if ( Neshd->byVRomSize > 0 )
  {
    MAP4->Map4_Chr01 = 0;
    MAP4->Map4_Chr23 = 2;
    MAP4->Map4_Chr4  = 4;
    MAP4->Map4_Chr5  = 5;
    MAP4->Map4_Chr6  = 6;
    MAP4->Map4_Chr7  = 7;
    Map4_Set_PPU_Banks();
  } else {
    MAP4->Map4_Chr01 = MAP4->Map4_Chr23 = 0;
    MAP4->Map4_Chr4 = MAP4->Map4_Chr5 = MAP4->Map4_Chr6 = MAP4->Map4_Chr7 = 0;
  }

  /* Initialize IRQ Registers */
  MAP4->Map4_IRQ_Enable = 0;
  MAP4->Map4_IRQ_Cnt = 0;
  MAP4->Map4_IRQ_Latch = 0;
  MAP4->Map4_IRQ_Request = 0;
  MAP4->Map4_IRQ_Present = 0;
  MAP4->Map4_IRQ_Present_Vbl = 0;

  /* Set up wiring of the interrupt pin */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 Write Function                                          */
/*-------------------------------------------------------------------*/
void Map4_Write( WORD wAddr, BYTE byData )
{
  DWORD dwBankNum;

  switch ( wAddr & 0xe001 )
  {
    case 0x8000:
      MAP4->Map4_Regs[ 0 ] = byData;
      Map4_Set_PPU_Banks();
      Map4_Set_CPU_Banks();
      break;

    case 0x8001:
      MAP4->Map4_Regs[ 1 ] = byData;
      dwBankNum = MAP4->Map4_Regs[ 1 ];

      switch ( MAP4->Map4_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks */
        case 0x00:
          if ( Neshd->byVRomSize > 0 )
          {
            dwBankNum &= 0xfe;
            MAP4->Map4_Chr01 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x01:
          if ( Neshd->byVRomSize > 0 )
          {
            dwBankNum &= 0xfe;
            MAP4->Map4_Chr23 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x02:
          if ( Neshd->byVRomSize > 0 )
          {
            MAP4->Map4_Chr4 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x03:
          if ( Neshd->byVRomSize > 0 )
          {
            MAP4->Map4_Chr5 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x04:
          if ( Neshd->byVRomSize > 0 )
          {
            MAP4->Map4_Chr6 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x05:
          if ( Neshd->byVRomSize > 0 )
          {
            MAP4->Map4_Chr7 = dwBankNum;
            Map4_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks */
        case 0x06:
          MAP4->Map4_Prg0 = dwBankNum;
          Map4_Set_CPU_Banks();
          break;

        case 0x07:
          MAP4->Map4_Prg1 = dwBankNum;
          Map4_Set_CPU_Banks();
          break;
      }
      break;

    case 0xa000:
      MAP4->Map4_Regs[ 2 ] = byData;

      if ( !NES->ROM_FourScr )
      {
        if ( byData & 0x01 )
        {
          InfoNES_Mirroring( 0 );
        } else {
          InfoNES_Mirroring( 1 );
        }
      }
      break;

    case 0xa001:
      MAP4->Map4_Regs[ 3 ] = byData;
      break;

    case 0xc000:
      MAP4->Map4_Regs[ 4 ] = byData;
      MAP4->Map4_IRQ_Latch = byData;
      break;

    case 0xc001:
      MAP4->Map4_Regs[ 5 ] = byData;
      if ( NES->PPU_Scanline < 240 )
      {
          MAP4->Map4_IRQ_Cnt |= 0x80;
          MAP4->Map4_IRQ_Present = 0xff;
      } else {
          MAP4->Map4_IRQ_Cnt |= 0x80;
          MAP4->Map4_IRQ_Present_Vbl = 0xff;
          MAP4->Map4_IRQ_Present = 0;
      }
      break;

    case 0xe000:
      MAP4->Map4_Regs[ 6 ] = byData;
      MAP4->Map4_IRQ_Enable = 0;
			MAP4->Map4_IRQ_Request = 0;
      break;

    case 0xe001:
      MAP4->Map4_Regs[ 7 ] = byData;
      MAP4->Map4_IRQ_Enable = 1;
			MAP4->Map4_IRQ_Request = 0;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 H-Sync Function                                         */
/*-------------------------------------------------------------------*/
void Map4_HSync()
{
/*
 *  Callback at HSync
 *
 */
  if ( ( /* 0 <= PPU_Scanline && */ NES->PPU_Scanline <= 239 ) && 
       ( NES->PPU_R1 & R1_SHOW_SCR || NES->PPU_R1 & R1_SHOW_SP ) )
  {
		if( MAP4->Map4_IRQ_Present_Vbl ) {
			MAP4->Map4_IRQ_Cnt = MAP4->Map4_IRQ_Latch;
			MAP4->Map4_IRQ_Present_Vbl = 0;
		}
		if( MAP4->Map4_IRQ_Present ) {
			MAP4->Map4_IRQ_Cnt = MAP4->Map4_IRQ_Latch;
			MAP4->Map4_IRQ_Present = 0;
		} else if( MAP4->Map4_IRQ_Cnt > 0 ) {
			MAP4->Map4_IRQ_Cnt--;
		}

		if( MAP4->Map4_IRQ_Cnt == 0 ) {
			if( MAP4->Map4_IRQ_Enable ) {
				MAP4->Map4_IRQ_Request = 0xFF;
			}
			MAP4->Map4_IRQ_Present = 0xFF;
		}
	}
	if( MAP4->Map4_IRQ_Request  ) {
		IRQ_REQ;
	}
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 Set CPU Banks Function                                  */
/*-------------------------------------------------------------------*/
//BYTE* Get_ROMPAGE(WORD page,BYTE num);
//BYTE* Get_ROMLASTPAGE(WORD page,BYTE num);
void Map4_Set_CPU_Banks()
{
    //void *temp1,*temp2,*temp3,*temp4;
  if ( Map4_Prg_Swap() )
  {
    ROMBANK0 = ROMLASTPAGE( 1 );
    ROMBANK1 = ROMPAGE( MAP4->Map4_Prg1 % ( Neshd->byRomSize << 1 ) );
    ROMBANK2 = ROMPAGE( MAP4->Map4_Prg0 % ( Neshd->byRomSize << 1 ) );
    ROMBANK3 = ROMLASTPAGE( 0 );      
  } else {
    ROMBANK0 = ROMPAGE( MAP4->Map4_Prg0 % ( Neshd->byRomSize << 1 ) );
    ROMBANK1 = ROMPAGE( MAP4->Map4_Prg1 % ( Neshd->byRomSize << 1 ) );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }

//     temp1=Get_ROMLASTPAGE(1,0);
//     temp2=Get_ROMPAGE(Map4_Prg1%(Neshd->byRomSize<<1),1);
//     temp3=Get_ROMPAGE(Map4_Prg0%(Neshd->byRomSize<<1),2);
//     temp4=Get_ROMLASTPAGE(0,3);   
//     if(Map4_Prg_Swap())
//     {
//         ROMBANK0=temp1;
//         ROMBANK1=temp2;
//         ROMBANK2=temp3;
//         ROMBANK3=temp4;      
//     } 
//     else 
//     {
//         ROMBANK0=temp3;
//         ROMBANK1=temp2;
//         ROMBANK2=temp1;
//         ROMBANK3=temp4;
//     }          
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 Set PPU Banks Function                                  */
/*-------------------------------------------------------------------*/
void Map4_Set_PPU_Banks()
{
  if ( Neshd->byVRomSize > 0 )
  {
    if ( Map4_Chr_Swap() )
    { 
      PPUBANK[ 0 ] = VROMPAGE( MAP4->Map4_Chr4 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( MAP4->Map4_Chr5 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( MAP4->Map4_Chr6 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( MAP4->Map4_Chr7 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( MAP4->Map4_Chr01 + 0 ) % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( MAP4->Map4_Chr01 + 1 ) % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( MAP4->Map4_Chr23 + 0 ) % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( MAP4->Map4_Chr23 + 1 ) % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( ( MAP4->Map4_Chr01 + 0 ) % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( MAP4->Map4_Chr01 + 1 ) % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( MAP4->Map4_Chr23 + 0 ) % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( MAP4->Map4_Chr23 + 1 ) % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( MAP4->Map4_Chr4 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( MAP4->Map4_Chr5 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( MAP4->Map4_Chr6 % ( Neshd->byVRomSize << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( MAP4->Map4_Chr7 % ( Neshd->byVRomSize << 3 ) );
      InfoNES_SetupChr();
    }
  }
  else
  {
    if ( Map4_Chr_Swap() )
    { 
#if 0
      PPUBANK[ 0 ] = VRAMPAGE0( 0 );
      PPUBANK[ 1 ] = VRAMPAGE0( 1 );
      PPUBANK[ 2 ] = VRAMPAGE0( 2 );
      PPUBANK[ 3 ] = VRAMPAGE0( 3 );
      PPUBANK[ 4 ] = VRAMPAGE1( 0 );
      PPUBANK[ 5 ] = VRAMPAGE1( 1 );
      PPUBANK[ 6 ] = VRAMPAGE1( 2 );
      PPUBANK[ 7 ] = VRAMPAGE1( 3 );
#else
      PPUBANK[ 0 ] = CRAMPAGE( 0 );
      PPUBANK[ 1 ] = CRAMPAGE( 1 );
      PPUBANK[ 2 ] = CRAMPAGE( 2 );
      PPUBANK[ 3 ] = CRAMPAGE( 3 );
      PPUBANK[ 4 ] = CRAMPAGE( 4 );
      PPUBANK[ 5 ] = CRAMPAGE( 5 );
      PPUBANK[ 6 ] = CRAMPAGE( 6 );
      PPUBANK[ 7 ] = CRAMPAGE( 7 );
#endif
      InfoNES_SetupChr();
    } else {
#if 0
      PPUBANK[ 0 ] = VRAMPAGE1( 0 );
      PPUBANK[ 1 ] = VRAMPAGE1( 1 );
      PPUBANK[ 2 ] = VRAMPAGE1( 2 );
      PPUBANK[ 3 ] = VRAMPAGE1( 3 );
      PPUBANK[ 4 ] = VRAMPAGE0( 0 );
      PPUBANK[ 5 ] = VRAMPAGE0( 1 );
      PPUBANK[ 6 ] = VRAMPAGE0( 2 );
      PPUBANK[ 7 ] = VRAMPAGE0( 3 );
#else
      PPUBANK[ 0 ] = CRAMPAGE( 0 );
      PPUBANK[ 1 ] = CRAMPAGE( 1 );
      PPUBANK[ 2 ] = CRAMPAGE( 2 );
      PPUBANK[ 3 ] = CRAMPAGE( 3 );
      PPUBANK[ 4 ] = CRAMPAGE( 4 );
      PPUBANK[ 5 ] = CRAMPAGE( 5 );
      PPUBANK[ 6 ] = CRAMPAGE( 6 );
      PPUBANK[ 7 ] = CRAMPAGE( 7 );
#endif
      InfoNES_SetupChr();
    }
  }    
}
