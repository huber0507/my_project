#include "bsp_sg90.h"

// 私有函数声明
static void SG90_PWM_Config(PWM_Type *pwmx, uint32_t pwm_clk_mask, \
                           GPIO_Type *gpio_port, uint32_t gpio_clk_mask, \
                           uint32_t pin, uint32_t mux);
static void SG90_Angle_ADC_Config(void);

// 全局变量：存储当前角度（0~180°）
static uint8_t g_current_angle = 0;

/**
 * @brief  初始化默认舵机（PWM1，GPIO1_IO08）
 */
void SG90_Init(void)
{
    // 1. 配置PWM1（控制舵机转动）
    SG90_PWM_Config(SG90_PWM1_TIMx, SG90_PWM1_CLK, \
                   SG90_PWM1_GPIO_PORT, SG90_PWM1_GPIO_CLK, \
                   SG90_PWM1_PIN, SG90_PWM1_MUX);
    
    // 2. 配置角度反馈ADC（需硬件改造舵机）
    SG90_Angle_ADC_Config();
    
    // 3. 初始角度设为0°（门关闭状态）
    SG90_SetAngle(SG90_PWM1_TIMx, 0);
}

/**
 * @brief  初始化第二个舵机（PWM2，GPIO1_IO09）
 */
void SG90_Init2(void)
{
    // 配置PWM2
    SG90_PWM_Config(SG90_PWM2_TIMx, SG90_PWM2_CLK, \
                   SG90_PWM2_GPIO_PORT, SG90_PWM2_GPIO_CLK, \
                   SG90_PWM2_PIN, SG90_PWM2_MUX);
    
    // 初始角度设为0°
    SG90_SetAngle(SG90_PWM2_TIMx, 0);
}

/**
 * @brief  配置PWM定时器（通用函数，支持PWM1/PWM2）
 * @param  pwmx：PWM定时器（PWM1或PWM2）
 * @param  pwm_clk_mask：PWM时钟掩码（如CCM_CCGR6_PWM1_MASK）
 * @param  gpio_port：GPIO端口（如GPIO1）
 * @param  gpio_clk_mask：GPIO时钟掩码（如CCM_CCGR1_GPIO1_MASK）
 * @param  pin：GPIO引脚（如GPIO_PIN_8）
 * @param  mux：复用功能（如IOMUXC_GPIO1_IO08_PWM1_OUT）
 */
static void SG90_PWM_Config(PWM_Type *pwmx, uint32_t pwm_clk_mask, \
                           GPIO_Type *gpio_port, uint32_t gpio_clk_mask, \
                           uint32_t pin, uint32_t mux)
{
    // 1. 使能PWM和GPIO时钟（IMX6ULL通过CCM模块控制时钟）
    CCM->CCGR1 |= gpio_clk_mask;  // 使能GPIO时钟
    CCM->CCGR6 |= pwm_clk_mask;   // 使能PWM时钟（PWM1/PWM2在CCGR6）
    
    // 2. 配置GPIO引脚为PWM复用功能（IMX6ULL通过IOMUXC配置复用）
    IOMUXC_SetPinMux(mux, 0);  // 复用为PWM输出
    // 配置引脚电气属性：上拉、驱动能力等级（0x10B0：默认配置）
    IOMUXC_SetPinConfig(mux, 0x10B0);
    
    // 3. 复位PWM定时器
    pwmx->PWMCR &= ~(PWM_PWMCR_EN_MASK | PWM_PWMCR_SWR_MASK);
    pwmx->PWMCR |= PWM_PWMCR_SWR_MASK;  // 软件复位
    while ((pwmx->PWMCR & PWM_PWMCR_SWR_MASK));  // 等待复位完成
    
    // 4. 配置PWM时钟源（选择IPG_CLK，66MHz）
    pwmx->PWMCR &= ~PWM_PWMCR_CLKSRC_MASK;
    pwmx->PWMCR |= PWM_PWMCR_CLKSRC(1);  // 01=IPG_CLK（66MHz）
    
    // 5. 配置PWM周期（20ms，50Hz，适合SG90）
    // 公式：周期 = (PERIOD + 1) * (PRESCALER + 1) / IPG_CLK
    uint32_t ipg_clk = 66000000;  // IPG_CLK频率：66MHz
    uint32_t prescaler = 0;       // 预分频：不分频
    uint32_t period = (ipg_clk / (prescaler + 1) / 50) - 1;  // 50Hz对应20ms
    pwmx->PWMPR = prescaler;      // 设置预分频
    pwmx->PWMPER = period;        // 设置周期（计数器上限）
    
    // 6. 初始占空比0（脉冲宽度0）
    pwmx->PWMDTY = 0;
    
    // 7. 使能PWM输出
    pwmx->PWMCR |= PWM_PWMCR_EN_MASK;    // 使能定时器
    pwmx->PWMCR |= PWM_PWMCR_OUTEN_MASK; // 使能输出
}

/**
 * @brief  配置角度反馈ADC（IMX6ULL的ADC配置）
 */
