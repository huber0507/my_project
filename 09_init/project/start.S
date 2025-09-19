.global _start  		/* 声明_start为全局标号，作为程序入口，告知链接器此为执行起点 */

/*
 * 描述：	_start函数是程序上电/复位后的第一个执行点，首先定义中断向量表
 *			ARM Cortex-A架构规定，中断向量表需存放8个异常处理入口（每个4字节）
 */
_start:
	/* 中断向量表：异常发生时，CPU自动跳转到对应地址执行 */
	ldr pc, =Reset_Handler			/* 0x00: 复位异常（上电/复位时触发，优先级最高） */
	ldr pc, =Undefined_Handler		/* 0x04: 未定义指令异常（CPU执行无法识别的指令） */
	ldr pc, =SVC_Handler			/* 0x08: SVC异常（系统调用，用于用户态到内核态切换） */
	ldr pc, =PrefAbort_Handler		/* 0x0C: 预取终止异常（指令读取失败，如访问无效内存） */
	ldr pc, =DataAbort_Handler		/* 0x10: 数据终止异常（数据访问失败，如非对齐访问） */
	ldr pc, =NotUsed_Handler		/* 0x14: 未使用的异常向量（ARM架构预留位置） */
	ldr pc, =IRQ_Handler			/* 0x18: IRQ异常（普通外部中断，如GPIO、UART等外设中断） */
	ldr pc, =FIQ_Handler			/* 0x1C: FIQ异常（快速中断，高优先级，用于紧急处理） */

/*
 * 复位异常处理函数：系统上电或复位后首先执行，完成底层硬件初始化
 * 流程：关闭中断 → 禁用Cache/MMU → 配置向量表（可选） → 设置各模式栈指针 → 跳转至main函数
 */
Reset_Handler:
	cpsid i					/* 关闭全局中断（置位CPSR寄存器的I位），防止初始化过程被中断干扰 */
	
	/*
	 * 关闭I Cache、D Cache、对齐检查、分支预测和MMU
	 * 操作CP15协处理器的C1寄存器（系统控制寄存器），采用"读-改-写"方式
	 */
	mrc p15,0,r0,c1,c0,0	/* 读取CP15协处理器C1寄存器的值到r0（C1控制Cache、MMU等功能） */
	bic r0,r0,#(0x1 << 12)	/* 清除bit12（I位）→ 关闭指令Cache（I Cache） */
	bic r0,r0,#(0x1<<2)		/* 清除bit2（C位）→ 关闭数据Cache（D Cache） */
	bic r0,r0,#0x2			/* 清除bit1（A位）→ 关闭对齐检查（避免非对齐访问触发异常） */
	bic r0,r0,#(0x1<<11)	/* 清除bit11（Z位）→ 关闭分支预测（简化启动流程） */
	bic r0,r0,#0x1			/* 清除bit0（M位）→ 关闭MMU（内存管理单元，裸机无需虚拟内存） */
	mcr p15,0,r0,c1,c0,0	/* 将修改后的值写回C1寄存器，使配置生效 */

	/*
	 * 可选：设置中断向量表偏移（如多阶段启动时，需将向量表移至APP所在地址）
	 * 此处注释掉表示使用默认向量表地址（通常为0x00000000或DDR起始地址）
	 */
	#if 0
		/* 设置中断向量表基地址为0X87800000 */
		ldr r0, =0X87800000

		dsb					/* 数据同步屏障：确保之前的内存操作完成后再执行后续指令 */
		isb					/* 指令同步屏障：确保后续指令从新的向量表地址取指 */
		mcr p15,0,r0,c12,0	/* 将向量表基地址写入CP15的C12寄存器（VBAR） */
		dsb
		isb
	#endif

	/*
	 * 设置各工作模式的栈指针（SP）
	 * ARM Cortex-A有多种工作模式，每种模式需独立栈空间（避免栈冲突）
	 * 栈地址需4字节对齐，IMX6ULL的DDR内存范围通常为0X80000000～0X9FFFFFFF
	 */
	
	/* 1. 进入IRQ模式，设置IRQ栈 */
	mrs r0,cpsr				/* 读取当前程序状态寄存器（CPSR）的值到r0 */
	bic r0,r0,#0x1f			/* 清除CPSR低5位（M0~M4，模式控制位） */
	orr r0,r0,#0x12			/* 低5位置为0x12 → 切换到IRQ模式 */
	msr cpsr, r0			/* 将修改后的值写回CPSR，完成模式切换 */
	ldr sp, =0x80600000		/* 设置IRQ模式栈顶地址为0x80600000（大小2MB） */

	/* 2. 进入SYS模式，设置SYS栈 */
	mrs r0,cpsr				/* 读取CPSR */
	bic r0,r0,#0x1f			/* 清除模式控制位 */
	orr r0,r0,#0x1f			/* 低5位置为0x1f → 切换到SYS模式（用户/系统模式） */
	msr cpsr, r0			/* 写入CPSR，完成模式切换 */
	ldr sp, =0x80400000		/* 设置SYS模式栈顶地址为0x80400000（大小2MB） */

	/* 3. 进入SVC模式，设置SVC栈 */
	mrs r0,cpsr				/* 读取CPSR */
	bic r0,r0,#0x1f			/* 清除模式控制位 */
	orr r0,r0,#0x13			/* 低5位置为0x13 → 切换到SVC模式（管理模式） */
	msr cpsr,r0				/* 写入CPSR，完成模式切换 */
	ldr sp, =0X80200000		/* 设置SVC模式栈顶地址为0X80200000（大小2MB） */

	cpsie i    /* 打开全局中断（清除CPSR的I位），允许后续响应中断 */

	/*
	 * 可选：单独使能IRQ中断（此处与上方cpsie i功能重复，故注释）
	 */
	#if 0
	mrs r0, cpsr			/* 读取cpsr寄存器值到r0 */
	bic r0, r0,#0x80		/* 清除bit7（I位），允许IRQ中断 */
	msr cpsr,r0				/* 写回cpsr */
	#endif

	b main					/* 跳转到C语言main函数，汇编启动阶段结束，移交控制权 */

