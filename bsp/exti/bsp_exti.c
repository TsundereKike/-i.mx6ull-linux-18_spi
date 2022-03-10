#include "bsp_exti.h"
#include "bsp_gpio.h"
#include "bsp_int.h"
#include "bsp_delay.h"
#include "bsp_beep.h"
#include "bsp_epit.h"
void exti_init(void)
{
    gpio_pin_config_t gpio_config;
    IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);

    IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xf080);

    gpio_config.direction = kGPIO_DigitalInput;
    gpio_config.int_mode = kGPIO_INT_FALLING_EDGE;
    gpio_init(GPIO1, 18, &gpio_config);

    GIC_EnableIRQ(GPIO1_Combined_16_31_IRQn);
    system_register_irqhandler(GPIO1_Combined_16_31_IRQn, (system_irq_handler_t)GPIO1_IO18_irqhandler, NULL);
    gpio_enable_int(GPIO1, 18);
}
void GPIO1_IO18_irqhandler(unsigned int gicciar,void *param)
{
    epit1_restart(66000000/100);/*重启定时器用于按键消抖*/
    /*清除中断标志位*/
    gpio_clear_int_flags(GPIO1, 18);
}