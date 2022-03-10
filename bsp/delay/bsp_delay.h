#ifndef __BSP_DELAY_H
#define __BSP_DELAY_H
#include "imx6u.h"
void delay_init(void);
void gpt1_irq_handler(unsigned int gicciar,void *param);
void delay_us(unsigned int usdelay);
void delay_ms(unsigned int msdelay);
void delay_short(volatile unsigned int n);
void delay_ms_nop(volatile unsigned int n);

#endif