#ifndef _BSP_SG90_H
#define _BSP_SG90_H

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim4;

void sg90_servo_init(void);
void set_servo_angle(uint8_t channel, double angle);

#endif
