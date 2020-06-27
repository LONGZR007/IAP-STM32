

//#include "Include.h"
//#include "x_file.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <emXGUI.h>
#include <x_libc.h>
#include "res_mgr.h"


/*============================================================================*/


//SD卡源数据路径！！
char src_dir[512] __EXRAM;
const char src_dir_const[] = RESOURCE_DIR;

static FIL file_temp __EXRAM;													/* file objects */
static char full_file_name[512] __EXRAM;
static char line_temp[512] __EXRAM;
static u32 file_num = 0;

extern HWND wnd_info_textbox ;
extern HWND wnd_progbar;

extern void SPI_FLASH_BulkErase_GUI(void);

 

/**
  * @brief  检查是否要忽略文件夹或文件
  * @param  check_name[in] 要检查的文件名,全路径
  * @param  ignore_file[in] ignore文件的路径，全路径
  * @retval 1表示忽略，0表示不忽略
  */
uint8_t Is_Ignore(char *check_name, char *ignore_file)
{
  FRESULT result;
  uint32_t i;
  
  char *ignore_file_dir;
  char *dir_ptr ;
  char *full_path;
  
  result = f_open(&file_temp, ignore_file, FA_OPEN_EXISTING | FA_READ);
  
  if(result == FR_NO_FILE)
  {
      BURN_DEBUG("不存在忽略文件，直接返回");
      return 0;
  }   
  
  ignore_file_dir = GUI_VMEM_Alloc(512);
  full_path = GUI_VMEM_Alloc(512);
  
  if(ignore_file_dir == NULL || full_path == NULL)
  {
    BURN_ERROR("错误！malloc时内存不足！");
    return 0;
  }
  
  /* 得到ignore文件所在的目录 */
  strcpy(ignore_file_dir, ignore_file);
  dir_ptr = strrchr(ignore_file_dir, '/');
  *(dir_ptr) = '\0';  

  /* 遍历忽略文件 */
  for(i=0;1;i++)
  {
     /* 读一行忽略的文件名 */
    f_gets(line_temp, sizeof(line_temp), &file_temp);
    /* 注释行，跳过*/
    if(line_temp[0] == '#')
      continue;
    
    /* 替换掉回车 */
    line_temp[strlen(line_temp)-1] = '\0';
    
    /* 拼接出完整的忽略文件路径 */
    sprintf(full_path, "%s/%s",ignore_file_dir,line_temp);
  
//    BURN_DEBUG("full_path=%s,\r\ncheck_name=%s\r\n",
//                full_path,
//                check_name);
    /* 比较是否一致 */
    if(strcasecmp(check_name,full_path) == 0)
    {      
      f_close(&file_temp);
      GUI_VMEM_Free(ignore_file_dir);
      GUI_VMEM_Free(full_path);
      
      return 1;
    }
    /* 已到文件尾，遍历完毕,跳出循环 */
    if(f_eof(&file_temp) != 0 )   
    { 
      f_close(&file_temp);
      break;
    }
  }  
  
  GUI_VMEM_Free(ignore_file_dir);
  GUI_VMEM_Free(full_path);
  
  return 0;
}

/**
  * @brief  Make_Catalog 创建资源目录文件，这是个递归函数
  * @param  path[in]:资源路径，必须输入一个空间足够的数组，
                     函数执行时会对它进行路径赋值操作
  * @param  clear: 0 表示重新生成目录文件，1表示在原文件下追加内容
  * @note   在外部调用本函数时，clear必须设置为0，
  *         函数内部递归调用才设置为1.
  * @retval result:文件系统的返回值
  */
FRESULT Make_Catalog (char* path,uint8_t clear)  
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i;            
  char *fn;        // 文件名	
   
  /* 记录地址偏移信息 */
  static uint32_t resource_addr = CATALOG_SIZE ;
    
