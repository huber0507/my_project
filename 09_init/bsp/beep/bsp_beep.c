#include "bsp_beep.h"  // 引入蜂鸣器驱动头文件，包含函数声明、宏定义（如ON/OFF）及寄存器依赖

/*
 * @description	: 蜂鸣器初始化函数
 * @param 		: 无
 * @return 		: 无
 * 核心流程：I.MX6ULL 引脚需经过"复用配置→属性配置→GPIO功能初始化"三步才能驱动外设，
 *          此处蜂鸣器连接 GPIO5_IO01 引脚，按硬件初始化规范完成配置。
 */
void beep_init(void)
{
	/* 1、初始化IO复用：将引脚功能从默认（防拆检测）切换为GPIO功能
	 * IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01：引脚复用选择宏（厂商定义）
	 *   - 原始功能：SNVS_TAMPER1（防拆检测引脚）
	 *   - 复用功能：GPIO5_IO01（第5组GPIO的第1个引脚）
	 * 第二个参数 0：默认配置（无特殊复用选项，如中断、时钟等）
	 */
	IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0);	


	/* 2、配置GPIO5_IO01的IO属性（电气特性）
	 * 配置值 0X10B0 对应寄存器位定义（按位解析，参考I.MX6ULL IO属性寄存器说明）：
	 * bit 16: 0 → HYS（迟滞比较器）关闭（无需电平缓冲，简化配置）
	 * bit [15:14]: 00 → 下拉模式（默认拉低，避免引脚悬空产生杂波）
	 * bit 13: 0 → Keeper（保持器）功能（引脚无驱动时保持当前电平）
	 * bit 12: 1 → Pull/Keeper（拉/保持）使能（启用下拉功能）
	 * bit 11: 0 → 关闭开路输出（推挽输出模式，可输出高低电平驱动蜂鸣器）
	 * bit [7:6]: 10 → 速度等级100MHz（满足蜂鸣器开关速度需求，无需最高速）
	 * bit [5:3]: 110 → 驱动能力R0/6（中等驱动，避免电流过大损坏引脚）
	 * bit 0: 0 → 低转换率（减少高频干扰，蜂鸣器无需快速电平切换）
	 */
	IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01, 0X10B0);


	/* 3、初始化GPIO为输出模式
	 * GPIO5：指向第5组GPIO的寄存器基地址（来自imx6ul.h）
	 * GDIR（GPIO Direction Register）：方向寄存器，bit[n]控制GPIOx_IO[n]的方向
	 *   - 0：输入模式；1：输出模式
	 * (1 << 1)：将bit1置1，配置GPIO5_IO01为输出（bit0对应IO0，bit1对应IO1）
	 */
	GPIO5->GDIR |= (1 << 1);	 

	/* 4、设置初始电平：输出高电平（关闭蜂鸣器，需结合硬件电路判断）
	 * DR（GPIO Data Register）：数据寄存器，bit[n]控制GPIOx_IO[n]的输出电平
	 *   - 0：输出低电平；1：输出高电平
	 * 注意：代码注释写"打开LED0"是笔误，实际控制蜂鸣器；
	 *       初始高电平是否关闭蜂鸣器，取决于硬件（有源蜂鸣器通常低电平触发，故高电平关闭）
	 */
	GPIO5->DR |= (1 << 1);		
}


/*
 * @description	: 蜂鸣器开关控制函数
 * @param 		: status - 控制状态（ON：打开蜂鸣器；OFF：关闭蜂鸣器，宏定义在bsp_beep.h中）
 * @return 		: 无
 * 核心逻辑：通过修改GPIO数据寄存器（DR）的对应位，切换输出电平以控制蜂鸣器
 */
void beep_switch(int status)
{		
        if(status == ON)
            GPIO5->DR &= ~(1 << 1);	/* 打开蜂鸣器：清除bit1（置0），输出低电平（对应硬件触发逻辑） */
        else if(status == OFF)
            GPIO5->DR |= (1 << 1);	/* 关闭蜂鸣器：置位bit1（置1），输出高电平 */
}