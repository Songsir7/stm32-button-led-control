#include "stm32f10x.h"                  // Device header
#include "Delay.h"
////PA8和PA11作为按键输入，低电位驱动，上拉输入
void Key_Init(void)
{
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
        delay_ms(20); //消抖
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0){
        //阻塞式按键检测代码，检测到按键被按下后，等待按键被松开
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0);
        delay_ms(20); //消抖
        KeyNum |=0x01;
        }
    }
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0){
        delay_ms(20); //消抖
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0){
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0);
        delay_ms(20); //消抖
        KeyNum |=0x02;
        }
    }
    return KeyNum;
}

/**
 * 非阻塞式按键检测代码
 * uint8_t Key_Scan(void)
{
    uint8_t KeyNum = 0;

    static uint8_t StableState[2] = {1, 1};  // 稳定状态，1松开，0按下
    static uint8_t LastState[2]   = {1, 1};  // 上一次读取状态
    static uint8_t Count[2]       = {0, 0};  // 稳定计数

    uint8_t NowState[2];

    NowState[0] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
    NowState[1] = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11);

    for (uint8_t i = 0; i < 2; i++)
    {
        if (NowState[i] == LastState[i])
        {
            if (Count[i] < 2)
            {
                Count[i]++;
            }
            else
            {
                if (StableState[i] != NowState[i])
                {
                    StableState[i] = NowState[i];

                    // 检测到稳定的“按下”动作
                    if (StableState[i] == 0)
                    {
                        KeyNum |= (1 << i);
                    }
                }
            }
        }
        else
        {
            Count[i] = 0;
            LastState[i] = NowState[i];
        }
    }

    return KeyNum;
}
 */

