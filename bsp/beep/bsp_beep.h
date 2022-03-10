#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H
#include "fsl_iomuxc.h"
#include "MCIMX6Y2.h"
#include "cc.h"
void beep_init(void);
void beep_switch(int beep_status);
#endif