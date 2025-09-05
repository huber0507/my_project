#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H

#include "imx6ul.h"  // 已包含MCIMX6Y2.h，其中有官方GPT_Type定义
#include <stdint.h>

// 【删除自定义的GPT_Type结构体】
// 芯片头文件MCIMX6Y2.h中已定义GPT_Type，直接使用即可


/*
 * @description	: 初始化延时模块（基于GPT1定时器）
 * 				  必须在main函数中先调用，否则延时函数无效
 * @param 		: 无
 * @return 		: 无
 */
void delay_init(void);

/*
 * @description	: 微秒级延时
 * @param - us	: 延时时间（单位：微秒，0~4294967295）
 * @return 		: 无
 */
void delay_us(uint32_t us);

/*
 * @description	: 毫秒级延时
 * @param - ms	: 延时时间（单位：毫秒，0~4294967）
 * @return 		: 无
 */
void delay_ms(uint32_t ms);

/*
 * @description	: 兼容旧版延时接口（映射为毫秒延时）
 * @param - n	: 延时时间（单位：毫秒）
 * @return 		: 无
 */
void delay(volatile unsigned int n);

#endif  // __BSP_DELAY_H
    