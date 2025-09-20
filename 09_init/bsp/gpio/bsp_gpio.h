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

typedef struct _gpio_pin_config
{
    gpio_pin_direction_t direction;
    u8 outputLogic;
    gpio_interrupt_mode_t interruputMode;
} gpio_pin_config_t;

void gpio_init(GPIO_Type *base,int pin,gpio_pin_config_t *config);
int gpio_pinread(GPIO_Type *base, int pin);
void gpio_pinwrite(GPIO_Type *base, int pin, int value);
void gpio_intconfig(GPIO_Type* base, unsigned int pin,
gpio_interrupt_mode_t pinInterruptMode);
void gpio_enableint(GPIO_Type* base, unsigned int pin);
void gpio_disableint(GPIO_Type* base, unsigned int pin);
void gpio_clearintflags(GPIO_Type* base, unsigned int pin);




#endif  // 结束头文件卫士，与开头 #ifndef __BSP_GPIO_H 配对