#include "bsp_led.h"
#include "bsp_clk.h"

static const LedConfig_t led_config[] = {
    {
        .gpio_group = GPIO1,
        .pin        = 3,
        .mux_reg    = IOMUXC_GPIO1_IO03_GPIO1_IO03,
        .pad_config = 0X10B0  // 确保此值与官方一致（如正点原子源码中的值）
    }
};

int led_init(uint8_t led)
{
    if (led >= ARRAY_SIZE(led_config)) {
        return -1;
    }

    const LedConfig_t *config = &led_config[led];

    // 使能GPIO1时钟（与时钟模块保持一致）
    clk_periph_enable(CLK_GPIO1);

    // 1. 配置引脚复用（严格匹配3个参数）
    IOMUXC_SetPinMux(
        config->mux_reg,  // 第1参数：复用寄存器地址
        0,                // 第2参数：复用模式（固定0）
        0                 // 第3参数：输入菊花链（输出模式设0）
    );

    // 2. 配置电气属性（严格匹配4个参数）
// 从“2个参数”改为“4个参数”，符合声明
IOMUXC_SetPinConfig(config->mux_reg, config->pad_config, 0, 0);

    // 3. 配置GPIO为输出模式
    config->gpio_group->GDIR |= (1 << config->pin);

    // 4. 初始状态：关闭LED
    led_set_status(led, LED_OFF);

    return 0;
}

// 其他函数（led_set_status、led_toggle）保持不变...
    