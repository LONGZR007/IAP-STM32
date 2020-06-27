/*===================================================================*/
/*                                                                   */
/*  InfoNES_pAPU.cpp : InfoNES Sound Emulation Function              */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on DarcNES and NesterJ )     */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/
#include "K6502.h"
#include "K6502_rw.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"

/*-------------------------------------------------------------------*/
/*   APU resources                                                   */
/*-------------------------------------------------------------------*/
//ApuResource APU;

// int  cur_event;
// WORD entertime;
// BYTE ApuCtrl;
// BYTE ApuCtrlNew;
// int ApuQuality;
// DWORD ApuPulseMagic;
// DWORD ApuTriangleMagic;
// DWORD ApuNoiseMagic;
// unsigned int ApuSamplesPerSync;
// unsigned int ApuCyclesPerSample;
// unsigned int ApuSampleRate;
// DWORD ApuCycleRate;

/*-------------------------------------------------------------------*/
/*  Rectangle Wave #1 resources                                      */
/*-------------------------------------------------------------------*/
// BYTE ApuC1a, ApuC1b, ApuC1c, ApuC1d;
// BYTE* ApuC1Wave;
// DWORD ApuC1Skip;
// DWORD ApuC1Index;
// DWORD ApuC1EnvPhase;
// BYTE  ApuC1EnvVol;
// BYTE  ApuC1Atl;
// DWORD ApuC1SweepPhase;
// DWORD ApuC1Freq;   
/*-------------------------------------------------------------------*/
/*  Rectangle Wave #2 resources                                      */
/*-------------------------------------------------------------------*/
// BYTE ApuC2a, ApuC2b, ApuC2c, ApuC2d;
// BYTE* ApuC2Wave;
// DWORD ApuC2Skip;
// DWORD ApuC2Index;
// DWORD ApuC2EnvPhase;
// BYTE  ApuC2EnvVol;
// BYTE  ApuC2Atl;   
// DWORD ApuC2SweepPhase;
// DWORD ApuC2Freq;   
/*-------------------------------------------------------------------*/
/*  Triangle Wave resources                                          */
/*-------------------------------------------------------------------*/
// BYTE ApuC3a, ApuC3b, ApuC3c, ApuC3d;
// DWORD ApuC3Skip;
// DWORD ApuC3Index;
// BYTE  ApuC3Atl;
// DWORD ApuC3Llc;                             /* Linear Length Counter */
// BYTE  ApuC3WriteLatency;
// BYTE  ApuC3CounterStarted;

/*-------------------------------------------------------------------*/
/*  Noise resources                                                  */
/*-------------------------------------------------------------------*/
// BYTE ApuC4a, ApuC4b, ApuC4c, ApuC4d;
// DWORD ApuC4Sr;                                     /* Shift register */
// DWORD ApuC4Fdc;                          /* Frequency divide counter */
// DWORD ApuC4Skip;
// DWORD ApuC4Index;
// BYTE  ApuC4Atl;
// BYTE  ApuC4EnvVol;
// DWORD ApuC4EnvPhase;

/*-------------------------------------------------------------------*/
/*  DPCM resources                                                   */
/*-------------------------------------------------------------------*/
// BYTE  ApuC5Reg[4];
// BYTE  ApuC5Enable;
// BYTE  ApuC5Looping;
// BYTE  ApuC5CurByte;
// BYTE  ApuC5DpcmValue;
// int   ApuC5Freq;
// int   ApuC5Phaseacc;
// WORD  ApuC5Address, ApuC5CacheAddr;
// int   ApuC5DmaLength, ApuC5CacheDmaLength;


/* NES模拟器包含4个声道，分别是：2个方波，一个三角波和一个噪声发生通道，
 * 方波   ： wave1
 * 方波   ： wave2
 * 三角波 ： wave3
 * 噪声   ： wave5
*/

/*-------------------------------------------------------------------*/
/*  Wave Data                                                        */
/*-------------------------------------------------------------------*/
const ApuQualityData ApuQual[] = {
  { 0xa2567000, 0xa2567000, 0xa2567000, 183, 164, 11025, 1062658 },
  { 0x512b3800, 0x512b3800, 0x512b3800, 367,  82, 22050, 531329 },
  { 0x289d9c00, 0x289d9c00, 0x289d9c00, 735,  41, 44100, 265664 },
};

