#include "bsp_sg90.h"
#include "bsp_delay.h"
#include <stdio.h>

// 私有函数声明
static void SG90_PWM_Config(uint32_t pwm_base, uint32_t pwm_clk_mask,
                           uint32_t mux_addr, uint8_t mux_mode,
                           uint32_t config_addr);
static void SG90_Angle_ADC_Config(void);

// 全局变量：当前角度
static uint8_t g_current_angle = 0;

// -------------------------- 公共函数实现（必须完整，解决链接错误） --------------------------
/**
 * @brief  初始化PWM1（GPIO1_IO08）舵机（main.c中调用）
 */
void SG90_Init(void)
{
    // 调用私有函数配置PWM1
    SG90_PWM_Config(PWM1_BASE, CCM_CCGR6_PWM1_MASK,
                   GPIO1_IO08_MUX_ADDR, MUX_MODE_PWM,
                   GPIO1_IO08_CONFIG_ADDR);
    
    // 配置角度反馈ADC
    SG90_Angle_ADC_Config();
    
    // 初始角度0°（关门）
    SG90_SetAngle(PWM1_BASE, 0);
}

/**
 * @brief  初始化PWM2（GPIO1_IO09）舵机（main.c中调用）
 */
void SG90_Init2(void)
{
    // 调用私有函数配置PWM2
    SG90_PWM_Config(PWM2_BASE, CCM_CCGR6_PWM2_MASK,
                   GPIO1_IO09_MUX_ADDR, MUX_MODE_PWM,
                   GPIO1_IO09_CONFIG_ADDR);
    
    // 初始角度0°
    SG90_SetAngle(PWM2_BASE, 0);
}

/**
 * @brief  设置舵机角度（0~180°）（被DoorOpen/DoorClose调用）
 */
void SG90_SetAngle(uint32_t pwm_base, uint8_t angle)
{
    // 1. 限制角度范围
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;
    
    // 2. 访问PWM周期/占空比寄存器
    volatile uint32_t *PWM_PER = (volatile uint32_t *)(pwm_base + PWM_PER_OFFSET);
    volatile uint32_t *PWM_DTY = (volatile uint32_t *)(pwm_base + PWM_DTY_OFFSET);
    uint32_t period = *PWM_PER;  // 获取周期值
    
    // 3. 角度→脉冲宽度转换（0.5ms~2.5ms对应0°~180°）
    float pulse_ms = 0.5f + (angle / 180.0f) * 2.0f;
    uint32_t duty = (uint32_t)((pulse_ms / 20.0f) * (period + 1) - 1);
    
    // 4. 设置占空比
    *PWM_DTY = duty;
    
    // 5. 记录角度并等待转动到位
    g_current_angle = angle;
    delay_ms(200);
}

/**
 * @brief  读取当前角度（被main.c调用，消除变量未使用警告）
 */
uint8_t SG90_GetCurrentAngle(void)
{
    // 访问ADC寄存器
    volatile uint32_t *ADC_CH = (volatile uint32_t *)(ADC1_BASE + ADC_CH_OFFSET);
    volatile uint32_t *ADC_GC = (volatile uint32_t *)(ADC1_BASE + ADC_GC_OFFSET);
    volatile uint32_t *ADC_STAT = (volatile uint32_t *)(ADC1_BASE + ADC_STAT_OFFSET);
    volatile uint32_t *ADC_RSLT = (volatile uint32_t *)(ADC1_BASE + ADC_RSLT_OFFSET);
    uint16_t adc_val;
    uint8_t angle = 0;
    
    // 选择通道并启动转换
    *ADC_CH = 0;
    *ADC_GC |= ADC_GC_SWTRG_BIT;
    while (!(*ADC_STAT & ADC_STAT_EOC_BIT));
    
    // 读取ADC值（12位）
    adc_val = (uint16_t)(*ADC_RSLT & 0xFFF);
    
    // ADC→角度校准（需根据硬件实测调整）
    #define ANGLE_0_ADC    300
    #define ANGLE_180_ADC  3700
    if (adc_val <= ANGLE_0_ADC) angle = 0;
    else if (adc_val >= ANGLE_180_ADC) angle = 180;
    else angle = (uint8_t)((adc_val - ANGLE_0_ADC) * 180.0f / (ANGLE_180_ADC - ANGLE_0_ADC));
    
    g_current_angle = angle;
    return angle;
}

/**
 * @brief  开门函数（main.c/bsp_key.c中调用）
 */
void SG90_DoorOpen(void)
{
    // 调用SetAngle设置开门角度（90°，可根据机械结构调整）
    SG90_SetAngle(PWM1_BASE, 90);
    printf("舵机已开门，当前角度：%d°\n", g_current_angle);
}

/**
 * @brief  关门函数（main.c/bsp_key.c中调用）
 */
void SG90_DoorClose(void)
{
    // 调用SetAngle设置关门角度（0°）
    SG90_SetAngle(PWM1_BASE, 0);
    printf("舵机已关门，当前角度：%d°\n", g_current_angle);
}

