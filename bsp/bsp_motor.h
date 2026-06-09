#ifndef _BSP_MOTOR_H_
#define _BSP_MOTOR_H_

#include "stm32f4xx_hal.h"

#define DRV_TIMER_PERIOD  1000

typedef enum {
    E_MOTOR_LB,
    E_MOTOR_LF,
    E_MOTOR_RB,
    E_MOTOR_RF,
} motor_t;

typedef enum {
    E_DIR_FORWORD,
    E_DIR_BACKWORD,
    E_DIR_LEFT,
    E_DIR_RIGHT
} dir_t;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;

void motor_init(void);
void motor_ctrl(motor_t motor, uint8_t dir, uint32_t speed);
void motor_move(dir_t dir, uint32_t speed);

#endif
