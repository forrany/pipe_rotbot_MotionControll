#include "motor.h"  
#include "can.h"
#include "encode.h"
#include "timer.h"
#include <Math.h>
void  Delay_Ms(u16 time);
void SpeedUp(_Bool dirt,_Bool speedUpOrDown );
void SpeedDown(void);
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
_Bool ifMaxSpeed = 0;  //�Ƿ���ҪS���߼��ٺͼ��٣��������ٶȣ���S���߼Ӽ��٣���������
short dirtflag =2;     // 1��ʾǰ��    2 ��ʾ����     2��ʾ��ʼ����ֵ��2��ʾ�տ���������ʵ�ʾ��벻��Ҫ���в���
u32 SpeedArr[130]={52,52,52,53,53,53,54,54,55,55,56,57,57,58,59,
										60,61,62,64,65,67,68,70,72,75,77,80,83,87,90,
										95,99,104,109,115,122,129,136,144,153,163,173,
										184,196,209,223,237,253,269,287,305,324,344,365,
										386,408,431,454,477,501,525,548,572,595,618,641,
										663,684,705,725,744,762,780,796,812,826,840,853,865,
										876,886,896,905,913,920,927,934,940,945,950,954,959,
										962,966,969,972,974,977,979,981,982,984,985,987,988,
										989,990,991,992,992,993,994,994,995,995,996,996,996,
										997,997,997,997,998,998,998,998,998,998,998,999};
