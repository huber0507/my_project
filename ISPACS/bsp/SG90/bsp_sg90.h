#ifndef __BSP_SG90_H
#define __BSP_SG90_H

#include "imx6ul.h"  // 替换STM32头文件为IMX6ULL头文件
#include "bsp_delay.h"  // 需确保IMX6ULL有对应的延时函数

// -------------------------- 硬件配置（IMX6ULL，GPIO1_IO08和GPIO1_IO09） --------------------------
// PWM1（GPIO1_IO08）配置（控制第一个舵机）
#define SG90_PWM1_TIMx          PWM1
#define SG90_PWM1_CLK           CCM_CCGR6_PWM1_MASK  // PWM1时钟掩码
#define SG90_PWM1_GPIO_PORT     GPIO1
#define SG90_PWM1_GPIO_CLK      CCM_CCGR1_GPIO1_MASK // GPIO1时钟掩码
#define SG90_PWM1_PIN           GPIO_PIN_8           // GPIO1_IO08
#define SG90_PWM1_MUX           IOMUXC_GPIO1_IO08_PWM1_OUT  // 复用功能

// PWM2（GPIO1_IO09）配置（控制第二个舵机，可选）
#define SG90_PWM2_TIMx          PWM2
#define SG90_PWM2_CLK           CCM_CCGR6_PWM2_MASK  // PWM2时钟掩码
#define SG90_PWM2_GPIO_PORT     GPIO1
#define SG90_PWM2_GPIO_CLK      CCM_CCGR1_GPIO1_MASK
#define SG90_PWM2_PIN           GPIO_PIN_9           // GPIO1_IO09
#define SG90_PWM2_MUX           IOMUXC_GPIO1_IO09_PWM2_OUT  // 复用功能

// 角度反馈ADC配置（需硬件改造，示例：ADC1_CH0，根据实际引脚修改）
#define SG90_ADCx               ADC1
#define SG90_ADC_CLK            CCM_CCGR5_ADC_MASK   // ADC时钟掩码
#define SG90_ADC_CHANNEL        0                    // ADC通道号
#define SG90_ADC_GPIO_PORT      GPIO1
#define SG90_ADC_GPIO_CLK       CCM_CCGR1_GPIO1_MASK
#define SG90_ADC_PIN            GPIO_PIN_0           // 示例：GPIO1_IO00作为ADC输入
#define SG90_ADC_MUX            IOMUXC_GPIO1_IO00_ADC1_CH0  // ADC复用功能
// --------------------------------------------------------------------------------

// 函数声明
void SG90_Init(void);                                  // 初始化舵机（默认初始化PWM1）
void SG90_Init2(void);                                 // 初始化第二个舵机（PWM2）
void SG90_SetAngle(PWM_Type *pwmx, uint8_t angle);     // 设置指定舵机角度（0~180°）
uint8_t SG90_GetCurrentAngle(void);                    // 读取当前角度（需硬件改造）
void SG90_DoorOpen(void);                              // 控制门打开（PWM1，90°）
void SG90_DoorClose(void);                             // 控制门关闭（PWM1，0°）
void SG90_DoorOpen2(void);                             // 控制第二个门打开（PWM2，90°）
void SG90_DoorClose2(void);                            // 控制第二个门关闭（PWM2，0°）

#endif