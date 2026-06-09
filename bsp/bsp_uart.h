#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "stm32f4xx_hal.h"
#include "ringbuffer.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern rbuffer_t rbuffer_user;
extern rbuffer_t rbuffer_camera;

void uart1_init(uint32_t baudrate);
void uart2_init(uint32_t baudrate);

#endif
