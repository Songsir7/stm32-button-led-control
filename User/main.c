#include "stm32f10x.h"                  // Device header
#include "Delay.h"					   // Delay header
#include "LED.h"					   // LED header
#include "buzzer.h"				   // Buzzer header
#include "KEY.h"					   // Key header


int main(void)
{
	LED_Init();
	Buzzer_Init();
	Key_Init();
	uint8_t KeyNum = 0;
	while(1)
	{
		KeyNum=Key_GetNum();
		//eyNum==0x01写法是不对的，0x01和0x03都满足条件，所以要用位运算的方式来判断按键状态
		if(KeyNum & 0x01)
		{
			LEDA0_ON();
		}
		else
		{
			LEDA0_OFF();
		}
		if (KeyNum & 0x02)
		{
			LEDC14_ON();
		}
		else
		{
			LEDC14_OFF();
		}

	}
}