/* 未定义指令异常处理函数（占位符） */
Undefined_Handler:
	ldr r0, =Undefined_Handler	/* 加载自身地址到r0 */
	bx r0						/* 跳转至自身，形成无限循环（防止程序跑飞） */
	
/* SVC异常处理函数（占位符） */
SVC_Handler:
	ldr r0, =SVC_Handler	/* 加载自身地址到r0 */
	bx r0					/* 无限循环 */

/* 预取终止异常处理函数（占位符） */
PrefAbort_Handler:
	ldr r0, =PrefAbort_Handler	/* 加载自身地址到r0 */
	bx r0						/* 无限循环 */

/* 数据终止异常处理函数（占位符） */
DataAbort_Handler:
	ldr r0, =DataAbort_Handler	/* 加载自身地址到r0 */
	bx r0						/* 无限循环 */

/* 未使用的异常处理函数（占位符） */
NotUsed_Handler:
	ldr r0, =NotUsed_Handler	/* 加载自身地址到r0 */
	bx r0						/* 无限循环 */

/* IRQ异常处理函数（处理普通外部中断） */
IRQ_Handler:
	push {lr}					/* 保存返回地址（lr）到栈，供后续恢复 */
	push {r0-r3,r12}			/* 保存通用寄存器（r0-r3、r12）到栈，避免被中断处理修改 */
	mrs r0,spsr				/* 读取SPSR寄存器（保存中断发生前的CPSR状态）到r0 */
	push {r0}					/* 保存SPSR到栈 */
	
	/* 读取GIC（通用中断控制器）中的中断号，确定哪个外设触发中断 */
	mrc p15,4,r1,c15,c0,0	/* 读取CP15的C15寄存器，获取GIC分配器基地址 */
	add r1,r1,#0X2000		/* GIC分配器基地址 + 0x2000 → 得到CPU接口端基地址 */
	ldr r0,[r1,#0XC]		/* 读取CPU接口端GICC_IAR寄存器（中断号寄存器），获取当前中断号 */
	
	push {r0, r1} 			/* 保存中断号（r0）和GIC地址（r1）到栈 */
	cps #0x13 				/* 切换到SVC模式（允许响应更高优先级中断，支持中断嵌套） */
	push {lr} 				/* 保存SVC模式下的返回地址（lr）到栈 */
	
	ldr r2, =system_irqhandler 	/* 加载C语言中断处理函数地址到r2 */
	blx r2						/* 跳转到C函数，传入中断号（r0）作为参数处理具体中断 */
	
	pop {lr} 				/* 恢复SVC模式的lr */
	cps #0x12 				/* 切回IRQ模式 */
	pop {r0, r1} 			/* 恢复中断号（r0）和GIC地址（r1） */
	str r0, [r1, #0X10] 	/* 写GICC_EOIR寄存器（中断结束寄存器），通知GIC中断处理完成 */
	pop {r0} 				/* 恢复SPSR */
	msr spsr_cxsf, r0 		/* 将SPSR的值写回当前CPSR，恢复中断前的状态 */
	pop {r0-r3, r12} 		/* 恢复通用寄存器 */
	pop {lr} 				/* 恢复lr */
	subs pc, lr, #4			/* 从lr-4跳转，返回中断发生的位置继续执行 */

/* FIQ异常处理函数（占位符） */
FIQ_Handler:
	ldr r0, =FIQ_Handler	/* 加载自身地址到r0 */
	bx r0					/* 无限循环 */
