#include "bsp_beep.h"
#include "bsp_gpio.h"
void beep_init(void)
{
    gpio_pin_config_t beep_config;
    IOMUXC_SetPinMux(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01,0);
    IOMUXC_SetPinConfig(IOMUXC_SNVS_SNVS_TAMPER1_GPIO5_IO01,0x10b0);
    
    beep_config.direction = kGPIO_DigitalOutput;
    beep_config.outputLogic = 1;
    gpio_init(GPIO5, 1, &beep_config);
}
void beep_switch(int beep_status)
{
    if(beep_status==ON)
    {
        gpio_pin_wirte(GPIO5, 1, 0);
    }
    else if(beep_status==OFF)
    {
        gpio_pin_wirte(GPIO5, 1 , 1);
    }
}
