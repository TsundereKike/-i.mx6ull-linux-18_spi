#include "bsp_gpio.h"
void gpio_init(GPIO_Type *base, int pin, gpio_pin_config_t *config)
{
    gpio_disable_int(base,pin);
    if(config->direction == kGPIO_DigitalInput)
    {
        base->GDIR &= ~(1<<pin);
    }
    else
    {
        base->GDIR |= (1<<pin);
        gpio_pin_wirte(base, pin, config->outputLogic);
    }
    gpio_int_config(base, pin, config->int_mode);
}
void gpio_pin_wirte(GPIO_Type *base, int pin, int value)
{
    if(value==0U)
    {
        base->DR &= ~(1U<<pin);
    }
    else
    {
        base->DR |= (1U<<pin);
    }
    
}
int gpio_pin_read(GPIO_Type *base, int pin)
{
    return (((base->DR) >> pin) & 0x01);
}
/*使能指定IO中断*/
void gpio_enable_int(GPIO_Type *base, unsigned int pin)
{
    base->IMR |= (1<<pin);
}
/*禁止指定IO中断*/
void gpio_disable_int(GPIO_Type *base, unsigned int pin)
{
    base->IMR &= ~(1<<pin);
}
/*清除中断标志位*/
void gpio_clear_int_flags(GPIO_Type *base, unsigned int pin)
{
    base->ISR |= (1<<pin);
}
/*GPIO中断初始化函数*/
void gpio_int_config(GPIO_Type *base, unsigned int pin, gpio_int_mode_t pin_int_mode)
{
    volatile uint32_t *icr;
    uint32_t icrShift;
    base->EDGE_SEL &= ~(1<<pin);
    icrShift = pin;
    if(pin<16)
    {
        icr = &(base->ICR1);
    }
    else
    {
        icr = &(base->ICR2);
        icrShift -=16;
    }
    switch (pin_int_mode)
    {
        case kGPIO_INT_LOW_LEVEL:
            *icr &= ~(3<<(2*icrShift));
            break;
        case kGPIO_INT_HIGH_LEVEL:
            *icr &= ~(3<<(2*icrShift));
            *icr |= (1<<(2*icrShift));
            break;
        case kGPIO_INT_RISING_EDGE:
            *icr &= ~(3<<(2*icrShift));
            *icr |= (2<<(2*icrShift));
            break;
        case kGPIO_INT_FALLING_EDGE:
            *icr &= ~(3<<(2*icrShift));
            *icr |= (3<<(2*icrShift));
            break;
        case kGPIO_INT_RISING_OR_FALLING_EDGE:
            base->EDGE_SEL |= (1<<pin);
            break;
        default:
            break;
    }
}