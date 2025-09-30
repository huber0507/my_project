#ifndef __BSP_DOOR_H
#define __BSP_DOOR_H

#include "imx6ul.h"

/* 定义门状态枚举（不是结构体） */
enum doorvalue{
    door_open   = 0,  // 门打开
    door_closed = 1   // 门关闭
};

/* 函数声明（必须包含返回值类型） */
// 初始化门状态检测引脚（GPIO1_IO04）
void door_init(void);

// 执行开门操作（如控制LED亮、重置超时等）
void open_door(void);

// 执行关门操作（如控制LED灭、停止报警等）
void close_door(void);

// 判断门是否打开（返回枚举值，直观表示状态）
enum doorvalue is_door_open(void);

// 门超时检测（实现10s/20s/30s分级报警逻辑）
void door_timeout_check(void);

#endif