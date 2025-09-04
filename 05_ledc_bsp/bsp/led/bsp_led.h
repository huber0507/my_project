#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "imx6ul.h"
#include <stdint.h>

// LED状态枚举（替代原代码的ON/OFF宏，类型更安全）
typedef enum {
    LED_OFF = 0,  // 熄灭
    LED_ON        // 点亮
} LedStatus_t;

// LED配置结构体（集中管理硬件参数，方便扩展）
typedef struct {
    GPIO_Type *gpio_group;  // GPIO组（如GPIO1、GPIO5）
    uint8_t pin;            // 引脚号（如3表示IO03）
    uint32_t mux_reg;       // 复用寄存器（如IOMUXC_GPIO1_IO03_GPIO1_IO03）
    uint32_t pad_config;    // 电气属性配置值（如0X10B0）
} LedConfig_t;

// 宏定义：LED编号（与led_config数组索引对应）
#define LED0 0  // 第一个LED（可扩展LED1、LED2等）

// 辅助宏：计算数组长度（用于检查LED编号合法性）
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * @description	: 初始化指定LED
 * @param - led	: LED编号（LED0、LED1等）
 * @return 		: 0-成功，-1-失败（参数无效）
 */
int led_init(uint8_t led);

/*
 * @description	: 设置LED状态
 * @param - led	: LED编号
 * @param - status: 目标状态（LED_ON/LED_OFF）
 * @return 		: 0-成功，-1-失败
 */
int led_set_status(uint8_t led, LedStatus_t status);

/*
 * @description	: 翻转LED状态
 * @param - led	: LED编号
 * @return 		: 0-成功，-1-失败
 */
int led_toggle(uint8_t led);

#endif  // __BSP_LED_H
    