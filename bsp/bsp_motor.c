#include "bsp_motor.h"

/*
 * 电机初始化
 * CubeMX 已完成 TIM3/TIM5 的 PWM 配置和 GPIO 映射
 * 此处只需启动 PWM 输出
 */
void motor_init(void)
{
    /* 启动 TIM5 CH1-CH4 PWM（左侧电机） */
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);

    /* 启动 TIM3 CH1-CH4 PWM（右侧电机） */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}

/*
 * 设置单个电机的 PWM 占空比和方向
 */
void motor_ctrl(motor_t motor, uint8_t forword, uint32_t speed)
{
    speed = (speed > DRV_TIMER_PERIOD) ? DRV_TIMER_PERIOD : speed;

    switch (motor) {
        case E_MOTOR_LB: {
            if (forword == 1) {
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, speed);
            } else {
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, speed);
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, 0);
            }
        } break;
        case E_MOTOR_LF: {
            if (forword == 1) {
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, 0);
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, speed);
            } else {
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, speed);
                __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, 0);
            }
        } break;
        case E_MOTOR_RB: {
            if (forword == 1) {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
            } else {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
            }
        } break;
        case E_MOTOR_RF: {
            if (forword == 1) {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, speed);
            } else {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, speed);
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
            }
        } break;
        default:
            break;
    }
}

/*
 * 整车运动控制
 */
void motor_move(dir_t dir, uint32_t speed)
{
    speed = (speed > DRV_TIMER_PERIOD) ? DRV_TIMER_PERIOD : speed;

    switch (dir) {
        case E_DIR_FORWORD: {
            motor_ctrl(E_MOTOR_LB, 1, speed);
            motor_ctrl(E_MOTOR_RB, 1, speed);
            motor_ctrl(E_MOTOR_LF, 1, speed);
            motor_ctrl(E_MOTOR_RF, 1, speed);
        } break;
        case E_DIR_BACKWORD: {
            motor_ctrl(E_MOTOR_LB, 0, speed);
            motor_ctrl(E_MOTOR_RB, 0, speed);
            motor_ctrl(E_MOTOR_LF, 0, speed);
            motor_ctrl(E_MOTOR_RF, 0, speed);
        } break;
        case E_DIR_LEFT: {
            motor_ctrl(E_MOTOR_LB, 0, speed);
            motor_ctrl(E_MOTOR_RB, 0, speed);
            motor_ctrl(E_MOTOR_LF, 1, speed);
            motor_ctrl(E_MOTOR_RF, 1, speed);
        } break;
        case E_DIR_RIGHT: {
            motor_ctrl(E_MOTOR_LB, 1, speed);
            motor_ctrl(E_MOTOR_RB, 1, speed);
            motor_ctrl(E_MOTOR_LF, 0, speed);
            motor_ctrl(E_MOTOR_RF, 0, speed);
        } break;
        default:
            break;
    }
}
