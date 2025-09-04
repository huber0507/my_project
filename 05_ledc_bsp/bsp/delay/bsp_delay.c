#include "bsp_delay.h"
#include "bsp_clk.h"  // 依赖时钟使能函数

// GPT1定时器寄存器基地址（IMX6ULL的GPT1外设）
#define GPT1_BASE_ADDR    0x02098000
#define GPT1              ((GPT_Type *)GPT1_BASE_ADDR)

// GPT定时器初始化（用于延时功能，需在main中先调用）
void delay_init(void)
{
    // 1. 确保GPT1时钟已使能（由clk_system_init统一管理）
    //    （若未使能，此处可手动调用：clk_periph_enable(CLK_GPT1);）

    // 2. 复位GPT1定时器（先复位，再清除复位状态）
    GPT1->CR = 1 << 15;  // 置位SWR位，软件复位
    while ((GPT1->CR & (1 << 15)));  // 等待复位完成

    // 3. 配置GPT1工作模式
    GPT1->CR = 0;  // 清除配置
    // bit6=1: 自由运行模式（计数器溢出后从0重新计数）
    // bit1=1: 使能时钟（使用IPG_CLK作为时钟源，默认66MHz）
    GPT1->CR |= (1 << 6) | (1 << 1);

    // 4. 设置分频系数：66MHz / (65 + 1) = 1MHz（1us计数1次）
    GPT1->PR = 65;  // 分频值=65，实际分频系数=65+1=66

    // 5. 启动定时器计数
    GPT1->CR |= 1 << 0;  // 置位EN位，启动计数
}

// 微秒级延时（精度±1us，最大延时约4294秒，满足绝大多数场景）
void delay_us(uint32_t us)
{
    uint32_t start, target;

    // 记录当前计数值（起始点）
    start = GPT1->CNT;

    // 计算目标计数值（1MHz时钟，1us对应1个计数）
    target = start + us;

    // 处理计数器溢出情况（自由运行模式下CNT会从0xFFFFFFFF回0）
    if (target < start)
    {
        // 先等待计数器溢出到0xFFFFFFFF
        while (GPT1->CNT < 0xFFFFFFFF);
    }
    // 等待计数达到目标值
    while (GPT1->CNT < target);
}

// 毫秒级延时（基于微秒延时实现，精度±1ms）
void delay_ms(uint32_t ms)
{
    // 循环调用微秒延时（1ms = 1000us）
    while (ms--)
    {
        delay_us(1000);
    }
}

// 兼容旧接口（可选，用于替换原delay函数）
void delay(volatile unsigned int n)
{
    delay_ms(n);  // 直接映射为毫秒延时，保持调用方式不变
}
    