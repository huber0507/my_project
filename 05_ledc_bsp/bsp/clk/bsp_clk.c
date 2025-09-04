#include "bsp_clk.h"

/*
 * 外设时钟使能标识（与你的硬件场景匹配）
 * 只定义当前用到的外设，未用到的不定义
 */
#define CLK_GPIO1  0  // LED用到的GPIO1
#define CLK_GPIO5  1  // 蜂鸣器用到的GPIO5
#define CLK_GPT1   2  // 延时函数用到的GPT1定时器

/*
 * @description	: 使能指定外设的时钟（按需使能，降低功耗）
 * @param - periph	: 外设标识（CLK_GPIO1/CLK_GPIO5/CLK_GPT1等）
 * @return 		: 无
 */
void clk_periph_enable(uint8_t periph)
{
    switch(periph)
    {
        case CLK_GPIO1:
            // GPIO1时钟控制位在CCGR1的bit26-27，设为0x3（始终使能）
            CCM->CCGR1 |= (0x3 << 26);
            break;
            
        case CLK_GPIO5:
            // GPIO5时钟控制位在CCGR2的bit10-11，设为0x3（始终使能）
            CCM->CCGR2 |= (0x3 << 10);
            break;
            
        case CLK_GPT1:
            // GPT1时钟控制位在CCGR1的bit20-21，设为0x3（始终使能）
            CCM->CCGR1 |= (0x3 << 20);
            break;
            
        default:
            // 不支持的外设，可添加错误处理（如打印日志）
            break;
    }
}

/*
 * @description	: 系统初始化时的基础时钟使能（只开启必要的核心外设）
 * @param 		: 无
 * @return 		: 无
 */
void clk_system_init(void)
{
    // 1. 使能当前场景必须的外设时钟（LED+蜂鸣器+延时）
    clk_periph_enable(CLK_GPIO1);   // 使能LED的GPIO1时钟
    clk_periph_enable(CLK_GPIO5);   // 使能蜂鸣器的GPIO5时钟
    clk_periph_enable(CLK_GPT1);    // 使能延时用的GPT1时钟
    
    // 2. 其他必要的基础时钟（如IPG总线时钟，默认已使能，无需重复操作）
    // 注：IMX6ULL复位后IPG时钟默认使能，若需修改频率可在此处配置
}
    