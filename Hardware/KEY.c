#include "stm32f10x.h"                  // Device header
#include "Delay.h"
void Key_Init(void)
{
    //PA8和PA11作为按键输入，低电位驱动，上拉输入
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
     GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //不用配置高电位，因为是上拉输入，默认就是高电位
}
uint8_t Key_GetNum(void){
    uint8_t KeyNum = 0;
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0){
        KeyNum |=0x01;
    }
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0){
        KeyNum |=0x02;
    }
    return KeyNum;
}
/**
 * 长按亮，松手灭
 */

