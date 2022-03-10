#include "imx6u.h"
#include "bsp_clk.h"
#include "bsp_led.h"
#include "bsp_delay.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_exti.h"
#include "bsp_epit.h"
#include "bsp_uart.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "bsp_lcdapi.h"
#include "bsp_rtc.h"
#include "bsp_i2c.h"
#include "bsp_ap3216c.h"
#include "bsp_spi.h"
#include "bsp_icm20608.h"
unsigned char led_state = OFF;
char buf[160];
rtc_date_time_t rtcdate;
unsigned int IR, PS,ALS;
int main(void)
{

    int_init();
    imx6u_clk_init();
    clk_enable();
    delay_init();
    uart_init();
    led_init();
    beep_init();
    key_init();
    exti_init();
    epit1_init(0,66000000/100);/*初始化定时器1,1分频66MHz,定时时间为10ms,用于按键消抖*/
    lcd_init();
    rtc_init();
    icm20608_init();

    lcd_show_string(10,10,360,32,32,"this tmh's spi test");
    memset(buf,0,sizeof(buf));
    
    while(ap3216_init())
    {
        printf("please check ap3216c connect!!!");
        lcd_show_string(10,43,400,32,32,"please check ap3216c connect!!!");
    }
	while(1)					
	{	
		rtc_get_date_time(&rtcdate);
		sprintf(buf,"%d/%d/%d %d:%d:%d",rtcdate.year, rtcdate.month, rtcdate.day, rtcdate.hour, rtcdate.minute, rtcdate.second);
        lcd_fill(50,110,300,126,LCD_WHITE);
        lcd_show_string(50, 110, 250, 16, 16,(char*)buf);  /* 显示字符串 */
		

        ap3216c_data_get(&IR,&ALS,&PS);
        sprintf(buf,"IR=%dALS=%dPS=%d",IR, ALS, PS);
        lcd_fill(50,130,300,146,LCD_WHITE);
        lcd_show_string(50, 130, 250, 16, 16,(char*)buf);  /* 显示字符串 */

        led_state = !led_state;
        led_switch(LED0,led_state);
        icm20608_get_data();
        printf("accx=%d\r\n",icm20608_imu_data.accel_x_adc);
        printf("accy=%d\r\n",icm20608_imu_data.accel_y_adc);
        printf("accz=%d\r\n",icm20608_imu_data.accel_z_adc);
        printf("gyrx=%d\r\n",icm20608_imu_data.gyro_x_adc);
        printf("gyry=%d\r\n",icm20608_imu_data.gyro_y_adc);
        printf("gyrz=%d\r\n",icm20608_imu_data.gyro_z_adc);
        delay_ms(100);
    }
    return 0;
}
