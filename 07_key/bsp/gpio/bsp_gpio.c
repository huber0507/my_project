#include "bsp_gpio.h"

void gpio_init(GPIO_Type *base,int pin,gpio_pin_config_t *config)
{
    if(config->direction == kGPIO_DigitalInput)  /*输入*/
    {
        base->GDIR &= ~(1<<pin);
    }
    else
    {
        base->GDIR |= 1<<pin;
        gpio_pinwrite(base,pin,config->outputLogic);/*默认输出电平*/
    }
}

