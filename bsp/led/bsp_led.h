#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "fsl_iomuxc.h"
#include "MCIMX6Y2.h"
#include "cc.h"
enum led_num
{
    LED0 = 0,
    LED1,
};
void led_init(void);
void led_on(void);
void led_off(void);
void led_switch(int led, int led_status);
#endif
