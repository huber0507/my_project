#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "stdio.h"


/*
 * @description	: 门开合状态函数
 * @param 		: 无
 * @return 		: 无
 */

int door_state(void);
{
	
}



/*
 * @description	: main函数
 * @param 		: 无
 * @return 		: 无
 */
int main(void)
{
	int i = 0;    /*记录按键按下多少次*/
	int t = 0;	/*记录时间过去了多少秒*/
	int keyvalue = 0;		/*记录案件值*/

	unsigned char led_state = OFF;
	unsigned char beep_state = OFF;

	imx6u_clkinit();	/* 初始化系统时钟 			*/
	clk_enable();		/* 使能所有的时钟 			*/
	led_init();			/* 初始化led 			*/
	beep_init();		/* 初始化beep	 		*/
	key_init();			/* 初始化key 			*/



	return 0;
}
