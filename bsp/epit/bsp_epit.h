#ifndef __BSP_EPIT_H
#define __BSP_EPIT_H
#include "imx6u.h"
void epit1_init(unsigned int frac, unsigned int value);
void epit1_open(void);
void epit1_close(void);
void epit1_restart(unsigned int value);
void epit1_irq_handler(unsigned int gicciar,void *param);
#endif