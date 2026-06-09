#ifndef __BSP_SYS_TICK_H__
#define __BSP_SYS_TICK_H__

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim7;

void sys_timer_init(void);
uint32_t get_sys_tick(void);

#endif