#if _USE_LFN 
  /* 长文件名支持 */
  /* 简体中文需要2个字节保存一个“字”*/
  static char lfn[_MAX_LFN*2 + 1]; 	
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
  
  if(clear == 0)
  {  
    BURN_INFO("正在生成烧录信息文件catalog.txt...\r\n"); 

    /* 重置进度条 */
    SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,0);
    SetWindowText(wnd_progbar,L"生成目录文件...");
    /* 设置进度条最大值为100 */
    SendMessage(wnd_progbar,PBM_SET_RANGLE,TRUE,100);

    SetWindowText(wnd_info_textbox,L"生成目录文件...");
    GUI_msleep(20);
    
    /* 第一次执行Make_Catalog函数时删除旧的烧录信息文件 */
    f_unlink(BURN_INFO_NAME_FULL);
  }
  
  //打开目录
  res = f_opendir(&dir, path); 
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    for (;;) 
		{ 
      //读取目录下的内容，再读会自动读下一个文件
      res = f_readdir(&dir, &fno); 								
      //为空时表示所有项目读取完毕，跳出
      if (res != FR_OK || fno.fname[0] == 0) break; 	
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      //点表示当前目录，跳过			
      if (*fn == '.') continue; 	
      //目录，递归读取      
      if (fno.fattrib & AM_DIR)         
			{ 			
        //合成完整目录名        
        sprintf(&path[i], "/%s", fn); 
        
        BURN_INFO("-------------------------------------");
        BURN_INFO("文件夹：%s",path);
        
        /* 若名字跟ignore文件中记录的相同，忽略*/
        if(Is_Ignore(path, IGNORE_NAME_FULL) == 1)
        {
          BURN_INFO("忽略文件夹：%s",path);
          continue;
        }
        //递归遍历         
        res = Make_Catalog(path,1);	
        path[i] = 0;         
        //打开失败，跳出循环        
        if (res != FR_OK) 
					break; 
      } 
			else 
			{ 

        uint32_t file_size; 
        
        /* 忽略本身的目录文件 和 忽略文件 */
        if(strcasecmp(fn,BURN_INFO_NAME) == 0 ||
            strcasecmp(fn,IGNORE_NAME) == 0)
          continue;     

        /* 得到全文件名 */
        sprintf(full_file_name, "%s/%s",path, fn); 
        
        /* 若名字跟ignore文件中记录的相同，忽略*/
        if(Is_Ignore(full_file_name, IGNORE_NAME_FULL) == 1)
        {
          BURN_INFO("-------------------------------------");

          BURN_INFO("忽略文件：%s",full_file_name);
          continue;
        }
        
        f_open(&file_temp, full_file_name, FA_OPEN_EXISTING | FA_READ);
        file_size = f_size(&file_temp);
        f_close(&file_temp);
        
        BURN_INFO("-------------------------------------"); 
				BURN_INFO("资源文件路径:%s", full_file_name);			//完整文件名	
        BURN_INFO("资源文件名:%s", fn);						     //文件名         
        BURN_INFO("资源大小:%d", file_size);				 //文件大小	
        BURN_INFO("要烧录到的偏移地址:%d，绝对地址:%d", 
                          resource_addr, 
                          resource_addr + RESOURCE_BASE_ADDR  );	 //文件名要存储到的资源目录	
        
        f_open(&file_temp, BURN_INFO_NAME_FULL, FA_OPEN_ALWAYS |FA_WRITE | FA_READ);
        
        f_lseek(&file_temp, f_size(&file_temp));
        /* 每个文件记录占5行 */
        f_printf(&file_temp, "%s\n", full_file_name);			//完整文件名	
        f_printf(&file_temp, "%s\n", fn);						     //文件名         
        f_printf(&file_temp, "%d\n", file_size);				 //文件大小	
        f_printf(&file_temp, "%d\n\n", resource_addr);	 //文件名要存储到的资源目录(未加上基地址)	

        /* 记录文件数 */
        file_num++;
        if(file_num < 100)
        {
          SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,file_num);
        }
        else
        {
          /* 超过进度条最大值时显示99 */
          SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,99);
        }
        GUI_msleep(20);
        
        f_close(&file_temp);

        resource_addr += file_size; /* 偏移文件的大小 */

        /* 可以在这里提取特定格式的文件路径 */        
      }//else
    } //for
    f_closedir(&dir);
  }
  
  /* 输出烧录文件信息 */
  if(clear == 0)
  {
    BURN_INFO("-------------------------------------\r\n"); 
    /* 执行至最后时resource_addr是最后一个资源的尾地址 */
    BURN_INFO("所有烧录的总数据大小:%d （含目录）", resource_addr );	 

    BURN_INFO("烧录后占用的地址空间:%d - %d",
                                      RESOURCE_BASE_ADDR, 
                                      resource_addr + RESOURCE_BASE_ADDR );	 
    
    /* 显示完成 */
    SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,100);
    GUI_msleep(20);

    }
      
  return res; 
}