const BYTE pulse_25[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

const BYTE pulse_50[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

const BYTE pulse_75[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00,
};

const BYTE pulse_87[0x20] = {
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x11, 0x11, 0x11, 0x11,
  0x00, 0x00, 0x00, 0x00,
};

const BYTE triangle_50[0x20] = {
  0x00, 0x10, 0x20, 0x30,
  0x40, 0x50, 0x60, 0x70,
  0x80, 0x90, 0xa0, 0xb0,
  0xc0, 0xd0, 0xe0, 0xf0,
  0xff, 0xef, 0xdf, 0xcf,
  0xbf, 0xaf, 0x9f, 0x8f,
  0x7f, 0x6f, 0x5f, 0x4f,
  0x3f, 0x2f, 0x1f, 0x0f,
};

const BYTE *pulse_waves[4] = {
  pulse_87, pulse_75, pulse_50, pulse_25,
};

/*-------------------------------------------------------------------*/
/*  Active Time Left Data                                            */
/*-------------------------------------------------------------------*/
const BYTE ApuAtl[0x20] = 
{
  5, 127, 10, 1, 19,  2, 40,  3, 80,  4, 30,  5, 7,  6, 13,  7,
  6,   8, 12, 9, 24, 10, 48, 11, 96, 12, 36, 13, 8, 14, 16, 15,
};

/*-------------------------------------------------------------------*/
/* Frequency Limit of Rectangle Channels                             */
/*-------------------------------------------------------------------*/
const WORD ApuFreqLimit[8] = 
{
   0x3FF, 0x555, 0x666, 0x71C, 0x787, 0x7C1, 0x7E0, 0x7F0
};

/*-------------------------------------------------------------------*/
/* DMC Transfer Clocks Table                                          */
/*-------------------------------------------------------------------*/
const DWORD ApuDpcmCycles[ 16 ] = 
{
  428, 380, 340, 320, 286, 254, 226, 214,
  190, 160, 142, 128, 106,  85,  72,  54
};

/*-------------------------------------------------------------------*/
/*   APU Register Write Functions                                    */
/*-------------------------------------------------------------------*/

#define APU_WRITEFUNC(name, evtype) \
void ApuWrite##name(WORD addr, BYTE value) \
{ \
  ApuEventQueue[APU->cur_event].time = APU->entertime - CPU->g_wPassedClocks; \
  ApuEventQueue[APU->cur_event].type = APUET_W_##evtype; \
  ApuEventQueue[APU->cur_event].data = value; \
  APU->cur_event++; \
}

APU_WRITEFUNC(C1a, C1A);
APU_WRITEFUNC(C1b, C1B);
APU_WRITEFUNC(C1c, C1C);
APU_WRITEFUNC(C1d, C1D);

APU_WRITEFUNC(C2a, C2A);
APU_WRITEFUNC(C2b, C2B);
APU_WRITEFUNC(C2c, C2C);
APU_WRITEFUNC(C2d, C2D);

APU_WRITEFUNC(C3a, C3A);
APU_WRITEFUNC(C3b, C3B);
APU_WRITEFUNC(C3c, C3C);
APU_WRITEFUNC(C3d, C3D);

APU_WRITEFUNC(C4a, C4A);
APU_WRITEFUNC(C4b, C4B);
APU_WRITEFUNC(C4c, C4C);
APU_WRITEFUNC(C4d, C4D);

APU_WRITEFUNC(C5a, C5A);
APU_WRITEFUNC(C5b, C5B);
APU_WRITEFUNC(C5c, C5C);
APU_WRITEFUNC(C5d, C5D);

APU_WRITEFUNC(Control, CTRL);

const ApuWritefunc pAPUSoundRegs[20] = 
{
  ApuWriteC1a,
  ApuWriteC1b,
  ApuWriteC1c,
  ApuWriteC1d,
  ApuWriteC2a,
  ApuWriteC2b,
  ApuWriteC2c,
  ApuWriteC2d,
  ApuWriteC3a,
  ApuWriteC3b,
  ApuWriteC3c,
  ApuWriteC3d,
  ApuWriteC4a,
  ApuWriteC4b,
  ApuWriteC4c,
  ApuWriteC4d,
  ApuWriteC5a,
  ApuWriteC5b,
  ApuWriteC5c,
  ApuWriteC5d,
};
/*===================================================================*/
/*                                                                   */
/*      ApuRenderingWave1() : Rendering Rectangular Wave #1          */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/* Write registers of rectangular wave #1                            */
/*-------------------------------------------------------------------*/

int ApuWriteWave1( int cycles, int event )
{
    /* APU Reg Write Event */
    while ( ( event < APU->cur_event ) && ( ApuEventQueue[event].time < cycles ) ) 
    {
      if ( ( ApuEventQueue[event].type & APUET_MASK ) == APUET_C1 ) 
      {
	switch ( ApuEventQueue[event].type & 0x03 ) 
        {
	case 0:
	  APU->ApuC1a    = ApuEventQueue[event].data;
	  APU->ApuC1Wave = (BYTE*)pulse_waves[ ApuC1DutyCycle >> 6 ];
	  break;

	case 1:
	  APU->ApuC1b    = ApuEventQueue[event].data; 
	  break;
	  
	case 2:
	  APU->ApuC1c = ApuEventQueue[event].data;
	  APU->ApuC1Freq = ( ( ( (WORD)APU->ApuC1d & 0x07 ) << 8 ) + APU->ApuC1c );
	  APU->ApuC1Atl = ApuAtl[ ( APU->ApuC1d & 0xf8 ) >> 3 ];
	  
	  if ( APU->ApuC1Freq ) 
          {
	    APU->ApuC1Skip = ( APU->ApuPulseMagic << 1 ) / APU->ApuC1Freq;
	  } else {
	    APU->ApuC1Skip = 0;
	  }
	  break;

	case 3:
	  APU->ApuC1d = ApuEventQueue[event].data;
	  APU->ApuC1Freq = ( ( ( (WORD)APU->ApuC1d & 0x07 ) << 8 ) + APU->ApuC1c );
	  APU->ApuC1Atl = ApuAtl[ ( APU->ApuC1d & 0xf8 ) >> 3 ];
	  
	  if ( APU->ApuC1Freq ) 
          {
	    APU->ApuC1Skip = ( APU->ApuPulseMagic << 1 ) / APU->ApuC1Freq;
	  } else {
	    APU->ApuC1Skip = 0;
	  }
	  break;
	}
      } 
      else if ( ApuEventQueue[event].type == APUET_W_CTRL ) 
      {
	APU->ApuCtrlNew = ApuEventQueue[event].data;

	if( !(ApuEventQueue[event].data&(1<<0)) ) {
	  APU->ApuC1Atl = 0;
	}
      }
      event++;
    }
    return event;
}

/*-------------------------------------------------------------------*/
/* Rendering rectangular wave #1                                     */
/*-------------------------------------------------------------------*/

void ApuRenderingWave1( void )
{
  int cycles = 0;
  int event = 0;
  unsigned int i;
  /* note: 41 CPU cycles occur between increments of i */
  APU->ApuCtrlNew = APU->ApuCtrl;
  for ( i = 0; i < APU->ApuSamplesPerSync; i++ ) 
  {
    /* Write registers */
    cycles += APU->ApuCyclesPerSample;
    event = ApuWriteWave1( cycles, event );

    /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
    APU->ApuC1EnvPhase -= 4;

    /*
     * TODO: using a table of max frequencies is not technically
     * clean, but it is fast and (or should be) accurate
     */
    if ( APU->ApuC1Freq < 8 || ( !ApuC1SweepIncDec && APU->ApuC1Freq > ApuC1FreqLimit ) )
    {
      wave_buffers[i] = 0;
	  continue;
    }
    /* Frequency sweeping at a rate of ( Sweep Delay + 1) / 120 secs */
    if ( ApuC1SweepOn && ApuC1SweepShifts )     
    {
      APU->ApuC1SweepPhase -= 2;           /* 120/60 */
      if ( APU->ApuC1Freq ) {
	APU->ApuC1Skip = ( APU->ApuPulseMagic << 1 ) / APU->ApuC1Freq;
      }
    }

    /* Wave Rendering */
    if ( ( APU->ApuCtrlNew & 0x01 ) && ( APU->ApuC1Atl || ApuC1Hold ) ) 
    {
      APU->ApuC1Index += APU->ApuC1Skip;
      APU->ApuC1Index &= 0x1fffffff;
      
      if ( ApuC1Env )
      {
        wave_buffers[i] = APU->ApuC1Wave[APU->ApuC1Index >> 24] * ( ApuC1Vol + APU->ApuC1EnvVol );
      } else {
        wave_buffers[i] = APU->ApuC1Wave[APU->ApuC1Index >> 24] * ApuC1Vol;
      }
    } else {
      wave_buffers[i] = 0;
    }
  }
  if ( APU->ApuC1Atl ) { APU->ApuC1Atl--;  }
}

/*===================================================================*/
/*                                                                   */
/*      ApuRenderingWave2() : Rendering Rectangular Wave #2          */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/* Write registers of rectangular wave #2                           */
/*-------------------------------------------------------------------*/

int ApuWriteWave2( int cycles, int event )
{
    /* APU Reg Write Event */
    while ( ( event < APU->cur_event ) && ( ApuEventQueue[event].time < cycles ) ) 
    {
      if ( ( ApuEventQueue[event].type & APUET_MASK ) == APUET_C2 ) 
      {
	switch ( ApuEventQueue[event].type & 0x03 ) 
        {
	case 0:
	  APU->ApuC2a    = ApuEventQueue[event].data;
	  APU->ApuC2Wave = (BYTE*)pulse_waves[ ApuC2DutyCycle >> 6 ];
	  break;

	case 1:
	  APU->ApuC2b    = ApuEventQueue[event].data; 
	  break;
	  
	case 2:
	  APU->ApuC2c = ApuEventQueue[event].data;
	  APU->ApuC2Freq = ( ( ( (WORD)APU->ApuC2d & 0x07 ) << 8 ) + APU->ApuC2c );
	  APU->ApuC2Atl = ApuAtl[ ( APU->ApuC2d & 0xf8 ) >> 3 ];
	  
	  if ( APU->ApuC2Freq ) 
          {
	    APU->ApuC2Skip = ( APU->ApuPulseMagic << 1 ) / APU->ApuC2Freq;
	  } else {
	    APU->ApuC2Skip = 0;
	  }
	  break;

	case 3:
	  APU->ApuC2d = ApuEventQueue[event].data;
	  APU->ApuC2Freq = ( ( ( (WORD)APU->ApuC2d & 0x07 ) << 8 ) + APU->ApuC2c );
	  APU->ApuC2Atl = ApuAtl[ ( APU->ApuC2d & 0xf8 ) >> 3 ];
	  
	  if ( APU->ApuC2Freq ) 
          {
	    APU->ApuC2Skip = ( APU->ApuPulseMagic << 1 ) / APU->ApuC2Freq;
	  } else {
	    APU->ApuC2Skip = 0;
	  }
	  break;
	}
      } 
      else if ( ApuEventQueue[event].type == APUET_W_CTRL ) 
      {
	APU->ApuCtrlNew = ApuEventQueue[event].data;

	if( !(ApuEventQueue[event].data&(1<<1)) ) {
	  APU->ApuC2Atl = 0;
	}
      }
      event++;
    }
    return event;
}

/*-------------------------------------------------------------------*/
/* Rendering rectangular wave #2                                     */
/*-------------------------------------------------------------------*/

void ApuRenderingWave2( void )
{
  int cycles = 0;
  int event = 0;
  unsigned int i;
  /* note: 41 CPU cycles occur between increments of i */
  APU->ApuCtrlNew = APU->ApuCtrl;
  for ( i = 0; i < APU->ApuSamplesPerSync; i++ ) 
  {
    /* Write registers */
    cycles += APU->ApuCyclesPerSample;
    event = ApuWriteWave2( cycles, event );

    /* Envelope decay at a rate of ( Envelope Delay + 1 ) / 240 secs */
    APU->ApuC2EnvPhase -= 4;

    /*
     * TODO: using a table of max frequencies is not technically
     * clean, but it is fast and (or should be) accurate
     */
    if ( APU->ApuC2Freq < 8 || ( !ApuC2SweepIncDec && APU->ApuC2Freq > ApuC2FreqLimit ) )
    {
      break;
    }

    /* Frequency sweeping at a rate of ( Sweep Delay + 1) / 120 secs */
    if ( ApuC2SweepOn && ApuC2SweepShifts )     
    {
      APU->ApuC2SweepPhase -= 2;           /* 120/60 */
      if ( APU->ApuC2Freq ) {
	APU->ApuC2Skip = ( APU->ApuPulseMagic << 1 ) / APU->ApuC2Freq;
      }
    }

    /* Wave Rendering */
    if ( ( APU->ApuCtrlNew & 0x02 ) && ( APU->ApuC2Atl || ApuC2Hold ) ) 
    {
      APU->ApuC2Index += APU->ApuC2Skip;
      APU->ApuC2Index &= 0x1fffffff;
      
      if ( ApuC2Env )
      {
        wave_buffers[i] += APU->ApuC2Wave[APU->ApuC2Index >> 24] * ( ApuC2Vol + APU->ApuC2EnvVol );
      } else {
        wave_buffers[i] += APU->ApuC2Wave[APU->ApuC2Index >> 24] * ApuC2Vol;
      }
    } 
  }
  if ( APU->ApuC2Atl ) { APU->ApuC2Atl--;  }
}

/*===================================================================*/
/*                                                                   */
/*      ApuRenderingWave3() : Rendering Triangle Wave                */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/* Write registers of triangle wave #3                              */
/*-------------------------------------------------------------------*/

int ApuWriteWave3( int cycles, int event )
{
  /* APU Reg Write Event */
  while (( event < APU->cur_event ) && ( ApuEventQueue[event].time < cycles ) ) 
  {
    if ( ( ApuEventQueue[event].type & APUET_MASK ) == APUET_C3 ) 
    {
      switch ( ApuEventQueue[event].type & 3 ) 
      {
      case 0:
	APU->ApuC3a = ApuEventQueue[event].data;
	APU->ApuC3Llc = ApuC3LinearLength;
	break;

      case 1:
	APU->ApuC3b = ApuEventQueue[event].data;
	break;

      case 2:
	APU->ApuC3c = ApuEventQueue[event].data;
	if ( ApuC3Freq ) 
        {
	  APU->ApuC3Skip = APU->ApuTriangleMagic / ApuC3Freq;
	} else {
	  APU->ApuC3Skip = 0;  
	}
	break;

      case 3:
	APU->ApuC3d = ApuEventQueue[event].data;
	APU->ApuC3Atl = ApuC3LengthCounter;
	if ( ApuC3Freq ) 
	{
	  APU->ApuC3Skip = APU->ApuTriangleMagic / ApuC3Freq;
	} else {
	  APU->ApuC3Skip = 0;
	}
      }
    } else if ( ApuEventQueue[event].type == APUET_W_CTRL ) {
      APU->ApuCtrlNew = ApuEventQueue[event].data;

      if( !(ApuEventQueue[event].data&(1<<2)) ) {
	APU->ApuC3Atl = 0;
	APU->ApuC3Llc = 0;
      }
    }
    event++;
  }
  return event;
}

/*-------------------------------------------------------------------*/
/* Rendering triangle wave #3                                        */
/*-------------------------------------------------------------------*/

void ApuRenderingWave3( void )
{
  int cycles = 0;
  int event = 0;
  unsigned int i;    
  /* note: 41 CPU cycles occur between increments of i */
  APU->ApuCtrlNew = APU->ApuCtrl;
  for ( i = 0; i < APU->ApuSamplesPerSync; i++) 
  {
    /* Write registers */
    cycles += APU->ApuCyclesPerSample;
    event = ApuWriteWave3( cycles, event );

    /* Cutting Min Frequency */
    if ( ApuC3Freq < 8 )break;

    /* Counter Control */
    if ( APU->ApuC3CounterStarted )
    {
      if ( APU->ApuC3Atl > 0 && !ApuC3Holdnote ) 
      {
	APU->ApuC3Atl--;
      }
      if ( APU->ApuC3Llc > 0 )
      {
	APU->ApuC3Llc--;
      }
    } else if ( !ApuC3Holdnote && APU->ApuC3WriteLatency > 0 ) {
      if ( --APU->ApuC3WriteLatency == 0 )
      {
	APU->ApuC3CounterStarted = 0x01;
      }
    }

    /* Wave Rendering */
    if ( ( APU->ApuCtrlNew & 0x04 ) && ( ( APU->ApuC3Atl > 0 || ApuC3Holdnote ) && APU->ApuC3Llc > 0 ) ) 
    {
      APU->ApuC3Index += APU->ApuC3Skip;
      APU->ApuC3Index &= 0x1fffffff;
      wave_buffers[i] += triangle_50[ APU->ApuC3Index >> 24 ];
    } 
  }
}
/*===================================================================*/
/*                                                                   */
/*      ApuRenderingWave5() : Rendering DPCM channel #5              */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/* Write registers of DPCM channel #5                               */
/*-------------------------------------------------------------------*/

int ApuWriteWave5( int cycles, int event )
{
  /* APU Reg Write Event */
  while ( (event < APU->cur_event) && (ApuEventQueue[event].time < cycles) ) 
  {
    if ( ( ApuEventQueue[event].type & APUET_MASK ) == APUET_C5 ) 
    {
      APU->ApuC5Reg[ ApuEventQueue[event].type & 3 ] = ApuEventQueue[event].data;

      switch (ApuEventQueue[event].type & 3) {
      case 0:
	APU->ApuC5Freq    = ApuDpcmCycles[ ( ApuEventQueue[event].data & 0x0F ) ] << 16;
	APU->ApuC5Looping = ApuEventQueue[event].data & 0x40;
	break;
      case 1:
	APU->ApuC5DpcmValue = ( ApuEventQueue[event].data & 0x7F ) >> 1;
	break;
      case 2:
	APU->ApuC5CacheAddr = 0xC000 + (WORD)( ApuEventQueue[event].data << 6 );
	break;
      case 3:
	APU->ApuC5CacheDmaLength = ( ( ApuEventQueue[event].data << 4 ) + 1 ) << 3;
	break;
      }
    } else if (ApuEventQueue[event].type == APUET_W_CTRL) {
      APU->ApuCtrlNew = ApuEventQueue[event].data;

      if( !(ApuEventQueue[event].data&(1<<4)) ) {
	APU->ApuC5Enable    = 0;
	APU->ApuC5DmaLength = 0;
      } else {
	APU->ApuC5Enable = 0xFF;
	if( !APU->ApuC5DmaLength ) {
	  APU->ApuC5Address   = APU->ApuC5CacheAddr;
	  APU->ApuC5DmaLength = APU->ApuC5CacheDmaLength;
	}
      }
    }
    event++;
  }
  return event;
}

/*-------------------------------------------------------------------*/
/* Rendering DPCM channel #5                                         */
/*-------------------------------------------------------------------*/

void ApuRenderingWave5(void)
{
  int cycles = 0;
  int event = 0;
  unsigned int i;
  APU->ApuCtrlNew = APU->ApuCtrl;
  for ( i = 0; i < APU->ApuSamplesPerSync; i++ ) 
  {
    /* Write registers */
    cycles += APU->ApuCyclesPerSample;
    event = ApuWriteWave5( cycles, event );

    if( APU->ApuC5DmaLength ) {
      APU->ApuC5Phaseacc -= APU->ApuCycleRate;

      while( APU->ApuC5Phaseacc < 0 ) {
	APU->ApuC5Phaseacc += APU->ApuC5Freq;
	if( !( APU->ApuC5DmaLength & 7 ) ) {
	  APU->ApuC5CurByte = K6502_Read( APU->ApuC5Address );
	  if( 0xFFFF == APU->ApuC5Address )
	    APU->ApuC5Address = 0x8000;
	  else
	    APU->ApuC5Address++;
	}
	if( !(--APU->ApuC5DmaLength) ) {
	  if( APU->ApuC5Looping ) {
	    APU->ApuC5Address = APU->ApuC5CacheAddr;
	    APU->ApuC5DmaLength = APU->ApuC5CacheDmaLength;
	  } else {
	    APU->ApuC5Enable = 0;
	    break;
	  }
	}

	// positive delta
	if( APU->ApuC5CurByte & ( 1 << ((APU->ApuC5DmaLength&7)^7)) ) {
	  if( APU->ApuC5DpcmValue < 0x3F )
	    APU->ApuC5DpcmValue += 1;
	} else {
	  // negative delta
	  if( APU->ApuC5DpcmValue > 1 )
	    APU->ApuC5DpcmValue -= 1;
	}
      }
    }

    /* Wave Rendering */
    if ( APU->ApuCtrlNew & 0x10 ) {
      wave_buffers[i] += ( APU->ApuC5Reg[1]&0x01 ) + ( APU->ApuC5DpcmValue << 1 );
    }
  }
}


/*===================================================================*/
/*                                                                   */
/*     InfoNES_pApuVsync() : Callback Function per Vsync             */
/*                                                                   */
/*===================================================================*/
//#include "fsl_debug_console.h"
void InfoNES_pAPUVsync(void)
{
  ApuRenderingWave1();
  ApuRenderingWave2();
  ApuRenderingWave3();
  ApuRenderingWave5();
    
  APU->ApuCtrl = APU->ApuCtrlNew;
    
  InfoNES_SoundOutput(APU->ApuSamplesPerSync, 
		      wave_buffers);
//  for(int i = 0; i < 1470; i++ )
//  {
//    PRINTF("%d", wave_buffers[i]);
//  }
  APU->entertime = CPU->g_wPassedClocks;
  APU->cur_event = 0;
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_pApuInit() : Initialize pApu                   */
/*                                                                   */
/*===================================================================*/

void InfoNES_pAPUInit(void)
{
  /* Sound Hardware Init */
  InfoNES_SoundInit();

  APU->ApuQuality = pAPU_QUALITY - 1;            // 1: 22050, 2: 44100 [samples/sec]

  APU->ApuPulseMagic      = ApuQual[ APU->ApuQuality ].pulse_magic;
  APU->ApuTriangleMagic   = ApuQual[ APU->ApuQuality ].triangle_magic;
  APU->ApuNoiseMagic      = ApuQual[ APU->ApuQuality ].noise_magic;
  APU->ApuSamplesPerSync  = ApuQual[ APU->ApuQuality ].samples_per_sync;
  APU->ApuCyclesPerSample = ApuQual[ APU->ApuQuality ].cycles_per_sample;
  APU->ApuSampleRate      = ApuQual[ APU->ApuQuality ].sample_rate;
  APU->ApuCycleRate       = ApuQual[ APU->ApuQuality ].cycle_rate;
	
  InfoNES_SoundOpen( APU->ApuSamplesPerSync, APU->ApuSampleRate );

  /*-------------------------------------------------------------------*/
  /* Initialize Rectangular, Noise Wave's Regs                         */
  /*-------------------------------------------------------------------*/
  APU->ApuCtrl = APU->ApuCtrlNew = 0;
  APU->ApuC1Wave = (BYTE*)pulse_50;
  APU->ApuC2Wave = (BYTE*)pulse_50;

  APU->ApuC1a = APU->ApuC1b = APU->ApuC1c = APU->ApuC1d = 0;
  APU->ApuC2a = APU->ApuC2b = APU->ApuC2c = APU->ApuC2d = 0;
  //ApuC4a = ApuC4b = ApuC4c = ApuC4d = 0;

  APU->ApuC1Skip = APU->ApuC2Skip = 0;
  APU->ApuC1Index = APU->ApuC2Index = 0;
  APU->ApuC1EnvPhase = APU->ApuC2EnvPhase = 0;
  APU->ApuC1EnvVol = APU->ApuC2EnvVol = 0;
  APU->ApuC1Atl = APU->ApuC2Atl = 0;
  APU->ApuC1SweepPhase = APU->ApuC2SweepPhase = 0;
  APU->ApuC1Freq = APU->ApuC2Freq = 0;

  /*-------------------------------------------------------------------*/
  /*   Initialize Triangle Wave's Regs                                 */
  /*-------------------------------------------------------------------*/
  APU->ApuC3a = APU->ApuC3b = APU->ApuC3c = APU->ApuC3d = 0;
  APU->ApuC3Atl = APU->ApuC3Llc = 0;
  APU->ApuC3WriteLatency = 3;                           /* Magic Number */
  APU->ApuC3CounterStarted = 0x00;

  /*-------------------------------------------------------------------*/
  /*   Initialize DPCM's Regs                                          */
  /*-------------------------------------------------------------------*/
  APU->ApuC5Reg[0] = APU->ApuC5Reg[1] = APU->ApuC5Reg[2] = APU->ApuC5Reg[3] = 0;
  APU->ApuC5Enable = APU->ApuC5Looping = APU->ApuC5CurByte = APU->ApuC5DpcmValue = 0;
  APU->ApuC5Freq = APU->ApuC5Phaseacc;
  APU->ApuC5Address = APU->ApuC5CacheAddr = 0;
  APU->ApuC5DmaLength = APU->ApuC5CacheDmaLength = 0;

  /*-------------------------------------------------------------------*/
  /*   Initialize Wave Buffers                                         */
  /*-------------------------------------------------------------------*/
  InfoNES_MemorySet( (void *)wave_buffers, 0, 1470 );   

  APU->entertime = CPU->g_wPassedClocks;
  APU->cur_event = 0;
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_pApuDone() : Finalize pApu                     */
/*                                                                   */
/*===================================================================*/

void InfoNES_pAPUDone(void)
{
  InfoNES_SoundClose();
}

/*
 * End of InfoNES_pAPU.cpp
 */
