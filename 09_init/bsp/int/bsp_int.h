#ifndef _BSP_INT_H
#define _BSP_INT_H
#include "imx6ul.h"  // 引入芯片头文件，提供IRQn_Type等中断相关类型定义

/*
 * 中断处理函数指针类型定义
 * @param - giccIar: 中断号相关的GICC_IAR寄存器值（包含中断ID）
 * @param - param: 用户自定义参数（传递给中断处理函数的数据）
 * 作用：标准化中断处理函数的原型，便于统一管理和注册
 */
typedef void (*system_irq_handler_t)(unsigned int giccIar, void *param);

/*
 * 中断处理结构体：存储中断对应的处理函数和用户参数
 * 用于构建中断处理表（类似中断向量表的扩展），实现动态注册中断处理函数
 */
typedef struct _sys_irq_handle
{
    system_irq_handler_t irqHandler;  // 中断处理函数指针（回调函数）
    void *userParam;                  // 传递给中断处理函数的用户参数
} sys_irq_handle_t;


/*
 * 函数声明：中断系统初始化
 * @description: 初始化GIC（通用中断控制器）、使能中断总开关、配置中断优先级等底层硬件
 * @return: 无
 */
void int_init(void);

/*
 * 函数声明：中断处理表初始化
 * @description: 初始化中断处理结构体数组（中断表），将所有中断默认绑定到default_irqhandler
 * @return: 无
 */
void system_irqtable_init(void);

/*
 * 函数声明：注册中断处理函数
 * @description: 为指定中断号绑定自定义处理函数和用户参数（动态替换中断表中的默认处理函数）
 * @param - irq: 中断号（IRQn_Type枚举类型，如GPIO1_IRQn，定义在MCIMX6Y2.h中）
 * @param - handler: 自定义中断处理函数（需符合system_irq_handler_t类型）
 * @param - userParam: 传递给中断处理函数的用户参数（可为NULL）
 * @return: 无
 */
void system_register_irqhandler(IRQn_Type irq, system_irq_handler_t handler, void *userParam);

/*
 * 函数声明：系统中断处理总入口
 * @description: 所有中断的公共入口函数，从GICC_IAR寄存器获取中断号，查表调用对应处理函数
 * @param - giccIar: GICC_IAR寄存器值（包含当前触发的中断ID）
 * @return: 无
 * 注：需在汇编中断向量表中配置为中断入口，是C语言与汇编的衔接点
 */
void system_irqhandler(unsigned int giccIar);

/*
 * 函数声明：默认中断处理函数
 * @description: 未注册自定义处理函数的中断会调用此函数（默认行为，如打印中断号提示未处理）
 * @param - giccIar: 中断号相关的GICC_IAR寄存器值
 * @param - userParam: 用户参数（默认处理函数中通常未使用，为兼容接口保留）
 * @return: 无
 */
void default_irqhandler(unsigned int giccIar, void *userParam);

#endif  // _BSP_INT_H  