static void SG90_Angle_ADC_Config(void)
{
    // 1. 使能ADC和GPIO时钟
    CCM->CCGR5 |= SG90_ADC_CLK;          // 使能ADC时钟
    CCM->CCGR1 |= SG90_ADC_GPIO_CLK;     // 使能ADC引脚GPIO时钟
    
    // 2. 配置ADC输入引脚为模拟输入
    IOMUXC_SetPinMux(SG90_ADC_MUX, 0);   // 复用为ADC功能
    IOMUXC_SetPinConfig(SG90_ADC_MUX, 0x10B0);  // 模拟输入配置（无上下拉）
    
    // 3. 配置ADC工作模式（IMX6ULL的ADC是12位，单通道单次转换）
    SG90_ADCx->CTRL &= ~ADC_CTRL_ADC_EN_MASK;  // 关闭ADC
    SG90_ADCx->CTRL = ADC_CTRL_ADC_CLK_DIV(4); // ADC时钟分频（IPG_CLK/4=16.5MHz）
    SG90_ADCx->CTRL |= ADC_CTRL_ADC_EN_MASK;   // 使能ADC
    
    // 4. ADC校准（IMX6ULL的ADC校准步骤）
    SG90_ADCx->CTRL |= ADC_CTRL_CAL_MASK;      // 启动校准
    while (SG90_ADCx->CTRL & ADC_CTRL_CAL_MASK); // 等待校准完成
}

/**
 * @brief  设置SG90舵机角度（0~180°，支持PWM1/PWM2）
 * @param  pwmx：PWM定时器（PWM1或PWM2）
 * @param  angle：目标角度（0~180）
 */
void SG90_SetAngle(PWM_Type *pwmx, uint8_t angle)
{
    uint32_t period = pwmx->PWMPER;  // 获取周期值（已在初始化时配置为20ms）
    uint32_t dty;                    // 占空比计数器值
    
    // 1. 限制角度范围
    if (angle > 180) angle = 180;
    if (angle < 0) angle = 0;
    
    // 2. 角度→脉冲宽度转换（0.5ms~2.5ms对应0°~180°）
    // 脉冲宽度 = 0.5ms + (angle / 180°) * 2ms
    // 占空比计数器值 = (脉冲宽度 / 20ms) * (period + 1) - 1
    float pulse_width_ms = 0.5f + (angle / 180.0f) * 2.0f;  // 0.5~2.5ms
    dty = (uint32_t)((pulse_width_ms / 20.0f) * (period + 1) - 1);
    
    // 3. 设置PWM占空比（PWMDTY寄存器）
    pwmx->PWMDTY = dty;
    
    // 4. 记录当前角度
    g_current_angle = angle;
    
    // 5. 等待舵机转动到位
    delay_ms(200);
}

/**
 * @brief  读取舵机当前角度（需硬件改造，IMX6ULL适配版）
 * @retval 当前角度（0~180°）
 */
uint8_t SG90_GetCurrentAngle(void)
{
    uint16_t adc_value;
    uint8_t angle;
    
    // 1. 配置ADC通道并启动转换（IMX6ULL的ADC转换步骤）
    SG90_ADCx->CHSEL = SG90_ADC_CHANNEL;  // 选择ADC通道
    SG90_ADCx->CTRL |= ADC_CTRL_SW_TRIG_MASK;  // 软件触发转换
    
    // 2. 等待转换完成（EOC=1表示完成）
    while (!(SG90_ADCx->STAT & ADC_STAT_EOC_MASK));
    
    // 3. 读取ADC值（12位，范围0~4095）
    adc_value = SG90_ADCx->RESULT;
    
    // 4. ADC值→角度转换（需实测校准）
    #define ANGLE_0_ADC    300   // 0°时的ADC值（示例，需实测）
    #define ANGLE_180_ADC  3700  // 180°时的ADC值（示例，需实测）
    
    if (adc_value <= ANGLE_0_ADC) {
        angle = 0;
    } else if (adc_value >= ANGLE_180_ADC) {
        angle = 180;
    } else {
        angle = (uint8_t)((adc_value - ANGLE_0_ADC) * 180.0f / (ANGLE_180_ADC - ANGLE_0_ADC));
    }
    
    // 5. 更新全局角度变量
    g_current_angle = angle;
    return angle;
}

// 门控控制函数（复用原有逻辑，适配PWM1）
void SG90_DoorOpen(void)
{
    SG90_SetAngle(SG90_PWM1_TIMx, 90);  // 门打开角度（根据机械结构调整）
}

void SG90_DoorClose(void)
{
    SG90_SetAngle(SG90_PWM1_TIMx, 0);   // 门关闭角度
}

// 第二个舵机的门控函数（PWM2）
void SG90_DoorOpen2(void)
{
    SG90_SetAngle(SG90_PWM2_TIMx, 90);
}

void SG90_DoorClose2(void)
{
    SG90_SetAngle(SG90_PWM2_TIMx, 0);
}