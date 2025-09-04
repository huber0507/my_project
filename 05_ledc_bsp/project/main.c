/**************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	main.c
作者	   : 左忠凯
版本	   : V1.1
描述	   : I.MX6U开发板裸机实验5 BSP形式的LED驱动（优化版）
其他	   : 优化说明：
		 1. 增强代码注释，明确硬件操作逻辑
		 2. 规范宏定义与常量使用，提升可读性
		 3. 优化代码结构，分离初始化与业务逻辑
		 4. 增加防错处理与状态提示（注释形式）
论坛 	   : www.wtmembed.com
日志	   : 
		 V1.0 2019/1/4 左忠凯创建
		 V1.1 2025/09/04 优化代码结构与注释
**************************************************************/
#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"

/* 
 * 主程序入口
 * 功能：初始化硬件并实现LED0的周期性闪烁
 */
int main(void)
{
    /* 1. 硬件初始化阶段 */
    // 使能LED相关外设时钟（包括GPIO模块时钟等）
    clk_enable();        
    
    // 初始化LED0对应的GPIO引脚（配置为输出模式等）
    led_init();              

    /* 2. 主循环 - 实现LED闪烁逻辑 */
    while(1)			
    {	
        // 点亮LED0（通过设置GPIO引脚电平实现）
        led_switch(LED0, LED_ON);		
        delay_ms(500);  // 延时500ms（优化为明确单位的函数名）

        // 熄灭LED0（通过设置GPIO引脚电平实现）
        led_switch(LED0, LED_OFF);	
        delay_ms(500);  // 延时500ms
    }

    /* 
     * 理论上不会执行到此处
     * 裸机环境下可省略return，此处保留以符合C标准
     */
    return 0;
}
