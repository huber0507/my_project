#ifndef __BSP_CLK_H  // 头文件卫士：判断宏 __BSP_CLK_H 是否未定义
#define __BSP_CLK_H  // 若未定义，则定义该宏，标记头文件已被包含

/*
 * 引入 imx6ul.h 头文件
 * 作用：包含 I.MX6ULL 芯片的底层寄存器结构体定义（如 CCM_TypeDef、CCM_ANALOG_TypeDef）、
 *       基地址宏（如 CCM、CCM_ANALOG）及位掩码定义，是操作硬件寄存器的基础。
 * 备注：imx6ul.h 通常由芯片厂商提供或根据参考手册生成，封装了芯片的硬件相关常量。
 */
#include "imx6ul.h"

/*
 * 函数声明：外设时钟全使能函数
 * 功能：通过配置 CCM 模块的 CCGR0~CCGR6 寄存器，使能 I.MX6ULL 所有外设的时钟（时钟门控全打开）。
 * 参数：无
 * 返回值：无
 * 备注：该函数的实现位于 bsp_clk.c 文件中，裸机初始化阶段需优先调用，确保外设可被驱动。
 */
void clk_enable(void);

#endif  // 结束头文件卫士条件编译，与开头 #ifndef __BSP_CLK_H 对应