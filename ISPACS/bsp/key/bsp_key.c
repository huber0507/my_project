#include "bsp_key.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"
#include "bsp_beep.h"  // 新增：包含蜂鸣器头文件，解决 beep_beep 未声明问题
#include "bsp_door.h"
#include "bsp_sg90.h"




/*
 * @description	: 初始化按键
 * @param 		: 无
 * @return 		: 无
 */
void key_init(void)
{	
	gpio_pin_config_t key_config;
	
	/* 1、初始化IO复用, 复用为GPIO1_IO18 */
	IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);

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
	IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);
	
	/* 3、初始化GPIO */
	//GPIO1->GDIR &= ~(1 << 18);	/* GPIO1_IO18设置为输入 */	
	key_config.direction = kGPIO_DigitalInput;
	gpio_init(GPIO1,18, &key_config);
	
}

/*
 * @description	: 获取按键值 
 * @param 		: 无
 * @return 		: 0 没有按键按下，其他值:对应的按键值
 */
// bsp_key.c 中的 key_getvalue 函数（同步修改无按键返回值）
int key_getvalue(void)
{
    int ret = KEY_VALUE_NONE;  // 初始化返回值为“无按键”
    static unsigned char release = 1;  // 按键释放标记（1=已释放，0=按下）

    // 按键按下（低电平）且处于释放状态
    if ((release == 1) && (gpio_pinread(GPIO1, 18) == 0))  
    {    
        delay(10);        // 延时10ms消抖
        release = 0;      // 标记按键已按下

        // 再次检测确认按下
        if (gpio_pinread(GPIO1, 18) == 0)
        {
            ret = KEY0_VALUE;  // 返回按键值
        }
    }
    // 按键释放（高电平）
    else if (gpio_pinread(GPIO1, 18) == 1)
    {
        ret = KEY_VALUE_NONE;  // 返回“无按键”
        release = 1;           // 标记按键已释放
    }

    return ret;  // 确保所有分支都有返回值
}

/*
 * @description	: 按键逻辑处理（核心：判断双击/长按/错误）
 * @param 		: 无
 * @return 		: 无
 * 依赖：door_open_time（EPIT定时器1ms累加，作为时间基准）
 */
void key_process(void)
{
    int key_val = key_getvalue();
    static unsigned int key_count = 0;
    static unsigned int first_press_time = 0;
    static unsigned char long_press_flag = 0;
    static unsigned int long_press_start = 0;

    // 打印当前按键值（无按键时不打印，避免刷屏）
    if (key_val != KEY_VALUE_NONE)
    {
        printf("按键检测：key_val=%d\n", key_val);
    }

    switch (key_val)
    {
        case KEY0_VALUE:  // 按键按下
            if (key_count == 0)  // 第一次按下
            {
                first_press_time = door_open_time;  // 记录第一次按下时间
                long_press_start = door_open_time;  // 记录长按开始时间
                long_press_flag = 1;  // 标记进入长按检测
                key_count = 1;
                printf("第一次按下：key_count=%d, 时间=%dms\n", key_count, first_press_time);
            }
            else if (key_count == 1)  // 第二次按下（双击）
            {
                unsigned int current_time = door_open_time;
                if (current_time - first_press_time <= 3000)  // 3秒内双击
                {
                    printf("双击有效：开门\n");
                      SG90_DoorOpen();  // 执行开门
                    // 重置状态变量
                    key_count = 0;
                    long_press_flag = 0;
                }
                else  // 超过3秒，视为新的第一次按下
                {
                    first_press_time = current_time;
                    key_count = 1;
                    printf("超时，视为新的第一次按下：key_count=%d\n", key_count);
                }
            }
            break;

        case KEY_VALUE_NONE:  // 按键释放
            if (long_press_flag)  // 处于长按检测状态
            {
                unsigned int press_duration = door_open_time - long_press_start;
                if (press_duration >= 5000)  // 长按5秒以上
                {
                    printf("长按有效（%dms）：开门\n", press_duration);
                    SG90_DoorOpen();   // 执行开门
                    key_count = 0;
                    long_press_flag = 0;
                }
                else if (press_duration > 0)  // 短按未达5秒
                {
                    if (key_count == 1)  // 仅按了1次
                    {
                        // 等待3秒判断是否双击
                        if (door_open_time - first_press_time > 3000)
                        {
                            printf("仅按1次（%dms）：错误提示\n", press_duration);
                            beep_toggle(2);  // 错误提示
                            key_count = 0;
                            long_press_flag = 0;
                        }
                    }
                }
            }
            break;
    }
}