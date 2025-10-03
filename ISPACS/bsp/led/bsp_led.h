#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "imx6ul.h"

// 正确宏定义（与上述led_switch逻辑匹配）
#define ON  1
#define OFF 0
#define LED0 0  // 若led_switch第一个参数是LED编号，确保定义正确
#define LED0_GPIO GPIO1  // LED0对应的GPIO端口
#define LED0_PIN  3      // LED0对应的引脚位


/* 函数声明 */
void led_init(void);
void led_switch(int led, int status);
#endif

