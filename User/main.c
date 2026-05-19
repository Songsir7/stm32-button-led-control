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
		//按键PA8
		if(KeyNum==1){
			LEDA0_ON();
		}
		//按键PA11
		if(KeyNum==2){
			LEDC14_ON();
		}
		// if(KeyNum==0){
		// 	LEDA0_OFF();
		// 	LEDC14_OFF();
		// }

	}
}
