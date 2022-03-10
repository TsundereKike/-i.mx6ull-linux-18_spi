#include "bsp_led.h"
/*
*@brief: 初始化led对应的GPIO相关寄存器
*@param in：  NULL
*@param out: NULL
*@ret:       NULL
*/
void led_init(void)
{
    /*初始化IO复用，复用为GPIO1_IO03*/
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO03_GPIO1_IO03,0);

    /*配置GPIO1_IO03的IO属性
     *bit [16]：     0   HYS关闭
     *bit [15:14]:  00  默认下拉
     *bit [13]:     0   kepper功能
     *bit [12]:     1   pull/kepper使能
     *bit [11]：    0   关闭开路输出
     *bit [7:6]:   10   速度100MHz
     *bit [5：3]：  110 R0/6驱动能力
     *bit [0]:      0   低转换率
     */
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO03_GPIO1_IO03,0x10b0);
    
    /*初始化GPIO，GPIO1_IO03设置为输出模式*/
    GPIO1->GDIR = 0X8;

    /*设置GPIO1_IO03输出高电平，关闭LED0*/
    GPIO1->DR |= (1<<3);
    
}
/*
*@brief: 打开led
*@param in:  NULL
*@param out: NULL
*@ret:       NULL
*/
void led_on(void)
{
    /*将GPIO1_DR寄存器的bit3清零*/
    GPIO1->DR &= ~(1<<3);

}
/*
*@brief: 关闭led
*@param in:  NULL
*@param out: NULL
*@ret:       NULL
*/
void led_off(void)
{
    /*将GPIO1_DR寄存器的bit3置1*/
    GPIO1->DR |= (1<<3);

}
void led_switch(int led, int led_status)
{
    switch (led)
    {
        case LED0:
            if(led_status==ON)
            {
                led_on();
            }
            else
            {
                led_off();
            }
            break;
        case LED1:
            break;
        default:
            break;
    }
}

