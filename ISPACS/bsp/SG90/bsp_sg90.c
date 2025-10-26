#include "bsp_sg90.h"
#include <stdio.h>

// 私有函数声明（保持不变）
static void SG90_PWM_Config(uint32_t pwm_base, uint32_t pwm_clk_mask,
                           uint32_t mux_addr, uint8_t mux_mode,
                           uint32_t config_addr);
static void SG90_Angle_ADC_Config(void);

static uint8_t g_current_angle = 0;

// 其他函数（SG90_Init、SG90_Init2）保持不变...

/**
 * @brief  PWM定时器配置（核心修改IOMUXC_SetPinMux参数为3个）
 */
static void SG90_PWM_Config(uint32_t pwm_base, uint32_t pwm_clk_mask,
                           uint32_t mux_addr, uint8_t mux_mode,
                           uint32_t config_addr)
{
    volatile uint32_t *CCM_CCGR1 = (volatile uint32_t *)(CCM_BASE + 0x006C);
    volatile uint32_t *CCM_CCGR6 = (volatile uint32_t *)(CCM_BASE + 0x0078);
    
    *CCM_CCGR1 |= CCM_CCGR1_GPIO1_MASK;
    *CCM_CCGR6 |= pwm_clk_mask;
    
    // 核心修改：改为3个参数（匹配函数声明）
    IOMUXC_SetPinMux(mux_addr, mux_mode, 0,0);  // 3参数：muxRegister, muxMode, inputRegister
    
    // IOMUXC_SetPinConfig保持6参数（标准格式，若报错再调整）
    IOMUXC_SetPinConfig(mux_addr, mux_mode, 0, 0, config_addr, PIN_CONFIG_VAL);
    
    // 后续PWM寄存器配置（保持不变）...
    volatile uint32_t *PWM_CR = (volatile uint32_t *)(pwm_base + PWM_CR_OFFSET);
    volatile uint32_t *PWM_PR = (volatile uint32_t *)(pwm_base + PWM_PR_OFFSET);
    volatile uint32_t *PWM_PER = (volatile uint32_t *)(pwm_base + PWM_PER_OFFSET);
    volatile uint32_t *PWM_DTY = (volatile uint32_t *)(pwm_base + PWM_DTY_OFFSET);
    
    *PWM_CR &= ~(PWM_CR_EN_BIT | PWM_CR_SWR_BIT);
    *PWM_CR |= PWM_CR_SWR_BIT;
    while ((*PWM_CR & PWM_CR_SWR_BIT));
    
    *PWM_CR &= ~(3 << 2);
    *PWM_CR |= PWM_CR_CLKSRC_IPG;
    
    uint32_t ipg_clk = 66000000;
    uint32_t prescaler = 0;
    uint32_t period = (ipg_clk / (prescaler + 1) / 50) - 1;
    
    *PWM_PR = prescaler;
    *PWM_PER = period;
    *PWM_DTY = 0;
    
    *PWM_CR |= PWM_CR_EN_BIT;
    *PWM_CR |= PWM_CR_OUTEN_BIT;
}

/**
 * @brief  ADC配置（同样修改IOMUXC_SetPinMux为3个参数）
 */
static void SG90_Angle_ADC_Config(void)
{
    volatile uint32_t *CCM_CCGR5 = (volatile uint32_t *)(CCM_BASE + 0x0074);
    volatile uint32_t *CCM_CCGR1 = (volatile uint32_t *)(CCM_BASE + 0x006C);
    
    *CCM_CCGR5 |= CCM_CCGR5_ADC_MASK;
    *CCM_CCGR1 |= CCM_CCGR1_GPIO1_MASK;
    
    // 核心修改：改为3个参数
    IOMUXC_SetPinMux(GPIO1_IO00_MUX_ADDR, MUX_MODE_ADC, 0,0);
    
    IOMUXC_SetPinConfig(GPIO1_IO00_MUX_ADDR, MUX_MODE_ADC, 0, 0,
                      GPIO1_IO00_CONFIG_ADDR, PIN_CONFIG_VAL);
    
    // 后续ADC寄存器配置（保持不变）...
    volatile uint32_t *ADC_GC = (volatile uint32_t *)(ADC1_BASE + ADC_GC_OFFSET);
    volatile uint32_t *ADC_CFG = (volatile uint32_t *)(ADC1_BASE + ADC_CFG_OFFSET);
    
    *ADC_GC &= ~ADC_GC_ADEN_BIT;
    *ADC_CFG |= ADC_CFG_CLK_DIV4;
    *ADC_GC |= ADC_GC_ADEN_BIT;
    
    *ADC_GC |= ADC_GC_CAL_BIT;
    while ((*ADC_GC & ADC_GC_CAL_BIT));
}

// 其他函数（SG90_SetAngle、SG90_GetCurrentAngle等）保持不变...