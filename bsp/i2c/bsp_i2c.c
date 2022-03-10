#include "bsp_i2c.h"
/*初始化i2c*/
void i2c_init(I2C_Type *base)
{
    base->I2CR &= ~(1<<7);/*关闭i2c*/
    base->IFDR = 0x15;/*PERCLK_CLK_ROOT时钟640分频 = 66MHz/640 = 103.125KHz*/
    base->I2CR |= (1<<7);/*开启i2c*/
    //base->I2CR &= ~(1<<6);/*关闭中断产生*/
}
/*start信号产生以及从机地址的发送*/
unsigned int i2c_master_start(I2C_Type *base, unsigned char address, enum i2c_transfer_direction direction)
{
    if(base->I2SR & (1<<5))
        return I2C_STATUS_BUSY;
    /*设置为主机发送模式*/
    base->I2CR |= (1<<5) | (1<<4);
    /*产生start信号*/
    base->I2DR = ((unsigned int)address << 1) | ((direction == kI2C_Read)?1:0);
    return I2C_STATUS_OK;
}
/*stop信号的产生*/
unsigned int i2c_master_stop(I2C_Type *base)
{
    unsigned short timeout = 0xffff;
    /*清除I2CR寄存器的bit5:3*/
    base->I2CR &= ~((1<<5)|(1<<4)|(1<<3));
    /*等待i2c忙结束*/
    while((base->I2SR & (1<<5)))
    {
        timeout--;
        if(timeout==0)
            return I2C_STATUS_TIMEOUT;
    }
    return I2C_STATUS_OK;
}
/*repeat start信号产生*/
unsigned int i2c_master_repeat_start(I2C_Type *base, unsigned char address, enum i2c_transfer_direction direction)
{
    /*检测I2C是否忙且处于从机模式下*/
    if((base->I2SR & (1<<5)) && ((base->I2CR & (1<<5))==0))
        return I2C_STATUS_BUSY;
    /*设置为发送模式及产生repeat_start信号*/
    base->I2CR |= (1<<4) | (1<<2);
    base->I2DR = ((unsigned int)address << 1) | ((direction==kI2C_Read)?1:0);
    return I2C_STATUS_OK;
}
/*错误检查与清除*/
unsigned int i2c_check_and_clear_error(I2C_Type *base, unsigned int status)
{
    if(status & (1<<4))/*先检查是否为仲裁丢失*/
    {
        base->I2SR &= ~(1<<4);/*清除仲裁丢失位*/
        base->I2CR &= ~(1<<7);/*关闭i2c*/
        base->I2CR |= ~(1<<7);/*开启i2c*/
        return I2C_STATUS_ARBITRATIONAL_LOST;
    }
    else if(status & (1<<0))/*从机未应答*/
    {
        return I2C_STATUS_NAK;
    }
    else
    {
        return I2C_STATUS_OK;
    }
}
/*发送数据*/
void i2c_master_write(I2C_Type *base, const unsigned char *buf, unsigned int size)
{
    /*等待传输完成*/
    while((base->I2SR & (1<<7))==0);

    base->I2SR &= ~(1<<1);/*清除标志位*/

    base->I2CR |= (1<<4);/*i2c发送模式*/

    while(size--)
    {
        base->I2DR = *buf++;/*将要传输的字节数据写入I2DR寄存器*/

        while((base->I2SR & (1<<1))==0);/*等待传输完成*/

        base->I2SR &= ~(1<<1);/*清除标志位*/

        /*检查ACK信号*/
        if(i2c_check_and_clear_error(base, base->I2SR))
            break;
    }
    base->I2SR &= ~(1<<1);/*清除标志位*/
    i2c_master_stop(base);
}
/*i2c读数据*/
void i2c_master_read(I2C_Type *base, unsigned char *buf, unsigned int size)
{
     volatile uint8_t dummy = 0;/*用于假读*/
     dummy++;

     while((base->I2SR & (1<<7))==0);/*等待发送完成*/

     base->I2SR &= ~(1<<1);/*清除标志位*/
     base->I2CR &= ~((1<<3) | (1<<4));/*接收模式*/
     if(size==1)/*只接一字节数据*/
     {
         base->I2CR |= (1<<3);/*告诉从机接收完该字节后的第九个时钟沿不用回复ACK*/
     }

     dummy = base->I2DR;/*假读*/

     while(size--)
     {
         while((base->I2SR & (1<<1))==0);/*等待数据传输完成*/
         base->I2SR &= ~(1<<1);/*清除标志位*/

         if(size==0)/*数据接收完成*/
         {
            i2c_master_stop(base);
         }
         if(size==1)/*最后一个字节数据*/
         {
            base->I2CR |= (1<<3);/*告诉从机接收完该字节后的第九个时钟沿不用回复ACK*/
         }
         *buf++ = base->I2DR;
     }  
}
/*
 * @description	: I2C数据传输，包括读和写
 * @param - base: 要使用的IIC
 * @param - xfer: 传输结构体
 * @return 		: 传输结果,0 成功，其他值 失败;
 */
