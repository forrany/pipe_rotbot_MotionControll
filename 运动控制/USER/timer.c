#include "timer.h"
__IO uint32_t TimingDelay;   //�൱��Voilate
void SysTick_Init(void)
{
	if(SysTick_Config(SystemCoreClock /100000))  //ÿ10us��һ���ж�
	{
		while(1);
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;   //�ȹرն�ʱ��
}

void Dealy_us( __IO u32 nTime )
{
	TimingDelay=nTime;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while(TimingDelay!=0);
	
}
