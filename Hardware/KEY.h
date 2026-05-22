#ifndef __KEY_H
#define __KEY_H
////PA8和PA11作为按键输入，低电位驱动，上拉输入
void Key_Init(void);
uint8_t Key_GetNum(void);
#endif
