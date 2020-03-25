
#include "main.h"
#include <stdbool.h>
uint16_t delay_count=0;

void SysTick_Handler(void)//1ms
{
	if (delay_count > 0){delay_count--;}
}

void delay_ms(uint16_t delay_temp)
{
	delay_count = delay_temp;
	while(delay_count){}
}

void LEDs_init(void)
{
	GPIO_InitTypeDef GPIO_Init_LED;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_Init_LED.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_Init_LED.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init_LED.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init_LED.GPIO_OType = GPIO_OType_PP;
	GPIO_Init_LED.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOD, &GPIO_Init_LED);
}

void BUTTON_init(void){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
}

uint16_t a = 0;
uint16_t pressCount = 0;
uint16_t array[] ={10, 20, 17, 14, 56, 90, 12, 15, 1, 29, 34, 145}; 

void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
		switch(pressCount){
			case 0: 
				GPIO_SetBits(GPIOD, GPIO_Pin_12|GPIO_Pin_13);
				GPIO_ResetBits(GPIOD, GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				break;
			case 1: 
				GPIO_SetBits(GPIOD, GPIO_Pin_14|GPIO_Pin_15);
				GPIO_ResetBits(GPIOD,  GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15);
				break;
		}
		if(pressCount < 1){
				pressCount++;
		} else {
			pressCount = 0;
		}
	}
	EXTI_ClearITPendingBit(EXTI_Line0);
}

void NVIC_Initial(void){
	
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the EXTI0 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void EXTI_Initial(void){
SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	EXTI_InitTypeDef extiTypeDef;
	extiTypeDef.EXTI_Line = EXTI_Line0;
	extiTypeDef.EXTI_Mode = EXTI_Mode_Interrupt;
	extiTypeDef.EXTI_LineCmd = ENABLE;
	extiTypeDef.EXTI_Trigger = EXTI_Trigger_Falling;
	
	EXTI_Init(&extiTypeDef);
	
	EXTI_ClearITPendingBit(EXTI_Line0);
}

int main(void)
{
	LEDs_init();
	SysTick_Config(SystemCoreClock/1000);
	NVIC_Initial();
	EXTI_Initial();
	
	GPIO_SetBits(GPIOD, GPIO_Pin_15);
	while(1)
	{	}
}

