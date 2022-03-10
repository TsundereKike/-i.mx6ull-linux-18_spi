#include "bsp_delay.h"
#include "bsp_int.h"
#include "bsp_led.h"
/*GPT1定时器初始化作为高精度延时函数时基*/
 void delay_init(void)
 {
    GPT1->CR = 0;
    GPT1->CR |= (1<<15);         /*设置GPT1的CR寄存器的bit15，置1表示复位CR寄存器*/
    while((GPT1->CR>>15) & 0x01);/*等待CR寄存器软复位完成*/


    GPT1->CR = (1<<1) | (1<<6);  /*bit1置1设置GPT定时器计数初值为0，
                                   *bit6-8置001设置时钟源选择为ipg_clk=66MHz,
                                   *bit9置0设置GPT定时器工作在restart模式
                                  */
    GPT1->PR = 65;               /*设置时钟预分频为66,即GPT时钟为66MHz/66 = 1MHz*/
    GPT1->OCR[0] = 0xFFFFFFFF;   /*1MHz的计数频率，计数到0xFFFFFFFF= 4294967295us=71.5min*/
#if 0
    GPT1->OCR[0] = 1000000/2;    /*设置输出比较通道1寄存器为1000000/2,中断周期为500ms*/
    GPT1->IR |= (1<<0);          /*使能输出比较中断*/

    GIC_EnableIRQ(GPT1_IRQn);
    system_register_irqhandler(GPT1_IRQn, (system_irq_handler_t)gpt1_irq_handler, NULL);
#endif
    GPT1->CR |= (1<<0);/*使能GPT定时器*/
 }
 /*GPT定时器中断服务函数*/
 void gpt1_irq_handler(unsigned int gicciar,void *param)
 {
    static unsigned char state = OFF;
    if((GPT1->SR)&0x01)
    {
        state = !state;
        led_switch(LED0, state);
    }
    GPT1->SR |= (1<<0);/*清除中断标志位*/
 }
 /*高精度微秒级延时*/
 void delay_us(unsigned int usdelay)
 {
     unsigned long oldcnt,newcnt;
     unsigned long tcnt_value = 0;
     oldcnt = GPT1->CNT;
     while(1)
     {
         newcnt = GPT1->CNT;
         if(newcnt!=oldcnt)
         {
            if(newcnt>oldcnt)
                tcnt_value += newcnt-oldcnt;
            else
                tcnt_value += 0xFFFFFFFF - oldcnt + newcnt;
            oldcnt = newcnt;
            if(tcnt_value>=usdelay)
                break;
         }
     }
 }
 /*高精度毫秒级延时*/
 void delay_ms(unsigned int msdelay)
 {
     while(msdelay--)
     {
         delay_us(1000);
     }
 }
/*
*@brief: 延时函数
*@param in:     n   延时循环次数
*@param out:    NULL
*@ret:          NULL
*/
void delay_short(volatile unsigned int n)
{
    while(n--){}
}
/*
*@brief: ms级延时函数,396MHz主频下大约延时时间为1ms
*@param in:      n      要延时的ms数
*@ret:           NULL*/
void delay_ms_nop(volatile unsigned int n)
{
    while(n--)
    {
        delay_short(0x7ff);
    }
}

