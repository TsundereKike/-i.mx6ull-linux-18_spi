#include "bsp_icm20608.h"
#include "bsp_spi.h"
#include "bsp_delay.h"
#include "stdio.h"
icm20608_dev_struct icm20608_imu_data;
/*初始化icm20608*/
unsigned char icm20608_init(void)
{
    unsigned char reg = 0;
    /*SPI引脚初始化*/
    IOMUXC_SetPinMux(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK,0);/*复用为SPI3_SCLK*/
    IOMUXC_SetPinMux(IOMUXC_UART2_CTS_B_ECSPI3_MOSI,0);/*复用为SPI3_MOSI*/
    IOMUXC_SetPinMux(IOMUXC_UART2_RTS_B_ECSPI3_MISO,0);/*复用为SPI3_MISO*/

    IOMUXC_SetPinConfig(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK,0x10b1);
    IOMUXC_SetPinConfig(IOMUXC_UART2_CTS_B_ECSPI3_MOSI,0x10b1);
    IOMUXC_SetPinConfig(IOMUXC_UART2_RTS_B_ECSPI3_MISO,0x10b1);

    /*片选引脚SS0初始化*/
    IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_GPIO1_IO20,0);/*复用为GPIO1_IO20*/
    IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_GPIO1_IO20,0x10b0);

    gpio_pin_config_t spi_ss0_gpio_config;
    spi_ss0_gpio_config.direction = kGPIO_DigitalOutput;
    spi_ss0_gpio_config.outputLogic = 0;
    gpio_init(GPIO1,20,&spi_ss0_gpio_config);

    /*SPI初始化*/
    spi_init(ECSPI3);

    /*icm20608初始化寄存器配置*/
    icm20608_write_data(ICM20_PWR_MGMT_1, 0x80);/*复位icm20608,复位后为睡眠模式*/
    delay_ms(50);
    icm20608_write_data(ICM20_PWR_MGMT_1, 0x01);/*关闭睡眠模式，自动选择时钟*/
    delay_ms(50);

    reg = icm20608_read_data(ICM20_WHO_AM_I);
    printf("icm20608's ID = %#x\r\n",reg);
    if((reg!=ICM_20608G_ID) && (reg!=ICM_20608D_ID))
        return 1;
    icm20608_write_data(ICM20_SMPLRT_DIV, 0x00);/*输出速率设置为内部采样率*/
    icm20608_write_data(ICM20_GYRO_CONFIG, 0x18);/*陀螺仪量程±2000dps*/
    icm20608_write_data(ICM20_ACCEL_CONFIG, 0x18);/*加速度计量程±16G*/
    icm20608_write_data(ICM20_CONFIG, 0X04);/*陀螺仪低通滤波器20Hz*/
    icm20608_write_data(ICM20_ACCEL_CONFIG2, 0X04);/*加速度低通滤波21.2Hz*/
    icm20608_write_data(ICM20_PWR_MGMT_2,0x00);/*打开加速度计和陀螺仪所有轴*/
    icm20608_write_data(ICM20_LP_MODE_CFG, 0x00);/*关闭低功耗*/
    icm20608_write_data(ICM20_FIFO_EN,0x00);/*关闭FIFO*/

    return 0;
}
/*icm20608读寄存器数据*/
unsigned char icm20608_read_data(unsigned char reg)
{
    unsigned char read_val = 0;
    reg |= 0x80;/*寄存器的bit7置1,表示读取数据*/
    /*片选拉低*/
    ICM20608_CSN(0);
    spi_ch0_read_write_data(ECSPI3,reg);/*发送要写入的寄存器地址*/

    read_val = spi_ch0_read_write_data(ECSPI3,0xff);/*读取从机返回的数据*/

    /*片选拉高*/
    ICM20608_CSN(1);
    return read_val;
}
/*icm20608写寄存器数据*/
void icm20608_write_data(unsigned char reg,unsigned char data)
{
    reg &= ~0x80;/*寄存器的bit7置0,表示写数据*/
    /*片选拉低*/
    ICM20608_CSN(0);

    spi_ch0_read_write_data(ECSPI3,reg);
    spi_ch0_read_write_data(ECSPI3,data);

    /*片选拉高*/
    ICM20608_CSN(1);
}
/*icm20608读取多个寄存器数据*/
void icm20608_read_len_data(unsigned char reg,unsigned char *buf,unsigned char len)
{
    unsigned char i=0;

    reg |= 0x80;
    ICM20608_CSN(0);
    spi_ch0_read_write_data(ECSPI3,reg);
    for(i=0;i<len;i++)
    {
        buf[i] = spi_ch0_read_write_data(ECSPI3,0xFF);
    }
    ICM20608_CSN(1);
}
/*获取陀螺仪加速度数据*/
void icm20608_get_data(void)
{
    unsigned char data[14] = {0};

    icm20608_read_len_data(ICM20_ACCEL_XOUT_H,data,14);
    icm20608_imu_data.accel_x_adc = (int16_t)(data[0]<<8 | data[1]);
    icm20608_imu_data.accel_y_adc = (int16_t)(data[2]<<8 | data[3]);
    icm20608_imu_data.accel_z_adc = (int16_t)(data[4]<<8 | data[5]);
    icm20608_imu_data.temp_adc    = (int16_t)(data[6]<<8 | data[7]);
    icm20608_imu_data.gyro_x_adc = (int16_t)(data[8]<<8 | data[9]);
    icm20608_imu_data.gyro_y_adc = (int16_t)(data[10]<<8 | data[11]);
    icm20608_imu_data.gyro_z_adc = (int16_t)(data[12]<<8 | data[13]);
}