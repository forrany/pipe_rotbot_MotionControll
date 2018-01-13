#include "timer.h"
__IO uint32_t TimingDelay;   //相当于Voilate
void SysTick_Init(void)
{
	if(SysTick_Config(SystemCoreClock /100000))  //每10us进一次中断
	{
		while(1);
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   //先关闭定时器
}

void Dealy_us( __IO u32 nTime )
{
	TimingDelay=nTime;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while(TimingDelay!=0);
	
}
