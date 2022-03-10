#include "bsp_epit.h"
#include "bsp_int.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_gpio.h"
/*初始化定时器EPIT1*/
void epit1_init(unsigned int frac, unsigned int value)
{
    if(frac>4095)
        frac = 4095;
    /*配置EPIT1_CR寄存器*/
    EPIT1->CR = 0;
    EPIT1->CR = (1<<1) | (1<<2) | (1<<3) | (frac<<4) | (1<<24);
    EPIT1->LR = value;/*加载值寄存器*/
    EPIT1->CMPR = 0;/*比较值寄存器*/

    /*EPIT1定时器中断初始化*/
    GIC_EnableIRQ(EPIT1_IRQn);
    system_register_irqhandler(EPIT1_IRQn, epit1_irq_handler, NULL);

}
/*打开定时器1*/
void epit1_open(void)
{
    EPIT1->CR |= (1<<0);
}
/*关闭定时器1*/
void epit1_close(void)
{
    EPIT1->CR &= ~(1<<0);
}
/*重启定时器1*/
void epit1_restart(unsigned int value)
{
    epit1_close();
    EPIT1->LR = value;
    epit1_open();
}
/*定时器1中断服务函数*/
void epit1_irq_handler(unsigned int gicciar,void *param)
{
    static uint8_t state = OFF;
    if((EPIT1->SR & (1<<0)))
    {
        /*关闭定时器1*/
        epit1_close();
        if(gpio_pin_read(GPIO1, 18)==0)/*按键仍然处于按下状态*/
        {
            state = !state;
            beep_switch(state);  
        }
    }
    EPIT1->SR |= (1<<0);/*清除中断标志位*/
}
