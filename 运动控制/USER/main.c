#include "motor.h"  
#include "can.h"
#include "encode.h"
#include "timer.h"
#include <Math.h>
void  Delay_Ms(u16 time);
void SpeedAdjust(void);
void ActPosition(void);
void SendSpeed(unsigned short avgspeed);
__IO uint32_t flag = 0xff;		 //����CAN��־�Ƿ���յ����ݣ����жϺ����и�ֵ
CanTxMsg TxMessage;			     //CAN1���ͻ�����
CanRxMsg RxMessage;				 //CAN1���ջ����� 
unsigned short count;//����������  ���ֽ�
unsigned short count1;
unsigned short speed; //Speed of Motor;
unsigned short avgspeed,distance;
volatile float acdis,calcycle;
volatile u32 cycle=127; 
_Bool speedflag = 0;
short dirtflag =2;

int main(void)  
{  
	CAN_Config();
	TIM3_Init();
	SysTick_Init();
	CAN_SetMsg();
	CAN_Transmit(CAN1, &TxMessage);
	while(1)
	{   
		while(flag==0xff);   //flag=0,can success
		if(RxMessage.Data[0]==0x0A)   // ǰ���˶�
			{  flag=0xff;
				if(RxMessage.Data[1]==0x03 |RxMessage.Data[1]==0x04)    //�ŷ��˶�
					{
						distance = RxMessage.Data[2];
						float calDistance;
						if(RxMessage.Data[3] == 0x00 )        //��ʾ��ť�Ǹ������,��ǰ��
						{	
								calDistance = 0 - (float)distance;
								calcycle = (float)cycle - 127;
								//acdis = (calcycle * 10.4) + ((TIM3->CNT/4) * 0.0165);
							  ActPosition();
								if(acdis > calDistance)
								{
										Motor1_control(0,999);
										dirtflag = 1;	
										do{
												calDistance = 0 - (float)distance;
												calcycle = (float)cycle - 127;
												//acdis = (calcycle * 10.4) + ((TIM3->CNT/4) * 0.0165);
												ActPosition();
												if(RxMessage.Data[1]==0x00)
													 break;
										} while (acdis > calDistance	);								
								}
								
								else if(acdis < calDistance)
								{
										Motor1_control(999,0);
										dirtflag = 0;
										do{
												calDistance = 0 - (float)distance;
												calcycle = (float)cycle - 127;
//												acdis = (calcycle * 10.4) + ((TIM3->CNT/4) * 0.0165);
												ActPosition();
												if(RxMessage.Data[1]==0x00)
													 break;											
										
										}while(acdis < calDistance);
								}						
							}
						else if (RxMessage.Data[3] == 0x01 )  //��ʾ��ť�������򣬼�����
						{	
							
 								  calcycle = (float)cycle - 127;
//								  acdis = (calcycle * 10.4) + ((TIM3->CNT/4) * 0.0165);
									ActPosition();
								  if(distance > acdis)
									{
												Motor1_control(999,0);
												dirtflag = 0;												
												do{
														calcycle = (float)cycle - 127;
														//acdis = (calcycle * 10.4) + ((TIM3->CNT/4) * 0.0165);
														ActPosition();
														if(RxMessage.Data[1]==0x00)
															 break;
												} while (acdis < distance);								 
							     }									
									else if(distance < acdis)
									{
												Motor1_control(0,999);
												dirtflag = 1;
												do{
														calcycle = (float)cycle - 127;
														//acdis = (calcycle * 10.4) + ((TIM3->CNT/4) * 0.0165);
														ActPosition();
														if(RxMessage.Data[1] == 0x00)
															break;											
												} while(distance < acdis);															
									}						
						}
						
						
						
//					  if(RxMessage.Data[1]==0x03 )   //ǰ��  ע�⣬����Ӳ����װ���⣬ǰ��ʵ���ϱ������ǵ���ת�����������С��cycleҲ���С(cycle =127),����ʱ�����ȥ127��Ϊ0��ʼֵ���㡣
//						   {
//								  float calDistance;    //���帡���������ڼ��������ľ��롣��ν�����ľ��룬��Ϊ��λ������������Զ����������������ǡ�ǰ���������ô����������Ϊ������
//									calDistance = 0 - (float)distance;
//									calcycle = (float)cycle - 127;
//									acdis = (calcycle * 11.3) + ((TIM3->CNT/4) * 0.071);


//									
//							 
//							 }
//						else if(RxMessage.Data[1]==0x04)    //����   ���ˣ�cycle�ͼ�����������������
//							 {							   

//						
//							 } 
									GPIO_ResetBits(GPIOC,GPIO_Pin_6);
									TxMessage.Data[0]=cycle&0x00ff;
									TxMessage.Data[1]=cycle>>8;
									TxMessage.Data[2]=(TIM3->CNT)/4&0x00ff;
									TxMessage.Data[3]=(TIM3->CNT)/4>>8;
									CAN_Transmit(CAN1, &TxMessage);
							
					}
				if(RxMessage.Data[1]==0x01)    //����
					{
						switch(RxMessage.Data[2])
								{
									case 1: Motor1_control(400,0); break;
									case 2: Motor1_control(600,0); break;
									case 3: Motor1_control(750,0); break;
									case 4: Motor1_control(999,0); break;	
								}					
		       }
				 if(RxMessage.Data[1]==0x02)   //ǰ��
					 { 
						switch(RxMessage.Data[2])
								{
									case 1: Motor1_control(0,400); break;
									case 2: Motor1_control(0,500); break;
									case 3: Motor1_control(0,750); break;
									case 4: Motor1_control(0,999); break;
								}
						}
				 if(RxMessage.Data[1]==0x00)
					 {
								GPIO_ResetBits(GPIOC,GPIO_Pin_6); //ֹͣ
								TxMessage.Data[0]=cycle&0x00ff;
						    TxMessage.Data[1]=cycle>>8;
								TxMessage.Data[2]=(TIM3->CNT)/4&0x00ff;
								TxMessage.Data[3]=(TIM3->CNT)/4>>8;
								CAN_Transmit(CAN1, &TxMessage);
					 }
		   }
	  if(RxMessage.Data[0]==0x0C)   // �����˶�
	       {   flag=0xff;
							if(RxMessage.Data[1]==0x01)   //����
							{
											switch(RxMessage.Data[2])
										{
											case 1: Motor2_control(0,400); break;
											case 2: Motor2_control(0,500); break;
											case 3: Motor2_control(0,750); break;
											case 4: Motor2_control(0,900); break;
										}
										Dealy_us(50000);  //50ms ��һ���𲽻��壬֮�����
									 do
											 {
													count=(TIM1->CNT);
													Dealy_us(10000);  //10ms
													count1=(TIM1->CNT);
												  speed = count - count1;
												  SendSpeed(speed);                //�����������˲�������
											 if(RxMessage.Data[1]==0x00)
							           break;
//												  TxMessage.Data[0]=count;
//												  TxMessage.Data[1]=count1;
//												  CAN_Transmit(CAN1,&TxMessage);
											 }  while(speed > 0x6b);
											 //
											 //while(count!=count1);
								 GPIO_ResetBits(GPIOC,GPIO_Pin_7);
								 TIM1->CNT=0;
	
							 }
							if(RxMessage.Data[1]==0x02)
								{ 
											switch(RxMessage.Data[2])   //����
											{
												case 1: Motor2_control(300,0); break;
												case 2: Motor2_control(500,0); break;
												case 3: Motor2_control(750,0); break;
												case 4: Motor2_control(900,0); break;
											}
											  Dealy_us(50000);  //50ms   ��һ���𲽻��� ��֮���ٲ���
						           	 do
											 {
													for(int i = 0; i < 10; i++)
														 {
																int speedarr[10];
																count=(TIM1->CNT);
																Dealy_us(1000);  //10ms
																count1=(TIM1->CNT);
																speed = count1 - count;
																		if (speed<0x08)              //���˲ʱ�ٶ��Ƿ�С����ֵ��С��������1 flag�������˲���ѭ����
																		{
																		 speedflag = 1;
																		 break;
																		}
															  speedarr[i] = speed;
																avgspeed += speedarr[i];
															 }
												  if( speedflag){ break;}        //������ڲ�ѭ���в��ҵ���speedС����ֵ������ֹͣ�˲�������ѭ����
												  avgspeed /= 10;
													SendSpeed(avgspeed);
													
											    if(RxMessage.Data[1]==0x00)
							                break;
											 }               	while(avgspeed > 0x09);
																				//while(speed > 0x50);
								 speedflag = 0;
								 GPIO_ResetBits(GPIOC,GPIO_Pin_7);
								 TIM1->CNT=0;
								}
							if(RxMessage.Data[1]==0x00)
							{
							 GPIO_ResetBits(GPIOC,GPIO_Pin_7); //ֹͣ
					//			TxMessage.Data[0]=cycle;                              ����֧�ţ��������̼��޹�
					//			TxMessage.Data[1]=(TIM1->CNT)/4;
					//			CAN_Transmit(CAN1, &TxMessage);
							}
						}
					}
	/*if(RxMessage.Data[1]==0x0B)   //ͨ��CAN�������ݣ���һ�鵥Ƭ��ִ��
	{
		std=0xff;
		CAN_SetMsg();
		TransmitMailbox=CAN_Transmit(CAN1,&TxMessage);
		while((CAN_TransmitStatus(CAN1,TransmitMailbox) !=CANTXOK)); //�ȴ��������	
   	printf("�������");
	}
	 */ 
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

void SendSpeed(unsigned short avgspeed)
{
	TxMessage.Data[0]= avgspeed&0x00ff;
	TxMessage.Data[1]= avgspeed>>8;
	//TxMessage.Data[0]= Count1&0x00ff;
	//TxMessage.Data[1]= Count1>>8;
	TxMessage.Data[2]= 0xAC;
	CAN_Transmit(CAN1, &TxMessage);

}

void SpeedAdjust()
{
	float difference;
	do{
		acdis =((cycle-127) * 11.3)+((TIM3->CNT/4)*0.071);	
		if(dirtflag == 1)
		{difference = distance + acdis;}
		else if(dirtflag == 0)
		{difference = distance - acdis;}
	}while(difference >0);

		GPIO_ResetBits(GPIOC,GPIO_Pin_6);
		TxMessage.Data[0]=TIM3->CNT/4;
		TxMessage.Data[1]=distance;
		TxMessage.Data[2]=0xDD;
		CAN_Transmit(CAN1, &TxMessage);	
	
//	while(difference >3)
//	{
//		acdis = ((cycle-127) * 11.3) + ((TIM3->CNT)/4 * 0.071);	
//		difference = distance - acdis;
//		yvalue = floor(34 * difference +614);
//		if(!dirtflag)
//		{Motor1_control(0,yvalue);}
//		 Motor1_control(yvalue,0);
//	}	
//	
//   while(difference >1)
//	{
//		acdis = ((cycle-127) * 11.3) + ((TIM3->CNT)/4 * 0.071);		
//		difference = distance - acdis;
//		if(!dirtflag)
//		{Motor1_control(0,750);}
//		 Motor1_control(750,0);
//	}
//	
//	while(difference >0.01)
//	{
//		acdis = ((cycle-127) * 11.3) + ((TIM3->CNT)/4 * 0.071);	
//		difference = distance - acdis;
//		if(!dirtflag)
//		{Motor1_control(0,550);}
//		 Motor1_control(550,0);
//	}

	
}

void ActPosition(void) {
		if(dirtflag == 0)
		{acdis = ((calcycle * 11.4) + ((TIM3->CNT/4) * 0.018))-1.5;}
		else if(dirtflag == 1)
		{acdis = ((calcycle * 11.4) + ((TIM3->CNT/4) * 0.018))+1.5;}
		else if(dirtflag == 2)
		{acdis = ((calcycle * 11.4) + ((TIM3->CNT/4) * 0.018));}
		
}




