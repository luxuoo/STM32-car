#include "bsp_sg90.h"

/*
 * SG90 舵机初始化
 * CubeMX 已完成 TIM4 的 PWM 配置（PD12-CH1, PD13-CH2, 50Hz）
 * 此处只需启动 PWM 输出
 */
void sg90_servo_init(void)
{
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
}

/*
 * 设置舵机角度
 * channel: 0=X轴(PD12), 1=Y轴(PD13)
 * angle: 0-180 度
 *
 * 脉宽计算:
 *   ARR = 199, 周期 = 20ms
 *   0.5ms → (0.5/20) * 200 = 5
 *   2.5ms → (2.5/20) * 200 = 25
 */
void set_servo_angle(uint8_t channel, double angle)
{
    angle = (angle > 180) ? 180 : angle;
    angle = (angle < 0) ? 0 : angle;

    uint32_t duty = (uint32_t)((0.5 + (angle / 180.0) * 2.0) / 20.0 * 200);

    if (channel > 0) {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, duty);
    } else {
        __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, duty);
    }
}
