#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"

/*
 * @description	: main函数
 * @param 		: 无
 * @return 		: 无
 */
int main(void)
{
	int i = 0;
	int keyvalue = 0;
	unsigned char led_state = OFF;  // LED初始状态：灭
	unsigned char beep_state = OFF;
	
	clk_enable();		/* 使能所有的时钟 			*/
	led_init();			/* 初始化led 			*/
	beep_init();		/* 初始化beep	 		*/
	key_init();			/* 初始化key 			*/

	while(1)			
	{	
		keyvalue = key_getvalue();  // 获取按键值
		if(keyvalue)  // 按键按下
		{
			switch (keyvalue)
			{
				case KEY0_VALUE:  // KEY0按下
					led_state = !led_state;  // 翻转LED状态（灭→亮 或 亮→灭）
					led_switch(LED0, led_state);  // 应用新状态
					break;
			}
		}
		// 移除else分支的"led_switch(LED0,0)"，避免松开按键时强制关闭LED

		delay(10);  // 延时消抖
	}

	return 0;
}
