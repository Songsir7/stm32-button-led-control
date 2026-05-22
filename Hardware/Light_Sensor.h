#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

//PB9作为光敏传感器输入，上拉输入，有光照输出0，无光照输出1
void Light_Sensor_Init(void);
uint8_t Light_Sensor_GetState(void);
#endif