/**
  * @brief  读取目录文件的信息到
  * @param  catalog_name[in] 存储了目录信息的文件名
  * @param  file_index 第file_index条记录
  * @param  dir[out] 要烧录到FLASH的目录信息
  * @param  full_file_name[out] 第file_index条记录中的文件的全路径
  * @retval 1表示到达了文件尾，0为正常
  */
uint8_t Read_CatalogInfo(uint32_t file_index,
                            CatalogTypeDef *dir,
                            char *full_name)
{
  uint32_t i;
  
  f_open(&file_temp, BURN_INFO_NAME_FULL, FA_OPEN_EXISTING | FA_READ);
  
  /* 跳过前N行,每个文件记录5行 */
  for(i=0;i<file_index*5;i++)
  {
    f_gets(line_temp, sizeof(line_temp), &file_temp);
  }
  /* 到达文件尾*/
  if(f_eof(&file_temp) != 0 )
    return 1;
  
  /* 文件全路径 */
  f_gets(line_temp, sizeof(line_temp), &file_temp);
  memcpy(full_name, line_temp, strlen(line_temp)+1);
  /* 替换掉回车 */
  full_name[strlen(full_name)-1] = '\0';
  
  /* 文件名 */
  f_gets(line_temp, sizeof(line_temp), &file_temp);
  memcpy(dir->name, line_temp, strlen(line_temp)> (sizeof(CatalogTypeDef)-8) ? (sizeof(CatalogTypeDef)-8):strlen(line_temp)+1 );
  dir->name[strlen(dir->name)-1] = '\0';
  
  /* 文件大小 */
  f_gets(line_temp, sizeof(line_temp), &file_temp);
  dir->size = atoi(line_temp);
  
  /* 文件要烧录的位置 */
  f_gets(line_temp, sizeof(line_temp), &file_temp);
  dir->offset = atoi(line_temp);
  
  f_close(&file_temp);  
  
  BURN_DEBUG("ful name=%s,\r\nname=%s,\r\nsize=%d,\r\noffset=%d\r\n",
              full_name,
              dir->name,
              dir->size,
              dir->offset
              );
  
  return 0;
}

/**
  * @brief  烧录目录到FLASH中
  * @param  catalog_name 记录了烧录信息的目录文件名
  * @note  使用本函数前需要确认flash已为擦除状态
  * @retval 1表示到达了文件尾，0为正常
  */
void Burn_Catalog(void)
{
  CatalogTypeDef dir;
  uint8_t i;
  uint8_t is_end;
  
  /* 重置进度条 */
  SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,0);
  SetWindowText(wnd_progbar,L"烧写目录中...");
  /* 设置最大值 */
  SendMessage(wnd_progbar,PBM_SET_RANGLE,TRUE,file_num);
  SetWindowText(wnd_info_textbox,L"烧写目录中...");

  GUI_msleep(20);

  /* 遍历目录文件 */
  for(i=0;1;i++)
  {
    is_end = Read_CatalogInfo(i, 
                                &dir,
                                full_file_name);  
    /* 已遍历完毕,跳出循环 */
    if(is_end !=0)   
      break;
    
    /* 更新进度条 */
    SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,i);
    GUI_msleep(20);

    /* 把dir信息烧录到FLASH中 */  
    SPI_FLASH_BufferWrite((u8*)&dir,RESOURCE_BASE_ADDR + sizeof(dir)*i,sizeof(dir));
  }
  
  SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,file_num);
  SetWindowText(wnd_info_textbox,L"烧写目录成功");
  GUI_msleep(20);

}

