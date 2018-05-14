#include "motor.h"  
#include "can.h"
#include "encode.h"
#include "timer.h"
#include "gpio.h"

void  Delay_Ms(u16 time);
void Data_Trans(_Bool IO);
_Bool isSpeedDown(_Bool IO);
void Speed_Trans(void);
void Stretch_InforSend(_Bool IO);
short JueDuiZ(short value);
__IO uint32_t flag = 0xff;		 //����CAN��־�Ƿ���յ����ݣ����жϺ����и�ֵ
CanTxMsg TxMessage;			     //CAN1���ͻ�����
CanRxMsg RxMessage;				 //CAN1���ջ����� 
signed short count=0;//����������  ���ֽ�
signed short count1=0; 
signed short speed ;
_Bool speedDownFlag = 1;
volatile u32 timer=0x0000;  //����ת̨ת����ʱ��
volatile u8 interFlag = 0;  //�����жϱ�־����������ж���1
int main(void)  
{  
	Key_INIT();
	CAN_Config();
	TIM3_Init();
	SysTick_Init();
	CAN_SetMsg();
	CAN_Transmit(CAN1, &TxMessage);
	while(1)
	{   
		while(flag==0xff);   //flag=0,can success
		if(RxMessage.Data[0]==0x0A)   //   ת̨�˶�
			{  flag=0xff;
    //     CAN_Transmit(CAN1, &TxMessage);
					if(RxMessage.Data[1]==0x01)    //
					{						
						timer=0x00;   //ʱ���λ��
					  Motor1_control(950,0);
								  do
								  {count= (TIM3->CNT)/4;} while(count<10000);
								  do {
													Dealy_us(1);	
												  count1=JueDuiZ((TIM3->CNT)/4-count);	
                          if(RxMessage.Data[1]==0x00)
                          break;
													if(interFlag == 1){
														interFlag = 0;
														break;
													}
                                          									 } while(count1<0x377f);	
                 GPIO_ResetBits(GPIOC,GPIO_Pin_6);		 //ֹͣ	
								 SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //�ر�Systick
							   Data_Trans(Rotate);       //ת̨����
                 TIM3->CNT=0;		
								 count1=0;
                 																												 
           }
 
					 if(RxMessage.Data[1]==0x02)
					 {
						 
						 timer=0x00;   //ʱ�����
						 Motor1_control(0,950);
						 count=0;
						 do{ TIM3->CNT=0;}  while((TIM3->CNT)/4>10000);    //Ī�����ÿ����ת�󣬽���ʱ��������ֵ��Ϊ�㣬���д����������
						 do
									{ Dealy_us(1);
									 count1= JueDuiZ((TIM3->CNT)/4-count);
									if(RxMessage.Data[1]==0x00)
									break;
									if(interFlag == 1){
										interFlag = 0;
										break;
									}
									}     while(count1<0x37a0);
						 GPIO_ResetBits(GPIOC,GPIO_Pin_6);
						 SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
						 Data_Trans(Rotate);
						 TIM3->CNT=0;							 
						 
					 }
            		
									
		      
					
//				 if(RxMessage.Data[1]==0x02)   //����
//					 { 
//						 now_cycle=cycle;
//						 switch(RxMessage.Data[2])
//								{
//									case 1: Motor1_control(0,400); break;
//									case 2: Motor1_control(0,500); break;
//									case 3: Motor1_control(0,750); break;
//									case 4: Motor1_control(0,900); break;
//								}
//									   do {
//													Dealy_us(1);	
//												  sub_cycle=JueDuiZ(cycle-now_cycle);		
//                          if(RxMessage.Data[1]==0x00)
//													break;		                           //ǿ��ֹͣ
//																									 
//										                                                 } while(sub_cycle<=320);	
//                 GPIO_ResetBits(GPIOC,GPIO_Pin_6);		 //ֹͣ	
//								 SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //�ر�Systick
//                 Data_Trans(Rotate);       //ת̨����
//								 cycle=127;																									   
//						}
//					 
//				 if(RxMessage.Data[1]==0x00)
//					 {
//								GPIO_ResetBits(GPIOC,GPIO_Pin_6); //ֹͣ
//								Data_Trans(_Rotate);    //��ת̨ʱ������     | �������ʹ��
////						    cycle=127;
//					 }
		   }
			
			 
	  if(RxMessage.Data[0]==0x0C)   // �����˶�
	       {   flag=0xff;
							if(RxMessage.Data[1]==0x01)      //����
							{
											
								    switch(RxMessage.Data[2])
										{
											case 1: Motor2_control(0,400); break;
											case 2: Motor2_control(0,500); break;
											case 3: Motor2_control(0,750); break;
											case 4: Motor2_control(0,999); break;
										}
										do
											{
										   count=(TIM1->CNT)/4;
											 Dealy_us(10000);   //100ms ������
                       count1=(TIM1->CNT)/4;
											 //Speed_Trans();
											 if(isSpeedDown(0)&&speedDownFlag){
												Motor2_control(0,500);
												 speedDownFlag = 0;
											 }
											 if(RxMessage.Data[1]==0x00)
							         break;
											 
											                                               }  while(count!=count1);
                    	 GPIO_ResetBits(GPIOC,GPIO_Pin_7); //ֹͣ		
                       SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //�ر�Systick			
											 speedDownFlag = 1;
																	

							 }
							if(RxMessage.Data[1]==0x02)    //��չ
								{ 
											switch(RxMessage.Data[2])
											{
												case 1: Motor2_control(300,0); break;
												case 2: Motor2_control(500,0); break;
												case 3: Motor2_control(750,0); break;
												case 4: Motor2_control(999,0); break;
											}
												do
											{
										   count=(TIM1->CNT)/4;
											 Dealy_us(10000); //100ms ������
                       count1=(TIM1->CNT)/4;
											 //Speed_Trans();
											 if(isSpeedDown(1)&&speedDownFlag){
												 Motor2_control(750,0);
												 speedDownFlag = 0;
											 }
											 if(RxMessage.Data[1]==0x00)
							         break;                     }  while(count!=count1);
                    	 GPIO_ResetBits(GPIOC,GPIO_Pin_7); //ֹͣ		
                       SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //�ر�Systick
											 Stretch_InforSend(0);
											 speedDownFlag = 1;

								}
								if(RxMessage.Data[1] == 0x03)
								{	
									switch(RxMessage.Data[2])
											{
												case 1: Motor2_control(0,400); break;
												case 2: Motor2_control(0,500); break;
												case 3: Motor2_control(0,750); break;
												case 4: Motor2_control(0,999); break;
											}
									Dealy_us(600000);
									GPIO_ResetBits(GPIOC,GPIO_Pin_7); //ֹͣ	
									SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //�ر�Systick
									Stretch_InforSend(1);
									
								}
//							if(RxMessage.Data[1]==0x00)
//							{
//							 GPIO_ResetBits(GPIOC,GPIO_Pin_7); //ֹͣ
//							TxMessage.DLC=1;	
//							TxMessage.Data[0]=TIM1->CNT;
//							TxMessage.Data[1]=(TIM1->CNT)/4;
//							CAN_Transmit(CAN1,&TxMessage);

//							}
						}
					}

}  

