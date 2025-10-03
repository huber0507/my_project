#ifndef __BSP_DOOR_H
#define __BSP_DOOR_H

#include "imx6ul.h"

// 1. 声明报警阶段枚举（若未定义）
enum AlarmStage{
    ALARM_STAGE_NONE,    // 0-10秒（无报警）
    ALARM_STAGE_10S,     // 10秒（3次短鸣）
    ALARM_STAGE_20S,     // 20秒（5次短鸣）
    ALARM_STAGE_30S      // 30秒（长鸣）
};

// 2. 声明门状态枚举
enum doorvalue{
    door_open   = 0,  // 门打开
    door_closed = 1   // 门关闭
};

// 3. 声明全局变量（用 extern，不初始化）
extern uint32_t door_open_time;          // 门打开时长（ms）
extern enum AlarmStage alarm_stage;      // 报警阶段

/* 函数声明（必须包含返回值类型） */
// 初始化门状态检测引脚（GPIO1_IO04）
void door_init(void);

// 执行开门操作（如控制LED亮、重置超时等）
void open_door(void);

// 执行关门操作（如控制LED灭、停止报警等）
void close_door(void);

// 判断门是否打开（返回枚举值，直观表示状态）
enum doorvalue door_get_state(void);

// 门超时检测（实现10s/20s/30s分级报警逻辑）
void door_timeout_check(void);

#endif