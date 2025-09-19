#ifndef _BSP_KEY_H  // 头文件卫士：判断宏 _BSP_KEY_H 是否未定义
#define _BSP_KEY_H  // 若未定义，则定义该宏，防止头文件被重复包含（避免类型/函数重复定义）

/*
 * 引入 imx6ul.h 头文件
 * 作用：提供 I.MX6ULL 芯片的底层硬件定义，包括：
 *       1. 基本数据类型（如 uint32_t、unsigned char 等）；
 *       2. GPIO 模块相关的寄存器结构体和基地址（如 GPIO1）；
 *       确保按键驱动中使用的类型和硬件地址与芯片架构匹配。
 */
#include "imx6ul.h"

/*
 * 枚举类型：按键值定义
 * 作用：标准化按键状态的返回值，用语义化名称替代数字，提高代码可读性和可维护性
 * 适用场景：区分"无按键按下"和"KEY0按下"两种状态（多按键场景可扩展更多值）
 */
enum keyvalue{
    KEY_NONE = 0,    // 0：无按键按下（默认状态）
    KEY0_VALUE,      // 1：KEY0按键按下（枚举值默认自增，此处值为1）
};

/*
 * 函数声明：按键初始化函数
 * @description	: 初始化按键对应的GPIO引脚，包括引脚复用、电气属性配置和输入模式设置
 * @param 		: 无
 * @return 		: 无
 * 实现位置：bsp_key.c，需配合硬件电路（按键连接到GPIO1_IO18）
 */
void key_init(void);

/*
 * 函数声明：获取按键值函数（含消抖处理）
 * @description	: 读取按键状态，通过软件延时消抖，返回有效按键值
 * @param 		: 无
 * @return 		: 按键值（KEY_NONE：无按键；KEY0_VALUE：KEY0按下，对应枚举值）
 * 实现位置：bsp_key.c，使用静态变量记录按键状态，避免一次按下被多次识别
 */
int key_getvalue(void);

#endif // _BSP_KEY_H  // 结束头文件卫士，与开头 #ifndef _BSP_KEY_H 配对