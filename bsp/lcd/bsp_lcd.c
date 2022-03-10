#include "bsp_lcd.h"
#include "bsp_gpio.h"
#include "stdio.h"
#include "bsp_delay.h"
/*屏幕参数结构体变量*/
lcd_tft_config_t lcd_tft_dev;
void lcd_init(void)
{
    unsigned short lcd_id = 0;
    lcd_id = lcd_panelID_read();
    printf("LCD ID = %#x\r\n",lcd_id);

    /*初始化屏幕IO*/
    lcd_gpio_init();
    lcd_reset();
    delay_ms(10);
    lcd_noreset();

    /*根据不同的屏幕ID设置屏幕参数*/
    if(lcd_id == ATK4342)
    {
        lcd_tft_dev.height = 272;
        lcd_tft_dev.width = 480;
        lcd_tft_dev.vspw = 1;
        lcd_tft_dev.vbpd = 8;
        lcd_tft_dev.vfpd = 8;
        lcd_tft_dev.hspw = 1;
        lcd_tft_dev.hbpd = 40;
        lcd_tft_dev.hfpd = 5;
        lcd_clock_init(27,8,8);/*设置10.1MHz的LCD时钟*/
    }
    else if(lcd_id == ATK4384 )
    {
        lcd_tft_dev.height = 480;
        lcd_tft_dev.width = 800;
        lcd_tft_dev.vspw = 3;
        lcd_tft_dev.vbpd = 32;
        lcd_tft_dev.vfpd = 13;
        lcd_tft_dev.hspw = 48;
        lcd_tft_dev.hbpd = 88;
        lcd_tft_dev.hfpd = 40;
        lcd_clock_init(42,4,8);/*设置31.5MHz的LCD时钟*/
    }
    else if(lcd_id == ATK7084)
    {
        lcd_tft_dev.height = 480;
        lcd_tft_dev.width = 800;
        lcd_tft_dev.vspw = 1;
        lcd_tft_dev.vbpd = 23;
        lcd_tft_dev.vfpd = 22;
        lcd_tft_dev.hspw = 1;
        lcd_tft_dev.hbpd = 46;
        lcd_tft_dev.hfpd = 210;
        lcd_clock_init(30,3,7);/*设置34.2MHz的LCD时钟*/
    }
    else if(lcd_id == ATK7016)
    {
        lcd_tft_dev.height = 600;
        lcd_tft_dev.width = 1024;
        lcd_tft_dev.vspw = 3;
        lcd_tft_dev.vbpd = 20;
        lcd_tft_dev.vfpd = 12;
        lcd_tft_dev.hspw = 20;
        lcd_tft_dev.hbpd = 140;
        lcd_tft_dev.hfpd = 160;
        lcd_clock_init(32,3,5);/*设置51.2MHz的LCD时钟*/
    }
    lcd_tft_dev.pixsize = 4;    /*每个像素4个字节*/
    lcd_tft_dev.framebuffer = LCD_FRAMEBUFF_ADDR;
    lcd_tft_dev.forecolor = LCD_BLACK;  /*前景色为黑色*/
    lcd_tft_dev.backcolor = LCD_WHITE;  /*背景色为白色*/
    
    /*配置LCDIF控制器接口*/
    LCDIF->CTRL = 0;
    LCDIF->CTRL |= (1<<5) | (3<<8) | (3<<10) | (1<<17) | (1<<19);

    LCDIF->CTRL1 = 0;
    LCDIF->CTRL1 |= (7<<16);

    LCDIF->TRANSFER_COUNT = 0;
    LCDIF->TRANSFER_COUNT = (lcd_tft_dev.height<<16) | (lcd_tft_dev.width<<0);

    LCDIF->VDCTRL0 = 0;
    LCDIF->VDCTRL0 = (lcd_tft_dev.vspw<<0) | (1<<20)
                     | (1<<21) | (1<<24) | (0<<25) | (0<<26) 
                     | (0<<27) | (1<<28) | (0<<29);

    LCDIF->VDCTRL1 = lcd_tft_dev.vspw + lcd_tft_dev.height + lcd_tft_dev.vfpd 
                     + lcd_tft_dev.vbpd;
    
    LCDIF->VDCTRL2 = (lcd_tft_dev.hspw + lcd_tft_dev.width + lcd_tft_dev.hbpd 
                     + lcd_tft_dev.hfpd) | (lcd_tft_dev.hspw<<18);

    LCDIF->VDCTRL3 = (lcd_tft_dev.vspw + lcd_tft_dev.vbpd) 
                     | (lcd_tft_dev.hspw + lcd_tft_dev.hbpd)<<16;
            
    LCDIF->VDCTRL4 = (lcd_tft_dev.width) | (1<<18);

    LCDIF->CUR_BUF = (unsigned int)lcd_tft_dev.framebuffer;
    LCDIF->NEXT_BUF = (unsigned int)lcd_tft_dev.framebuffer;

    lcd_enable();
    lcd_clear(LCD_WHITE);
}
/*LCD时钟初始化
 *loopDiv: 设置DIV_SELECT,范围27～54
 *preDiv:  设置为1～8
 *backDiv: 可选范围1～8
 *LCD_CLK = 24 * loopDiv / preDiv / backDiv
 */
