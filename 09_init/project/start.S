/***************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 start.s
作者	   : 左忠凯
版本	   : V1.0
描述	   : ZERO-I.MX6UL/I.MX6ULL开发板启动文件，完成C环境初始化，
		 C环境初始化完成以后跳转到C代码。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版 2019/1/3 左忠凯修改
**************************************************************/

.global _start  		/* 全局标号 */

/*
 * 描述：	_start函数，创建中断向量表
 *		 
 */
_start:
	ldr pc, =Reset_Handler											/*复位中断 */
	ldr pc, =Undefined_Handler								/*未定义指令中断 */
	ldr pc, =SVC_Handler											/*SVC中断 */
	ldr pc, =PrefAbort_Handler								/*预取终止中断 */
	ldr pc, =DataAbort_Handler								/*数据终止中断 */
	ldr pc, =NotUsed_Handler								/*未使用中断 */
	ldr pc, =IRQ_Handler											/*IRQ中断 */
	ldr pc, =FIQ_Handler											/*FIQ快速中断 */

Reset_Handler:
	cpsid i																			/*关闭全局中断 */
	/*
	*关闭I，DCache和MMU
	*采取读-改-写的方式
	 */
	mrc p15,0,r0,c1,c0,0											/*读取CP15的C1寄存器到R0中 */
	bic r0,r0,#(0x1 << 12)											/*清除C1的I位，关闭I Cache */	
	bic r0,r0,#(0x1<<2)												/*清除C1的C位，关闭D Cache */
	bic r0,r0,#0x2														/*清除C1的A位，关闭对齐检查 */
	bic r0,r0,#(0x1<<11)										/*清除C1的Z位，关闭分支预测 */
	bic r0,r0,#0x1														/*清除C1的M位，关闭MMU */
	mcr p15,0,r0,c1,c0,0										/*将r0的值写入到CP15的C1里 */

	#if 0
		/*设置中断向量表偏移 */
		ldr r0, =0X87800000

		dsb
		isb
		mcr p15,0,r0,c12,0
		dsb
		isb
	#endif

	/*
	*设置各个模式下的栈指针
	*IMX6ULL的堆栈是向下增长的
	*堆栈指针地址一定要4字节对其
	*DDR范围：0X80000000～0X9FFFFFFF或者0X8FFFFFFF
	 */
	/*进入IRQ模式 */
mrs r0,cpsr
bic r0,r0,#0x1f							/*将r0的低五位清零，也就是cpsr的M0～M4 */
orr r0,r0,#0x12							/*将r0的数据写入到cpsr中 */
ldr sp, =0x80600000				/*IRQ模式栈首地址为0X80600000,大小为2MB */

/*进入SYS模式 */
mrs r0,cpsr
bic r0,r0,#0x1f							/*将r0的低五位清零，也就是cpsr的M0～M4 */
orr r0,r0,#0x1f						



	ldr sp,=0X80200000	/* 设置用户模式下的栈首地址为0X80200000,大小为2MB	  	   			*/
	b main				/* 跳转到main函数 										*/

