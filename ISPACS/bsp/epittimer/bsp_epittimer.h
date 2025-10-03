#ifndef __BSP_EPITTIMER_H
#define __BSP_EPITTIMER_H

#include "imx6ul.h"

// 以下是正确的声明，替换原有的“void epit1_irqhandler(void);”
void epit1_init(unsigned int frac, unsigned int value);
void epit1_irqhandler(unsigned int irq, void *userParam);  // 关键修改：带两个参数


#endif