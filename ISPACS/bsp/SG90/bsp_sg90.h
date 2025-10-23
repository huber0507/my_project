#ifndef __BSP_SG90_H
#define __BSP_SG90_H

#include "imx6ul.h"

// -------------------------- 基地址定义（条件编译避免重定义） --------------------------
#ifndef PWM1_BASE
#define PWM1_BASE                 0x02080000  // PWM1基地址
#endif

#ifndef PWM2_BASE
#define PWM2_BASE                 0x02080400  // PWM2基地址
#endif

#ifndef ADC1_BASE
#define ADC1_BASE                 0x02198000  // ADC1基地址
#endif

#ifndef CCM_BASE
#define CCM_BASE                  0x020C4000  // CCM基地址
#endif

#ifndef IOMUXC_BASE
#define IOMUXC_BASE               0x020E0000  // IOMUXC基地址
#endif

// -------------------------- 其他宏定义（保持不变） --------------------------
// CCM时钟掩码
#define CCM_CCGR1_GPIO1_MASK      (3 << 2)    // GPIO1时钟（bit2-3）
#define CCM_CCGR6_PWM1_MASK       (3 << 0)    // PWM1时钟（bit0-1）
#define CCM_CCGR6_PWM2_MASK       (3 << 2)    // PWM2时钟（bit2-3）
#define CCM_CCGR5_ADC_MASK        (3 << 12)   // ADC1时钟（bit12-13）

// GPIO复用/配置地址
#define GPIO1_IO08_MUX_ADDR       (IOMUXC_BASE + 0x0024)
#define GPIO1_IO08_CONFIG_ADDR    (IOMUXC_BASE + 0x026C)
#define GPIO1_IO09_MUX_ADDR       (IOMUXC_BASE + 0x0028)
#define GPIO1_IO09_CONFIG_ADDR    (IOMUXC_BASE + 0x0270)
#define GPIO1_IO00_MUX_ADDR       (IOMUXC_BASE + 0x0014)
#define GPIO1_IO00_CONFIG_ADDR    (IOMUXC_BASE + 0x0254)

// 复用模式
#define MUX_MODE_PWM              2           // PWM模式
#define MUX_MODE_ADC              1           // ADC模式

// PWM寄存器偏移
#define PWM_CR_OFFSET             0x00
#define PWM_PR_OFFSET             0x04
#define PWM_PER_OFFSET            0x08
#define PWM_DTY_OFFSET            0x0C

// PWM控制位
#define PWM_CR_EN_BIT             (1 << 0)
#define PWM_CR_SWR_BIT            (1 << 1)
#define PWM_CR_CLKSRC_IPG         (1 << 2)
#define PWM_CR_OUTEN_BIT          (1 << 3)

// ADC寄存器偏移
#define ADC_GC_OFFSET             0x00
#define ADC_CFG_OFFSET            0x04
#define ADC_CH_OFFSET             0x08
#define ADC_STAT_OFFSET           0x0C
#define ADC_RSLT_OFFSET           0x10

// ADC控制位
#define ADC_GC_ADEN_BIT           (1 << 0)
#define ADC_GC_CAL_BIT            (1 << 1)
#define ADC_GC_SWTRG_BIT          (1 << 2)
#define ADC_STAT_EOC_BIT          (1 << 0)
#define ADC_CFG_CLK_DIV4          (1 << 1)

// 引脚配置值
#define PIN_CONFIG_VAL            0x10B0

// 函数声明
void SG90_Init(void);
void SG90_Init2(void);
void SG90_SetAngle(uint32_t pwm_base, uint8_t angle);
uint8_t SG90_GetCurrentAngle(void);
void SG90_DoorOpen(void);
void SG90_DoorClose(void);
void delay_ms(uint32_t ms);

#endif