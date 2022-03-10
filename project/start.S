.global _start

_start:
    ldr pc, =Reset_Hanlder      /*复位中断服务函数 */
    ldr pc, =Undefined_Handler  /*未定义指令中断服务函数 */
    ldr pc, =SVC_Handler        /*SVC*/
    ldr pc, =PreAbort_Handler   /*预取终止 */
    ldr pc, =DataAbort_Hanlder  /*数据终止 */
    ldr pc, =NotUsed_Hanlder    /*未使用 */
    ldr pc, =IRQ_Handler        /*IRQ中断 */
    ldr pc, =FIQ_Handler        /*FIQ中断 */

/*复位中断服务函数 */
Reset_Hanlder:
     cpsid i        /*关闭全局中断 */
    /*关闭I,Dcache 
     *修改SCTLR寄存器，采用读-改-写的方式
     */
     mrc p15, 0, r0, c1, c0, 0 /*读取SCTLR寄存器的数据到r0寄存器里面 */
     bic r0, r0, #(1 << 12)    /*关闭Icache  */
     bic r0, r0, #(1 << 11)      /*关闭分支预测 */
     bic r0, r0, #(1 << 2)       /*关闭Dcache */
     bic r0, r0, #(1 << 1)       /*关闭对齐 */
     bic r0, r0, #(1 << 0)       /*关闭MMU */
     mcr p15, 0, r0, c1, c0, 0/*将r0寄存器的数据写入到SCTLR寄存器 */
 
#if 0
    /*设置中断向量偏移 */
    ldr r0, =0x87800000
    dsb
    isb
    mcr p15, 0, r0, c12, c0, 0/*设置VBAR寄存器=0x87800000 */
    dsb
    isb
#endif

.global _bss_start
_bss_start:
    .word __bss_start

.global _bss_end
_bss_end:
    .word __bss_end
       
    /*清除BSS段 */
    ldr r0, _bss_start
    ldr r1, _bss_end
    mov r2, #0
bss_loop:
    stmia r0!, {r2}
    cmp r0, r1      /*比较r0和r1寄存器里的值 */
    ble bss_loop    /*如果r0地址小于等于r1，继续清除bss段 */

    /*设置各个处理器运行模式下的栈指针
     *注意：IMX6ULL的堆栈指针向下增长
     *堆栈指针地址一定要是4字节地址对齐！！！
     *DDR范围：0x80000000~0x9FFFFFFF
     */
    /* 设置处理器进入IRQ模式*/
    mrs r0, cpsr            /*读取cpsr到r0 */
    bic r0, r0, #0x1f       /*清除cpsr到bit4-0 */
    orr r0, r0, #0x12       /*使用IRQ模式 */
    msr cpsr, r0            /*将r0写入到cpsr */
    ldr sp, =0x80600000     /*设置IRQ模式下的栈首地址为0x80600000,大小为2MB */

    /* 设置处理器进入SYS模式*/
    mrs r0, cpsr            /*读取cpsr到r0 */
    bic r0, r0, #0x1f       /*清除cpsr到bit4-0 */
    orr r0, r0, #0x1f       /*使用SYS模式 */
    msr cpsr, r0            /*将r0写入到cpsr */
    ldr sp, =0x80400000     /*设置SYS模式下的栈首地址为0x80400000,大小为2MB */

    /* 设置处理器进入SVC模式*/
    mrs r0, cpsr            /*读取cpsr到r0 */
    bic r0, r0, #0x1f       /*清除cpsr到bit4-0 */
    orr r0, r0, #0x13       /*使用SVC模式 */
    msr cpsr, r0            /*将r0写入到cpsr */
    ldr sp, =0x80200000     /*设置SVC模式下的栈首地址为0x80200000,大小为2MB */

    cpsie i         /*打开全局中断 */
   
    b main          /*跳转到main函数 */

/*未定义指令中断服务函数 */
Undefined_Handler:
    ldr r0, =Undefined_Handler
    bx r0

/*SVC中断服务函数 */
SVC_Handler:
    ldr r0, =SVC_Handler
    bx r0

/*预取终止中断服务函数 */
PreAbort_Handler:
    ldr r0, =PreAbort_Handler
    bx r0

/*数据终止中断服务函数 */
DataAbort_Hanlder:
    ldr r0, =DataAbort_Hanlder
    bx r0

/*未使用中断服务函数 */
NotUsed_Hanlder:
    ldr r0, =NotUsed_Hanlder
    bx r0

/*IRQ中断服务函数 */
IRQ_Handler:
    push {lr}                       /*保存lr地址 */
    push {r0-r3, r12}               /*保存r0-r3，r12寄存器 */

    mrs r0, spsr                    /*读取spsr寄存器 */
    push {r0}                       /*保存spsr寄存器 */

    mrc p15, 4, r1, c15, c0, 0      /*将cp15的c0寄存器内的值读取到r1寄存器中，
                                     *目的在于获取到保存有GIC控制器寄存器组首
                                     *地址的CBAR寄存器的值 
                                     */
    add r1, r1, #0x2000             /*GIC控制器基地址偏移0x2000,即GIC的CPU接口端
                                     *基地址
                                     */
    ldr r0, [r1, #0xc]              /*GIC控制器的CPU接口端基地址再偏移0x0c就是GICC_IAR
                                     *寄存器的地址。其中GICC_IAR寄存器的bit9～0保存了中断的ID号 */
    push {r0, r1}                   /*保存r0,r1 */
    cps #0x13                       /*进入SVC模式，允许其他中断再次进去 */

    push {lr}                       /*保存SVC模式的栈首地址lr寄存器 */
    ldr r2, =system_irqhandler      /*加载C语言中断处理函数system_irqhandler到r2寄存器中 */
    blx r2                          /*跳转到r2寄存器所代表的C语言中断处理函数system_irqhandler
                                     *中运行。
                                     *注：函数system_irqhandler带有一个参数，保存在r0寄存器中
                                     */
    pop {lr}                        /*执行完C语言中断处理函数system_irqhandler后，lr出栈 */
    cps #0x12                       /*进入IRQ模式 */
    pop {r0, r1}
    str r0, [r1, #0x10]             /*将r0寄存器(GICC_IAR寄存器)里的值写入到r1所代表的GIC的CPU接口端
                                     *基地址再偏移0x10地址去。
                                     *即：中断执行完成，必须将GICC_IAR寄存器里的值写入到EOIR寄存器中去
                                     */
    pop {r0}
    msr spsr_cxsf, r0               /*恢复spsr */

    pop {r0-r3, r12}                /*r0-r3,r12出栈 */
    pop {lr}                        /*lr出栈 */
    subs pc, lr, #4                  /*将栈首地址lr-4赋给pc */

/*FIQ中断服务函数 */
FIQ_Handler:
    ldr r0, =FIQ_Handler
    bx r0

