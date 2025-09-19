#include "bsp_clk.h"    // 时钟驱动头文件：包含系统时钟初始化函数
#include "bsp_delay.h"  // 延时驱动头文件：包含毫秒级延时函数
#include "bsp_led.h"    // LED驱动头文件：包含LED初始化和控制函数
#include "bsp_beep.h"   // 蜂鸣器驱动头文件：包含蜂鸣器初始化和控制函数
#include "bsp_key.h"    // 按键驱动头文件：包含按键初始化和值获取函数

/*
 * @description	: 主函数，程序入口
 * @return 		: 无（嵌入式程序通常无返回值，无限循环运行）
 * 核心功能：初始化系统硬件，通过按键控制蜂鸣器开关，同时让LED定时闪烁，演示多外设协同工作
 */
int main(void)
{
	int i = 0;                  // 计数变量，用于控制LED闪烁频率
	int keyvalue = 0;           // 存储按键值（0：无按键；KEY0_VALUE：KEY0按下）
	unsigned char led_state = OFF;  // LED状态变量（OFF：关闭；ON：打开，初始为关闭）
	unsigned char beep_state = OFF; // 蜂鸣器状态变量（OFF：关闭；ON：打开，初始为关闭）


	/* 系统硬件初始化阶段：按依赖顺序初始化各外设 */
	imx6u_clkinit();  // 初始化I.MX6ULL系统时钟（配置PLL、总线分频，确保CPU和外设工作在正确频率）
	clk_enable();     // 使能所有外设时钟（通过CCGR寄存器打开外设时钟门控，否则外设无法工作）
	led_init();       // 初始化LED对应的GPIO引脚（复用配置、属性设置、输出模式初始化）
	beep_init();      // 初始化蜂鸣器对应的GPIO引脚（同上，为输出模式）
	key_init();       // 初始化按键对应的GPIO引脚（复用配置、上拉设置、输入模式初始化）


	/* 主循环：嵌入式程序核心，无限循环处理外设交互 */
	while(1)			
	{	
		keyvalue = key_getvalue();  // 读取按键值（含10ms消抖，返回KEY0_VALUE或0）
		
		if(keyvalue)  // 若有按键按下（keyvalue非0）
		{
			switch (keyvalue)  // 根据按键值处理（当前仅支持KEY0）
			{
				case KEY0_VALUE:  // KEY0按下
					beep_state = !beep_state;  // 切换蜂鸣器状态（OFF→ON或ON→OFF）
					beep_switch(beep_state);   // 应用新状态到蜂鸣器（控制GPIO电平）
					break;
			}
		}

		// LED定时闪烁逻辑：每500ms切换一次状态（每次循环延时10ms，50次即500ms）
		i++;  // 循环计数
		if(i == 50)  // 计数到50（累计延时50*10ms=500ms）
		{
			i = 0;  // 重置计数
			led_state = !led_state;  // 切换LED状态（OFF→ON或ON→OFF）
			led_switch(LED0, led_state);  // 应用新状态到LED0（控制GPIO电平）
		}

		delay(10);  // 每次循环延时10ms，降低CPU占用，同时作为LED闪烁的时间基准
	}
}
    