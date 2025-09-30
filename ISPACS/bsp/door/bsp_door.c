#include "bsp_door.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"


/*
 * @description	: 门状态初始化
 * @param 		: 无
 * @return 		: 无
 */
void door_init(void)
{	
	gpio_pin_config_t door_config;
	
	/* 1、初始化IO复用, 复用为GPIO1_IO4*/
	IOMUXC_SetPinMux(IOMUXC_GPIO1_IO04_GPIO1_IO04,0);

	/* 2、、配置UART1_CTS_B的IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 11 默认22K上拉
	 *bit [13]: 1 pull功能
	 *bit [12]: 1 pull/keeper使能
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 000 关闭输出
	 *bit [0]: 0 低转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO04_GPIO1_IO04,0xF080);
	
	/* 3、初始化GPIO */
	//GPIO1->GDIR &= ~(1 << 4);	/* GPIO1_IO4设置为输入 */	
	door_config.direction = kGPIO_DigitalInput;
	gpio_init(GPIO1,4, &door_config);
	
}

/*
 * @description	: 获取当前门状态
 * @param 		: 无
 * @return 		: door_open（0）=门打开；door_closed（1）=门关闭
 * 				  （硬件逻辑：门打开时传感器输出高电平1，关闭时输出低电平0）
 */
enum doorvalue door_get_state(void)
{
    uint32_t gpio_level = gpio_pinread(GPIO1, 4);  // 读取GPIO1_IO04电平
    
    // 电平与状态对应：GPIO=1→门打开（返回0）；GPIO=0→门关闭（返回1）
    // （若硬件相反，将"==1"改为"==0"即可）
    if (gpio_level == 1) {
        return door_open;    // 门打开
    } else {
        return door_closed;  // 门关闭
    }
}
