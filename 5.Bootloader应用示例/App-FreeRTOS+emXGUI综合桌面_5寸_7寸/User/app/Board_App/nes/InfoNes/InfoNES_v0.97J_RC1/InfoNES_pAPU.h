/*===================================================================*/
/*                                                                   */
/*  InfoNES_pAPU.h : InfoNES Sound Emulation Function                */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on DarcNES and NesterJ )     */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_PAPU_H_INCLUDED
#define InfoNES_PAPU_H_INCLUDED

#ifdef	__cplusplus
extern	"C"{
#endif

#include "InfoNES_Types.h"  
#define Limit_Speed 1  
/*-------------------------------------------------------------------*/
/*  Macros                                                           */
/*-------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/ 
/* Rectangle Wave #0                                                 */
/* Reg0: 0-3=Volume, 4=Envelope, 5=Hold, 6-7=Duty Cycle              */
/* Reg1: 0-2=sweep shifts, 3=sweep inc, 4-6=sweep length, 7=sweep on */
/* Reg2: 8 bits of freq                                              */
/* Reg3: 0-2=high freq, 7-4=vbl length counter                       */
/*-------------------------------------------------------------------*/ 
#define ApuC1Vol            ( APU->ApuC1a & 0x0f )
#define ApuC1Env            ( APU->ApuC1a & 0x10 )
#define ApuC1Hold           ( APU->ApuC1a & 0x20 )
#define ApuC1DutyCycle      ( APU->ApuC1a & 0xc0 )
#define ApuC1EnvDelay       ( ( APU->ApuC1a & 0x0f ) + 1 )
#define ApuC1SweepOn        ( APU->ApuC1b & 0x80 )
#define ApuC1SweepIncDec    ( APU->ApuC1b & 0x08 )
#define ApuC1SweepShifts    ( APU->ApuC1b & 0x07 ) 
#define ApuC1SweepDelay     ( ( ( ApuC1b & 0x70 ) >> 4 ) + 1 )
#define ApuC1FreqLimit      ( ApuFreqLimit[ ( APU->ApuC1b & 0x07 ) ] )

/*-------------------------------------------------------------------*/ 
/* Rectangle Wave #1                                                 */
/* Reg0: 0-3=Volume, 4=Envelope, 5=Hold, 6-7=Duty Cycle              */
/* Reg1: 0-2=sweep shifts, 3=sweep inc, 4-6=sweep length, 7=sweep on */
/* Reg2: 8 bits of freq                                              */
/* Reg3: 0-2=high freq, 7-4=vbl length counter                       */
/*-------------------------------------------------------------------*/ 
#define ApuC2Vol            ( APU->ApuC2a & 0x0f )
#define ApuC2Env            ( APU->ApuC2a & 0x10 )
#define ApuC2Hold           ( APU->ApuC2a & 0x20 )
#define ApuC2DutyCycle      ( APU->ApuC2a & 0xc0 )
#define ApuC2EnvDelay       ( ( APU->ApuC2a & 0x0f ) + 1 )
#define ApuC2SweepOn        ( APU->ApuC2b & 0x80 )
#define ApuC2SweepIncDec    ( APU->ApuC2b & 0x08 )
#define ApuC2SweepShifts    ( APU->ApuC2b & 0x07 ) 
#define ApuC2SweepDelay     ( ( ( APU->ApuC2b & 0x70 ) >> 4 ) + 1 )
#define ApuC2FreqLimit      ( ApuFreqLimit[ ( APU->ApuC2b & 0x07 ) ] )

/*-------------------------------------------------------------------*/ 
/* Triangle Wave                                                     */
/* Reg0: 7=Holdnote, 6-0=Linear Length Counter                       */
/* Reg2: 8 bits of freq                                              */
/* Reg3: 0-2=high freq, 7-4=vbl length counter                       */
/*-------------------------------------------------------------------*/ 
#define ApuC3Holdnote       ( APU->ApuC3a & 0x80 )
#define ApuC3LinearLength   ( ( (WORD)APU->ApuC3a & 0x7f ) << 6 )
#define ApuC3LengthCounter  ( ApuAtl[ ( ( APU->ApuC3d & 0xf8) >> 3 ) ] )
#define ApuC3Freq           ( ( ( (WORD)APU->ApuC3d & 0x07) << 8) + APU->ApuC3c )

/*-------------------------------------------------------------------*/ 
/* White Noise Channel                                               */
/* Reg0: 0-3=Volume, 4=Envelope, 5=Hold                              */
/* Reg2: 7=Small(93byte) sample, 3-0=Freq Lookup                     */
/* Reg3: 7-3=vbl length counter                                      */
/*-------------------------------------------------------------------*/ 
#define ApuC4Vol            ( ApuC4a & 0x0f )
#define ApuC4EnvDelay       ( ( ApuC4a & 0x0f ) + 1 )
#define ApuC4Env            ( ApuC4a & 0x10 )
#define ApuC4Hold           ( ApuC4a & 0x20 )
#define ApuC4Freq           ( ApuNoiseFreq [ ( ApuC4c & 0x0f ) ] )
#define ApuC4Small          ( ApuC4c & 0x80 )
#define ApuC4LengthCounter  ( ApuAtl[ ( ApuC4d >> 3 ) ] << 1 )

/*-------------------------------------------------------------------*/ 
/* DPCM Channel                                                      */
/* Reg0: 0-3=Frequency, 6=Looping                                    */
/* Reg1: 0-6=DPCM Value                                              */
/* Reg2: 0-7=Cache Addr                                              */
/* Reg3: 0-7=Cache DMA Length                                        */
/*-------------------------------------------------------------------*/ 
#if 0
#define ApuC5Freq           ( ApuDpcmCycles[ ( ApuC5a & 0x0F ) ] )
#define ApuC5Looping        ( ApuC5a & 0x40 )
#define ApuC5DpcmValue      ( ( ApuC5b & 0x7F ) >> 1 )
#define ApuC5CacheAddr      ( 0xc000 + (WORD)(ApuC5c << 6) )
#define ApuC5CacheDmaLength ( ( ( ApuC5d << 4 ) + 1 ) << 3 )
#endif

/*-------------------------------------------------------------------*/
/*  pAPU Event resources                                             */
/*-------------------------------------------------------------------*/

#define APU_EVENT_MAX   50

typedef struct{
  long time;
  BYTE type;
  BYTE data;
}ApuEvent;

typedef struct  
{
  DWORD pulse_magic;
  DWORD triangle_magic;
  DWORD noise_magic;
  unsigned int samples_per_sync;
  unsigned int cycles_per_sample;
  unsigned int sample_rate;
  DWORD cycle_rate;
}ApuQualityData; 

typedef struct
{
    int  cur_event;
    WORD entertime;
    BYTE ApuCtrl;
    BYTE ApuCtrlNew;
    int ApuQuality;
    DWORD ApuPulseMagic;
    DWORD ApuTriangleMagic;
    DWORD ApuNoiseMagic;
    unsigned int ApuSamplesPerSync;
    unsigned int ApuCyclesPerSample;
    unsigned int ApuSampleRate;
    DWORD ApuCycleRate;  
    //Rectangle Wave #1 resources
    BYTE ApuC1a, ApuC1b, ApuC1c, ApuC1d;
    BYTE* ApuC1Wave;
    DWORD ApuC1Skip;
    DWORD ApuC1Index;
    DWORD ApuC1EnvPhase;
    BYTE  ApuC1EnvVol;
    BYTE  ApuC1Atl;
    DWORD ApuC1SweepPhase;
    DWORD ApuC1Freq; 
    //Rectangle Wave #2 resources         
    BYTE ApuC2a, ApuC2b, ApuC2c, ApuC2d;
    BYTE* ApuC2Wave;
    DWORD ApuC2Skip;
    DWORD ApuC2Index;
    DWORD ApuC2EnvPhase;
    BYTE  ApuC2EnvVol;
    BYTE  ApuC2Atl;   
    DWORD ApuC2SweepPhase;
    DWORD ApuC2Freq; 
    //Triangle Wave resources                           
    BYTE ApuC3a, ApuC3b, ApuC3c, ApuC3d;
    DWORD ApuC3Skip;
    DWORD ApuC3Index;
    BYTE  ApuC3Atl;
    DWORD ApuC3Llc;                         
    BYTE  ApuC3WriteLatency;
    BYTE  ApuC3CounterStarted; 
    //DPCM resources
    BYTE  ApuC5Reg[4];
    BYTE  ApuC5Enable;
    BYTE  ApuC5Looping;
    BYTE  ApuC5CurByte;
    BYTE  ApuC5DpcmValue;
    int   ApuC5Freq;
    int   ApuC5Phaseacc;
    WORD  ApuC5Address, ApuC5CacheAddr;
    int   ApuC5DmaLength, ApuC5CacheDmaLength;

    unsigned char Soundcount;
}ApuResource;


#define APUET_MASK      0xfc
#define APUET_C1        0x00
#define APUET_W_C1A     0x00
#define APUET_W_C1B     0x01
#define APUET_W_C1C     0x02
#define APUET_W_C1D     0x03
#define APUET_C2        0x04
#define APUET_W_C2A     0x04
#define APUET_W_C2B     0x05
#define APUET_W_C2C     0x06
#define APUET_W_C2D     0x07
#define APUET_C3        0x08
#define APUET_W_C3A     0x08
#define APUET_W_C3B     0x09
#define APUET_W_C3C     0x0a
#define APUET_W_C3D     0x0b
#define APUET_C4        0x0c
#define APUET_W_C4A     0x0c
#define APUET_W_C4B     0x0d
#define APUET_W_C4C     0x0e
#define APUET_W_C4D     0x0f
#define APUET_C5        0x10
#define APUET_W_C5A     0x10
#define APUET_W_C5B     0x11
#define APUET_W_C5C     0x12
#define APUET_W_C5D     0x13
#define APUET_W_CTRL    0x20
#define APUET_SYNC      0x40

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/
typedef void (*ApuWritefunc)(WORD addr, BYTE value);
extern ApuEvent *ApuEventQueue;
extern WORD wave_buffers[1470];
extern ApuResource *APU;
extern const ApuWritefunc pAPUSoundRegs[20];
void ApuWriteControl(WORD addr, BYTE value);

#define InfoNES_pAPUWriteControl(addr,value) \
{ \
  ApuWriteControl(addr,value); \
}

void InfoNES_pAPUInit(void);
void InfoNES_pAPUDone(void);
void InfoNES_pAPUVsync(void);

/*-------------------------------------------------------------------*/
/*  pAPU Quality resources                                           */
/* ApuQuality is used to control the sound playback rate.            */
/* 1 is 11015 Hz.                                                    */
/* 2 is 22050 Hz.                                                    */
/* 3 is 44100 Hz.                                                    */
/* these values subject to change without notice.                    */
/*-------------------------------------------------------------------*/
#define pAPU_QUALITY 2

#ifdef	__cplusplus
}
#endif

#endif /* InfoNES_PAPU_H_INCLUDED */

/*
 * End of InfoNES_pAPU.h
 */