void lcd_clock_init(unsigned char loopDiv, unsigned char preDiv, unsigned char backDiv )
{
    CCM_ANALOG->PLL_VIDEO_NUM = 0;
    CCM_ANALOG->PLL_VIDEO_DENOM = 0;
    CCM_ANALOG->PLL_VIDEO = (loopDiv<<0) | (2<<19) | (1<<13);
    CCM_ANALOG->MISC2 &= ~(3<<30);

    CCM->CSCDR2 &= ~(7<<15);
    CCM->CSCDR2 |= (2<<15);

    CCM->CSCDR2 &= ~(7<<12);
    CCM->CSCDR2 |= (preDiv-1)<<12;

    CCM->CBCMR &= ~(7<<23);
    CCM->CBCMR |= (backDiv-1)<<23;

    CCM->CSCDR2 &= ~(7<<9);
    CCM->CSCDR2 |= (0<<9);

}
/*复位LCD控制器*/
void lcd_reset(void)
{
    LCDIF->CTRL |= 1<<31;
}
/*停止复位*/
void lcd_noreset(void)
{
    LCDIF->CTRL &= ~(1<<31);
}
/*使能LCD控制器*/
void lcd_enable(void)
{
    LCDIF->CTRL |= (1<<0);
}
/*
 * 读取屏幕ID，
 * 描述：LCD_DATA23=R7(M0);LCD_DATA15=G7(M1);LCD_DATA07=B7(M2);
 * 		M2:M1:M0
 *		0 :0 :0	//4.3寸480*272 RGB屏,ID=0X4342
 *		0 :0 :1	//7寸800*480 RGB屏,ID=0X7084
 *	 	0 :1 :0	//7寸1024*600 RGB屏,ID=0X7016
 *  	1 :0 :1	//10.1寸1280*800,RGB屏,ID=0X1018
 *		1 :0 :0	//4.3寸800*480 RGB屏,ID=0X4384
 * @param 		: 无
 * @return 		: 屏幕ID
 */
