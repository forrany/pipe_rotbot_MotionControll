#ifndef __uart_H
#define __uart_H
#include "stm32f10x.h"  
#include"stm32f10x_tim.h"

void LED_CONF(void);
void Motor1_control(u16 CCR3_Val,u16 CCR4_Val);
void Motor2_control(u16 CCR3_Val,u16 CCR4_Val);
void Motor3_control(u16 CCR3_Val,u16 CCR4_Val);
void Motor4_control(u16 CCR3_Val,u16 CCR4_Val);
void TIM1_PWM_Init(u16 arr,u16 psc);
void TIM4_GPIO_Config(void);
#endif 
