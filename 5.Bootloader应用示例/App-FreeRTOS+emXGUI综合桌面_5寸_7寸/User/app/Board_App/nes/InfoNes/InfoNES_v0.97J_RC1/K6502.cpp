/*===================================================================*/
/*                                                                   */
/*  K6502.cpp : 6502 Emulator                                        */
/*                                                                   */
/*  2000/5/10   InfoNES Project ( based on pNesX )                   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "K6502.h"
#include "InfoNES_System.h"
#include "infoNES.h"

/*-------------------------------------------------------------------*/
/*  Global valiables                                                 */
/*-------------------------------------------------------------------*/
//CpuResource CPU;
// // 6502 Register
// WORD PC;
// BYTE SP;
// BYTE F;
// BYTE A;
// BYTE X;
// BYTE Y;
// BYTE IRQ_State;// The state of the IRQ pin
// BYTE IRQ_Wiring;// Wiring of the IRQ pin
// BYTE NMI_State;// The state of the NMI pin
// BYTE NMI_Wiring;// Wiring of the NMI pin
// WORD g_wPassedClocks;// The number of the clocks that it passed
// BYTE g_byTestTable[ 256 ];// A table for the test
// value_table g_ASLTable[ 256 ];// A table for ASL
// value_table g_LSRTable[ 256 ];// A table for LSR
// value_table g_ROLTable[ 2 ][ 256 ];// A table for ROL
// value_table g_RORTable[ 2 ][ 256 ];// A table for ROR


/*-------------------------------------------------------------------*/
/*  Operation Macros                                                 */
/*-------------------------------------------------------------------*/

// Clock Op.
#define CLK(a)   CPU->g_wPassedClocks += (a);

// Addressing Op.
// Address
// (Indirect,X)
#define AA_IX    K6502_ReadZpW( K6502_Read( CPU->PC++ ) + CPU->X )
// (Indirect),Y
#define AA_IY    K6502_ReadZpW( K6502_Read( CPU->PC++ ) ) + CPU->Y
// Zero Page
#define AA_ZP    K6502_Read( CPU->PC++ )
// Zero Page,X
#define AA_ZPX   (BYTE)( K6502_Read( CPU->PC++ ) + CPU->X )
// Zero Page,Y
#define AA_ZPY   (BYTE)( K6502_Read( CPU->PC++ ) + CPU->Y )
// Absolute
#define AA_ABS   ( K6502_Read( CPU->PC++ ) | (WORD)K6502_Read( CPU->PC++ ) << 8 )
// Absolute2 ( PC-- )
#define AA_ABS2  ( K6502_Read( CPU->PC++ ) | (WORD)K6502_Read( CPU->PC ) << 8 )
// Absolute,X
#define AA_ABSX  AA_ABS + CPU->X
// Absolute,Y
#define AA_ABSY  AA_ABS + CPU->Y

// Data
// (Indirect,X)
#define A_IX    K6502_Read( AA_IX )
// (Indirect),Y
#define A_IY    K6502_ReadIY()
// Zero Page
#define A_ZP    K6502_ReadZp( AA_ZP )
// Zero Page,X
#define A_ZPX   K6502_ReadZp( AA_ZPX )
// Zero Page,Y
#define A_ZPY   K6502_ReadZp( AA_ZPY )
// Absolute
#define A_ABS   K6502_Read( AA_ABS )
// Absolute,X
#define A_ABSX  K6502_ReadAbsX()
// Absolute,Y
#define A_ABSY  K6502_ReadAbsY()
// Immediate
#define A_IMM   K6502_Read( CPU->PC++ )

// Flag Op.
#define SETF(a)  CPU->F |= (a)
#define RSTF(a)  CPU->F &= ~(a)
#define TEST(a)  RSTF( FLAG_N | FLAG_Z ); SETF( CPU->g_byTestTable[ a ] )

// Load & Store Op.
#define STA(a)    K6502_Write( (a), CPU->A );
#define STX(a)    K6502_Write( (a), CPU->X );
#define STY(a)    K6502_Write( (a), CPU->Y );
#define LDA(a)    CPU->A = (a); TEST( CPU->A );
#define LDX(a)    CPU->X = (a); TEST( CPU->X );
#define LDY(a)    CPU->Y = (a); TEST( CPU->Y );

// Stack Op.
#define PUSH(a)   K6502_Write( BASE_STACK + CPU->SP--, (a) )
#define PUSHW(a)  PUSH( (a) >> 8 ); PUSH( (a) & 0xff )
#define POP(a)    a = K6502_Read( BASE_STACK + ++CPU->SP )
#define POPW(a)   POP(a); a |= ( K6502_Read( BASE_STACK + ++CPU->SP ) << 8 )

