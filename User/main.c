#include "stm32f10x.h"                  // Device header
#include "Delay.h"					   // Delay header
#include "LED.h"					   // LED header
#include "buzzer.h"				   	  // Buzzer header
#include "KEY.h"					  // Key header
#include "Light_Sensor.h"			  // Light Sensor header

int main(void)
{
	LED_Init();
	//Buzzer_Init();
	Key_Init();
	Light_Sensor_Init();
	uint8_t KeyNum = 0;
	while(1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum & 0x01||Light_Sensor_GetState()==1)
		{
			LEDA0_TURN();
		}
		if (KeyNum & 0x02)
		{
			LEDC14_TURN();
		}
		
	}
}
