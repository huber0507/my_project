// 1. 补充所有缺失的头文件（解决函数隐式声明）
#include "bsp_int.h"        // 中断框架
#include "bsp_led.h"        // LED驱动
#include "bsp_beep.h"       // 蜂鸣器驱动（含beep_short_2times声明）
#include "bsp_door.h"       // 门控模块（含door_get_state声明）
#include "bsp_key.h"        // 按键模块
#include "bsp_epittimer.h"  // EPIT定时器
#include "bsp_delay.h"      // 延时函数（含delay_ms声明）
#include "bsp_uart.h"       // 串口打印（若printf是串口实现，需包含此头文件）
#include <stdio.h>          // 若printf是标准库实现，需包含此头文件（二选一）
#include "bsp_sg90.h"

int main(void)
{
    /************************** 1. 初始化外设（按依赖顺序） **************************/
    int_init();                  // 中断框架（最优先）
     clk_enable();       // 使能外设时钟
    led_init();                  // LED初始化
    beep_init();                 // 蜂鸣器初始化
    delay_init();                // 延时初始化（使能delay_ms）
    uart_init();                 // 串口初始化（使能printf）
    door_init();                 // 门状态检测初始化（使能door_get_state）
    key_init();                  // 按键初始化
    epit1_init(65, 1000);        // 1ms定时器初始化
    SG90_Init();   
    SG90_Init2();


    /************************** 2. 系统启动提示 **************************/
    // 解决printf隐式声明：已包含bsp_uart.h或stdio.h
    printf("\n===== 门控报警系统启动 =====\n");
    printf("系统初始化完成！开始测试...\n");
    printf("蜂鸣器自检：短鸣2次\n");
    
    // 解决beep_short_2times隐式声明：已包含bsp_beep.h
    beep_toggle(2);  // 蜂鸣器自检（需在bsp_beep.c中实现）

    
    // 解决delay_ms隐式声明：已包含bsp_delay.h
    delayms(1000);       // 等待自检完成

//     // 主函数初始化后添加，测试LED是否能被强制控制
// led_switch(LED0, ON);  // 亮3秒
// delayms(5000);
// led_switch(LED0, OFF); // 灭3秒
// delayms(5000);

//采用SG90作为实际门的开合机械装置，增加角度反馈读取，获取门状态信息。
//增加串口打印信息，判断各个参数状态
//后续延伸会增加屏幕控制
//直接采用屏幕打印串口信息。


    /************************** 3. 主循环变量初始化（修改静态变量赋值方式） **************************/
    // 错误写法：static enum doorvalue last_door_state = door_get_state();（初始化用了函数调用）
    // 正确写法：先声明静态变量，再在初始化后赋值（运行时赋值，符合C规则）
    static enum doorvalue last_door_state;  // 仅声明，不初始化
    last_door_state = door_get_state();     // 外设初始化后，再读取初始状态（运行时赋值）

    // 打印初始门状态（验证door_get_state正常工作）
    printf("初始门状态：%d（0=开，1=关），开门时长：%dms\n", 
           last_door_state, door_open_time);

    /************************** 4. 主循环（处理核心业务） **************************/
    while (1)
    {
        // 4.1 读取当前门状态
        enum doorvalue current_door_state = door_get_state();

        // 4.2 检测门状态变化（开→关 或 关→开）
        if (current_door_state != last_door_state)
        {
            if (current_door_state == door_open)
            {
               SG90_DoorOpen();
                printf("状态变化：门已打开\n");
            }
            else
            {
                SG90_DoorClose();
                printf("状态变化：门已关闭\n");
            }
            last_door_state = current_door_state;  // 更新上一次状态
        }

        // 4.3 按键处理（双击/长按开门）
        key_process();

        // 4.4 门超时检测（10s/20s/30s分级报警）
        door_timeout_check();

        // 4.5 主循环延时（降低CPU占用）
        delayms(10);
    }
}