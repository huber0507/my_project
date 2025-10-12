#include "bsp_sg90.h"
#include "stm32f10x.h"

// 私有函数声明
static void SG90_TIM_Config(void);
static void SG90_Angle_ADC_Config(void);

// 全局变量：存储当前角度（0~180°）
static uint8_t g_current_angle = 0;

/**
 * @brief  SG90舵机初始化（含PWM控制和角度反馈ADC）
 * @param  无
 * @retval 无
 */
void SG90_Init(void)
{
    // 1. 配置PWM定时器（控制舵机转动）
    SG90_TIM_Config();
    
    // 2. 配置角度反馈ADC（需硬件改造舵机）
    SG90_Angle_ADC_Config();
    
    // 3. 初始角度设为0°（门关闭状态）
    SG90_SetAngle(0);
}

/**
 * @brief  配置PWM定时器（生成20ms周期的PWM信号）
 * @param  无
 * @retval 无
 */
static void SG90_TIM_Config(void)
{
    // 1. 使能时钟
    RCC_APB1PeriphClockCmd(SG90_TIM_CLK, ENABLE);    // 定时器时钟
    RCC_APB2PeriphClockCmd(SG90_GPIO_CLK, ENABLE);   // GPIO时钟
    
    // 2. 配置PWM输出引脚（复用推挽输出）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SG90_PWM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出（定时器控制）
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SG90_GPIO_PORT, &GPIO_InitStructure);
    
    // 3. 配置定时器时基（20ms周期）
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_Period = SG90_TIM_ARR;       // 自动重装载值：19999
    TIM_TimeBaseInitStructure.TIM_Prescaler = SG90_TIM_PSC;    // 预分频值：71
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(SG90_TIMx, &TIM_TimeBaseInitStructure);
    
    // 4. 配置PWM输出模式（PWM1）
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          // PWM模式1
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  // 高电平有效
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;                         // 初始脉冲宽度0
    SG90_TIM_OCxInit(SG90_TIMx, &TIM_OCInitStructure);         // 初始化对应通道
    
    // 5. 启动定时器
    TIM_Cmd(SG90_TIMx, ENABLE);
}

/**
 * @brief  配置角度反馈ADC（读取舵机内部电位器电压）
 * @param  无
 * @retval 无
 */
static void SG90_Angle_ADC_Config(void)
{
    // 1. 使能ADC时钟和GPIO时钟
    RCC_APB2PeriphClockCmd(SG90_ADC_CLK | SG90_ADC_GPIO_CLK, ENABLE);
    
    // 2. 配置ADC输入引脚（模拟输入模式）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = SG90_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;  // 模拟输入（无上下拉）
    GPIO_Init(SG90_ADC_GPIO_PORT, &GPIO_InitStructure);
    
    // 3. 配置ADC工作模式
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;          // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                // 非扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;          // 单次转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;       // 右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                      // 1个通道
    ADC_Init(SG90_ADCx, &ADC_InitStructure);
    
    // 4. 使能ADC
    ADC_Cmd(SG90_ADCx, ENABLE);
    
    // 5. ADC校准
    ADC_ResetCalibration(SG90_ADCx);
    while (ADC_GetResetCalibrationStatus(SG90_ADCx));
    ADC_StartCalibration(SG90_ADCx);
    while (ADC_GetCalibrationStatus(SG90_ADCx));
}

/**
 * @brief  设置SG90舵机角度（0~180°）
 * @param  angle: 目标角度（0~180）
 * @retval 无
 */
void SG90_SetAngle(uint8_t angle)
{
    uint16_t ccr_value;
    
    // 1. 限制角度范围（0~180°）
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;
    
    // 2. 角度→CCR值转换（0.5ms~2.5ms对应0°~180°）
    // 公式：CCR = 500 + (angle × 2000) / 180
    ccr_value = 500 + (uint32_t)angle * 2000 / 180;
    
    // 3. 设置PWM脉冲宽度（CCR寄存器）
    SG90_TIM_SetComparex(SG90_TIMx, ccr_value);
    
    // 4. 记录当前角度
    g_current_angle = angle;
    
    // 5. 等待舵机转动到位（约200ms，根据舵机性能调整）
    delay_ms(200);
}

/**
 * @brief  读取舵机当前角度（需硬件改造，提取内部电位器信号）
 * @param  无
 * @retval 当前角度（0~180°）
 */
uint8_t SG90_GetCurrentAngle(void)
{
    uint16_t adc_value;
    uint8_t angle;
    
    // 1. 启动ADC转换（软件触发）
    ADC_RegularChannelConfig(SG90_ADCx, SG90_ADC_CHANNEL, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(SG90_ADCx, ENABLE);
    
    // 2. 等待转换完成
    while (!ADC_GetFlagStatus(SG90_ADCx, ADC_FLAG_EOC));
    
    // 3. 读取ADC值（12位ADC，范围0~4095）
    adc_value = ADC_GetConversionValue(SG90_ADCx);
    
    // 4. ADC值→角度转换（需根据实际硬件校准）
    // 校准参数：ANGLE_0_ADC（0°时的ADC值）、ANGLE_180_ADC（180°时的ADC值）
    #define ANGLE_0_ADC    300   // 示例值，需实测校准
    #define ANGLE_180_ADC  3700  // 示例值，需实测校准
    
    if (adc_value <= ANGLE_0_ADC) {
        angle = 0;
    } else if (adc_value >= ANGLE_180_ADC) {
        angle = 180;
    } else {
        // 线性转换
        angle = (uint8_t)((adc_value - ANGLE_0_ADC) * 180.0f / (ANGLE_180_ADC - ANGLE_0_ADC));
    }
    
    // 5. 更新全局角度变量
    g_current_angle = angle;
    return angle;
}

/**
 * @brief  舵机控制门打开（固定角度，如90°）
 * @param  无
 * @retval 无
 */
void SG90_DoorOpen(void)
{
    SG90_SetAngle(90);  // 门打开角度（根据机械结构调整）
}

/**
 * @brief  舵机控制门关闭（固定角度，如0°）
 * @param  无
 * @retval 无
 */
void SG90_DoorClose(void)
{
    SG90_SetAngle(0);   // 门关闭角度（根据机械结构调整）
}
    