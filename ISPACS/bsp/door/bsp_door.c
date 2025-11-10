#include "bsp_door.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include <stdio.h>
#include "bsp_gpio.h"

/* 1. 全局变量定义（与头文件声明对应） */
uint32_t door_open_time = 0;                  // 门打开时长（ms，用uint32_t更合理）
enum AlarmStage alarm_stage = ALARM_STAGE_NONE;  // 报警阶段，初始化为无报警


/*
 * @description	: 门状态检测引脚初始化（GPIO1_IO04）
 * @param 		: 无
 * @return 		: 无
 */
void door_init(void)
{	
	gpio_pin_config_t door_config;
	
	/* 1、初始化IO复用为GPIO1_IO04 */
	IOMUXC_SetPinMux(IOMUXC_GPIO1_IO04_GPIO1_IO04, 0);

	/* 2、配置GPIO1_IO04的IO属性（上拉输入） */
	// IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO04_GPIO1_IO04, 0xF080);
        // 2. 引脚配置：将上拉改为下拉（关键修改bit15:14为01）
    // 原上拉配置（可能类似）：0xF080（bit15:14=11，上拉）
    // 修改后下拉配置：0x5080（bit15:14=01，下拉；其他位不变）
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO04_GPIO1_IO04, 0x5080);
	
	/* 3、初始化GPIO为输入模式 */
	door_config.direction = kGPIO_DigitalInput;
	gpio_init(GPIO1, 4, &door_config);
}
/*
 * @description    : 获取当前门状态
 * @param         : 无
 * @return         : door_open（0）=门打开；door_closed（1）=门关闭
 *                   （硬件逻辑：门打开时传感器输出高电平1，关闭时输出低电平0）
 */
enum doorvalue door_get_state(void)
{
    // 读取GPIO4的电平（假设门状态检测引脚为GPIO1_IO4）
    uint32_t gpio_level = (GPIO1->DR & (1 << 4)) ? 1 : 0;  // 1=高电平，0=低电平
    gpio_print_level(GPIO1, 4);

    // 核心：GPIO4电平直接控制LED0（强关联）
    if (gpio_level == 0) {  
        // GPIO4低电平 → 门打开 → LED0亮
        led_switch(LED0, ON);  
    } else {  
        // GPIO4高电平 → 门关闭 → LED0灭
        led_switch(LED0, OFF);  
    }

    // 同时返回门状态枚举（不影响原有逻辑）
    return (gpio_level == 0) ? door_open : door_closed;
}

/*
 * @description	: 执行开门操作（仅当门当前处于关闭状态时生效）
 * @param 		: 无
 * @return 		: 无
 * 操作逻辑：打开LED（门状态指示）→ 停止蜂鸣器（若之前报警）→ 重置门超时计时和报警阶段
 */
void open_door(void)
{
    door_open_time = 0;         
    alarm_stage = ALARM_STAGE_NONE;  
    printf("状态变化：门已打开\n");
}

void close_door(void)
{
    beep_switch(OFF);           
    door_open_time = 0;         
    alarm_stage = ALARM_STAGE_NONE;  
    printf("close_door执行：计时/报警重置\n");
}


/*
 * @description	: 门超时检测与分级报警
 * @param 		: 无
 * @return 		: 无
 * 逻辑：门打开时累加时长，根据阈值触发不同报警；门关闭时重置状态
 * 注意：door_open_time 需由定时器中断（如1ms一次）累加（door_open_time++）
 */
/* 5. 门超时检测（10s/20s/30s分级报警） */
void door_timeout_check(void)
{
    enum doorvalue current_state = door_get_state();
    static enum AlarmStage last_alarm_stage = ALARM_STAGE_NONE;

    if (current_state == door_open) {
        // 门打开时：计时 + 分级报警（原有逻辑保留）
        door_open_time++;
        if (door_open_time >= 30000 && alarm_stage < ALARM_STAGE_30S) {
            alarm_stage = ALARM_STAGE_30S;
            beep_long_on();
        } else if (door_open_time >= 20000 && alarm_stage < ALARM_STAGE_20S) {
            alarm_stage = ALARM_STAGE_20S;
            beep_toggle(5);
        } else if (door_open_time >= 10000 && alarm_stage < ALARM_STAGE_10S) {
            alarm_stage = ALARM_STAGE_10S;
            beep_toggle(3);
        }

        // 报警阶段变化时打印（原有逻辑保留）
        if (alarm_stage != last_alarm_stage) {
            printf("Door Open: Time=%dms, Alarm Stage=%d\n", door_open_time, alarm_stage);
            last_alarm_stage = alarm_stage;
        }
    } else {
        // 门关闭时：强制停止蜂鸣器 + 重置报警状态
        if (last_alarm_stage != ALARM_STAGE_NONE) {
            beep_switch(OFF);            // 停止蜂鸣器
            alarm_stage = ALARM_STAGE_NONE;  // 重置报警阶段
            last_alarm_stage = ALARM_STAGE_NONE;
            door_open_time = 0;          // 重置开门时长
            printf("Door Closed: Reset Alarm. Total Open Time=%dms\n", door_open_time);
        }
    }
}