// Logical Op.
#define ORA(a)  CPU->A |= (a); TEST( CPU->A )
#define AND(a)  CPU->A &= (a); TEST( CPU->A )
#define EOR(a)  CPU->A ^= (a); TEST( CPU->A )
#define BIT(a)  byD0 = (a); RSTF( FLAG_N | FLAG_V | FLAG_Z ); SETF( ( byD0 & ( FLAG_N | FLAG_V ) ) | ( ( byD0 & CPU->A ) ? 0 : FLAG_Z ) );
#define CMP(a)  wD0 = (WORD)CPU->A - (a); RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( CPU->g_byTestTable[ wD0 & 0xff ] | ( wD0 < 0x100 ? FLAG_C : 0 ) );
#define CPX(a)  wD0 = (WORD)CPU->X - (a); RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( CPU->g_byTestTable[ wD0 & 0xff ] | ( wD0 < 0x100 ? FLAG_C : 0 ) );
#define CPY(a)  wD0 = (WORD)CPU->Y - (a); RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( CPU->g_byTestTable[ wD0 & 0xff ] | ( wD0 < 0x100 ? FLAG_C : 0 ) );
  
// Math Op. (A D flag isn't being supported.)
#define ADC(a)  byD0 = (a); \
                wD0 = CPU->A + byD0 + ( CPU->F & FLAG_C ); \
                byD1 = (BYTE)wD0; \
                RSTF( FLAG_N | FLAG_V | FLAG_Z | FLAG_C ); \
                SETF( CPU->g_byTestTable[ byD1 ] | ( ( ~( CPU->A ^ byD0 ) & ( CPU->A ^ byD1 ) & 0x80 ) ? FLAG_V : 0 ) | ( wD0 > 0xff ) ); \
                CPU->A = byD1;

#define SBC(a)  byD0 = (a); \
                wD0 = CPU->A - byD0 - ( ~CPU->F & FLAG_C ); \
                byD1 = (BYTE)wD0; \
                RSTF( FLAG_N | FLAG_V | FLAG_Z | FLAG_C ); \
                SETF( CPU->g_byTestTable[ byD1 ] | ( ( ( CPU->A ^ byD0 ) & ( CPU->A ^ byD1 ) & 0x80 ) ? FLAG_V : 0 ) | ( wD0 < 0x100 ) ); \
                CPU->A = byD1;

#define DEC(a)  wA0 = a; byD0 = K6502_Read( wA0 ); --byD0; K6502_Write( wA0, byD0 ); TEST( byD0 )
#define INC(a)  wA0 = a; byD0 = K6502_Read( wA0 ); ++byD0; K6502_Write( wA0, byD0 ); TEST( byD0 )

// Shift Op.
#define ASLA    RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( CPU->g_ASLTable[ CPU->A ].byFlag ); CPU->A = CPU->g_ASLTable[ CPU->A ].byValue 
#define ASL(a)  RSTF( FLAG_N | FLAG_Z | FLAG_C ); wA0 = a; byD0 = K6502_Read( wA0 ); SETF( CPU->g_ASLTable[ byD0 ].byFlag ); K6502_Write( wA0, CPU->g_ASLTable[ byD0 ].byValue )
#define LSRA    RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( CPU->g_LSRTable[ CPU->A ].byFlag ); CPU->A = CPU->g_LSRTable[ CPU->A ].byValue 
#define LSR(a)  RSTF( FLAG_N | FLAG_Z | FLAG_C ); wA0 = a; byD0 = K6502_Read( wA0 ); SETF( CPU->g_LSRTable[ byD0 ].byFlag ); K6502_Write( wA0, CPU->g_LSRTable[ byD0 ].byValue ) 
#define ROLA    byD0 = CPU->F & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( CPU->g_ROLTable[ byD0 ][ CPU->A ].byFlag ); CPU->A = CPU->g_ROLTable[ byD0 ][ CPU->A ].byValue 
#define ROL(a)  byD1 = CPU->F & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); wA0 = a; byD0 = K6502_Read( wA0 ); SETF( CPU->g_ROLTable[ byD1 ][ byD0 ].byFlag ); K6502_Write( wA0, CPU->g_ROLTable[ byD1 ][ byD0 ].byValue )
#define RORA    byD0 = CPU->F & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( CPU->g_RORTable[ byD0 ][ CPU->A ].byFlag ); CPU->A = CPU->g_RORTable[ byD0 ][ CPU->A ].byValue 
#define ROR(a)  byD1 = CPU->F & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); wA0 = a; byD0 = K6502_Read( wA0 ); SETF( CPU->g_RORTable[ byD1 ][ byD0 ].byFlag ); K6502_Write( wA0, CPU->g_RORTable[ byD1 ][ byD0 ].byValue )

