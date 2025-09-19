#ifndef __BSP_GPIO_H  // 头文件卫士：判断宏 __BSP_GPIO_H 是否未定义
#define __BSP_GPIO_H  // 若未定义，则定义该宏，防止头文件被重复包含（避免函数/类型重复定义）

/*
 * 引入 imx6ul.h 头文件
 * 作用：提供 I.MX6ULL 芯片的底层硬件定义，包括：
 *       1. GPIO 模块寄存器结构体（GPIO_Type）及基地址映射（如 GPIO1、GPIO5 等）；
 *       2. 基本数据类型定义（如 uint8_t，确保跨平台兼容性）；
 *       是 GPIO 驱动操作硬件寄存器的基础依赖。
 */
#include "imx6ul.h"

/* 枚举类型：定义 GPIO 引脚方向（输入/输出）
 * 作用：用枚举值替代 magic number（魔术数字），使代码更易读、易维护
 * 命名规范：前缀 kGPIO_ 表示 GPIO 相关常量，后缀区分输入/输出
 */
typedef enum _gpio_pin_direction
{
    kGPIO_DigitalInput = 0U,  // 0：数字输入模式（引脚用于读取外部电平信号）
    kGPIO_DigitalOutput = 1U, // 1：数字输出模式（引脚用于输出高低电平控制外设）
} gpio_pin_direction_t;  // 枚举类型别名，简化声明（后续可用该别名定义变量）

typedef enum _gpio_interrupt_mode
{
    kGPIO_NoIntmode = 0U,
    kGPIO_IntLowLevel=1U,
    kGPIO_IntHighlevel=2U,
    kGPIO_IntRisingEdge = 3U,
    kGPIO_IntFallingEdge = 4U, 
    kGPIO_IntRisingOrFallingEdge = 5U,
}gpio_interrupt_mode_t;




/* 结构体：GPIO 引脚配置参数
 * 作用：封装 GPIO 初始化所需的全部配置信息，通过结构体参数传递，简化函数接口
 */
typedef struct  _gpio_pin_config
{
   gpio_pin_direction_t direction;  // 引脚方向（输入/输出，使用上面定义的枚举类型）
   uint8_t outputLogic;             // 输出模式下的初始电平（0：低电平；1：高电平）
                                    // 输入模式下该字段无效（可忽略）
} gpio_pin_config_t;  // 结构体类型别名，简化声明




/*
 * 函数声明：GPIO 引脚初始化函数
 * @description	: 配置 GPIO 引脚的方向（输入/输出），输出模式下同时设置初始电平
 * @param - base	: 指向 GPIO 组基地址的指针（如 &GPIO5，表示操作第5组 GPIO）
 * @param - pin	: 要初始化的引脚号（0~31，对应组内的第几个引脚，如 1 表示 GPIOx_IO1）
 * @param - config: 指向 gpio_pin_config_t 结构体的指针，包含初始化配置信息
 * @return 		: 无
 * 实现位置：bsp_gpio.c
 */
void gpio_init(GPIO_Type *base, int pin, gpio_pin_config_t *config);

/*
 * 函数声明：读取 GPIO 引脚电平
 * @description	: 获取指定 GPIO 引脚的当前电平状态（输入/输出模式均适用）
 * @param - base	: 指向 GPIO 组基地址的指针
 * @param - pin	: 要读取的引脚号
 * @return 		: 引脚电平（0：低电平；1：高电平）
 * 实现位置：bsp_gpio.c
 */
int gpio_pinread(GPIO_Type *base, int pin);

/*
 * 函数声明：设置 GPIO 引脚输出电平
 * @description	: 控制输出模式下 GPIO 引脚的电平（仅对输出模式引脚有效）
 * @param - base	: 指向 GPIO 组基地址的指针
 * @param - pin	: 要设置的引脚号
 * @param - value: 输出电平（0：低电平；非0：高电平）
 * @return 		: 无
 * 实现位置：bsp_gpio.c
 */
void gpio_pinwrite(GPIO_Type *base, int pin, int value);

#endif  // 结束头文件卫士，与开头 #ifndef __BSP_GPIO_H 配对