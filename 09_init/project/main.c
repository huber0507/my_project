#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_exit.h"

/*
 * @description	: main函数
 * @param 		: 无
 * @return 		: 无
 */
int main(void)
{
	    unsigned char led_state = OFF;
	    unsigned char beep_state = OFF;  // 蜂鸣器状态（独立于LED）

	int_init(); 		/* 初始化中断(一定要最先调用！) */
	imx6u_clkinit();	/* 初始化系统时钟 			*/
	clk_enable();		/* 使能所有的时钟 			*/
	led_init();			/* 初始化led 			*/
	beep_init();		/* 初始化beep	 		*/
	key_init();			/* 初始化key 			*/
	exit_init();		/* 初始化按键中断			*/

	/*测试日志：2025/09/22     第一次按键按下是没有响应的，需要查明是什么原因导致*/

	while(1)			
	{	
		if(key_irq_flag==1)
		{
			delay(10);
			if(gpio_pinread(GPIO1,18)==0)
			{
				beep_state=!beep_state;
				beep_switch(beep_state);
			key_irq_flag=0;
		}
		led_state = !led_state;
		
		led_switch(LED0, led_state);
		delay(100);
	}

	return 0;
}