// Jump Op.
#define JSR     wA0 = AA_ABS2; PUSHW( CPU->PC ); CPU->PC = wA0; 
#if 0
#define BRA(a)  if ( a ) { wA0 = CPU->PC; CPU->PC += (char)K6502_Read( CPU->PC ); CLK( 3 + ( ( wA0 & 0x0100 ) != ( CPU->PC & 0x0100 ) ) ); ++CPU->PC; } else { ++CPU->PC; CLK( 2 ); }
#else
#define BRA(a) { \
  if ( a ) \
  { \
    wA0 = CPU->PC; \
	byD0 = K6502_Read( CPU->PC ); \
	CPU->PC += ( ( byD0 & 0x80 ) ? ( 0xFF00 | (WORD)byD0 ) : (WORD)byD0 ); \
	CLK( 3 + ( ( wA0 & 0x0100 ) != ( CPU->PC & 0x0100 ) ) ); \
    ++CPU->PC; \
  } else { \
	++CPU->PC; \
	CLK( 2 ); \
  } \
}
#endif
#define JMP(a)  CPU->PC = a;




/*===================================================================*/
/*                                                                   */
/*                K6502_Init() : Initialize K6502                    */
/*                                                                   */
/*===================================================================*/
void K6502_Init()
{
/*
 *  Initialize K6502
 *
 *  You must call this function only once at first.
 */

  BYTE idx;
  BYTE idx2;

  // The establishment of the IRQ pin
  CPU->NMI_Wiring = CPU->NMI_State = 1;
  CPU->IRQ_Wiring = CPU->IRQ_State = 1;

  // Make a table for the test
  idx = 0;
  do
  {
    if ( idx == 0 )
      CPU->g_byTestTable[ 0 ] = FLAG_Z;
    else
    if ( idx > 127 )
      CPU->g_byTestTable[ idx ] = FLAG_N;
    else
      CPU->g_byTestTable[ idx ] = 0;

    ++idx;
  } while ( idx != 0 );

  // Make a table ASL
  idx = 0;
  do
  {
    CPU->g_ASLTable[ idx ].byValue = idx << 1;
    CPU->g_ASLTable[ idx ].byFlag = 0;

    if ( idx > 127 )
      CPU->g_ASLTable[ idx ].byFlag = FLAG_C;

    if ( CPU->g_ASLTable[ idx ].byValue == 0 )
      CPU->g_ASLTable[ idx ].byFlag |= FLAG_Z;
    else
    if ( CPU->g_ASLTable[ idx ].byValue & 0x80 )
      CPU->g_ASLTable[ idx ].byFlag |= FLAG_N;

    ++idx;
  } while ( idx != 0 );

  // Make a table LSR
  idx = 0;
  do
  {
    CPU->g_LSRTable[ idx ].byValue = idx >> 1;
    CPU->g_LSRTable[ idx ].byFlag = 0;

    if ( idx & 1 )
      CPU->g_LSRTable[ idx ].byFlag = FLAG_C;

    if ( CPU->g_LSRTable[ idx ].byValue == 0 )
      CPU->g_LSRTable[ idx ].byFlag |= FLAG_Z;

    ++idx;
  } while ( idx != 0 );

  // Make a table ROL
  for ( idx2 = 0; idx2 < 2; ++idx2 )
  {
    idx = 0;
    do
    {
      CPU->g_ROLTable[ idx2 ][ idx ].byValue = ( idx << 1 ) | idx2;
      CPU->g_ROLTable[ idx2 ][ idx ].byFlag = 0;

      if ( idx > 127 )
        CPU->g_ROLTable[ idx2 ][ idx ].byFlag = FLAG_C;

      if ( CPU->g_ROLTable[ idx2 ][ idx ].byValue == 0 )
        CPU->g_ROLTable[ idx2 ][ idx ].byFlag |= FLAG_Z;
      else
      if ( CPU->g_ROLTable[ idx2 ][ idx ].byValue & 0x80 )
        CPU->g_ROLTable[ idx2 ][ idx ].byFlag |= FLAG_N;

      ++idx;
    } while ( idx != 0 );
  }

  // Make a table ROR
  for ( idx2 = 0; idx2 < 2; ++idx2 )
  {
    idx = 0;
    do
    {
      CPU->g_RORTable[ idx2 ][ idx ].byValue = ( idx >> 1 ) | ( idx2 << 7 );
      CPU->g_RORTable[ idx2 ][ idx ].byFlag = 0;

      if ( idx & 1 )
        CPU->g_RORTable[ idx2 ][ idx ].byFlag = FLAG_C;

      if ( CPU->g_RORTable[ idx2 ][ idx ].byValue == 0 )
        CPU->g_RORTable[ idx2 ][ idx ].byFlag |= FLAG_Z;
      else
      if ( CPU->g_RORTable[ idx2 ][ idx ].byValue & 0x80 )
        CPU->g_RORTable[ idx2 ][ idx ].byFlag |= FLAG_N;

      ++idx;
    } while ( idx != 0 );
  }
}

