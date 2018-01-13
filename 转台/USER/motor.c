#include"motor.h"
//#include"RCCconfig.h"
/**********************************
         ���һ����
**********************************/

void TIM2_GPIO_Config(void) 
 {
 //����TIM4�������PWM��IO
   GPIO_InitTypeDef GPIO_InitStructure;
  // ��TIM4��ʱ��
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
   //��GPIOB��ʱ��
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE); 
   //����PA2,PA3�Ĺ���ģʽ,ʵ��TIM2��3��4ͨ��
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
	 /* ����ʹ�ܶ˿�EN1 */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //����IN����  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //ͨ���������  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //�����������  
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��GPIOC�����Ų���,д��  
      
    GPIO_SetBits(GPIOC,GPIO_Pin_6); // ����ʹ�ܶ˿� 
	 

 }

//���Ƶ������ת
 void Motor1_control(u16 CCR3_Val,u16 CCR4_Val)
 {
         TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//��ʼ��TIM3��ʱ�������λ
         TIM_OCInitTypeDef  TIM_OCInitStructure;//��ʼ��TIM3������
/* ----------------------------------------------------------------------- 
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR+1)* 100% = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR+1)* 100% = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR+1)* 100% = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR+1)* 100% = 12.5%
  ----------------------------------------------------------------------- */
   TIM2_GPIO_Config();
   //TIM3��ʱ�������λ����(�������ֵֹ��999,��0��ʼ ��������ʽ�����ϼ�����         
   TIM_TimeBaseStructure.TIM_Period = 999;       
   TIM_TimeBaseStructure.TIM_Prescaler = 0;      // ���ò���Ƶ��72MHZ        
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
   //TIM3�����������
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //TIM�����ȵ���ģʽ1 
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM����Ƚϼ��Ը� 
   
   /*
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�����ʱ��֪����stm32�̼�����û���ѵ���Ӧ���Ƕ�ʱ���������ʹ�ܵ���˼        
   TIM_OCInitStructure.TIM_Pulse = CCR1_Val;//�����˴�װ�벶��ȽϼĴ���������ֵ          
   TIM_OC1Init(TIM4, &TIM_OCInitStructure);      
   TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//ʹ�ܻ���ʧ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
  // ����3·PWM����������һ�����ٽ�˵
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR2_Val;         
   TIM_OC2Init(TIM3, &TIM_OCInitStructure);          
   TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

   */
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR3_Val;        
   TIM_OC3Init(TIM2, &TIM_OCInitStructure);         
   TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ͨ��3
   
  
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR4_Val;        
   TIM_OC4Init(TIM2, &TIM_OCInitStructure);        
   TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ͨ��4   
	 //  ע�ͣ� ��ת�ͷ�ת��ֻ��������һ��Ϊ�㣬��һ�����PWM���ɡ�
    	 

   TIM_ARRPreloadConfig(TIM2, ENABLE);        //ʹ��TIM3���ؼĴ���ARR                


   TIM_Cmd(TIM2, ENABLE);//ʹ��TIM3              
 }

/***********************************************************/



/***********************************
      ���������
***********************************/

//��������������
void TIM4_GPIO_Config(void) 
 {
 //����TIM4�������PWM��IO
   GPIO_InitTypeDef GPIO_InitStructure;
  // ��TIM4��ʱ��
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
   //��GPIOB��ʱ��
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE); 
   //����PB8.PB9�Ĺ���ģʽ
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
		 /* ����ʹ�ܶ˿�EN2 */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //����IN����  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //ͨ���������  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //�����������  
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��GPIOC�����Ų���,д��  
      
    GPIO_SetBits(GPIOC,GPIO_Pin_7); // ����ʹ�ܶ˿� 
 }

//���Ƶ��2����ת
 void Motor2_control(u16 CCR3_Val,u16 CCR4_Val)
 {
         TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//��ʼ��TIM3��ʱ�������λ
         TIM_OCInitTypeDef  TIM_OCInitStructure;//��ʼ��TIM3������
/* ----------------------------------------------------------------------- 
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR+1)* 100% = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR+1)* 100% = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR+1)* 100% = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR+1)* 100% = 12.5%
  ----------------------------------------------------------------------- */
   TIM4_GPIO_Config();
   //TIM3��ʱ�������λ����(�������ֵֹ��999,��0��ʼ ��������ʽ�����ϼ�����         
   TIM_TimeBaseStructure.TIM_Period = 999;       
   TIM_TimeBaseStructure.TIM_Prescaler = 0;            
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
   TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
   //TIM3�����������
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //TIM�����ȵ���ģʽ1 
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM����Ƚϼ��Ը� 
   
   /*
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�����ʱ��֪����stm32�̼�����û���ѵ���Ӧ���Ƕ�ʱ���������ʹ�ܵ���˼        
   TIM_OCInitStructure.TIM_Pulse = CCR1_Val;//�����˴�װ�벶��ȽϼĴ���������ֵ          
   TIM_OC1Init(TIM4, &TIM_OCInitStructure);      
   TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//ʹ�ܻ���ʧ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
  // ����3·PWM����������һ�����ٽ�˵
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR2_Val;         
   TIM_OC2Init(TIM3, &TIM_OCInitStructure);          
   TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	*/
   
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR3_Val;        
   TIM_OC3Init(TIM4, &TIM_OCInitStructure);        
   TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ͨ��3
   
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR4_Val;        
   TIM_OC4Init(TIM4, &TIM_OCInitStructure);        
   TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);   //ͨ��4
    	 

   TIM_ARRPreloadConfig(TIM4, ENABLE);        //ʹ��TIM4���ؼĴ���ARR                


   TIM_Cmd(TIM4, ENABLE);//ʹ��TIM4              
 }
 
 void LED_CONF(void)
 {
	  GPIO_InitTypeDef GPIO_InitStructure;
	 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 	 
	 /* ����LED5�˿�PC8 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //����IN����  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //ͨ���������  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //�����������  
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //��ʼ��GPIOC�����Ų���,д��  
    GPIO_SetBits(GPIOC,GPIO_Pin_8); // ����ʹ�ܶ˿� 
 }
