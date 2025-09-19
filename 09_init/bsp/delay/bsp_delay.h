#ifndef __BSP_DELAY_H  // 头文件卫士：判断宏 __BSP_DELAY_H 是否未定义
#define __BSP_DELAY_H  // 若未定义，则定义该宏，防止头文件被重复包含

/*
 * 引入 imx6ul.h 头文件
 * 作用：提供 I.MX6ULL 芯片的基础类型定义和寄存器映射，
 *       确保延时函数中使用的变量类型（如 unsigned int）与芯片架构匹配，
 *       同时为可能的扩展（如结合定时器硬件延时）提供底层支持。
 */
#include "imx6ul.h"


/*
 * 函数声明：毫秒级延时函数
 * @description	: 实现近似毫秒级的延时功能，基于软件空循环实现
 * @param - n	: 延时的毫秒数（n=1 表示延时约1ms，具体时长与CPU主频相关）
 * @return 		: 无
 * 备注：
 *   1. 函数参数使用 volatile 修饰，防止编译器优化掉延时循环（确保延时生效）；
 *   2. 该函数的具体实现位于 bsp_delay.c 文件中，内部通过调用短延时函数 delay_short 累积实现；
 *   3. 延时精度依赖CPU主频（在396MHz主频下校准），主频变化时需重新调整内部循环参数。
 */
void delay(volatile unsigned int n);

#endif  // __BSP_DELAY_H  // 结束头文件卫士，与开头 #ifndef 配对