/**
  * @brief  根据目录文件信息烧录内容
  * @param  catalog_name 记录了烧录信息的目录文件名
  * @note  使用本函数前需要确认flash已为擦除状态
  * @retval 文件系统操作返回值
  */
FRESULT Burn_Content(void)
{  
  CatalogTypeDef dir;
  uint8_t i;
  uint8_t is_end;   
  
  FRESULT result;   
  UINT  bw;            					    /* File R/W count */
  uint32_t write_addr=0;
  uint8_t tempbuf[256];
  
  /* 重置进度条 */
  SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,0);
  SetWindowText(wnd_progbar,L"烧写文件");
  /* 设置最大值*/
  SendMessage(wnd_progbar,PBM_SET_RANGLE,TRUE,file_num);
  GUI_msleep(20);

 
  /* 遍历目录文件 */
  for(i=0;1;i++)
  {
    is_end = Read_CatalogInfo(i, 
                                &dir,
                                full_file_name);  
    /* 已遍历完毕,跳出循环 */
    if(is_end !=0)   
      break;  
  
    BURN_INFO("-------------------------------------"); 
    BURN_INFO("准备烧录内容：%s",full_file_name);
    
    x_wsprintf((WCHAR*)tempbuf,L"烧写中(%d/%d),烧写大文件需要很长时间.\r\n请耐心等待...",i,file_num);
    SetWindowText(wnd_info_textbox,(WCHAR*)tempbuf);
    SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,i);

    GUI_msleep(20);

    LED_BLUE;
     
     result = f_open(&file_temp,full_file_name,FA_OPEN_EXISTING | FA_READ);
      if(result != FR_OK)
      {
          BURN_ERROR("打开文件失败！");
          LED_RED;
          return result;
      }
  
      BURN_INFO("正在向FLASH写入内容...");
      
      write_addr = dir.offset + RESOURCE_BASE_ADDR;
      while(result == FR_OK) 
      {
        result = f_read( &file_temp, tempbuf, 256, &bw);//读取数据	 
        if(result!=FR_OK)			 //执行错误
        {
          BURN_ERROR("读取文件失败！");
          LED_RED;
          return result;
        }      
        SPI_FLASH_BufferWrite(tempbuf,write_addr,bw);  //拷贝数据到外部flash上    
        write_addr+=bw;				
        if(bw !=256)break;
      }
      BURN_INFO("内容写入完毕");          
           
    f_close(&file_temp);     
  }
  
  BURN_INFO("************************************");
  BURN_INFO("所有文件均已烧录完毕！（非文件系统部分）");
  
  SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,file_num);
  SetWindowText(wnd_info_textbox,L"所有资源烧写完成");
  GUI_msleep(20);

  return FR_OK;
}

