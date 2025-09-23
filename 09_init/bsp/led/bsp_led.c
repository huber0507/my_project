#include "bsp_led.h"  // 引入LED驱动头文件，包含宏定义（如LED0、ON、OFF）及函数声明

/*
 * @description	: 初始化LED对应的GPIO引脚
 * @param 		: 无
 * @return 		: 无
 * 核心流程：LED通过GPIO引脚控制，需完成"引脚复用→电气属性配置→GPIO输出模式初始化→初始电平设置"
 * 硬件说明：此处LED0连接到GPIO1_IO03，低电平点亮（引脚输出低电平时，LED导通）
 */
void led_init(void)
{
	/* 1、初始化IO复用：将引脚功能设置为GPIO（默认可能为其他功能，如外设接口）
	 * IOMUXC_GPIO1_IO03_GPIO1_IO03：引脚复用选择宏（厂商定义）
	 *   - 含义：将GPIO1_IO03引脚复用为其本身的GPIO功能（部分引脚可复用为多种功能）
	 * 第二个参数0：默认配置（无特殊复用选项，如中断、时钟等）
	 */
	IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0);	
	
	/* 2、配置GPIO1_IO03的电气属性（针对LED场景优化）
	 * 配置值0X10B0对应寄存器位解析（参考I.MX6ULL IO属性寄存器说明）：
	 * bit 16: 0 → HYS（迟滞比较器）关闭（LED为数字输出，无需电平缓冲）
	 * bit [15:14]: 00 → 默认下拉（未输出时引脚被拉低，避免悬空）
	 * bit 13: 0 → Keeper（保持器）功能（无驱动时保持当前电平）
	 * bit 12: 1 → Pull/Keeper（拉/保持）使能（启用下拉功能）
	 * bit 11: 0 → 关闭开路输出（推挽输出模式，可输出高低电平驱动LED）
	 * bit [7:6]: 10 → 速度100MHz（满足LED开关速度需求，无需最高速）
	 * bit [5:3]: 110 → 驱动能力R0/6（中等驱动，确保LED亮度适中）
	 * bit 0: 0 → 低转换率（减少高频干扰，LED无需快速电平切换）
	 */
	IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03, 0X10B0);
	
	/* 3、初始化GPIO为输出模式
	 * GPIO1：指向第1组GPIO的寄存器基地址（定义在imx6ul.h中）
	 * GDIR（GPIO Direction Register）：方向寄存器，bit[n]控制GPIOx_IO[n]的方向
	 *   - 0：输入模式；1：输出模式
	 * (1 << 3)：将bit3置1，配置GPIO1_IO03为输出（bit0对应IO0，bit3对应IO3）
	 */
	GPIO1->GDIR |= (1 << 3);	 
	
	/* 4、设置初始电平：输出低电平（打开LED0）
	 * DR（GPIO Data Register）：数据寄存器，bit[n]控制输出电平
	 *   - 0：低电平；1：高电平
	 * ~(1 << 3)：将bit3置0，其他位保持不变，确保仅操作目标引脚
	 */
	GPIO1->DR &= ~(1 << 3);		
	  GPIO1->DR |= (1 << 3);   // 置为高电平（熄灭）

}


/*
 * @description		: LED控制函数，控制LED打开或关闭
 * @param - led		: 要控制的LED编号（当前仅支持LED0，可扩展多LED）
 * @param - status	: 控制状态（ON：打开LED；OFF：关闭LED，宏定义在bsp_led.h中）
 * @return 			: 无
 * 核心逻辑：通过修改GPIO数据寄存器（DR）的对应位，切换输出电平以控制LED
 */
void led_switch(int led, int status)
{	
	switch(led)  // 支持多LED扩展，当前仅处理LED0
	{
		case LED0:  // 操作LED0（对应GPIO1_IO03）
			if(status == ON)  // 打开LED：输出低电平（LED导通）
				GPIO1->DR &= ~(1 << 3);	/* 清除bit3（置0），输出低电平 */
			else if(status == OFF)  // 关闭LED：输出高电平（LED截止）
				GPIO1->DR |= (1 << 3);	/* 置位bit3（置1），输出高电平 */
			break;  // 跳出switch语句
	}
}

void led_toggle(int led)
{
    switch(led)
    {
        case LED0:
            // 读取GPIO1_IO03当前电平（bit3的值）
            // 若当前为高电平（LED灭），则置为低电平（LED亮）
            // 若当前为低电平（LED亮），则置为高电平（LED灭）
            if((GPIO1->DR & (1 << 3)) != 0)  // 当前为高电平（LED0灭）
            {
                GPIO1->DR &= ~(1 << 3);  // 置为低电平（点亮）
            }
            else  // 当前为低电平（LED0亮）
            {
                GPIO1->DR |= (1 << 3);   // 置为高电平（熄灭）
            }
            break;
    }
}