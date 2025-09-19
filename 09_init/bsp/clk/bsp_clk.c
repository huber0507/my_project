#include "bsp_clk.h"  // 引入时钟控制相关宏定义与寄存器结构体声明
                      // 内含 CCM、CCM_ANALOG 等模块的基地址及位定义


/*
 * @description	: 使能I.MX6U所有外设时钟
 * @param 		: 无
 * @return 		: 无
 * 核心原理：I.MX6ULL 通过 CCM（Clock Controller Module，时钟控制模块）的 CCGR（Clock Gating Register）
 *          实现外设时钟的"门控控制"——每2位控制一个外设的时钟状态，避免未使用外设浪费功耗。
 *          裸机开发中为简化初始化，采用"全使能"策略（0xFFFFFFFF 即所有位为 11），确保任何外设均可正常驱动。
 */
void clk_enable(void)
{
	CCM->CCGR0 = 0XFFFFFFFF;  // 使能 CCGR0 管控的外设（如 GPIO1、I2C1 等低速外设）
	CCM->CCGR1 = 0XFFFFFFFF;  // 使能 CCGR1 管控的外设（如 UART1、SPI1 等通信外设）
	CCM->CCGR2 = 0XFFFFFFFF;  // 使能 CCGR2 管控的外设（如 USB OTG、Ethernet 等高速外设）
	CCM->CCGR3 = 0XFFFFFFFF;  // 使能 CCGR3 管控的外设（如 PWM1、ADC 等功能外设）
	CCM->CCGR4 = 0XFFFFFFFF;  // 使能 CCGR4 管控的外设（如 GPIO2~GPIO5 等扩展 GPIO）
	CCM->CCGR5 = 0XFFFFFFFF;  // 使能 CCGR5 管控的外设（如 SDIO、CSI 摄像头接口）
	CCM->CCGR6 = 0XFFFFFFFF;  // 使能 CCGR6 管控的外设（如 CAN、FlexCAN 等总线外设）
}

/*
 * @description	: I.MX6U 系统时钟初始化（核心时钟、总线时钟配置）
 * @param 		: 无
 * @return 		: 无
 * 时钟架构：I.MX6ULL 时钟分"底层晶振→中层PLL→高层总线"三级：
 *          1. 底层：24MHz 外部晶振（备用时钟，稳定但频率低）
 *          2. 中层：PLL 锁相环（倍频为高频时钟，如 PLL1=800MHz、PLL2=600MHz）
 *          3. 高层：AHB/APB 总线（分频 PLL 时钟，供外设使用）
 * 本函数完成"时钟源切换→总线分频→时钟稳定等待"全流程。
 */
void imx6u_clkinit(void)
{
	unsigned int reg = 0;  // 临时寄存器变量，用于中间配置存储

	/* 第一步：判断并切换系统时钟源为 PLL1（主锁相环时钟）
	 * 背景：上电默认使用 24MHz 晶振，需切换到 PLL1 以提升 CPU 性能
	 */
	// CCM->CCSR（Clock Control Status Register）：时钟控制状态寄存器
	// bit2（PLL1_CLK_SEL_STATUS）：0=当前用晶振，1=当前用 PLL1
	if ((((CCM->CCSR) >> 2) & 0x1) == 0)  
	{
		// CCM->CCSR bit8（BYPass_CLK_SEL）：备用时钟源选择，0=24MHz 晶振（唯一选项）
		CCM->CCSR &= ~(1 << 8);  
		// CCM->CCSR bit2（PLL1_CLK_SEL）：1=切换系统时钟源为 PLL1（800MHz 主时钟）
		CCM->CCSR |= (1 << 2);  
	}

	/* 第二步：预留 PLL480 的 PFD 时钟配置
	 * 背景：PLL480 是专用 480MHz 锁相环，PFD（相位频率检测器）可生成高精度分频时钟
	 *      供 USB、HDMI 等高速外设使用，当前 reg=0 为默认值，需用时补充配置
	 */
	// CCM_ANALOG：模拟时钟模块（独立于数字 CCM，负责 PLL/PFD 等高频时钟）
	CCM_ANALOG->PFD_480 = reg;

	/* 第三步：配置 AHB 总线时钟（系统核心总线，连接 CPU 与高带宽外设）
	 */
	// CCM->CBCMR（Clock Bus Configuration Module Register）：总线源配置寄存器
	// bit18~19（AHB_CLK_SEL）：01=选择 PLL2/PLL3 为 AHB 源（PLL2 默认为 600MHz）
	CCM->CBCMR &= ~(3 << 18);  // 清除原有配置，避免干扰
	CCM->CBCMR |= (1 << 18);   // 配置 AHB 源为 PLL2
	// CCM->CBCDR（Clock Bus Configuration Divider Register）：总线分频寄存器
	// bit25（AHB_CLK_DIV_EN）：0=禁用分频（AHB 时钟=PLL2 时钟=600MHz）
	CCM->CBCDR &= ~(1 << 25);  
	// CCM->CDHIPR（Clock Domain Initialization Pending Register）：时钟稳定等待寄存器
	// bit25（AHB_CLK_PEND）：0=AHB 时钟稳定，1=未稳定；循环等待至稳定
	while (CCM->CDHIPR & (1 << 25));
	
	/* 第四步：（预留）配置 APB1 总线分频（低带宽外设总线，如 UART、I2C）
	 * 说明：APB1 最大支持 200MHz，此处配置为 AHB/3=200MHz，暂注释备用
	 */
	#if 0
	CCM->CBCDR &= ~(7 << 10);  // 清除 bit10~12（APB1_CLK_DIV）原有配置
	CCM->CBCDR |= 2 << 10;     // bit10~12=010，APB1 时钟=AHB 时钟/3=200MHz
	while(CCM->CDHIPR&(1<<1))	;  // 等待 APB1 时钟稳定（bit1=APB1_CLK_PEND）
	#endif

	/* 第五步：配置 APB2 总线与 IPG 外设时钟（高带宽外设总线，如 SPI、PWM）
	 */
	// CCM->CBCDR bit8~9（APB2_CLK_DIV）：01=APB2 时钟=AHB 时钟/2=300MHz（APB2 最大支持 300MHz）
	CCM->CBCDR &= ~(3 << 8);   // 清除原有分频配置
	CCM->CBCDR |= 1 << 8;      // 配置 APB2 为 2 分频
	// CCM->CSCMR1（Clock Source Control Module Register 1）：外设时钟源寄存器
	// bit6（IPG_CLK_SEL）：0=选择 AHB 为 IPG 源（IPG 是外设通用时钟）
	CCM->CSCMR1 &= ~(1 << 6);  
	// bit0~2（IPG_CLK_DIV）：000=IPG 时钟=AHB 时钟/1=600MHz
	CCM->CSCMR1 &= ~(7 << 0);
}