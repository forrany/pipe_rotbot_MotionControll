#include "stm32f10x.h"


void NVIC_Configuration(void){
	NVIC_InitTypeDef NVIC_InitStructure;//����ṹ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; //ʹ���ⲿ�ж����ڵ�ͨ��

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //��ռ���ȼ�0 

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //�����ȼ� 0

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ�� 

	NVIC_Init(&NVIC_InitStructure); //���ݽṹ����Ϣ�������ȼ���ʼ��
	
	
}
/* 

��ʼ��A15�����ã���������ý�ʹ��PA15���Ե�������IO��ʹ�ã�Ȼ��PA15��Ϊ��������ڣ���Ϊ�����жϵ�����

*/
void Key_INIT(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
	NVIC_Configuration();
	 /**********************
	 1.ִ�ж˿���ӳ��ʱ,���ù���ʱ�ӵ�ʹ��:RCC_APB2Periph_AFIO
	 
	 2.  &1.GPIO_Remap_SWJ_Disable: !< Full SWJ Disabled (JTAG-DP + SW-DP)
				��ʱPA13|PA14|PA15|PB3|PB4������Ϊ��ͨIO����
	 
	 Ϊ�˱���ĳЩ���Զ˿�,GPIO_Remap_SWJ_DisableҲ��ѡ��Ϊ��������ģʽ��
		
			 &2.GPIO_Remap_SWJ_JTAGDisable: !< JTAG-DP Disabled and SW-DP Enabled
			 ��ʱPA15|PB3|PB4����Ϊ��ͨIO����
		
			 &3.GPIO_Remap_SWJ_NoJTRST: !< Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST
			 ��ʱֻ��PB4����Ϊ��ͨIO���� 
	 **********************/
	 
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //ʹ�ܽ�ֹJTAG

	 //��ʼ��GPIOB  ��������
	 GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_15);
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
	 GPIO_Init(GPIOA, &GPIO_InitStructure); 
	 
	 //EXTI ����
	 GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource15);
	 EXTI_InitStructure.EXTI_Line=EXTI_Line15; //�ж��ߵı�� ȡֵ��ΧΪEXTI_Line0~EXTI_Line15
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж�ģʽ����ѡֵΪ�ж� EXTI_Mode_Interrupt ���¼� EXTI_Mode_Event��
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//������ʽ���������½��ش��� EXTI_Trigger_Falling�������ش��� EXTI_Trigger_Rising�����������ƽ�������غ��½��أ�����EXTI_Trigger_Rising_Falling
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);//���ݽṹ����Ϣ���г�ʼ��
	 
}

void GPIO_INIT(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = (GPIO_Pin_3);
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);  //��ʼ���͵�ƽ
}


