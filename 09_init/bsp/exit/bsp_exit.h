#ifndef _BSP_EXIT_H
#define _BSP_EXIT_H

#include "imx6ul.h"

// 全局中断标志位（volatile确保编译器不优化，中断和主循环共享）
extern volatile unsigned char key_irq_flag;

/* 函数声明 */
void exit_init(void);						/* 中断初始化 */
void gpio1_io18_irqhandler(void); 			/* 中断处理函数 */

#endif
