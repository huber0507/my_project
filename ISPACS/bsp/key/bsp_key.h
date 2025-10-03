#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "imx6ul.h"


/* 1. 按键值枚举（表示“具体哪个按键按下/无按键”）
   改名 KEY_NONE → KEY_VALUE_NONE，避免与“按键状态枚举”冲突 */
enum keyvalue{
    KEY_VALUE_NONE = 0,  // 无按键（原 KEY_NONE，改名后语义更明确）
    KEY0_VALUE,          // KEY0 按键值（对应 key_getvalue 返回的 1）
    KEY1_VALUE,          // 预留：若有其他按键可扩展
    KEY2_VALUE           // 预留：若有其他按键可扩展
};

/* 2. 按键状态枚举（表示“按键的动作状态”，与“按键值”语义区分）
   保留 KEY_NONE，明确表示“无任何按键动作” */
typedef enum {
    KEY_NONE,            // 无按键动作（无按、无双击、无长按）
    KEY_DOUBLE_CLICK,    // 3秒内双击（有效动作）
    KEY_LONG_PRESS,      // 长按5秒（有效动作）
    KEY_ERROR            // 按键错误（1次或≥3次，无效动作）
} key_state_t;

/* 函数声明 */
void key_init(void);
int key_getvalue(void);    // 返回值对应 enum keyvalue（如 KEY_VALUE_NONE、KEY0_VALUE）
void key_process(void);    // 处理按键状态，返回对应 key_state_t

#endif