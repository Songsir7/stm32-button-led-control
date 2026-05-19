#include "stm32f10x.h"
/**
 * @brief 微秒级延时
 * @param nus 可以延时0-233015微秒
 * @retval None
 */
void delay_us(uint32_t xus){
    SysTick->LOAD =72*xus; //72MHz时钟，1us需要72个时钟周期
    SysTick->VAL = 0x00; //清空计数器
    SysTick->CTRL =0x00000005;//打开系统内核时钟，不使用中断，开始定时器倒计时
    while(!(SysTick->CTRL & 0x00010000)); //等待计数器到达0
    SysTick->CTRL =0x00000004;//关闭定时器
}
/**
 * @brief 毫秒级延时
 * @param nms 可以延时_毫秒
 * @retval None
 */
void delay_ms(uint16_t xms){
    while (xms--)
    {
        delay_us(1000);       
    }
    
}
/**
 * @brief 秒级延时
 * @param ns 可以延时_秒
 * @retval None
 */
void delay_s(uint16_t xs){
    while (xs--)
    {
        delay_ms(1000);
    }
}
