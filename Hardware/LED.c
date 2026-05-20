#include "stm32f10x.h"                  // Device header
void LED_Init(void){
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
    GPIO_SetBits(GPIOC, GPIO_Pin_14);
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void LEDA0_ON(void){
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}
void LEDC14_ON(void){
    GPIO_ResetBits(GPIOC, GPIO_Pin_14);
}
void LEDA0_OFF(void){
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
}
void LEDC14_OFF(void){
    GPIO_SetBits(GPIOC, GPIO_Pin_14);
}
