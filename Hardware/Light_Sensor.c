#include "stm32f10x.h"                  // Device header

void Light_Sensor_Init(void)
{
    /**
     * 光敏电阻的工作原理是：光照越强，阻值越小，输出低电位；光照越弱，阻值越大，输出高电位。
     * PB9作为光敏传感器输入，低电位驱动，上拉输入
     */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Light_Sensor_GetState(void)
{
    /**
     * 有光照-D0输出低电位-寄存器读到0
     * 没有光照-D0输出高电位-寄存器读到1
     */
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
}