/*===================================================================*/
/*                                                                   */
/*                K6502_Reset() : Reset a CPU                        */
/*                                                                   */
/*===================================================================*/
void K6502_Reset()
{
/*
 *  Reset a CPU
 *
 */

  // Reset Registers
  CPU->PC = K6502_ReadW( VECTOR_RESET );
  CPU->SP = 0xFF;
  CPU->A = CPU->X = CPU->Y = 0;
  CPU->F = FLAG_Z | FLAG_R | FLAG_I;

  // Set up the state of the Interrupt pin.
  CPU->NMI_State = CPU->NMI_Wiring;
  CPU->IRQ_State = CPU->IRQ_Wiring;

  // Reset Passed Clocks
  CPU->g_wPassedClocks = 0;
}

/*===================================================================*/
/*                                                                   */
/*    K6502_Set_Int_Wiring() : Set up wiring of the interrupt pin    */
/*                                                                   */
/*===================================================================*/
void K6502_Set_Int_Wiring( BYTE byNMI_Wiring, BYTE byIRQ_Wiring )
{
/*
 * Set up wiring of the interrupt pin
 *
 */

  CPU->NMI_Wiring = byNMI_Wiring;
  CPU->IRQ_Wiring = byIRQ_Wiring;
}

/*===================================================================*/
/*                                                                   */
/*  K6502_Step() :                                                   */
/*          Only the specified number of the clocks execute Op.      */
/*                                                                   */
/*===================================================================*/
void K6502_Step( WORD wClocks )
{
/*
 *  Only the specified number of the clocks execute Op.
 *
 *  Parameters
 *    WORD wClocks              (Read)
 *      The number of the clocks
 */

  BYTE byCode;

  WORD wA0;
  BYTE byD0;
  BYTE byD1;
  WORD wD0;

  // Dispose of it if there is an interrupt requirement
  if ( CPU->NMI_State != CPU->NMI_Wiring )
  {
    // NMI Interrupt
    CPU->NMI_State = CPU->NMI_Wiring;
    CLK( 7 );

    PUSHW( CPU->PC );
    PUSH( CPU->F & ~FLAG_B );

    RSTF( FLAG_D );
    SETF( FLAG_I );

    CPU->PC = K6502_ReadW( VECTOR_NMI );
  }
  else
  if ( CPU->IRQ_State != CPU->IRQ_Wiring )
  {
    // IRQ Interrupt
    // Execute IRQ if an I flag isn't being set
    if ( !( CPU->F & FLAG_I ) )
    {
      CPU->IRQ_State = CPU->IRQ_Wiring;
      CLK( 7 );

      PUSHW( CPU->PC );
      PUSH( CPU->F & ~FLAG_B );

      RSTF( FLAG_D );
      SETF( FLAG_I );
    
      CPU->PC = K6502_ReadW( VECTOR_IRQ );
    }
  }

  // It has a loop until a constant clock passes
  while ( CPU->g_wPassedClocks < wClocks )
  {
    // Read an instruction
    byCode = K6502_Read( CPU->PC++ );

    // Execute an instruction.
    switch ( byCode )
    {
      case 0x00:  // BRK
        ++CPU->PC; PUSHW( CPU->PC ); SETF( FLAG_B ); PUSH( CPU->F ); SETF( FLAG_I ); RSTF( FLAG_D ); CPU->PC = K6502_ReadW( VECTOR_IRQ ); CLK( 7 );
        break;

      case 0x01:  // ORA (Zpg,X)
        ORA( A_IX ); CLK( 6 );
        break;

      case 0x05:  // ORA Zpg
        ORA( A_ZP ); CLK( 3 );
        break;

      case 0x06:  // ASL Zpg
        ASL( AA_ZP ); CLK( 5 );
        break;

      case 0x08:  // PHP
        SETF( FLAG_B ); PUSH( CPU->F ); CLK( 3 );
        break;

      case 0x09:  // ORA #Oper
        ORA( A_IMM ); CLK( 2 );
        break;

      case 0x0A:  // ASL A
        ASLA; CLK( 2 );
        break;

      case 0x0D:  // ORA Abs
        ORA( A_ABS ); CLK( 4 );
        break;

      case 0x0e:  // ASL Abs 
        ASL( AA_ABS ); CLK( 6 );
        break;

      case 0x10: // BPL Oper
        BRA( !( CPU->F & FLAG_N ) );
        break;

      case 0x11: // ORA (Zpg),Y
        ORA( A_IY ); CLK( 5 );
        break;

      case 0x15: // ORA Zpg,X
        ORA( A_ZPX ); CLK( 4 );
        break;

      case 0x16: // ASL Zpg,X
        ASL( AA_ZPX ); CLK( 6 );
        break;

      case 0x18: // CLC
        RSTF( FLAG_C ); CLK( 2 );
        break;

      case 0x19: // ORA Abs,Y
        ORA( A_ABSY ); CLK( 4 );
        break;

      case 0x1D: // ORA Abs,X
        ORA( A_ABSX ); CLK( 4 );
        break;

      case 0x1E: // ASL Abs,X
        ASL( AA_ABSX ); CLK( 7 );
        break;

      case 0x20: // JSR Abs
        JSR; CLK( 6 );
        break;

      case 0x21: // AND (Zpg,X)
        AND( A_IX ); CLK( 6 );
        break;

      case 0x24: // BIT Zpg
        BIT( A_ZP ); CLK( 3 );
        break;

      case 0x25: // AND Zpg
        AND( A_ZP ); CLK( 3 );
        break;

      case 0x26: // ROL Zpg
        ROL( AA_ZP ); CLK( 5 );
        break;

      case 0x28: // PLP
        POP( CPU->F ); SETF( FLAG_R ); CLK( 4 );
        break;

      case 0x29: // AND #Oper
        AND( A_IMM ); CLK( 2 );
        break;

      case 0x2A: // ROL A
        ROLA; CLK( 2 );
        break;

      case 0x2C: // BIT Abs
        BIT( A_ABS ); CLK( 4 );
        break;

      case 0x2D: // AND Abs 
        AND( A_ABS ); CLK( 4 );
        break;

      case 0x2E: // ROL Abs
        ROL( AA_ABS ); CLK( 6 );
        break;

      case 0x30: // BMI Oper 
        BRA( CPU->F & FLAG_N );
        break;

      case 0x31: // AND (Zpg),Y
        AND( A_IY ); CLK( 5 );
        break;

      case 0x35: // AND Zpg,X
        AND( A_ZPX ); CLK( 4 );
        break;

      case 0x36: // ROL Zpg,X
        ROL( AA_ZPX ); CLK( 6 );
        break;

      case 0x38: // SEC
        SETF( FLAG_C ); CLK( 2 );
        break;

      case 0x39: // AND Abs,Y
        AND( A_ABSY ); CLK( 4 );
        break;

      case 0x3D: // AND Abs,X
        AND( A_ABSX ); CLK( 4 );
        break;

      case 0x3E: // ROL Abs,X
        ROL( AA_ABSX ); CLK( 7 );
        break;

      case 0x40: // RTI
        POP( CPU->F ); SETF( FLAG_R ); POPW( CPU->PC ); CLK( 6 );
        break;

      case 0x41: // EOR (Zpg,X)
        EOR( A_IX ); CLK( 6 );
        break;

      case 0x45: // EOR Zpg
        EOR( A_ZP ); CLK( 3 );
        break;

      case 0x46: // LSR Zpg
        LSR( AA_ZP ); CLK( 5 );
        break;

      case 0x48: // PHA
        PUSH( CPU->A ); CLK( 3 );
        break;

      case 0x49: // EOR #Oper
        EOR( A_IMM ); CLK( 2 );
        break;

      case 0x4A: // LSR A
        LSRA; CLK( 2 );
        break;

      case 0x4C: // JMP Abs
        JMP( AA_ABS ); CLK( 3 );
        break;

      case 0x4D: // EOR Abs
        EOR( A_ABS ); CLK( 4 );
        break;

      case 0x4E: // LSR Abs
        LSR( AA_ABS ); CLK( 6 );
        break;

      case 0x50: // BVC
        BRA( !( CPU->F & FLAG_V ) );
        break;

      case 0x51: // EOR (Zpg),Y
        EOR( A_IY ); CLK( 5 );
        break;

      case 0x55: // EOR Zpg,X
        EOR( A_ZPX ); CLK( 4 );
        break;

      case 0x56: // LSR Zpg,X
        LSR( AA_ZPX ); CLK( 6 );
        break;

      case 0x58: // CLI
        byD0 = CPU->F;
        RSTF( FLAG_I ); CLK( 2 );
        if ( ( byD0 & FLAG_I ) && CPU->IRQ_State != CPU->IRQ_Wiring )  
        {
          CPU->IRQ_State = CPU->IRQ_Wiring;          
          CLK( 7 );

          PUSHW( CPU->PC );
          PUSH( CPU->F & ~FLAG_B );

          RSTF( FLAG_D );
          SETF( FLAG_I );
    
          CPU->PC = K6502_ReadW( VECTOR_IRQ );
        }
        break;

      case 0x59: // EOR Abs,Y
        EOR( A_ABSY ); CLK( 4 );
        break;

      case 0x5D: // EOR Abs,X
        EOR( A_ABSX ); CLK( 4 );
        break;

      case 0x5E: // LSR Abs,X
        LSR( AA_ABSX ); CLK( 7 );
        break;

      case 0x60: // RTS
        POPW( CPU->PC ); ++CPU->PC; CLK( 6 );
        break;

      case 0x61: // ADC (Zpg,X)
        ADC( A_IX ); CLK( 6 );
        break;

      case 0x65: // ADC Zpg
        ADC( A_ZP ); CLK( 3 );
        break;

      case 0x66: // ROR Zpg
        ROR( AA_ZP ); CLK( 5 );
        break;

      case 0x68: // PLA
        POP( CPU->A ); TEST( CPU->A ); CLK( 4 );
        break;

      case 0x69: // ADC #Oper
        ADC( A_IMM ); CLK( 2 );
        break;

      case 0x6A: // ROR A
        RORA; CLK( 2 );
        break;

      case 0x6C: // JMP (Abs)
        JMP( K6502_ReadW2( AA_ABS ) ); CLK( 5 );
        break;

      case 0x6D: // ADC Abs
        ADC( A_ABS ); CLK( 4 );
        break;

      case 0x6E: // ROR Abs
        ROR( AA_ABS ); CLK( 6 );
        break;

      case 0x70: // BVS
        BRA( CPU->F & FLAG_V );
        break;

      case 0x71: // ADC (Zpg),Y
        ADC( A_IY ); CLK( 5 );
        break;

      case 0x75: // ADC Zpg,X
        ADC( A_ZPX ); CLK( 4 );
        break;

      case 0x76: // ROR Zpg,X
        ROR( AA_ZPX ); CLK( 6 );
        break;

      case 0x78: // SEI
        SETF( FLAG_I ); CLK( 2 );
        break;

      case 0x79: // ADC Abs,Y
        ADC( A_ABSY ); CLK( 4 );
        break;

      case 0x7D: // ADC Abs,X
        ADC( A_ABSX ); CLK( 4 );
        break;

      case 0x7E: // ROR Abs,X
        ROR( AA_ABSX ); CLK( 7 );
        break;

      case 0x81: // STA (Zpg,X)
        STA( AA_IX ); CLK( 6 );
        break;
      
      case 0x84: // STY Zpg
        STY( AA_ZP ); CLK( 3 );
        break;

      case 0x85: // STA Zpg
        STA( AA_ZP ); CLK( 3 );
        break;

      case 0x86: // STX Zpg
        STX( AA_ZP ); CLK( 3 );
        break;

      case 0x88: // DEY
        --CPU->Y; TEST( CPU->Y ); CLK( 2 );
        break;

      case 0x8A: // TXA
        CPU->A = CPU->X; TEST( CPU->A ); CLK( 2 );
        break;

      case 0x8C: // STY Abs
        STY( AA_ABS ); CLK( 4 );
        break;

      case 0x8D: // STA Abs
        STA( AA_ABS ); CLK( 4 );
        break;

      case 0x8E: // STX Abs
        STX( AA_ABS ); CLK( 4 );
        break;

      case 0x90: // BCC
        BRA( !( CPU->F & FLAG_C ) );
        break;

      case 0x91: // STA (Zpg),Y
        STA( AA_IY ); CLK( 6 );
        break;

      case 0x94: // STY Zpg,X
        STY( AA_ZPX ); CLK( 4 );
        break;

      case 0x95: // STA Zpg,X
        STA( AA_ZPX ); CLK( 4 );
        break;

      case 0x96: // STX Zpg,Y
        STX( AA_ZPY ); CLK( 4 );
        break;

      case 0x98: // TYA
        CPU->A = CPU->Y; TEST( CPU->A ); CLK( 2 );
        break;

      case 0x99: // STA Abs,Y
        STA( AA_ABSY ); CLK( 5 );
        break;

      case 0x9A: // TXS
        CPU->SP = CPU->X; CLK( 2 );
        break;

      case 0x9D: // STA Abs,X
        STA( AA_ABSX ); CLK( 5 );
        break;

      case 0xA0: // LDY #Oper
        LDY( A_IMM ); CLK( 2 );
        break;

      case 0xA1: // LDA (Zpg,X)
        LDA( A_IX ); CLK( 6 );
        break;

      case 0xA2: // LDX #Oper
        LDX( A_IMM ); CLK( 2 );
        break;

      case 0xA4: // LDY Zpg
        LDY( A_ZP ); CLK( 3 );
        break;

      case 0xA5: // LDA Zpg
        LDA( A_ZP ); CLK( 3 );
        break;

      case 0xA6: // LDX Zpg
        LDX( A_ZP ); CLK( 3 );
        break;

      case 0xA8: // TAY
        CPU->Y = CPU->A; TEST( CPU->A ); CLK( 2 );
        break;

      case 0xA9: // LDA #Oper
        LDA( A_IMM ); CLK( 2 );
        break;

      case 0xAA: // TAX
        CPU->X = CPU->A; TEST( CPU->A ); CLK( 2 );
        break;

      case 0xAC: // LDY Abs
        LDY( A_ABS ); CLK( 4 );
        break;

      case 0xAD: // LDA Abs
        LDA( A_ABS ); CLK( 4 );
        break;

      case 0xAE: // LDX Abs
        LDX( A_ABS ); CLK( 4 );
        break;

      case 0xB0: // BCS
        BRA( CPU->F & FLAG_C );
        break;

      case 0xB1: // LDA (Zpg),Y
        LDA( A_IY ); CLK( 5 );
        break;

      case 0xB4: // LDY Zpg,X
        LDY( A_ZPX ); CLK( 4 );
        break;

      case 0xB5: // LDA Zpg,X
        LDA( A_ZPX ); CLK( 4 );
        break;

      case 0xB6: // LDX Zpg,Y
        LDX( A_ZPY ); CLK( 4 );
        break;

      case 0xB8: // CLV
        RSTF( FLAG_V ); CLK( 2 );
        break;

      case 0xB9: // LDA Abs,Y
        LDA( A_ABSY ); CLK( 4 );
        break;

      case 0xBA: // TSX
        CPU->X = CPU->SP; TEST( CPU->X ); CLK( 2 );
        break;

      case 0xBC: // LDY Abs,X
        LDY( A_ABSX ); CLK( 4 );
        break;

      case 0xBD: // LDA Abs,X
        LDA( A_ABSX ); CLK( 4 );
        break;

      case 0xBE: // LDX Abs,Y
        LDX( A_ABSY ); CLK( 4 );
        break;

      case 0xC0: // CPY #Oper
        CPY( A_IMM ); CLK( 2 );
        break;

      case 0xC1: // CMP (Zpg,X)
        CMP( A_IX ); CLK( 6 );
        break;

      case 0xC4: // CPY Zpg
        CPY( A_ZP ); CLK( 3 );
        break;

      case 0xC5: // CMP Zpg
        CMP( A_ZP ); CLK( 3 );
        break;

      case 0xC6: // DEC Zpg
        DEC( AA_ZP ); CLK( 5 );
        break;

      case 0xC8: // INY
        ++CPU->Y; TEST( CPU->Y ); CLK( 2 );
        break;

      case 0xC9: // CMP #Oper
        CMP( A_IMM ); CLK( 2 );
        break;

      case 0xCA: // DEX
        --CPU->X; TEST( CPU->X ); CLK( 2 );
        break;

      case 0xCC: // CPY Abs
        CPY( A_ABS ); CLK( 4 );
        break;

      case 0xCD: // CMP Abs
        CMP( A_ABS ); CLK( 4 );
        break;

      case 0xCE: // DEC Abs
        DEC( AA_ABS ); CLK( 6 );
        break;

      case 0xD0: // BNE
        BRA( !( CPU->F & FLAG_Z ) );
        break;

      case 0xD1: // CMP (Zpg),Y
        CMP( A_IY ); CLK( 5 );
        break;

      case 0xD5: // CMP Zpg,X
        CMP( A_ZPX ); CLK( 4 );
        break;

      case 0xD6: // DEC Zpg,X
        DEC( AA_ZPX ); CLK( 6 );
        break;

      case 0xD8: // CLD
        RSTF( FLAG_D ); CLK( 2 );
        break;

      case 0xD9: // CMP Abs,Y
        CMP( A_ABSY ); CLK( 4 );
        break;

      case 0xDD: // CMP Abs,X
        CMP( A_ABSX ); CLK( 4 );
        break;

      case 0xDE: // DEC Abs,X
        DEC( AA_ABSX ); CLK( 7 );
        break;

      case 0xE0: // CPX #Oper
        CPX( A_IMM ); CLK( 2 );
        break;

      case 0xE1: // SBC (Zpg,X)
        SBC( A_IX ); CLK( 6 );
        break;

      case 0xE4: // CPX Zpg
        CPX( A_ZP ); CLK( 3 );
        break;

      case 0xE5: // SBC Zpg
        SBC( A_ZP ); CLK( 3 );
        break;

      case 0xE6: // INC Zpg
        INC( AA_ZP ); CLK( 5 );
        break;

      case 0xE8: // INX
        ++CPU->X; TEST( CPU->X ); CLK( 2 );
        break;

      case 0xE9: // SBC #Oper
        SBC( A_IMM ); CLK( 2 );
        break;

      case 0xEA: // NOP
        CLK( 2 );
        break;

      case 0xEC: // CPX Abs
        CPX( A_ABS ); CLK( 4 );
        break;

      case 0xED: // SBC Abs
        SBC( A_ABS ); CLK( 4 );
        break;

      case 0xEE: // INC Abs
        INC( AA_ABS ); CLK( 6 );
        break;

      case 0xF0: // BEQ
        BRA( CPU->F & FLAG_Z );
        break;

      case 0xF1: // SBC (Zpg),Y
        SBC( A_IY ); CLK( 5 );
        break;

      case 0xF5: // SBC Zpg,X
        SBC( A_ZPX ); CLK( 4 );
        break;

      case 0xF6: // INC Zpg,X
        INC( AA_ZPX ); CLK( 6 );
        break;

      case 0xF8: // SED
        SETF( FLAG_D ); CLK( 2 );
        break;

      case 0xF9: // SBC Abs,Y
        SBC( A_ABSY ); CLK( 4 );
        break;

      case 0xFD: // SBC Abs,X
        SBC( A_ABSX ); CLK( 4 );
        break;

      case 0xFE: // INC Abs,X
        INC( AA_ABSX ); CLK( 7 );
        break;

      /*-----------------------------------------------------------*/
      /*  Unlisted Instructions ( thanks to virtualnes )           */
      /*-----------------------------------------------------------*/

			case	0x1A: // NOP (Unofficial)
			case	0x3A: // NOP (Unofficial)
			case	0x5A: // NOP (Unofficial)
			case	0x7A: // NOP (Unofficial)
			case	0xDA: // NOP (Unofficial)
			case	0xFA: // NOP (Unofficial)
				CLK( 2 );
				break;

			case	0x80: // DOP (CYCLES 2)
			case	0x82: // DOP (CYCLES 2)
			case	0x89: // DOP (CYCLES 2)
			case	0xC2: // DOP (CYCLES 2)
			case	0xE2: // DOP (CYCLES 2)
				CPU->PC++;
				CLK( 2 );
				break;

			case	0x04: // DOP (CYCLES 3)
			case	0x44: // DOP (CYCLES 3)
			case	0x64: // DOP (CYCLES 3)
				CPU->PC++;
				CLK( 3 );
				break;

			case	0x14: // DOP (CYCLES 4)
			case	0x34: // DOP (CYCLES 4)
			case	0x54: // DOP (CYCLES 4)
			case	0x74: // DOP (CYCLES 4)
			case	0xD4: // DOP (CYCLES 4)
			case	0xF4: // DOP (CYCLES 4)
        CPU->PC++; 
        CLK( 4 );
        break;

			case	0x0C: // TOP
			case	0x1C: // TOP
			case	0x3C: // TOP
			case	0x5C: // TOP
			case	0x7C: // TOP
			case	0xDC: // TOP
			case	0xFC: // TOP
				CPU->PC+=2;
				CLK( 4 );
				break;

      default:   // Unknown Instruction
        CLK( 2 );
#if 0
        InfoNES_MessageBox( "0x%02x is unknown instruction.\n", byCode ) ;
#endif
        break;
        
    }  /* end of switch ( byCode ) */

  }  /* end of while ... */

  // Correct the number of the clocks
  CPU->g_wPassedClocks -= wClocks;
}

// Addressing Op.
// Data
// Absolute,X
static __inline BYTE K6502_ReadAbsX(){ WORD wA0, wA1; wA0 = AA_ABS; wA1 = wA0 + CPU->X; CLK( ( wA0 & 0x0100 ) != ( wA1 & 0x0100 ) ); return K6502_Read( wA1 ); };
// Absolute,Y
static __inline BYTE K6502_ReadAbsY(){ WORD wA0, wA1; wA0 = AA_ABS; wA1 = wA0 + CPU->Y; CLK( ( wA0 & 0x0100 ) != ( wA1 & 0x0100 ) ); return K6502_Read( wA1 ); };
// (Indirect),Y
static __inline BYTE K6502_ReadIY(){ WORD wA0, wA1; wA0 = K6502_ReadZpW( K6502_Read( CPU->PC++ ) ); wA1 = wA0 + CPU->Y; CLK( ( wA0 & 0x0100 ) != ( wA1 & 0x0100 ) ); return K6502_Read( wA1 ); };

/*===================================================================*/
/*                                                                   */
/*                  6502 Reading/Writing Operation                   */
/*                                                                   */
/*===================================================================*/
#include "K6502_rw.h"
