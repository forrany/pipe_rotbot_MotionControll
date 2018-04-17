/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "can.h"
extern __IO uint32_t flag;
extern CanRxMsg RxMessage;
extern CanTxMsg TxMessage;
extern volatile u32 timer;
extern volatile u8 interFlag;
extern __IO uint32_t TimingDelay;
#include <stdio.h>
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}


void TIM3_IRQHandler(void)
{ 	
	if(TIM3->SR&0x0001)//����ж�
	{
//		switch(RxMessage.Data[1])
//		{
//			case 1: cycle+=1; break;
//			case 2: cycle-=1; break;
//			default : cycle=cycle; break;
//		}
	}	
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	
	
}

void EXTI15_10_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line15)!=RESET){
		if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)){
		interFlag = 1;
		}
		EXTI_ClearITPendingBit(EXTI_Line15);   //��� LINE �ϵ��жϱ�־λ
	}

}

void TIM4_IRQHandler(void)
{ 	
	if(TIM4->SR&0x0001)//����ж�
	{
		//GPIO_ResetBits(GPIOC,GPIO_Pin_6);
//		switch(RxMessage.Data[1])
//		{
//			case 1: cycle+=1; break;
//			case 2: cycle-=1; break;
//			default : cycle=cycle; break;
//		}
//		TxMessage.Data[0]=cycle;
//		TxMessage.Data[1]=(TIM4->CNT)/4;  //���͵�ǰ���������飬����Ϊ0
//		CAN_Transmit(CAN1, &TxMessage);
	}	
	TIM4->SR&=~(1<<0);//����жϱ�־λ 	
}
void TIM1_UP_IRQHandler(void)
{ 	
	if(TIM1->SR&0x0001)//����ж�   TIM1�����������̶�������Ҫ֪�����˶���
	{

//		switch(RxMessage.Data[1])
//		{
//			case 1: cycle+=1; break;
//			case 2: cycle-=1; break;
//			default : cycle=cycle; break;
//		}
	}
//		GPIO_ResetBits(GPIOC,GPIO_Pin_6);
//		TxMessage.Data[0]=(TIM1->CNT)/4;
//		CAN_Transmit(CAN1, &TxMessage);	
	TIM1->SR&=~(1<<0);//����жϱ�־λ 	
	
}
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */

void USB_LP_CAN1_RX0_IRQHandler(void)
{
 
 /*�������ж�������*/
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

  /* �Ƚ�ID�������Ƿ�Ϊ0x1314��DCBA */ 
  if( RxMessage.ExtId==0x00AB0000)
  {
    flag = 0; 			//���ճɹ�  
	
	}
  else
  {
    flag = 0xff; 					   //����ʧ��
  }
}
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	timer+=1;
	if(TimingDelay!=0x00)
	{
		TimingDelay--;
	}
}

void USART1_IRQHandler(void)
{
}
/*{ 
    
	//unsigned int i;
	  //  unsigned int rch;
  //static char rCnt = 0;
	
   unsigned char RxData;
      if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
            USART_ClearITPendingBit(USART1, USART_IT_RXNE);
            RxData=USART_ReceiveData(USART1); 
            RxData = RxData + 1;
           USART_SendData(USART1,RxData);
      
 }
			}*/
  /*�����ж�*/
/*  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)   
  {        
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		rch=USART_ReceiveData(USART1);
		RxBuffer1[rCnt]=rch;
		if(rCnt==0)
		{ 
			if(rch==0x0A||rch==0x0B)
			   {
				rCnt++;
			    }
			else
			{
				RxBuffer1[0]=0x00;
			}
			
				}
		if(rCnt==1)
		 {
			rCnt=0;
			std=0;
		  }
  
	if (std==0)   //�������
	{ 
		for (i=0;i<2;i++)
		{
		  printf("%x",RxBuffer1[i]);
		}
	}
   
}
	}  */
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
