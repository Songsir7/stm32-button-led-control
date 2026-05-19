#ifndef _DELAY_H
#define _DELAY_H
#include "stm32f10x.h"
void delay_us(uint32_t nus);
void delay_ms(uint16_t nms);
void delay_s(uint16_t ns);
#endif
