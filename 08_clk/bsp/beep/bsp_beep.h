#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H
#include "imx6ul.h"

#define BEEP	0

/* 函数声明 */
void beep_init(void);
void beep_switch(int status);


#endif  // __BSP_BEEP_H