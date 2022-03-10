#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "fsl_iomuxc.h"
#include "MCIMX6Y2.h" 
#include "bsp_delay.h"
enum key_num
{
    KEY_NONE = 0,
    KEY0_VALUE = 1,
    KEY1_VALUE,
};
void key_init(void);
int read_key(void);
int get_key_value(void);
#endif