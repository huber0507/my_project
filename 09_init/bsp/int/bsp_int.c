#include "bsp_int.h"  // 引入中断驱动头文件，包含类型定义和函数声明

/*
 * 静态全局变量：中断嵌套计数器
 * 作用：记录当前中断嵌套的层数（如中断A中触发中断B，嵌套层数为2）
 * 用途：在中断处理中判断是否处于嵌套状态，用于临界区保护等场景
 */
static unsigned int irqNesting;

/*
 * 静态全局数组：中断处理表（软件层面的中断向量表）
 * 类型：sys_irq_handle_t结构体数组，每个元素对应一个中断号的处理信息
 * 大小：NUMBER_OF_INT_VECTORS（定义在MCIMX6Y2.h中，IMX6ULL的中断总数）
 * 作用：建立"中断号→处理函数→用户参数"的映射关系，实现动态中断管理
 */
static sys_irq_handle_t irqTable[NUMBER_OF_INT_VECTORS];

/*
 * @description	: 中断系统初始化（硬件+软件）
 * @param 		: 无
 * @return 		: 无
 * 核心流程：初始化GIC硬件→初始化中断表→设置异常向量表基地址
 */
void int_init(void)
{
    GIC_Init();  // 初始化GIC（通用中断控制器）：配置GIC的Distributor和CPU Interface，使能中断分发
    system_irqtable_init();  // 初始化软件中断表：将所有中断默认绑定到默认处理函数
    _set_VBAR((u32)0x87800000);  // 设置异常向量表基地址（VBAR：Vector Base Address Register）
                                 // 0x87800000是IMX6ULL中向量表的典型存放地址（与链接脚本对应）
}

/*
 * @description	: 初始化中断处理表（软件层面）
 * @param 		: 无
 * @return 		: 无
 * 核心逻辑：将中断表中所有中断项的处理函数初始化为默认处理函数，清零嵌套计数器
 */
void system_irqtable_init(void)
{
    unsigned int i = 0;
    irqNesting = 0;  // 初始化中断嵌套计数器为0（无中断嵌套）

    // 遍历所有中断向量，为每个中断号注册默认处理函数
    for(i = 0; i < NUMBER_OF_INT_VECTORS; i++)
    {
        // 第i个中断号绑定默认处理函数default_irqhandler，无用户参数（NULL）
        system_register_irqhandler((IRQn_Type)i, default_irqhandler, NULL);
    }
}

/*
 * @description	: 注册中断处理函数（将自定义函数绑定到指定中断号）
 * @param - irq	: 中断号（IRQn_Type枚举，如GPIO1_IRQn，对应硬件中断源）
 * @param - handler: 自定义中断处理函数（需符合system_irq_handler_t类型）
 * @param - userParam: 传递给中断处理函数的用户参数（可为NULL）
 * @return 		: 无
 * 核心逻辑：更新中断表中对应中断号的处理函数和用户参数
 */
void system_register_irqhandler(IRQn_Type irq, system_irq_handler_t handler, void *userParam)
{
    // 中断号作为数组索引，更新中断表中该位置的处理函数和用户参数
    irqTable[irq].irqHandler = handler;    // 绑定处理函数
    irqTable[irq].userParam = userParam;  // 绑定用户参数
}

/*
 * @description	: 系统中断处理总入口（所有中断的公共处理函数）
 * @param - giccIar: GICC_IAR寄存器的值（包含当前触发的中断ID）
 * @return 		: 无
 * 核心逻辑：解析中断号→检查有效性→调用对应处理函数→管理中断嵌套
 * 注：该函数需在汇编代码的中断向量表中注册为IRQ异常的入口（如start.S中的IRQ向量）
 */
void system_irqhandler(unsigned int giccIar)
{
    u32 intNum = giccIar & 0x3FFUL;  // 提取中断号：GICC_IAR寄存器低10位为中断ID（0~1023）

    // 中断号有效性检查：
    // 1020是特殊中断号（SPI中断超出范围），NUMBER_OF_INT_VECTORS是芯片支持的最大中断号
    if((intNum == 1020) || (intNum >= NUMBER_OF_INT_VECTORS))
    {
        return;  // 无效中断，直接返回
    }

    irqNesting++;  // 中断嵌套计数+1（进入中断）

    // 调用中断表中对应中断号的处理函数，传入中断号和用户参数
    irqTable[intNum].irqHandler(intNum, irqTable[intNum].userParam);

    irqNesting--;  // 中断嵌套计数-1（退出中断）
}

/*
 * @description	: 默认中断处理函数（未注册自定义函数时调用）
 * @param - giccIar: 中断号（当前触发的中断ID）
 * @param - userParam: 用户参数（默认处理中未使用，为兼容接口保留）
 * @return 		: 无
 * 核心逻辑：死循环（防止未处理的中断导致系统异常）
 * 注：实际应用中可添加日志打印（如"未处理的中断：xxx"）方便调试
 */
void default_irqhandler(unsigned int giccIar, void *userParam)
{
    while(1)  // 死循环：捕获未处理的中断，避免系统失控（实际调试时可在此处定位问题）
    {
        
    }
}
    