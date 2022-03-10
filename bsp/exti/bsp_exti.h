#ifndef __BSP_EXTI_H
#define __BSP_EXTI_H
#include "imx6u.h"
void exti_init(void);
void GPIO1_IO18_irqhandler(unsigned int gicciar,void *param);
#endif