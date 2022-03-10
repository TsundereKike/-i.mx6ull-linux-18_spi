#ifndef __BSP_AP3216C_H
#define __BSP_AP3216C_H
#include "imx6u.h"

#define AP32163_IS_OK           0x00
#define AP3216C_IS_NOT_OK       0x01
#define AP3216C_ADDR            0x1E

#define AP3216C_SYSTEM_CONFIG   0x00
#define AP3216C_INT_STATUS      0x01
#define AP3216C_INT_CLEAR       0x02
#define AP3216C_IR_DATA_LOW     0x0A
#define AP3216C_IR_DATA_HIGH    0x0B
#define AP3216C_ALS_DATA_LOW    0x0C
#define AP3216C_ALS_DATA_HIGH   0x0D
#define AP3216C_PS_DATA_LOW     0x0E
#define AP3216C_PS_DATA_HIGH    0x0F
unsigned char ap3216_init(void);
unsigned char ap3216c_read_one_byte(unsigned char addr, unsigned char reg);
unsigned char ap3216c_write_one_byte(unsigned char addr, unsigned char reg,unsigned char data);
void ap3216c_data_get(unsigned int *ir,unsigned int *als,unsigned int *ps);
#endif