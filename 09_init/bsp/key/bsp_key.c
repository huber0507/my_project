#include "bsp_key.h"      // 引入按键驱动头文件，包含按键相关宏定义（如KEY0_VALUE）
#include "bsp_gpio.h"     // 引入GPIO驱动头文件，使用GPIO初始化和读取函数
#include "bsp_delay.h"    // 引入延时头文件，用于按键消抖

/*
 * @description	: 按键初始化函数
 * @param 		: 无
 * @return 		: 无
 * 核心流程：按键通过GPIO引脚读取，需完成"引脚复用→电气属性配置→GPIO输入模式初始化"三步
 * 硬件说明：此处按键连接到 UART1_CTS_B 引脚，复用为 GPIO1_IO18，低电平表示按下（上拉配置）
 */
void key_init(void)
{
    gpio_pin_config_t key_config;  // 定义GPIO配置结构体，用于存储按键引脚的配置信息

    /* 1、初始化IO复用：将UART1_CTS_B引脚功能切换为GPIO1_IO18
     * IOMUXC_UART1_CTS_B_GPIO1_IO18：引脚复用选择宏（厂商定义）
     *   - 原始功能：UART1的CTS（硬件流控）
     *   - 复用功能：GPIO1组的第18号引脚（GPIO1_IO18）
     * 第二个参数0：默认配置（无特殊复用选项，如中断模式等）
     */
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0);

    /* 2、配置GPIO1_IO18的电气属性（针对按键场景优化）
     * 配置值0xF080对应寄存器位解析（参考I.MX6ULL IO属性寄存器说明）：
     * bit 16: 0 → HYS（迟滞比较器）关闭（按键为数字信号，无需电平缓冲）
     * bit [15:14]: 11 → 22KΩ上拉（按键未按下时，引脚被上拉为高电平，稳定状态）
     * bit 13: 1 → 使能pull（上拉功能生效）
     * bit 12: 1 → 使能pull/keeper（确保上拉电阻有效）
     * bit 11: 0 → 关闭开路输出（按键为输入，无需输出驱动）
     * bit [7:6]: 10 → 速度100MHz（满足按键输入的响应速度需求）
     * bit [5:3]: 000 → 关闭输出驱动（输入模式下无需输出能力）
     * bit 0: 0 → 低转换率（减少高频干扰，按键状态变化缓慢）
     */
    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18, 0xF080);

    /* 3、初始化GPIO为输入模式（按键用于读取外部状态，需配置为输入）
     * key_config.direction = kGPIO_DigitalInput：设置方向为输入
     * gpio_init参数：GPIO1组，18号引脚，使用上面的配置
     */
    key_config.direction = kGPIO_DigitalInput;
    gpio_init(GPIO1, 18, &key_config);
}

/*
 * @description	: 获取按键值（含软件消抖）
 * @param 		: 无
 * @return 		: 按键值（0：无按键按下；KEY0_VALUE：KEY0按下，宏定义在bsp_key.h中）
 * 消抖原理：按键按下/松开时会有机械抖动（约10ms），通过延时后再次检测确认状态
 * 状态管理：使用静态变量release标记按键是否松开，避免一次按下被多次识别
 */
int key_getvalue(void)
{
    int ret = 0;  // 按键值返回变量，默认0（无按键按下）
    // 静态变量release：记录按键状态，1=松开，0=按下（静态变量可保持状态，跨函数调用有效）
    static unsigned char release = 1;  

    // 条件1：release==1（上一次已松开）；条件2：GPIO1_IO18读取为0（当前检测到按下）
    if ((release == 1) && (gpio_pinread(GPIO1, 18) == 0))   
    {
        delay(10);  // 延时10ms，等待机械抖动结束（消抖核心步骤）
        release = 0;  // 标记按键已按下，避免再次进入此分支

        // 再次检测，确认按键确实按下（排除抖动干扰）
        if (gpio_pinread(GPIO1, 18) == 0)
            ret = KEY0_VALUE;  // 返回KEY0对应的有效值（如1，定义在bsp_key.h中）
    }
    // 若当前引脚为高电平（按键未按下或已松开）
    else if (gpio_pinread(GPIO1, 18) == 1) 
    {
        ret = 0;  // 无按键按下，返回0
        release = 1;  // 标记按键已松开，允许下次检测按下
    }

    return ret;  // 返回最终按键值
}
    