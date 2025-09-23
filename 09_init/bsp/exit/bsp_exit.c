
#include "bsp_exit.h"
#include "bsp_gpio.h"
#include "bsp_int.h"
#include "bsp_delay.h"
#include "bsp_beep.h"

/*
 * @description			: 初始化外部中断
 * @param				: 无
 * @return 				: 无
 */
void exit_init(void)
{
	gpio_pin_config_t key_config;

	/* 1、设置IO复用 */
	IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);			/* 复用为GPIO1_IO18 */
	IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);

	/* 2、初始化GPIO为中断模式 */
	key_config.direction = kGPIO_DigitalInput;
	key_config.interruputMode = kGPIO_IntFallingEdge;
	key_config.outputLogic = 1;
	gpio_init(GPIO1, 18, &key_config);

	    /* 关键补充：初始化时清除一次中断标志位，避免复位残留标志 */
    gpio_clearintflags(GPIO1, 18);

	GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);				/* 使能GIC中对应的中断 */
	system_register_irqhandler(GPIO1_Combined_16_31_IRQn, (system_irq_handler_t)gpio1_io18_irqhandler, NULL);	/* 注册中断服务函数 */
	gpio_enableint(GPIO1, 18);								/* 使能GPIO1_IO18的中断功能 */
}

/*
 * @description			: GPIO1_IO18最终的中断处理函数
 * @param				: 无
 * @return 				: 无
 */
void gpio1_io18_irqhandler(void)
{ 

	    // 1. 先清除中断标志位（避免重复触发）

	gpio_clearintflags(GPIO1, 18); /* 清除中断标志位 */
	    // 2. 置位中断标志（通知主循环：有按键事件）
    key_irq_flag = 1;  
}



