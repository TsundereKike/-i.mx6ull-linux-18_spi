#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H
#include "MCIMX6Y2.h"
#include "fsl_common.h"
#include "fsl_iomuxc.h"
#include "cc.h"
/*描述中断触发类型*/
typedef enum _gpio_int_mode
{
    kGPIO_NO_INT_MODE = 0U,                  /*无中断功能*/
    kGPIO_INT_LOW_LEVEL = 1U,                /*低电平触发*/
    kGPIO_INT_HIGH_LEVEL = 2U,               /*高电平触发*/
    kGPIO_INT_RISING_EDGE = 3U,              /*上升沿触发*/
    kGPIO_INT_FALLING_EDGE = 4U,             /*下降沿触发*/
    kGPIO_INT_RISING_OR_FALLING_EDGE = 5U,   /*上升沿和下降沿均触发*/
}gpio_int_mode_t;

typedef enum _gpio_pin_direction
{
    kGPIO_DigitalInput = 0U,
    kGPIO_DigitalOutput = 1U,
}gpio_pin_direction_t;

typedef struct _gpio_pin_config
{
    gpio_pin_direction_t direction;
    gpio_int_mode_t int_mode;
    uint8_t outputLogic;
}gpio_pin_config_t;
void gpio_init(GPIO_Type *base, int pin, gpio_pin_config_t *config);
void gpio_pin_wirte(GPIO_Type *base, int pin, int value);
int gpio_pin_read(GPIO_Type *base, int pin);
void gpio_enable_int(GPIO_Type *base, unsigned int pin);
void gpio_disable_int(GPIO_Type *base, unsigned int pin);
void gpio_clear_int_flags(GPIO_Type *base, unsigned int pin);
void gpio_int_config(GPIO_Type *base, unsigned int pin , gpio_int_mode_t pin_int_mode);
#endif