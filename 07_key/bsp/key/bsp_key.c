#include "bsp_key.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"

void key_init(void)
{
    gpio_pin_config_t key_config;
    /*初始化IO复用，复用为GPIO1_IO18*/
IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);


}