// -------------------------- 私有函数实现（已正确修改6参数调用） --------------------------
/**
 * @brief  PWM定时器配置（6参数调用IOMUXC_SetPinMux）
 */
static void SG90_PWM_Config(uint32_t pwm_base, uint32_t pwm_clk_mask,
                           uint32_t mux_addr, uint8_t mux_mode,
                           uint32_t config_addr)
{
    volatile uint32_t *CCM_CCGR1 = (volatile uint32_t *)(CCM_BASE + 0x006C);
    volatile uint32_t *CCM_CCGR6 = (volatile uint32_t *)(CCM_BASE + 0x0078);
    
    // 使能GPIO1和对应PWM时钟
    *CCM_CCGR1 |= CCM_CCGR1_GPIO1_MASK;
    *CCM_CCGR6 |= pwm_clk_mask;
    
    // 6参数调用IOMUXC_SetPinMux（匹配头文件声明，无用参数填0）
    IOMUXC_SetPinMux(mux_addr, mux_mode, 0, 0, config_addr, 0);
    
    // 配置引脚电气属性（标准6参数）
    IOMUXC_SetPinConfig(mux_addr, mux_mode, 0, 0, config_addr, PIN_CONFIG_VAL);
    
    // 配置PWM寄存器
    volatile uint32_t *PWM_CR = (volatile uint32_t *)(pwm_base + PWM_CR_OFFSET);
    volatile uint32_t *PWM_PR = (volatile uint32_t *)(pwm_base + PWM_PR_OFFSET);
    volatile uint32_t *PWM_PER = (volatile uint32_t *)(pwm_base + PWM_PER_OFFSET);
    volatile uint32_t *PWM_DTY = (volatile uint32_t *)(pwm_base + PWM_DTY_OFFSET);
    
    // 软件复位PWM
    *PWM_CR &= ~(PWM_CR_EN_BIT | PWM_CR_SWR_BIT);
    *PWM_CR |= PWM_CR_SWR_BIT;
    while ((*PWM_CR & PWM_CR_SWR_BIT));
    
    // 配置时钟源为IPG_CLK（66MHz）
    *PWM_CR &= ~(3 << 2);
    *PWM_CR |= PWM_CR_CLKSRC_IPG;
    
    // 计算20ms周期（50Hz）参数
    uint32_t ipg_clk = 66000000;
    uint32_t prescaler = 0;
    uint32_t period = (ipg_clk / (prescaler + 1) / 50) - 1;
    
    // 配置预分频、周期、占空比
    *PWM_PR = prescaler;
    *PWM_PER = period;
    *PWM_DTY = 0;
    
    // 使能PWM定时器和输出
    *PWM_CR |= PWM_CR_EN_BIT;
    *PWM_CR |= PWM_CR_OUTEN_BIT;
}

/**
 * @brief  ADC配置（6参数调用IOMUXC_SetPinMux）
 */
static void SG90_Angle_ADC_Config(void)
{
    volatile uint32_t *CCM_CCGR5 = (volatile uint32_t *)(CCM_BASE + 0x0074);
    volatile uint32_t *CCM_CCGR1 = (volatile uint32_t *)(CCM_BASE + 0x006C);
    
    // 使能ADC1和GPIO1时钟
    *CCM_CCGR5 |= CCM_CCGR5_ADC_MASK;
    *CCM_CCGR1 |= CCM_CCGR1_GPIO1_MASK;
    
    // 6参数调用IOMUXC_SetPinMux（匹配头文件声明）
    IOMUXC_SetPinMux(GPIO1_IO00_MUX_ADDR, MUX_MODE_ADC, 0, 0, GPIO1_IO00_CONFIG_ADDR, 0);
    
    // 配置ADC引脚电气属性
    IOMUXC_SetPinConfig(GPIO1_IO00_MUX_ADDR, MUX_MODE_ADC, 0, 0,
                      GPIO1_IO00_CONFIG_ADDR, PIN_CONFIG_VAL);
    
    // 配置ADC
    volatile uint32_t *ADC_GC = (volatile uint32_t *)(ADC1_BASE + ADC_GC_OFFSET);
    volatile uint32_t *ADC_CFG = (volatile uint32_t *)(ADC1_BASE + ADC_CFG_OFFSET);
    
    // 关闭ADC→配置时钟→使能ADC
    *ADC_GC &= ~ADC_GC_ADEN_BIT;
    *ADC_CFG |= ADC_CFG_CLK_DIV4;  // 分频4，16.5MHz
    *ADC_GC |= ADC_GC_ADEN_BIT;
    
    // ADC校准
    *ADC_GC |= ADC_GC_CAL_BIT;
    while ((*ADC_GC & ADC_GC_CAL_BIT));
}

// -------------------------- 延时函数实现（若bsp_delay.h未提供） --------------------------
