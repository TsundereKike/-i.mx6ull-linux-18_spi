#include "bsp_ap3216c.h"
#include "bsp_i2c.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"
#include "stdio.h"
unsigned char ap3216_init(void)
{
    unsigned char val = 0;
    /*引脚gpio初始化*/
    IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL,1);/*引脚复用为i2c_SCL*/
    IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA,1);/*已经爱哦复用为i2c_SDA*/

    IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL,0x70b0);
    IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA,0x70b0);
    /*i2c初始化*/
    i2c_init(I2C1);
    /*ap3216c初始化*/
    ap3216c_write_one_byte(AP3216C_ADDR, AP3216C_SYSTEM_CONFIG, 0x04);/*复位ap3216c*/
    delay_ms(50);
    ap3216c_write_one_byte(AP3216C_ADDR, AP3216C_SYSTEM_CONFIG, 0X03);/*设置ap3216c工作模式*/
    val = ap3216c_read_one_byte(AP3216C_ADDR, AP3216C_SYSTEM_CONFIG);
    if(val==0x03)
    {
        return AP32163_IS_OK;
    }
    else
    {
        return AP3216C_IS_NOT_OK;
    }
}
/*ap3216c读取一字节数据,返回值为读取到的数据*/
unsigned char ap3216c_read_one_byte(unsigned char addr, unsigned char reg)
{
    unsigned char val = 0;
    struct i2c_transfer xfer;

    xfer.slaveAddress = addr;
    xfer.subaddress = reg;
    xfer.subaddressSize = 1;
    xfer.direction = kI2C_Read;
    xfer.data = &val;
    xfer.dataSize = 1;
    i2c_master_transfer(I2C1, &xfer);

    return val;
}
/*ap3216c写一字节数据，返回值为错误类型*/
unsigned char ap3216c_write_one_byte(unsigned char addr, unsigned char reg,unsigned char data)
{
    unsigned char write_data = data;
    struct i2c_transfer xfer;

    xfer.slaveAddress = addr;
    xfer.subaddress = reg;
    xfer.subaddressSize = 1;
    xfer.direction = kI2C_Write;
    xfer.data = &write_data;
    xfer.dataSize = 1;
    return(i2c_master_transfer(I2C1, &xfer));
}
/*获取AP3216C传感器数据*/
void ap3216c_data_get(unsigned int *ir,unsigned int *als,unsigned int *ps)
{
    unsigned char data[6] = {0};
    int i =0;
    for(i=0;i<6;i++)
    {
        data[i] = ap3216c_read_one_byte(AP3216C_ADDR,AP3216C_IR_DATA_LOW + i);
    }
    if(data[0] & 0x80)
    {
        *ir = 0;
        *ps = 0;
    }
    else
    {
        *ir = ((unsigned int)data[1]<<2) | ((unsigned int)data[0] & 0x03);
        *ps = (((unsigned int)data[5] & 0x3f)<<4) | ((unsigned int)data[4] & 0x0f);
    }
    *als = ((unsigned int)data[3]<<8) | ((unsigned int)data[2]);    
}