#ifndef __BSP_LED_H  // 头文件卫士：判断宏 __BSP_LED_H 是否未定义
#define __BSP_LED_H  // 若未定义，则定义该宏，防止头文件被重复包含（避免编译错误）

/*
 * 引入 imx6ul.h 头文件
 * 作用：提供 I.MX6ULL 芯片的底层硬件定义，是LED驱动操作硬件的基础，包括：
 *       1. GPIO 模块寄存器结构体（如 GPIO_Type）及基地址映射（如 GPIO1）；
 *       2. 基本数据类型（如 int、unsigned int），确保与芯片32位架构匹配；
 *       3. IOMUXC 复用控制相关宏定义（间接支撑 .c 文件中的引脚配置）。
 */
#include "imx6ul.h"

/*
 * LED 设备编号宏定义
 * 定义：#define LED0 0
 * 用途：
 *   1. 标识系统中的LED设备（当前为第0个LED，即LED0）；
 *   2. 预留多LED扩展接口（如新增LED1可定义 #define LED1 1，配合 led_switch 函数的 switch 分支扩展）；
 *   3. 统一LED编号管理，避免代码中直接使用数字（魔术数字），提高可读性。
 */
#define LED0	0

/*
 * 函数声明：LED初始化函数
 * @description	: 完成LED0对应GPIO引脚的硬件初始化，包括：
 *                1. 引脚复用配置（切换为GPIO功能）；
 *                2. 引脚电气属性配置（下拉、驱动能力、速度等）；
 *                3. GPIO输出模式设置及初始电平配置（默认点亮LED0）。
 * @param 		: 无
 * @return 		: 无
 * 实现位置：对应 bsp_led.c 文件中的同名函数
 */
void led_init(void);

/*
 * 函数声明：LED开关控制函数
 * @description	: 根据LED编号和状态，控制指定LED的打开/关闭。
 * @param - led	: 要控制的LED编号（当前仅支持 LED0，对应宏定义 LED0=0）；
 * @param - status	: 控制状态（需配合外部定义的 ON/OFF 宏使用，通常：
 *                    ON=1 表示打开LED，OFF=0 表示关闭LED）。
 * @return 		: 无
 * 实现位置：对应 bsp_led.c 文件中的同名函数
 * 备注：函数通过 switch 语句预留了多LED控制能力，新增LED时只需扩展 case 分支。
 */
void led_switch(int led, int status);

#endif  // 结束头文件卫士，与开头 #ifndef __BSP_LED_H 配对