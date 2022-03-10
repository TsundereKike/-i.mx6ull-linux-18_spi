#include "bsp_spi.h"
/*SPI初始化*/
void spi_init(ECSPI_Type *base)
{
    base->CONREG = 0;
    /*bit[0] = 1:使能SPI、
     *bit[3] = 1：写数据给TXFIFO时马上产生一次SPI传输、
     *bit[7:4] = 0001：SPI ch0设置为主机模式
     *bit[19:18] = 00：SPI通道选择为通道0
     *bit[31:20] = 0x007：SPI突发访问字节长度为8位*/
    base->CONREG |= (1<<0) | (1<<3) | (1<<4) | (0<<18) | (7<<20);

    /*bit[3:0] = 0000：CLK时钟第一个跳变沿开始进行采样
     *bit[7:4] = 0000: CLK时钟线空闲时为低电平
     *bit[11:8] = 0000: 主机模式下传输一次数据，仅进行一次SPI突发
     *bit[15:12] = 0000: 片选SS选中时为低电平
     *bit[19:16] = 0000: 数据线空闲时为地电平
     *bit[23:20] = 0000: 时钟线空闲时为低电平
    */
    base->CONFIGREG  = 0;

    /*bit[14:0] = 0x2000: 设置SPI的wait state状态时间
     *bit[15] = 0: 设置SPI时钟来源为SPL_CLK*/
    base->PERIODREG  = 0x2000;

    /*SPI时钟配置，最高时钟为8MHz*/
    base->CONREG &= ~(0xff<<8);/*清除CONREG寄存器的bit[15:8]*/
    base->CONREG |= (9<<12);/*前级分频，SPL_CLK = ECSPI_CLK_ROOT/10 = 6MHz*/
    base->CONREG |= (0<<8);/*一分频*/
}
/*SPI接收发送数据*/
unsigned char spi_ch0_read_write_data(ECSPI_Type *base, unsigned char data)
{
    uint32_t spi_rx_data = 0;
    uint32_t spi_tx_data = data;

    /*选择通道0*/
    base->CONREG &= ~(3<<18);
    base->CONREG |= (0<<18);

    /*数据发送*/
    while(!(base->STATREG&(1<<0)));/*等待TXFIFO空*/
    base->TXDATA = spi_tx_data;

    /*数据接收*/
    while(!(base->STATREG&(1<<3)));/*等待RXFIFO非空*/
    spi_rx_data = base->RXDATA;
    return spi_rx_data;
}