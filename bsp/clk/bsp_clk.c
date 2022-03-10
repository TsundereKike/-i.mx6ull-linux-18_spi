#include "bsp_clk.h"
/*
*@brief: 使能i.mx6ull外设时钟
*@param in:  NULL
*@param out: NULL
*@ret:    NULL
*/
void clk_enable(void)
{
    CCM->CCGR0 = 0xffffffff;
    CCM->CCGR1 = 0xffffffff;
    CCM->CCGR2 = 0xffffffff;
    CCM->CCGR3 = 0xffffffff;
    CCM->CCGR4 = 0xffffffff;
    CCM->CCGR5 = 0xffffffff;
    CCM->CCGR6 = 0xffffffff;
}
void imx6u_clk_init(void)
{
    unsigned int reg = 0;
    /*1：设置PLL1(ARM_PLL)为1056MHz及ARM内核时钟(ARM_CLK_ROOT)为528MHz*/
    if(((CCM->CCSR) & 0x4)==0) /*pll_main_clk*/
    {
        CCM->CCSR &= ~(1<<8); /*配置step_clk时钟源为24MHz OSC*/
        CCM->CCSR |= (1<<2); /*配置pll_sw_clk时钟源为step_clk*/
    }
    CCM_ANALOG->PLL_ARM = (1<<13) | ((88<<0) & 0x7f); /*设置pll_main_clk为1056MHz*/

    CCM->CACRR = 0x1; /*ARM内核时钟设置为pll_sw_clk时钟的2分频 = 1056MHz/2 = 528MHz*/
    CCM->CCSR &= ~(1<<2); /*配置pll_sw_clk时钟源为pll_main_clk*/

    /*2：设置PLL2(SYS_PLL)及4路PFD*/
    reg = CCM_ANALOG->PFD_528;
    reg &= ~(0x3f3f3f3f);
    reg |= (32<<24);                    /*PLL2_PFD3为297MHz*/
    reg |= (24<<16);                    /*PLL2_PFD2为396MHz*/
    reg |= (16<<8);                     /*PLL2_PFD1为594MHz*/
    reg |= (27<<0);                     /*PLL2_PFD0为352MHz*/
    CCM_ANALOG->PFD_528 = reg;

    /*3:设置PLL3(USB1_PLL)及4路PFD*/
    reg = 0;
    reg = CCM_ANALOG->PFD_480;
    reg &= ~(0x3f3f3f3f);
    reg |= (19<<24);                    /*PLL3_PFD3为454.7MHz*/
    reg |= (17<<16);                    /*PLL3_PFD2为508.2MHz*/
    reg |= (16<<8);                     /*PLL3_PFD1为540MHz*/
    reg |= (12<<0);                     /*PLL3_PFD0为720MHz*/
    CCM_ANALOG->PFD_480 = reg;

    /*4:设置AHB_CLK_ROOT为132MHz*/
    CCM->CBCMR &= ~(3<<18);
    CCM->CBCMR |= (1<<18);

    CCM->CBCDR &= ~(1<<25);
    while((CCM->CDHIPR) & (1<<5));/*等待握手信号*/

    CCM->CBCDR &= ~(7<<10);
    CCM->CBCDR |= (2<<10);        /*三分频*/
    while((CCM->CDHIPR) & (1<<1));/*等待握手信号*/

    /*5:设置IPG_CLK_ROOT为66MHz*/
    CCM->CBCDR &= ~(3<<8);
    CCM->CBCDR |= (1<<8);         /*IPG_CLK_ROOT = AHB_CLK_ROOT/2 = 66MHz*/

    /*6:设置PER_CLK_ROOT为66MHz*/
    CCM->CSCMR1 &= ~(1<<6);
    CCM->CSCMR1 &= ~(0x3f<<0);    /*一分频，PER_CLK_ROOT = IPG_CLK_ROOT/1 = 66MHz*/

    /*设置UART_CLK_ROOT为480MHz*/
    CCM->CCSR &= ~(1<<0);         /*设置pll3_sw_clk = PLL3 = 480MHz*/
    CCM->CSCDR1 &= ~(1<<6);       /*设置UART_CLK_ROOT时钟源选择为pll3_sw_clk/6 = 80MHz*/
    CCM->CSCDR1 &= ~(0x3f<<0);    /*设置pll3_sw_clk/6的时钟1分频，则UART_CLK_ROOT = 80MHz*/
}