unsigned short lcd_panelID_read(void)
{
    unsigned char IDX = 0;
    /*打开模拟开关，设置LCD_VSYNC为高电平*/
    gpio_pin_config_t lcd_config;
    IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_GPIO3_IO03,0);
    IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_GPIO3_IO03,0x10b0);
    /*GPIO初始化*/
    lcd_config.direction = kGPIO_DigitalOutput;
    lcd_config.outputLogic = 1;
    gpio_init(GPIO3, 3,&lcd_config);

    /*读取屏幕ID*/
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_GPIO3_IO12,0);   /*B7(M2)*/
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_GPIO3_IO20,0);   /*G7(M1)*/
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_GPIO3_IO28,0);   /*R7(M0)*/

    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_GPIO3_IO12,0xf080);   /*B7(M2)*/
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_GPIO3_IO20,0xf080);   /*G7(M1)*/
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_GPIO3_IO28,0xf080);   /*R7(M0)*/
    lcd_config.direction = kGPIO_DigitalInput;
    gpio_init(GPIO3,12,&lcd_config);
    gpio_init(GPIO3,20,&lcd_config);
    gpio_init(GPIO3,28,&lcd_config);

    IDX = (unsigned char)gpio_pin_read(GPIO3,28);
    IDX |= (unsigned char)gpio_pin_read(GPIO3,20)<<1;
    IDX |= (unsigned char)gpio_pin_read(GPIO3,12)<<2;

    if(IDX == 0) return ATK4342;
    else if(IDX == 1) return ATK7084;
    else if(IDX == 2) return ATK7016;
    else if(IDX == 4) return ATK4384;
    else if(IDX == 5) return ATK1018;
    else return 0;
}
/*屏幕IO初始化*/
void lcd_gpio_init(void)
{
    /*IO复用*/
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA00_LCDIF_DATA00,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA01_LCDIF_DATA01,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA02_LCDIF_DATA02,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA03_LCDIF_DATA03,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA04_LCDIF_DATA04,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA05_LCDIF_DATA05,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA06_LCDIF_DATA06,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_LCDIF_DATA07,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA08_LCDIF_DATA08,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA09_LCDIF_DATA09,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA10_LCDIF_DATA10,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA11_LCDIF_DATA11,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA12_LCDIF_DATA12,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA13_LCDIF_DATA13,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA14_LCDIF_DATA14,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_LCDIF_DATA15,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA16_LCDIF_DATA16,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA17_LCDIF_DATA17,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA18_LCDIF_DATA18,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA19_LCDIF_DATA19,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA20_LCDIF_DATA20,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA21_LCDIF_DATA21,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA22_LCDIF_DATA22,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_LCDIF_DATA23,0);

    IOMUXC_SetPinMux(IOMUXC_LCD_CLK_LCDIF_CLK,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_ENABLE_LCDIF_ENABLE,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_HSYNC_LCDIF_HSYNC,0);
    IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_LCDIF_VSYNC,0);
    
    /*IO电气属性
     *bit[16]：       0   HYS关闭
     *bit[15:14]:    0   默认22K下拉
     *bit[13]:       0   pull功能
     *bit[12]:       0   pull/keeper使能
     *bit[11]：      0   关闭开路输出
     *bit[7:6]:     10  速度100MHz
     *bit[5:3]:     111 驱动能力为R0/7
     *bit[0]:       1   高转换率
    */
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA00_LCDIF_DATA00,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA01_LCDIF_DATA01,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA02_LCDIF_DATA02,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA03_LCDIF_DATA03,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA04_LCDIF_DATA04,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA05_LCDIF_DATA05,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA06_LCDIF_DATA06,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_LCDIF_DATA07,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA08_LCDIF_DATA08,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA09_LCDIF_DATA09,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA10_LCDIF_DATA10,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA11_LCDIF_DATA11,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA12_LCDIF_DATA12,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA13_LCDIF_DATA13,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA14_LCDIF_DATA14,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_LCDIF_DATA15,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA16_LCDIF_DATA16,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA17_LCDIF_DATA17,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA18_LCDIF_DATA18,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA19_LCDIF_DATA19,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA20_LCDIF_DATA20,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA21_LCDIF_DATA21,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA22_LCDIF_DATA22,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_LCDIF_DATA23,0xb9);

    IOMUXC_SetPinConfig(IOMUXC_LCD_CLK_LCDIF_CLK,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_ENABLE_LCDIF_ENABLE,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_HSYNC_LCDIF_HSYNC,0xb9);
    IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_LCDIF_VSYNC,0xb9);

    /*背光IO初始化*/
    gpio_pin_config_t lcd_bl_config;
    IOMUXC_SetPinMux(IOMUXC_GPIO1_IO08_GPIO1_IO08,0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO08_GPIO1_IO08,0x10b0);
    lcd_bl_config.direction = kGPIO_DigitalOutput;
    lcd_bl_config.outputLogic = 1;
    gpio_init(GPIO1,8,&lcd_bl_config);

}
/*画点函数*/
inline void lcd_draw_point(unsigned short x, unsigned short y, unsigned int color)
{
    *(unsigned int *)((unsigned int)lcd_tft_dev.framebuffer 
                                    + lcd_tft_dev.pixsize*(lcd_tft_dev.width*y+x)) = color;

}
/*读点函数*/
inline unsigned int lcd_read_point(unsigned short x, unsigned short y)
{
    return *(unsigned int *)((unsigned int)lcd_tft_dev.framebuffer 
                                    + lcd_tft_dev.pixsize*(lcd_tft_dev.width*y+x));
}
/*清屏函数*/
void lcd_clear(unsigned int color)
{
    unsigned int num;
    unsigned int i = 0;

    unsigned int *start_addr = (unsigned int *)lcd_tft_dev.framebuffer;

    num = lcd_tft_dev.height * lcd_tft_dev.width;

    for(i=0;i<num;i++)
    {
        start_addr[i] = color;
    }
}
/*
 * @description		: 以指定的颜色填充一块矩形
 * @param - x0		: 矩形起始点坐标X轴
 * @param - y0		: 矩形起始点坐标Y轴
 * @param - x1		: 矩形终止点坐标X轴
 * @param - y1		: 矩形终止点坐标Y轴
 * @param - color	: 要填充的颜色
 * @return 			: 读取到的指定点的颜色值
 */
void lcd_fill(unsigned    short x0, unsigned short y0, 
                 unsigned short x1, unsigned short y1, unsigned int color)
{ 
    unsigned short x, y;

	if(x0 < 0) x0 = 0;
	if(y0 < 0) y0 = 0;
	if(x1 >= lcd_tft_dev.width) x1 = lcd_tft_dev.width - 1;
	if(y1 >= lcd_tft_dev.height) y1 = lcd_tft_dev.height - 1;
	
    for(y = y0; y <= y1; y++)
    {
        for(x = x0; x <= x1; x++)
			lcd_draw_point(x, y, color);
    }
}

