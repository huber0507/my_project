#include "bsp_epittimer.h"
#include "bsp_int.h"
#include "bsp_led.h"
#include "bsp_door.h"  // 包含门状态头文件

/*
 * @description		: 初始化EPIT定时器（1ms中断一次，用于门超时计时）
 *					  时钟源：ipg=66MHz，配置为1ms触发一次中断
 * @param - frac	: 分频值（0~4095），建议传65（66分频，66MHz/66=1MHz）
 * @param - value	: 倒计数值，建议传1000（1MHz/1000=1kHz，即1ms中断）
 * @return 			: 无
 */
void epit1_init(unsigned int frac, unsigned int value)
{
	if(frac > 0XFFF)
		frac = 0XFFF;
		
	EPIT1->CR = 0;	/* 先清零CR寄存器 */
	
	/* CR寄存器配置：
     * bit25:24=01  时钟源选择Peripheral clock（66MHz）
     * bit15:4=frac 分频值（frac+1为实际分频系数）
     * bit3=1       计数器到0后从LR重新加载
     * bit2=1       使能比较中断
     * bit1=1       初始值来源于LR寄存器
     * bit0=0       先关闭定时器，配置完成后再使能
     */
	EPIT1->CR = (1<<24 | frac << 4 | 1<<3 | 1<<2 | 1<<1);
	
	EPIT1->LR = value;	/* 倒计数值（1ms中断：1MHz时钟下，value=1000） */
	EPIT1->CMPR	= 0;	/* 比较值为0，计数器到0时触发中断 */

	/* 使能GIC中断控制器中的EPIT1中断 */
	GIC_EnableIRQ(EPIT1_IRQn);

	/* 注册中断服务函数（参数匹配system_irq_handler_t类型） */
	system_register_irqhandler(EPIT1_IRQn, epit1_irqhandler, NULL);	

	EPIT1->CR |= 1<<0;	/* 使能EPIT1定时器 */ 
}

/*
 * @description			: EPIT1中断服务函数（1ms触发一次）
 * @param - irq			: 中断号（此处为EPIT1_IRQn）
 * @param - userParam	: 用户参数（未使用，传NULL）
 * @return 				: 无
 * 功能：门打开时累加door_open_time（每1ms+1），用于超时检测
 */
void epit1_irqhandler(unsigned int irq, void *userParam)
{ 
    static unsigned int irq_cnt = 0;  // 中断计数器（累计1000次=1秒）

    // 清除中断标志
    if (EPIT1->SR & (1 << 0))
    {
        EPIT1->SR |= 1 << 0;
    }

    // 门打开时累加计时
    if (door_get_state() == door_open)
    {
        door_open_time++;
        irq_cnt++;

        // 每1000次中断（1秒）打印一次计时
        if (irq_cnt >= 1000)           //测试代码用，正常项目需删掉
        {
            printf("定时器计时：door_open_time=%dms\n", door_open_time);
            irq_cnt = 0;
        }
    }
    else
    {
        irq_cnt = 0;  // 门关闭时，重置计数器
    }
}


