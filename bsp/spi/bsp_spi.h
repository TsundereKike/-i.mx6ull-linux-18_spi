#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "imx6u.h"
void spi_init(ECSPI_Type *base);
unsigned char spi_ch0_read_write_data(ECSPI_Type *base, unsigned char data);
#endif