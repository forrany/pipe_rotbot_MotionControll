#include "motor.h"  
#include "can.h"
#include "encode.h"
#include "timer.h"
#include <Math.h>
void  Delay_Ms(u16 time);
void SpeedAdjust(void);
void SendSpeed(unsigned short avgspeed);
__IO uint32_t flag = 0xff;		 //用于CAN标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			     //CAN1发送缓冲区
CanRxMsg RxMessage;				 //CAN1接收缓冲区 
unsigned short count;//编码器计数  两字节
unsigned short count1;
unsigned short speed; //Speed of Motor;
unsigned short avgspeed,distance;
float acdis,calcycle;
volatile u32 cycle=127; 
_Bool speedflag = 0,dirtflag = 1;

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
		if(RxMessage.Data[0]==0x0A)   // 前后运动
			{  flag=0xff;
				if(RxMessage.Data[1]==0x03 |RxMessage.Data[1]==0x04)    //伺服运动
					{
						distance = RxMessage.Data[2];
					  if(RxMessage.Data[1]==0x03 )   //前进  注意，由于硬件安装问题，前进实际上编码器是倒着转，脉冲数会减小，cycle也会减小(cycle =127),计算时，会减去127作为0初始值计算。
						   {
								  float calDistance;    //定义浮点数，用于计算真正的距离。所谓真正的距离，因为上位机传过来的永远是正数，这里如果是【前进】命令，那么将距离设置为负数。
									calDistance = 0 - (float)distance;
									calcycle = (float)cycle - 127;
									acdis = (calcycle * 11.3) + ((TIM3->CNT/4) * 0.071);
								  if(acdis > calDistance)
									{
											Motor1_control(0,999);
											dirtflag = 1;	
											do{
													calDistance = 0 - (float)distance;
													calcycle = (float)cycle - 127;
													acdis = (calcycle * 11.3) + ((TIM3->CNT/4) * 0.071);
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
													acdis = (calcycle * 11.3) + ((TIM3->CNT/4) * 0.071);
													if(RxMessage.Data[1]==0x00)
														 break;											
											
											}while(acdis < calDistance);
									}
									
							 
							 }
						else if(RxMessage.Data[1]==0x04)    //后退   后退，cycle和计数器都是正向增加
							 {							   
 								  calcycle = (float)cycle - 127;
								  acdis = (calcycle * 11.3) + ((TIM3->CNT/4) * 0.071);
								  if(distance > acdis)
									{
												Motor1_control(999,0);
												dirtflag = 0;												
												do{
														calcycle = (float)cycle - 127;
														acdis = (calcycle * 11.3) + ((TIM3->CNT/4) * 0.071);
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
														acdis = (calcycle * 11.3) + ((TIM3->CNT/4) * 0.071);
														if(RxMessage.Data[1] == 0x00)
															break;											
												} while(distance < acdis);															
									}
						
							 } 
									GPIO_ResetBits(GPIOC,GPIO_Pin_6);
									TxMessage.Data[0]=cycle&0x00ff;
									TxMessage.Data[1]=cycle>>8;
									TxMessage.Data[2]=(TIM3->CNT)/4;
									CAN_Transmit(CAN1, &TxMessage);
							
					}
				if(RxMessage.Data[1]==0x01)    //后退
					{
						switch(RxMessage.Data[2])
								{
									case 1: Motor1_control(400,0); break;
									case 2: Motor1_control(600,0); break;
									case 3: Motor1_control(750,0); break;
									case 4: Motor1_control(999,0); break;	
								}	
		       }
				 if(RxMessage.Data[1]==0x02)   //前进
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
								GPIO_ResetBits(GPIOC,GPIO_Pin_6); //停止
								TxMessage.Data[0]=cycle&0x00ff;
						    TxMessage.Data[1]=cycle>>8;
								TxMessage.Data[2]=(TIM3->CNT)/4;
								CAN_Transmit(CAN1, &TxMessage);
					 }
		   }
	  if(RxMessage.Data[0]==0x0C)   // 伸缩运动
	       {   flag=0xff;
							if(RxMessage.Data[1]==0x01)   //收缩
							{
											switch(RxMessage.Data[2])
										{
											case 1: Motor2_control(0,400); break;
											case 2: Motor2_control(0,500); break;
											case 3: Motor2_control(0,750); break;
											case 4: Motor2_control(0,900); break;
										}
										Dealy_us(50000);  //50ms 给一个起步缓冲，之后测速
									 do
											 {
													count=(TIM1->CNT);
													Dealy_us(10000);  //10ms
													count1=(TIM1->CNT);
												  speed = count - count1;
												  SendSpeed(speed);                //收缩不存在滤波的问题
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
											switch(RxMessage.Data[2])   //扩张
											{
												case 1: Motor2_control(300,0); break;
												case 2: Motor2_control(500,0); break;
												case 3: Motor2_control(750,0); break;
												case 4: Motor2_control(900,0); break;
											}
											  Dealy_us(50000);  //50ms   给一个起步缓冲 ，之后再测速
						           	 do
											 {
													for(int i = 0; i < 10; i++)
														 {
																int speedarr[10];
																count=(TIM1->CNT);
																Dealy_us(1000);  //10ms
																count1=(TIM1->CNT);
																speed = count1 - count;
																		if (speed<0x08)              //检测瞬时速度是否小于阈值，小于立即置1 flag，跳出滤波和循环。
																		{
																		 speedflag = 1;
																		 break;
																		}
															  speedarr[i] = speed;
																avgspeed += speedarr[i];
															 }
												  if( speedflag){ break;}        //如果在内部循环中查找到，speed小于阈值，立马停止滤波，跳出循环。
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
							 GPIO_ResetBits(GPIOC,GPIO_Pin_7); //停止
					//			TxMessage.Data[0]=cycle;                              控制支撑，与距离里程计无关
					//			TxMessage.Data[1]=(TIM1->CNT)/4;
					//			CAN_Transmit(CAN1, &TxMessage);
							}
						}
					}
	/*if(RxMessage.Data[1]==0x0B)   //通过CAN发送数据，另一块单片机执行
	{
		std=0xff;
		CAN_SetMsg();
		TransmitMailbox=CAN_Transmit(CAN1,&TxMessage);
		while((CAN_TransmitStatus(CAN1,TransmitMailbox) !=CANTXOK)); //等待发送完成	
   	printf("传输完毕");
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
void Delay_Ms(u16 time)  //延时函数
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




