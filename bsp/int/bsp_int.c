#include "bsp_int.h"
/*中断嵌套次数*/
static unsigned int irqNesting;
/*中断处理函数表*/
static _sys_irq_handle_t irqTable[NUMBER_OF_INT_VECTORS];

/*初始化中断处理函数表*/
void system_irqtable_init(void)
{
    unsigned int i = 0;
    irqNesting = 0;
    for(i=0;i<NUMBER_OF_INT_VECTORS;i++)
    {
        irqTable[i].irqhandler = default_irqhandler;
        irqTable[i].userParam = NULL;
    }
}
/*注册中断处理函数*/
void system_register_irqhandler(IRQn_Type irq, system_irq_handler_t handler, void *userparam)
{
    irqTable[irq].irqhandler = handler;
    irqTable[irq].userParam = userparam;
}
/*中断初始化函数*/
void int_init(void)
{
    GIC_Init();
    system_irqtable_init();
    /*中断向量偏移设置*/
    __set_VBAR((uint32_t)0x87800000);

}
/*具体的中断处理函数，当中断触发时，进入IRQ_Handler，IRQ_Handler再调用此函数*/
void system_irqhandler(unsigned int gicciar)
{
    uint32_t intNum = gicciar & 0x3ffUL;
    /*检查中断ID是否正常*/
    if(intNum >= NUMBER_OF_INT_VECTORS)
    {
        return;
    }
    irqNesting++;

    /*根据中断ID号读取对应中断处理函数并执行*/
    irqTable[intNum].irqhandler(intNum,irqTable[intNum].userParam);
    irqNesting--;
}
/*默认初始化中断处理函数*/
void default_irqhandler(unsigned int gicciar,void *param)
{
    while(1)
    {

    }
}