unsigned char i2c_master_transfer(I2C_Type *base, struct i2c_transfer *xfer)
{
	unsigned char ret = 0;
	 enum i2c_transfer_direction direction = xfer->direction;	

	base->I2SR &= ~((1 << 1) | (1 << 4));			/* 清除标志位 */

	/* 等待传输完成 */
	while(!((base->I2SR >> 7) & 0X1)){}; 

	/* 如果是读的话，要先发送寄存器地址，所以要先将方向改为写 */
    if ((xfer->subaddressSize > 0) && (xfer->direction == kI2C_Read))
    {
        direction = kI2C_Write;
    }

	ret = i2c_master_start(base, xfer->slaveAddress, direction); /* 发送开始信号 */
    if(ret)
    {	
		return ret;
	}

	while(!(base->I2SR & (1 << 1))){};			/* 等待传输完成 */

    ret = i2c_check_and_clear_error(base, base->I2SR);	/* 检查是否出现传输错误 */
    if(ret)
    {
      	i2c_master_stop(base); 						/* 发送出错，发送停止信号 */
        return ret;
    }
	
    /* 发送寄存器地址 */
    if(xfer->subaddressSize)
    {
        do
        {
			base->I2SR &= ~(1 << 1);			/* 清除标志位 */
            xfer->subaddressSize--;				/* 地址长度减一 */
			
            base->I2DR =  ((xfer->subaddress) >> (8 * xfer->subaddressSize)); //向I2DR寄存器写入子地址
  
			while(!(base->I2SR & (1 << 1)));  	/* 等待传输完成 */

            /* 检查是否有错误发生 */
            ret = i2c_check_and_clear_error(base, base->I2SR);
            if(ret)
            {
             	i2c_master_stop(base); 				/* 发送停止信号 */
             	return ret;
            }  
        } while ((xfer->subaddressSize > 0) && (ret == I2C_STATUS_OK));

        if(xfer->direction == kI2C_Read) 		/* 读取数据 */
        {
            base->I2SR &= ~(1 << 1);			/* 清除中断挂起位 */
            i2c_master_repeat_start(base, xfer->slaveAddress, kI2C_Read); /* 发送重复开始信号和从机地址 */
    		while(!(base->I2SR & (1 << 1))){};/* 等待传输完成 */

            /* 检查是否有错误发生 */
			ret = i2c_check_and_clear_error(base, base->I2SR);
            if(ret)
            {
             	ret = I2C_STATUS_ADDRNAK;
                i2c_master_stop(base); 		/* 发送停止信号 */
                return ret;  
            }
           	          
        }
    }	


    /* 发送数据 */
    if ((xfer->direction == kI2C_Write) && (xfer->dataSize > 0))
    {
    	i2c_master_write(base, xfer->data, xfer->dataSize);
	}

    /* 读取数据 */
    if ((xfer->direction == kI2C_Read) && (xfer->dataSize > 0))
    {
       	i2c_master_read(base, xfer->data, xfer->dataSize);
	}
	return 0;	
}
