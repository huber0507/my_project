#include "bsp_clk.h"


/*
 * @description	: 使能I.MX6U所有外设时钟
 * @param 		: 无
 * @return 		: 无
 */
void clk_enable(void)
{
	CCM->CCGR0 = 0XFFFFFFFF;
	CCM->CCGR1 = 0XFFFFFFFF;
	CCM->CCGR2 = 0XFFFFFFFF;
	CCM->CCGR3 = 0XFFFFFFFF;
	CCM->CCGR4 = 0XFFFFFFFF;
	CCM->CCGR5 = 0XFFFFFFFF;
	CCM->CCGR6 = 0XFFFFFFFF;
}

void imx6u_clkinit(void)
{
	unsigned int reg =0;
	if((((CCM->CCSR)>>2)&0x1)==0)
	{
		CCM->CCSR &= ~(1<<8);
		CCM->CCSR |=(1<<2);
	}
	CCM_ANALOG->PFD_480=reg;

	CCM->CBCMR &= ~(3<<18);
	CCM->CBCMR |=(1<<18);
	CCM->CBCDR &=~(1<<25);
	while (CCM->CDHIPR &(1<<25));
	
	#if 0
	CCM->CBCDR &=~(7<<10);
	CCM->CBCDR |= 2<<10;
	while(CCM->CDHIPR&(1<<1))	;
	#endif

	CCM->CBCDR &=~(3<<8);
	CCM->CBCDR |=1<<8;
	CCM->CSCMR1 &=~(1<<6);
	CCM->CSCMR1 &=~(7<<0);

}