/*******************************************************************************
* Function Name  : Delay_Ms
* Description    : delay 1 ms.
* Input          : time (ms)
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_Ms(u16 time)  //��ʱ����
{ 
	u16 i,j;
	for(i=0;i<time;i++)
  		for(j=0;j<10260;j++)
    		;
}

/*******************************************************************************
* Function Name  : JueDuiZ
* Description    : get abs value
* Input          : short
* Output         : short
* Return         : None
*******************************************************************************/
short JueDuiZ(short value)
{
	if (value>=0)
	{return value;}
	else
	{return -value;}
}
/*******************************************************************************
* Function Name  : Data_Trans
* Description    : transmate data
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Data_Trans(_Bool IO)
{
    if(IO==0)     // ��Ҫ����ת̨ʱ�����ݰ�
		{
	  u16 shortime=(u16)(timer/100);	  // ��λת��Ϊ1ms			 
	  TxMessage.DLC=3;
		TxMessage.Data[0]= 0x00;   //��ʾ����ʱ�����ݰ�
		TxMessage.Data[1]=shortime&0x00ff;
		TxMessage.Data[2]=shortime>>8;
    CAN_Transmit(CAN1,&TxMessage);
    timer=0;			
		}
		else       //  ��ת̨���ݰ�(������ʱ��)
		{
			TxMessage.DLC=3;	
//			TxMessage.Data[0]=cycle&0x00ff;
//		  TxMessage.Data[1]=cycle>>8;
			TxMessage.Data[0]=((TIM3->CNT)/4)&0x00ff;
			TxMessage.Data[1]=((TIM3->CNT)/4)>>8;
		  TxMessage.Data[2]=TIM_GetCounter(TIM3);	
			CAN_Transmit(CAN1,&TxMessage);
		}
	
}


void Speed_Trans(void)
{
	speed = count1 > count? count1 - count : count - count1;
	TxMessage.Data[0] = speed;
	TxMessage.Data[1] = speed;
	TxMessage.Data[2] = speed;
	CAN_Transmit(CAN1,&TxMessage);
	TIM1->CNT = 0X00;
}


_Bool isSpeedDown(_Bool IO){
  speed = count1 > count? count1 - count : count - count1;
	if(IO == 1)  //����  ��ֵe4
	{
		if(speed > 0xf0){
			return 1;
		}
	}else if(IO == 0){
		if(speed < 0x10){
			return 1;
		}
	}
	return 0;
}
/*******************************************************************************
* Function Name  : Stretch_InforSend
* Description    : send the status of Stretch structure
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Stretch_InforSend(_Bool IO) {
	 if(IO == 0){
		 TxMessage.Data[0]= 0x01;  //��ʾ��չ�����״̬
	 }else{
		 TxMessage.Data[0]= 0x02;  //��ʾ�Ѿ�����6��
	 }
	 CAN_Transmit(CAN1,&TxMessage);
}
