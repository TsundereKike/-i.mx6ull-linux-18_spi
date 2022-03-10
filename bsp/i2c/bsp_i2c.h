#ifndef __BSP_I2C_H
#define __BSP_I2C_H
#include "imx6u.h"
/*i2c相关宏定义*/
#define I2C_STATUS_OK                   (0)
#define I2C_STATUS_BUSY                 (1)
#define I2C_STATUS_IDLE                 (2)
#define I2C_STATUS_NAK                  (3)/*从机未返回应答信号*/
#define I2C_STATUS_ARBITRATIONAL_LOST   (4)/*仲裁丢失情况发生*/
#define I2C_STATUS_TIMEOUT              (5)
#define I2C_STATUS_ADDRNAK              (6)
enum i2c_transfer_direction
{
    kI2C_Write = 0x0,   /*主机向从机大宋数据*/
    kI2C_Read = 0x1,    /*主机从从机读取数据*/
};
/*
 * 主机传输结构体
 */
struct i2c_transfer
{
    unsigned char slaveAddress;      	    /* 7位从机地址 		*/
    enum i2c_transfer_direction direction; 	/* 传输方向 		*/
    unsigned int subaddress;       		    /* 寄存器地址		*/
    unsigned char subaddressSize;    	    /* 寄存器地址长度 	 */
    unsigned char *volatile data;    	    /* 数据缓冲区 		*/
    volatile unsigned int dataSize;  	    /* 数据缓冲区长度 	 */
};
void i2c_init(I2C_Type *base);
unsigned int i2c_master_start(I2C_Type *base, unsigned char address, enum i2c_transfer_direction direction);
unsigned int i2c_master_stop(I2C_Type *base);
unsigned int i2c_master_repeat_start(I2C_Type *base, unsigned char address, enum i2c_transfer_direction direction);
unsigned int i2c_check_and_clear_error(I2C_Type *base, unsigned int status);
void i2c_master_write(I2C_Type *base, const unsigned char *buf, unsigned int size);
void i2c_master_read(I2C_Type *base, unsigned char *buf, unsigned int size);
unsigned char i2c_master_transfer(I2C_Type *base, struct i2c_transfer *xfer);
#endif