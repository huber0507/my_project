#include "bsp_gpio.h"  // 引入GPIO驱动头文件，包含类型定义（如GPIO_Type、gpio_pin_config_t）和函数声明

/*
 * @description	: 初始化GPIO引脚
 * @param - base	: 指向GPIO组基地址的指针（如GPIO1、GPIO5等，定义在imx6ul.h中）
 * @param - pin	: 要初始化的引脚号（0~31，对应GPIOx_IO0~GPIOx_IO31）
 * @param - config: 指向gpio_pin_config_t结构体的指针，包含引脚配置信息（方向、默认输出电平）
 * @return 		: 无
 * 核心逻辑：根据配置结构体设置引脚方向（输入/输出），输出模式下同时设置初始电平
 */
void gpio_init(GPIO_Type *base, int pin, gpio_pin_config_t *config)
{
    if(config->direction == kGPIO_DigitalInput)  /* 配置为输入模式 */
    {
        // GDIR（GPIO Direction Register）：方向寄存器，bit[pin] = 0 表示输入
        base->GDIR &= ~(1 << pin);  // 清除bit[pin]（置0），设置为输入
    }
    else  /* 配置为输出模式 */
    {
        // GDIR bit[pin] = 1 表示输出
        base->GDIR |= 1 << pin;  // 置位bit[pin]（置1），设置为输出
        // 输出模式下，通过gpio_pinwrite设置默认电平（高/低）
        gpio_pinwrite(base, pin, config->outputLogic);
    }
}

/*
 * @description	: 读取GPIO引脚的电平状态
 * @param - base	: 指向GPIO组基地址的指针
 * @param - pin	: 要读取的引脚号
 * @return 		: 引脚电平（0：低电平；1：高电平）
 * 核心逻辑：通过读取DR寄存器的对应位获取引脚电平，输入/输出模式均可读取
 */
int gpio_pinread(GPIO_Type *base, int pin)
{
    // DR（GPIO Data Register）：数据寄存器，bit[pin]表示引脚电平（0低1高）
    // 右移pin位后与0x1相与，提取出bit[pin]的值（0或1）
    return (((base->DR) >> pin) & 0x1);
}

/*
 * @description	: 设置GPIO引脚的输出电平
 * @param - base	: 指向GPIO组基地址的指针
 * @param - pin	: 要设置的引脚号
 * @param - value: 输出电平（0：低电平；非0：高电平）
 * @return 		: 无
 * 核心逻辑：通过修改DR寄存器的对应位设置输出电平，仅对输出模式的引脚有效
 */
void gpio_pinwrite(GPIO_Type *base, int pin, int value)
{
    if (value == 0U)  // 输出低电平
    {
        // 清除DR寄存器的bit[pin]（置0），1U确保运算为无符号类型，避免符号扩展
        base->DR &= ~(1U << pin);
    }
    else  // 输出高电平
    {
        // 置位DR寄存器的bit[pin]（置1）
        base->DR |= (1U << pin);
    }    
}
    