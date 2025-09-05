#ifndef __BSP_CLK_H
#define __BSP_CLK_H

#include "imx6ul.h"
#include <stdint.h>

/*
 * 外设时钟使能标识（与硬件场景对应）
 * 数值与bsp_clk.c中的switch-case分支严格对应
 */
#define CLK_GPIO1  0  // GPIO1时钟（LED等外设使用）
#define CLK_GPIO5  1  // GPIO5时钟（蜂鸣器等外设使用）
#define CLK_GPT1   2  // GPT1定时器时钟（延时函数使用）

/*
 * @description	: 使能指定外设的时钟
 * @param - periph	: 外设标识（CLK_GPIO1/CLK_GPIO5/CLK_GPT1等）
 * @return 		: 无
 */
void clk_periph_enable(uint8_t periph);

/*
 * @description	: 系统初始化时的基础时钟配置
 * 				  自动使能当前场景必需的外设时钟（LED/蜂鸣器/延时）
 * @param 		: 无
 * @return 		: 无
 */
void clk_system_init(void);

#endif  // __BSP_CLK_H
    