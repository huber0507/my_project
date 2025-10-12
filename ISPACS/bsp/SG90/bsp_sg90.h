#ifndef __BSP_SG90_H
#define __BSP_SG90_H

#include "stm32f10x.h"
#include "bsp_delay.h"  // 需包含延时函数头文件

// -------------------------- 硬件配置（根据实际接线修改） --------------------------
// PWM控制引脚配置（示例：TIM2_CH2，PA1）
#define SG90_TIMx               TIM2
#define SG90_TIM_CLK            RCC_APB1Periph_TIM2
#define SG90_TIM_PSC            71          // 预分频：72MHz/72=1MHz
#define SG90_TIM_ARR            19999       // 自动重装载值：1MHz×20ms=20000
#define SG90_TIM_OCxInit        TIM_OC2Init // 通道初始化函数
#define SG90_TIM_SetComparex    TIM_SetCompare2  // 通道比较值设置函数

#define SG90_GPIO_PORT          GPIOA
#define SG90_GPIO_CLK           RCC_APB2Periph_GPIOA
#define SG90_PWM_PIN            GPIO_Pin_1  // PWM输出引脚

// 角度反馈ADC配置（需硬件改造舵机，示例：ADC1_CH0，PA0）
#define SG90_ADCx               ADC1
#define SG90_ADC_CLK            RCC_APB2Periph_ADC1
#define SG90_ADC_CHANNEL        ADC_Channel_0
#define SG90_ADC_GPIO_PORT      GPIOA
#define SG90_ADC_GPIO_CLK       RCC_APB2Periph_GPIOA
#define SG90_ADC_PIN            GPIO_Pin_0  // ADC输入引脚（反馈信号）
// --------------------------------------------------------------------------------

// 函数声明
void SG90_Init(void);                      // 初始化舵机
void SG90_SetAngle(uint8_t angle);         // 设置舵机角度（0~180°）
uint8_t SG90_GetCurrentAngle(void);        // 读取当前角度（需硬件改造）
void SG90_DoorOpen(void);                  // 控制门打开
void SG90_DoorClose(void);                 // 控制门关闭
//增加SG90模块
#endif /* __BSP_SG90_H */
    