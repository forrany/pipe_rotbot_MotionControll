#include "stm32f10x.h"


void NVIC_Configuration(void){
	NVIC_InitTypeDef NVIC_InitStructure;//定义结构体
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; //使能外部中断所在的通道

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级0 

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //子优先级 0

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道 

	NVIC_Init(&NVIC_InitStructure); //根据结构体信息进行优先级初始化
	
	
}
/* 

初始化A15的配置，这里的配置将使得PA15可以当作正常IO口使用，然后PA15作为上拉输入口，成为进入中断的条件

*/
void Key_INIT(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	NVIC_Configuration();
	 /**********************
	 1.执行端口重映射时,复用功能时钟得使能:RCC_APB2Periph_AFIO
	 
	 2.  &1.GPIO_Remap_SWJ_Disable: !< Full SWJ Disabled (JTAG-DP + SW-DP)
				此时PA13|PA14|PA15|PB3|PB4都可作为普通IO用了
	 
	 为了保存某些调试端口,GPIO_Remap_SWJ_Disable也可选择为下面两种模式：
		
			 &2.GPIO_Remap_SWJ_JTAGDisable: !< JTAG-DP Disabled and SW-DP Enabled
			 此时PA15|PB3|PB4可作为普通IO用了
		
			 &3.GPIO_Remap_SWJ_NoJTRST: !< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST
			 此时只有PB4可作为普通IO用了 
	 **********************/
	 
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //使能禁止JTAG

	 //初始化GPIOB  悬空输入
	 GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_15);
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	 GPIO_Init(GPIOA, &GPIO_InitStructure); 
	 
	 //EXTI 配置
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	 EXTI_InitStructure.EXTI_Line=EXTI_Line15; //中断线的标号 取值范围为EXTI_Line0~EXTI_Line15
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式，可选值为中断 EXTI_Mode_Interrupt 和事件 EXTI_Mode_Event。
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//触发方式，可以是下降沿触发 EXTI_Trigger_Falling，上升沿触发 EXTI_Trigger_Rising，或者任意电平（上升沿和下降沿）触发EXTI_Trigger_Rising_Falling
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);//根据结构体信息进行初始化
	 
}

void GPIO_INIT(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_3);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);  //初始化低电平
}


