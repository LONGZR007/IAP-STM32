#ifndef __GUI_INPUT_PORT_H
#define	__GUI_INPUT_PORT_H

#ifdef __cplusplus
extern "C" {
#endif  
  
#include	"emXGUI.h"
#include	"gui_drv_cfg.h"
#include "emXGUI_Arch.h"

#if defined(STM32F429_439xx)
#include "stm32f4xx.h"
#elif defined(STM32H743xx)
#include "stm32h7xx.h"
#endif
//extern GUI_SEM *Input_Sem;
BOOL GUI_InputInit(void);
void GUI_InputHandler(void);

#ifdef	__cplusplus
}
#endif  
  
#endif /* __GUI_INPUT_PORT_H */
