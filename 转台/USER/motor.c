#include"motor.h"
//#include"RCCconfig.h"
/**********************************
         电机一控制
**********************************/

void TIM2_GPIO_Config(void) 
 {
 //配置TIM4复用输出PWM的IO
   GPIO_InitTypeDef GPIO_InitStructure;
  // 打开TIM4的时钟
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
   //打开GPIOB的时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE); 
   //配置PA2,PA3的工作模式,实用TIM2的3和4通道
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
	 
	 /* 配置使能端口EN1 */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //定义IN引脚  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //通用推挽输出  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //设置输出功率  
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化GPIOC的引脚参数,写进  
      
    GPIO_SetBits(GPIOC,GPIO_Pin_6); // 拉高使能端口 
	 

 }

//控制电机正反转
 void Motor1_control(u16 CCR3_Val,u16 CCR4_Val)
 {
         TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//初始化TIM3的时间基数单位
         TIM_OCInitTypeDef  TIM_OCInitStructure;//初始化TIM3的外设
/* ----------------------------------------------------------------------- 
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR+1)* 100% = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR+1)* 100% = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR+1)* 100% = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR+1)* 100% = 12.5%
  ----------------------------------------------------------------------- */
   TIM2_GPIO_Config();
   //TIM3的时间基数单位设置(如计数终止值：999,从0开始 ；计数方式：向上计数）         
   TIM_TimeBaseStructure.TIM_Period = 999;       
   TIM_TimeBaseStructure.TIM_Prescaler = 0;      // 设置部分频，72MHZ        
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
   TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
   //TIM3的外设的设置
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //TIM脉冲宽度调制模式1 
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM输出比较极性高 
   
   /*
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//这个暂时不知道，stm32固件库里没有搜到。应该是定时器输出声明使能的意思        
   TIM_OCInitStructure.TIM_Pulse = CCR1_Val;//设置了待装入捕获比较寄存器的脉冲值          
   TIM_OC1Init(TIM4, &TIM_OCInitStructure);      
   TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR1上的预装载寄存器
  // 下面3路PWM输出和上面的一样不再解说
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR2_Val;         
   TIM_OC2Init(TIM3, &TIM_OCInitStructure);          
   TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

   */
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR3_Val;        
   TIM_OC3Init(TIM2, &TIM_OCInitStructure);         
   TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  //通道3
   
  
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR4_Val;        
   TIM_OC4Init(TIM2, &TIM_OCInitStructure);        
   TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);  //通道4   
	 //  注释： 正转和反转，只需让其中一个为零，另一个输出PWM即可。
    	 

   TIM_ARRPreloadConfig(TIM2, ENABLE);        //使能TIM3重载寄存器ARR                


   TIM_Cmd(TIM2, ENABLE);//使能TIM3              
 }

/***********************************************************/



/***********************************
      电机二控制
***********************************/

//电机输出引脚配置
void TIM4_GPIO_Config(void) 
 {
 //配置TIM4复用输出PWM的IO
   GPIO_InitTypeDef GPIO_InitStructure;
  // 打开TIM4的时钟
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
   //打开GPIOB的时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE); 
   //配置PB8.PB9的工作模式
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;        
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(GPIOB, &GPIO_InitStructure);
		 /* 配置使能端口EN2 */
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; //定义IN引脚  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //通用推挽输出  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //设置输出功率  
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化GPIOC的引脚参数,写进  
      
    GPIO_SetBits(GPIOC,GPIO_Pin_7); // 拉高使能端口 
 }

//控制电机2正反转
 void Motor2_control(u16 CCR3_Val,u16 CCR4_Val)
 {
         TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//初始化TIM3的时间基数单位
         TIM_OCInitTypeDef  TIM_OCInitStructure;//初始化TIM3的外设
/* ----------------------------------------------------------------------- 
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR+1)* 100% = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR+1)* 100% = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR+1)* 100% = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR+1)* 100% = 12.5%
  ----------------------------------------------------------------------- */
   TIM4_GPIO_Config();
   //TIM3的时间基数单位设置(如计数终止值：999,从0开始 ；计数方式：向上计数）         
   TIM_TimeBaseStructure.TIM_Period = 999;       
   TIM_TimeBaseStructure.TIM_Prescaler = 0;            
   TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
   TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
   TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
   //TIM3的外设的设置
   TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;        //TIM脉冲宽度调制模式1 
   TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM输出比较极性高 
   
   /*
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//这个暂时不知道，stm32固件库里没有搜到。应该是定时器输出声明使能的意思        
   TIM_OCInitStructure.TIM_Pulse = CCR1_Val;//设置了待装入捕获比较寄存器的脉冲值          
   TIM_OC1Init(TIM4, &TIM_OCInitStructure);      
   TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR1上的预装载寄存器
  // 下面3路PWM输出和上面的一样不再解说
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR2_Val;         
   TIM_OC2Init(TIM3, &TIM_OCInitStructure);          
   TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	*/
   
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR3_Val;        
   TIM_OC3Init(TIM4, &TIM_OCInitStructure);        
   TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //通道3
   
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse = CCR4_Val;        
   TIM_OC4Init(TIM4, &TIM_OCInitStructure);        
   TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);   //通道4
    	 

   TIM_ARRPreloadConfig(TIM4, ENABLE);        //使能TIM4重载寄存器ARR                


   TIM_Cmd(TIM4, ENABLE);//使能TIM4              
 }
 
 void LED_CONF(void)
 {
	  GPIO_InitTypeDef GPIO_InitStructure;
	 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 	 
	 /* 配置LED5端口PC8 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //定义IN引脚  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //通用推挽输出  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //设置输出功率  
    GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化GPIOC的引脚参数,写进  
    GPIO_SetBits(GPIOC,GPIO_Pin_8); // 拉高使能端口 
 }
