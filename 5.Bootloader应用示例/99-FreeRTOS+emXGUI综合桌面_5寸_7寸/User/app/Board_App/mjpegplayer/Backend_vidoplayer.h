#ifndef __VIDOPLAYER_H
#define __VIDOPLAYER_H

#include <stm32f4xx.h>
#include "emXGUI.h"

#define  T_vids	 0x6463
#define  T_auds	 0x7762


#define FILE_NAME_LEN           100	
#define FILE_MAX_NUM            20
#define File_Path               "0:/srcdata/视频文件/"

#define AVI_DEBUG_ON         	0
#define AVI_DEBUG_ARRAY_ON    0
#define AVI_DEBUG_FUNC_ON   	0
// Log define
#define AVI_INFO(fmt,arg...)           printf("<<-AVI-INFO->> "fmt"\n",##arg)
#define AVI_ERROR(fmt,arg...)          printf("<<-AVI-ERROR->> "fmt"\n",##arg)
#define AVI_DEBUG(fmt,arg...)          do{\
                                         if(AVI_DEBUG_ON)\
                                         printf("<<-AVI-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
																					}while(0)

#define AVI_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(AVI_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-AVI-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)

#define AVI_DEBUG_FUNC()               do{\
                                         if(AVI_DEBUG_FUNC_ON)\
                                         printf("<<-AVI-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)


#if 0
void AVI_play(void);
void Prog_ctl(void);
#else
void AVI_play(char *filename, HWND hwnd, int vol, int vol_horn);
extern u32 alltime;		//总时长 
extern u32 cur_time; 		//当前播放时间 
#endif
#endif

