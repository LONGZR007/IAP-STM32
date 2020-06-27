
#ifndef __RES_MGR__
#define __RES_MGR__

#ifdef	__cplusplus
extern "C"{
#endif

#include "stm32f4xx.h"
#include "ff.h"
  
#include "gui_drv_cfg.h"
  
//SD卡及flash的根目录
#define SD_ROOT       "0:"
#define FLASH_ROOT    "1:"

/* 资源在SD卡中的路径 */
#define RESOURCE_DIR         "0:/srcdata"
/* 资源烧录到的FLASH基地址（目录地址） */
#define RESOURCE_BASE_ADDR    GUI_RES_BASE
/* 存储在FLASH中的资源目录大小 */
#define CATALOG_SIZE           GUI_CATALOG_SIZE
  
  
/* 生成的烧录信息文件名 */
#define BURN_INFO_NAME        "burn_info.txt"
/* 完整烧录信息文件名*/  
#define BURN_INFO_NAME_FULL   (RESOURCE_DIR "/" BURN_INFO_NAME)
/* 记录忽略的文件名 */
#define IGNORE_NAME           "ignore.txt"  
/* 完整忽略文件名*/    
#define IGNORE_NAME_FULL      (RESOURCE_DIR "/" IGNORE_NAME)

/*=========================================================================================*/
///* 目录信息类型 */
//typedef struct 
//{
//	char 	name[40];  /* 资源的名字 */
//	u32  	size;      /* 资源的大小 */ 
//	u32 	offset;    /* 资源相对于基地址的偏移 */
//}CatalogTypeDef;

/*=========================================================================================*/

/*信息输出*/
#define BURN_DEBUG_ON         0
#define BURN_DEBUG_FUNC_ON    0

#define BURN_INFO(fmt,arg...)           printf("<<-BURN-INFO->> "fmt"\n",##arg)
#define BURN_ERROR(fmt,arg...)          printf("<<-BURN-ERROR->> "fmt"\n",##arg)
#define BURN_DEBUG(fmt,arg...)          do{\
                                          if(BURN_DEBUG_ON)\
                                          printf("<<-BURN-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                          }while(0)

#define BURN_DEBUG_FUNC()               do{\
                                         if(BURN_DEBUG_FUNC_ON)\
                                         printf("<<-BURN-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)
/*=========================================================================================*/

int GetResOffset(const char *res_name);
FRESULT Make_Catalog (char* path, uint8_t clear)  ;
uint8_t Read_CatalogInfo( uint32_t file_index,
                            CatalogTypeDef *dir,
                            char *full_name);
void Burn_Catalog(void);
FRESULT Burn_Content(void);   
FRESULT Check_Resource(void);
FRESULT BurnFile(void);
                            

/*=========================================================================================*/

#ifdef	__cplusplus
}
#endif

#endif /*__RES_MGR__*/

