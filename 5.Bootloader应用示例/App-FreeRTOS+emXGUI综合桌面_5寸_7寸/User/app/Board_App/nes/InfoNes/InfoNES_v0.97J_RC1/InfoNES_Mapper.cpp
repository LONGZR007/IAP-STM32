#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_Mapper.h"
#include "K6502.h"


/*-------------------------------------------------------------------*/
/*  body of Mapper functions                                         */
/*-------------------------------------------------------------------*/

#include "mapper/InfoNES_Mapper_000.cpp"//
#include "mapper/InfoNES_Mapper_001.cpp"
#include "mapper/InfoNES_Mapper_002.cpp"//
#include "mapper/InfoNES_Mapper_003.cpp"//
#include "mapper/InfoNES_Mapper_004.cpp"
#include "mapper/InfoNES_Mapper_006.cpp"
#include "mapper/InfoNES_Mapper_007.cpp"
#include "mapper/InfoNES_Mapper_008.cpp"
#include "mapper/InfoNES_Mapper_011.cpp"
#include "mapper/InfoNES_Mapper_015.cpp"
#include "mapper/InfoNES_Mapper_021.cpp"
#include "mapper/InfoNES_Mapper_023.cpp"
#include "mapper/InfoNES_Mapper_032.cpp"
#include "mapper/InfoNES_Mapper_033.cpp"
#include "mapper/InfoNES_Mapper_034.cpp"
#include "mapper/InfoNES_Mapper_058.cpp"
#include "mapper/InfoNES_Mapper_064.cpp"
#include "mapper/InfoNES_Mapper_066.cpp"
#include "mapper/InfoNES_Mapper_069.cpp"
#include "mapper/InfoNES_Mapper_071.cpp"
#include "mapper/InfoNES_Mapper_079.cpp"
#include "mapper/InfoNES_Mapper_091.cpp"
#include "mapper/InfoNES_Mapper_113.cpp"
#include "mapper/InfoNES_Mapper_118.cpp"
#include "mapper/InfoNES_Mapper_200.cpp"
#include "mapper/InfoNES_Mapper_201.cpp"
#include "mapper/InfoNES_Mapper_225.cpp"
#include "mapper/InfoNES_Mapper_229.cpp"
#include "mapper/InfoNES_Mapper_231.cpp"


/*-------------------------------------------------------------------*/
/*  Table of Mapper initialize function                              */
/*-------------------------------------------------------------------*/
const MapperTable MapTab[] =
{
  {   0, Map0_Init   },
  {   1, Map1_Init   },
  {   2, Map2_Init   },
  {   3, Map3_Init   },
  {   4, Map4_Init   },
  {   6, Map6_Init   },
  {   7, Map7_Init   },
  {   8, Map8_Init   },
  {  11, Map11_Init  },
  {  15, Map15_Init  },
  {  21, Map21_Init  },
  {  23, Map23_Init  },
  {  32, Map32_Init  },
  {  33, Map33_Init  },
  {  34, Map34_Init  },
  {  58, Map58_Init  },
  {  64, Map64_Init  },
  {  66, Map66_Init  },
  {  69, Map69_Init  },
  {  71, Map71_Init  },
  {  79, Map79_Init  }, 
  {  91, Map91_Init  },
  { 113, Map113_Init },
  { 118, Map118_Init },
  { 200, Map200_Init },
  { 201, Map201_Init },
  { 225, Map225_Init },
  { 229, Map229_Init },
  { 231, Map231_Init },
  { -1, NULL }
};

/* End of InfoNES_Mapper.cpp */
