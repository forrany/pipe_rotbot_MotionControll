#include "motor.h"  
#include "can.h"
#include "encode.h"
#include "timer.h"
void  Delay_Ms(u16 time);
void SendSpeed(unsigned short Count1, unsigned short Count2);
__IO uint32_t flag = 0xff;		 //用于CAN标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			     //CAN1发送缓冲区
CanRxMsg RxMessage;				 //CAN1接收缓冲区 
unsigned short count;//编码器计数  两字节
unsigned short count1;
unsigned short speed; //Speed of Motor;
volatile u32 cycle=127; 

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

					if(RxMessage.Data[1]==0x01)    //前进
					{
						switch(RxMessage.Data[2])
								{
									case 1: Motor1_control(400,0); break;
									case 2: Motor1_control(600,0); break;
									case 3: Motor1_control(750,0); break;
									case 4: Motor1_control(999,0); break;	
								}	
		       }
				 if(RxMessage.Data[1]==0x02)   //后退
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
							if(RxMessage.Data[1]==0x01)
							{
											switch(RxMessage.Data[2])
										{
											case 1: Motor2_control(0,400); break;
											case 2: Motor2_control(0,500); break;
											case 3: Motor2_control(0,750); break;
											case 4: Motor2_control(0,900); break;
										}
									 do
											 {
													count=(TIM1->CNT);
													Dealy_us(10000);  //10ms
													count1=(TIM1->CNT);
												  SendSpeed(count1,count);
											 if(RxMessage.Data[1]==0x00)
							         break;
//												  TxMessage.Data[0]=count;
//												  TxMessage.Data[1]=count1;
//												  CAN_Transmit(CAN1,&TxMessage);
											 } 	 while(count!=count1);
								 GPIO_ResetBits(GPIOC,GPIO_Pin_7);
								 TIM1->CNT=0;
	
							 }
							if(RxMessage.Data[1]==0x02)
								{ 
											switch(RxMessage.Data[2])
											{
												case 1: Motor2_control(300,0); break;
												case 2: Motor2_control(500,0); break;
												case 3: Motor2_control(750,0); break;
												case 4: Motor2_control(900,0); break;
											}
						           	 do
											 {
													count=(TIM1->CNT);
													Dealy_us(10000);  //10ms
													count1=(TIM1->CNT);
												  SendSpeed(count,count1);
											if(RxMessage.Data[1]==0x00)
							         break;
//												  TxMessage.Data[0]=count;
//												  TxMessage.Data[1]=count1;
//												  CAN_Transmit(CAN1,&TxMessage);
											 } 	 while(count!=count1);
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

void SendSpeed(unsigned short Count1, unsigned short Count2)
{
		speed = (Count2 - Count1);
	  TxMessage.Data[0]= speed&0x00ff;
		TxMessage.Data[1]= speed>>8;
		//TxMessage.Data[0]= Count1&0x00ff;
		//TxMessage.Data[1]= Count1>>8;
		TxMessage.Data[2]= 0xAC;
		CAN_Transmit(CAN1, &TxMessage);
}