/**
  * @brief  校验写入的内容
  * @param  catalog_name 记录了烧录信息的目录文件名
  * @retval 文件系统操作返回值
*/
FRESULT Check_Resource(void)
{

  CatalogTypeDef dir;
  uint8_t i;
  uint8_t is_end;
  int offset;
  
  FRESULT result; 
  UINT  bw;            					    /* File R/W count */
  uint32_t read_addr=0,j=0;
  uint8_t tempbuf[256],flash_buf[256];
  
  /* 重置进度条 */
  SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,0);
  SetWindowText(wnd_progbar,L"校验文件中...");

  /* 设置最大值 */
  SendMessage(wnd_progbar,PBM_SET_RANGLE,TRUE,file_num);
  GUI_msleep(20);
 
  /* 遍历目录文件 */
  for(i=0;1;i++)
  {
    is_end = Read_CatalogInfo(i, 
                                &dir,
                                full_file_name);  
    /* 已遍历完毕,跳出循环 */
    if(is_end !=0)   
      break;    
    
    x_wsprintf((WCHAR *)tempbuf,L"校验中(%d/%d),校验大文件需要很长时间.\r\n请耐心等待...",i,file_num);
    SetWindowText(wnd_info_textbox,(WCHAR *)tempbuf);
    SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,i);

    GUI_msleep(20);
    
    /* 在FLASH的目录里查找对应的offset */
    offset = GetResOffset(dir.name);
    if(offset == -1)
    {
      LED_RED;
      BURN_INFO("校验失败，无法在FLASH中找到文件：%s 的目录",dir.name);
      return FR_NO_FILE;
    }
    else
      dir.offset = offset + RESOURCE_BASE_ADDR;
       
    BURN_INFO("-------------------------------------"); 
    BURN_INFO("准备校验内容：%s",full_file_name);
    BURN_INFO("内容名：%s ",dir.name);
    BURN_INFO("内容绝对地址：%d ",dir.offset);
    BURN_INFO("内容大小：%d ",dir.size);
    
      result = f_open(&file_temp,full_file_name,FA_OPEN_EXISTING | FA_READ);
      if(result != FR_OK)
      {
          BURN_ERROR("打开文件失败！");
              LED_RED;
          return result;
      }
          
       //校验数据
      read_addr = dir.offset;
     
      f_lseek(&file_temp,0);
      
      while(result == FR_OK) 
      {
        result = f_read( &file_temp, tempbuf, 256, &bw);//读取数据	 
        if(result!=FR_OK)			 //执行错误
        {
          BURN_ERROR("读取文件失败！");
          LED_RED;
          return result;
        }      
        SPI_FLASH_BufferRead(flash_buf,read_addr,bw);  //从FLASH中读取数据
        read_addr+=bw;		
        
        for(j=0;j<bw;j++)
        {
          if(tempbuf[i] != flash_buf[i])
          {
            BURN_ERROR("数据校验失败！");
            LED_RED;
            return FR_INT_ERR;
          }
         }  
     
        if(bw !=256)break;//到了文件尾
      }      

      BURN_INFO("数据校验正常！");
      LED_BLUE;
               
      f_close(&file_temp);     
  }
  
  LED_GREEN;
  BURN_INFO("************************************");
  BURN_INFO("所有文件校验正常！（非文件系统部分）");
  
  SendMessage(wnd_progbar,PBM_SET_VALUE,TRUE,file_num);
  SetWindowText(wnd_info_textbox,L"所有文件校验正常!");
  GUI_msleep(20);

  return FR_OK;
}


/**
  * @brief  所有烧录操作
  * @param  无
  * @retval 无
  */
FRESULT BurnFile(void)
{
  FRESULT result;   
  DIR dir; 

//  BURN_INFO("注意该操作会把FLASH的原内容会被删除！！");   
  file_num = 0;
  
  /* 复制初始路径 */
  strcpy(src_dir,src_dir_const);
  
  /* 打开路径测试 */
  result = f_opendir(&dir, src_dir); 

  if(result != FR_OK)
  {
    GUI_ERROR("请插入带‘srcdata’烧录数据的SD卡,并重新复位开发板！ result = %d",result);
    SetWindowText(wnd_info_textbox,L"1.插入带有srcdata文件夹的SD卡.\r\n2.重新给板子上电.");
    GUI_msleep(20);
    
    return result;
  }
  f_closedir(&dir);
  
  SetWindowText(wnd_info_textbox,L"在进行整片擦除,时间很长,\r\n请耐心等候...");
  GUI_msleep(20);
  
  BURN_INFO("正在进行整片擦除，时间很长，请耐心等候...");
  

  /* 整片FLASH擦除 */
  SPI_FLASH_BulkErase_GUI();    
  

  /* 生成烧录目录信息文件 */
  Make_Catalog(src_dir,0);
  
  /* 烧录 目录信息至FLASH*/
  Burn_Catalog();  
  /* 根据 目录 烧录内容至FLASH*/
  Burn_Content();
  /* 校验烧录的内容 */
  return Check_Resource();
}

/*********************************************END OF FILE**********************/