u32 SpeedArrBe[100]={209,209,209,209,209,209,209,209,209,209,209,209,209,																			
										209,209,223,237,253,269,287,305,324,344,365,
										386,408,431,454,477,501,525,548,572,595,618,641,
										663,684,705,725,744,762,780,796,812,826,840,853,865,
										876,886,896,905,913,920,927,934,940,945,950,954,959,
										962,966,969,972,974,977,979,981,982,984,985,987,988,
										989,990,991,992,992,993,994,994,995,995,996,996,996,
										997,997,997,997,998,998,998,998,998,998,998,999};

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
						if(RxMessage.Data[3] == 0x00 ) {       //��ʾ��ť�Ǹ������,��ǰ��   
								calDistance = 0 - (float)distance;   //���ʱ��,��Ҫ�����Ŀ�ĵأ���һ�����������  (0 - distance)
								calcycle = (float)cycle - 127;
								//acdis = (calcycle * 10.4) + ((TIM3->CNT/4) * 0.0165);
							  ActPosition();    //����õ�ʵ�ʾ��룬���ݱ������õ���ֵ
								calDistance +=(acdis - 1.0);
		           	SpeedUp(1,1);   
								do{ 
										calcycle = (float)cycle - 127;
										ActPosition();
									if(RxMessage.Data[1]==0x00)
											 break;
								} while (acdis > calDistance	);	
						 }else if (RxMessage.Data[3] == 0x01 ){  //��ʾ��ť�������򣬼�����
 								  calDistance = (float)distance;
									calcycle = (float)cycle - 127;
									ActPosition();
									calDistance += (acdis+1.0);
									SpeedUp(0,1);     //�ĳ�S���߼���										
									do{
											calcycle = (float)cycle - 127;
											ActPosition();
											if(RxMessage.Data[1]==0x00)
												 break;
									} while (acdis < calDistance);		
						   }
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
									case 4: SpeedUp(0,1); break;	
								}					
		       }
				 if(RxMessage.Data[1]==0x02)   //ǰ��
					 { 
						switch(RxMessage.Data[2])
								{
									case 1: Motor1_control(0,400); break;
									case 2: Motor1_control(0,500); break;
									case 3: Motor1_control(0,750); break;
									case 4: SpeedUp(1,1); break;
								}
						}
				 if(RxMessage.Data[1]==0x00)
					 {
								if(ifMaxSpeed){
									SpeedUp(1,0);
								}else{
									GPIO_ResetBits(GPIOC,GPIO_Pin_6); //ֹͣ								
								}
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
//											switch(RxMessage.Data[2])
//										{
//											case 1: Motor2_control(0,400); break;
//											case 2: Motor2_control(0,500); break;
//											case 3: Motor2_control(0,750); break;
//											case 4: Motor2_control(0,900); break;
//										}
										Motor2_control(0,900);
										Dealy_us(50000);  //50ms ��һ���𲽻��壬֮�����
									 do
											 {
													count=(TIM1->CNT);
													Dealy_us(10000);  //10ms
													count1=(TIM1->CNT);
												  speed = count - count1;
//												  SendSpeed(speed);                //�����������˲�������
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
//											switch(RxMessage.Data[2])   //����
//											{
//												case 1: Motor2_control(300,0); break;
//												case 2: Motor2_control(500,0); break;
//												case 3: Motor2_control(750,0); break;
//												case 4: Motor2_control(900,0); break;
//											}
												Motor2_control(900,0);
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
//													SendSpeed(avgspeed);
													
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
//		if(dirtflag == 0)
//		{acdis = ((calcycle * 11.4) + ((TIM3->CNT/4) * 0.018));}
//		else if(dirtflag == 1)
//		{acdis = ((calcycle * 11.4) + ((TIM3->CNT/4) * 0.018));}
//		else if(dirtflag == 2)
//		{acdis = ((calcycle * 11.4) + ((TIM3->CNT/4) * 0.018));}
//		
		acdis = ((calcycle * 11.4) + ((TIM3->CNT/4) * 0.018));
}

void SpeedUp(_Bool dirt,_Bool speedUpOrDown ){    //�Ӽ��ٿ��ƣ���һ������������ǰ�����ߺ��ˣ� �ڶ����������ٻ��߼��١�true Ϊǰ��  ����
	if(speedUpOrDown){
		if(dirt){
			for(int i = 0; i < 130; i++ ){
				if(RxMessage.Data[1]==0x00){
					GPIO_ResetBits(GPIOC,GPIO_Pin_6);
					break;
				}
				u32 MoveSpeed = SpeedArr[i];
				Motor1_control(0,MoveSpeed);
				dirtflag = 1;
				Delay_Ms(5);
			}
		}else{
			for(int i = 0; i < 130; i++ ){
				if(RxMessage.Data[1]==0x00){
					GPIO_ResetBits(GPIOC,GPIO_Pin_6);
					break;
				}				
				u32 MoveSpeed = SpeedArr[i];
				Motor1_control(MoveSpeed,0);
				dirtflag = 0;
				Delay_Ms(5);
			}
		}
	}else{
		u32 len = sizeof(SpeedArr)-1;
		if(dirtflag){
			for(int i = 0; i < 130; i++){
				if(RxMessage.Data[1]==0x00){
					GPIO_ResetBits(GPIOC,GPIO_Pin_6);
					break;
				}
				u32 MoveSpeed = SpeedArr[len-i];
				Motor1_control(0,MoveSpeed);
				Delay_Ms(5);
			}
		}else{
			for(int i = 0; i < 130; i++){
				if(RxMessage.Data[1]==0x00){
					GPIO_ResetBits(GPIOC,GPIO_Pin_6);
					break;
				}
				u32 MoveSpeed = SpeedArr[len-i];
				Motor1_control(MoveSpeed,0);
				Delay_Ms(5);
			}
		}
	}
}

void SpeedDown(void){
	if(dirtflag){
		for(int i = 0; i < 100; i++){
			if(RxMessage.Data[1]==0x00){
				GPIO_ResetBits(GPIOC,GPIO_Pin_6);
				break;		
			}
			u32 MoveSpeed = SpeedArrBe[99-i];
			Motor1_control(0,MoveSpeed);
			Dealy_us(100);
			
		}	
	}else{
		for(int i = 0; i < 100; i++){
			if(RxMessage.Data[1]==0x00){
				GPIO_ResetBits(GPIOC,GPIO_Pin_6);
				break;		
			}
			u32 MoveSpeed = SpeedArrBe[99-i];
			Motor1_control(MoveSpeed,0);
			Dealy_us(100);
		}
	}
}

//		for(int i = 0; i < 130; i++) {
//			u32 MoveSpeed = SpeedArr[i];
//			if(dirt){
//				
//			}else{
//				Motor1_control(MoveSpeed,0);
//			}
//		}	
//	}else{
//		
//	}


